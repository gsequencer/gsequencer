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

#include <ags/plugin/ags_base_plugin.h>

#include <ags/libags.h>

#include <stdlib.h>
#include <string.h>

#include <ags/plugin/ags_plugin_port.h>

#include <ags/i18n.h>

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
void ags_base_plugin_dispose(GObject *gobject);
void ags_base_plugin_finalize(GObject *gobject);

/**
 * SECTION:ags_base_plugin
 * @short_description: The base plugin class
 * @title: AgsBasePlugin
 * @section_id:
 * @include: ags/plugin/ags_base_plugin.h
 *
 * The #AgsBasePlugin loads/unloads plugins on a abstraction level.
 */

enum{
  INSTANTIATE,
  INSTANTIATE_WITH_PARAMS,
  CONNECT_PORT,
  ACTIVATE,
  DEACTIVATE,
  RUN,
  LOAD_PLUGIN,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_UUID,
  PROP_FILENAME,
  PROP_EFFECT,
  PROP_PLUGIN_PORT,
  PROP_EFFECT_INDEX,
  PROP_PLUGIN_SO,
  PROP_UI_FILENAME,
  PROP_UI_EFFECT,
  PROP_UI_EFFECT_INDEX,  
  PROP_UI_PLUGIN_SO,
  PROP_UI_PLUGIN,
};

static gpointer ags_base_plugin_parent_class = NULL;
static guint base_plugin_signals[LAST_SIGNAL];

GType
ags_base_plugin_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_base_plugin = 0;

    static const GTypeInfo ags_base_plugin_info = {
      sizeof(AgsBasePluginClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_base_plugin_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsBasePlugin),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_base_plugin_init,
    };

    ags_type_base_plugin = g_type_register_static(G_TYPE_OBJECT,
						  "AgsBasePlugin",
						  &ags_base_plugin_info,
						  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_base_plugin);
  }

  return g_define_type_id__volatile;
}

GType
ags_base_plugin_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_BASE_PLUGIN_IS_INSTRUMENT, "AGS_BASE_PLUGIN_IS_INSTRUMENT", "base-plugin-is-instrument" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsBasePluginFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
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

  gobject->dispose = ags_base_plugin_dispose;
  gobject->finalize = ags_base_plugin_finalize;

  /* properties */
  /**
   * AgsBasePlugin:uuid:
   *
   * The assigned AgsUUID
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("uuid",
				    i18n_pspec("UUID"),
				    i18n_pspec("The UUID"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_UUID,
				  param_spec);

  /**
   * AgsBasePlugin:filename:
   *
   * The assigned filename.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("filename",
				   i18n_pspec("filename of the plugin"),
				   i18n_pspec("The filename this plugin is located in"),
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
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("effect",
				   i18n_pspec("effect of the plugin"),
				   i18n_pspec("The effect this plugin is assigned with"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_EFFECT,
				  param_spec);

  /**
   * AgsBasePlugin:plugin-port: (type GList(AgsPluginPort)) (transfer full)
   *
   * The assigned #GList-struct containing #AgsPluginPort
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("plugin-port",
				    i18n_pspec("plugin port of base plugin"),
				    i18n_pspec("The plugin port of base plugin"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLUGIN_PORT,
				  param_spec);
  
  /**
   * AgsBasePlugin:effect-index:
   *
   * The assigned effect-index.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("effect-index",
				 i18n_pspec("effect index of the plugin"),
				 i18n_pspec("The effect's index this plugin is assigned with"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_EFFECT_INDEX,
				  param_spec);

  /**
   * AgsBasePlugin:plugin-so:
   *
   * The assigned plugin.so
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("plugin-so",
				    i18n_pspec("plugin.so"),
				    i18n_pspec("The plugin.so"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLUGIN_SO,
				  param_spec);
  
  /**
   * AgsBasePlugin:ui-filename:
   *
   * The assigned UI filename.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("ui-filename",
				   i18n_pspec("UI filename of the plugin"),
				   i18n_pspec("The UI filename this plugin is located in"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_UI_FILENAME,
				  param_spec);

  /**
   * AgsBasePlugin:ui-effect:
   *
   * The assigned ui-effect.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("ui-effect",
				   i18n_pspec("UI effect of the plugin"),
				   i18n_pspec("The UI effect this plugin is assigned with"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_UI_EFFECT,
				  param_spec);

  /**
   * AgsBasePlugin:ui-effect-index:
   *
   * The assigned ui-effect-index.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("ui-effect-index",
				 i18n_pspec("UI effect-index of the plugin"),
				 i18n_pspec("The UI effect-index this plugin is assigned with"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_UI_EFFECT_INDEX,
				  param_spec);

  /**
   * AgsBasePlugin:ui-plugin-so:
   *
   * The assigned ui_plugin.so
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("ui-plugin-so",
				    i18n_pspec("ui_plugin.so"),
				    i18n_pspec("The ui_plugin.so"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_UI_PLUGIN_SO,
				  param_spec);
  
  /**
   * AgsBasePlugin:ui-plugin:
   *
   * The assigned ui-plugin.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("ui-plugin",
				   i18n_pspec("ui-plugin of the plugin"),
				   i18n_pspec("The ui-plugin this plugin is assigned with"),
				   AGS_TYPE_BASE_PLUGIN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_UI_PLUGIN,
				  param_spec);

  /* AgsBasePluginClass */
  base_plugin->instantiate = NULL;
  base_plugin->instantiate_with_params = NULL;

  base_plugin->connect_port = NULL;
  
  base_plugin->activate = NULL;
  base_plugin->deactivate = NULL;

  base_plugin->run = NULL;

  base_plugin->load_plugin = NULL;

  /* signals */
  /**
   * AgsBasePlugin::instantiate:
   * @base_plugin: the plugin to instantiate
   * @samplerate: the samplerate
   * @buffer_size: the fixed buffer size
   *
   * The ::instantiate signal creates a new instance of plugin.
   * 
   * Returns: the new plugin instance
   * 
   * Since: 3.0.0
   */
  base_plugin_signals[INSTANTIATE] =
    g_signal_new("instantiate",
		 G_TYPE_FROM_CLASS (base_plugin),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsBasePluginClass, instantiate),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__UINT_UINT,
		 G_TYPE_POINTER, 2,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsBasePlugin::instantiate-with-params:
   * @base_plugin: the plugin to instantiate
   * @n_params: pointer to array length
   * @parameter_name: parameter name string vector
   * @value: the #GValue-struct array
   *
   * The ::instantiate-with-params signal creates a new instance of plugin.
   * 
   * Returns: the new plugin instance
   * 
   * Since: 3.0.0
   */
  base_plugin_signals[INSTANTIATE_WITH_PARAMS] =
    g_signal_new("instantiate-with-params",
		 G_TYPE_FROM_CLASS (base_plugin),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsBasePluginClass, instantiate_with_params),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__POINTER_POINTER_POINTER,
		 G_TYPE_POINTER, 3,
		 G_TYPE_POINTER,
		 G_TYPE_POINTER,
		 G_TYPE_POINTER);
  
  /**
   * AgsBasePlugin::connect-port:
   * @base_plugin: the plugin to connect-port
   * @plugin_handle: the plugin handle
   * @port_index: the port index
   * @data_location: the data location
   *
   * The ::connect-port signal creates a new instance of plugin.
   * 
   * Since: 3.0.0
   */
  base_plugin_signals[CONNECT_PORT] =
    g_signal_new("connect-port",
		 G_TYPE_FROM_CLASS (base_plugin),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsBasePluginClass, connect_port),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__POINTER_UINT_POINTER,
		 G_TYPE_NONE, 3,
		 G_TYPE_POINTER,
		 G_TYPE_UINT,
		 G_TYPE_POINTER);

  /**
   * AgsBasePlugin::activate:
   * @base_plugin: the plugin to activate
   * @plugin_handle: the plugin handle
   *
   * The ::activate signal creates a new instance of plugin.
   * 
   * Since: 3.0.0
   */
  base_plugin_signals[ACTIVATE] =
    g_signal_new("activate",
		 G_TYPE_FROM_CLASS (base_plugin),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsBasePluginClass, activate),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__POINTER,
		 G_TYPE_NONE, 1,
		 G_TYPE_POINTER);

  /**
   * AgsBasePlugin::deactivate:
   * @base_plugin: the plugin to deactivate
   * @plugin_handle: the plugin handle
   *
   * The ::deactivate signal creates a new instance of plugin.
   * 
   * Since: 3.0.0
   */
  base_plugin_signals[DEACTIVATE] =
    g_signal_new("deactivate",
		 G_TYPE_FROM_CLASS (base_plugin),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsBasePluginClass, deactivate),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__POINTER,
		 G_TYPE_NONE, 1,
		 G_TYPE_POINTER);

  /**
   * AgsBasePlugin::run:
   * @base_plugin: the plugin to run
   * @plugin_handle: the plugin handle
   * @seq_event: the MIDI data
   * @frame_count: the frame count
   *
   * The ::run signal creates a new instance of plugin.
   * 
   * Since: 3.0.0
   */
  base_plugin_signals[RUN] =
    g_signal_new("run",
		 G_TYPE_FROM_CLASS (base_plugin),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsBasePluginClass, run),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__POINTER_POINTER_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_POINTER,
		 G_TYPE_POINTER,
		 G_TYPE_UINT);

  /**
   * AgsBasePlugin::load-plugin:
   * @base_plugin: the plugin to load_plugin
   *
   * The ::load-plugin signal creates a new instance of plugin.
   * 
   * Since: 3.0.0
   */
  base_plugin_signals[LOAD_PLUGIN] =
    g_signal_new("load-plugin",
		 G_TYPE_FROM_CLASS (base_plugin),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsBasePluginClass, load_plugin),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_base_plugin_init(AgsBasePlugin *base_plugin)
{
  base_plugin->flags = 0;

  /* add base plugin mutex */
  g_rec_mutex_init(&(base_plugin->obj_mutex));

  /*  */
  base_plugin->uuid = NULL;
  
  base_plugin->filename = NULL;
  base_plugin->effect = NULL;

  base_plugin->port_group_count = 0;
  base_plugin->port_group = NULL;
  base_plugin->plugin_port = NULL;

  base_plugin->effect_index = 0;
  base_plugin->plugin_so = NULL;
  base_plugin->plugin_descriptor = NULL;
  base_plugin->plugin_handle = NULL;

  base_plugin->ui_filename = NULL;
  base_plugin->ui_effect = NULL;

  base_plugin->ui_effect_index = 0;
  base_plugin->ui_plugin_so = NULL;
  base_plugin->ui_plugin_descriptor = NULL;
  base_plugin->ui_plugin_handle = NULL;

  base_plugin->ui_plugin = NULL;
}

void
ags_base_plugin_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsBasePlugin *base_plugin;

  GRecMutex *base_plugin_mutex;

  base_plugin = AGS_BASE_PLUGIN(gobject);

  /* get base plugin mutex */
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(base_plugin);

  switch(prop_id){
  case PROP_UUID:
    {
      g_rec_mutex_lock(base_plugin_mutex);

      base_plugin->uuid = (AgsUUID *) g_value_get_pointer(value);

      g_rec_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = (gchar *) g_value_get_string(value);

      g_rec_mutex_lock(base_plugin_mutex);

      if(base_plugin->filename == filename){
	g_rec_mutex_unlock(base_plugin_mutex);
	
	return;
      }
      
      if(base_plugin->filename != NULL){
	g_free(base_plugin->filename);
      }

      base_plugin->filename = g_strdup(filename);

      g_rec_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_EFFECT:
    {
      gchar *effect;

      effect = (gchar *) g_value_get_string(value);

      g_rec_mutex_lock(base_plugin_mutex);

      if(base_plugin->effect == effect){
	g_rec_mutex_unlock(base_plugin_mutex);

	return;
      }
      
      if(base_plugin->effect != NULL){
	g_free(base_plugin->effect);
      }

      base_plugin->effect = g_strdup(effect);

      g_rec_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_PLUGIN_PORT:
    {
      AgsPluginPort *plugin_port;

      plugin_port = (AgsPluginPort *) g_value_get_pointer(value);

      g_rec_mutex_lock(base_plugin_mutex);

      if(!AGS_IS_PLUGIN_PORT(plugin_port) ||
	 g_list_find(base_plugin->plugin_port, plugin_port) != NULL){
	g_rec_mutex_unlock(base_plugin_mutex);
	
	return;
      }

      g_object_ref(plugin_port);
      base_plugin->plugin_port = g_list_prepend(base_plugin->plugin_port,
						plugin_port);

      g_rec_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_EFFECT_INDEX:
    {
      guint effect_index;

      effect_index = g_value_get_uint(value);

      g_rec_mutex_lock(base_plugin_mutex);

      base_plugin->effect_index = effect_index;

      g_rec_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_PLUGIN_SO:
    {
      g_rec_mutex_lock(base_plugin_mutex);

      base_plugin->plugin_so = g_value_get_pointer(value);

      g_rec_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_UI_FILENAME:
    {
      gchar *ui_filename;

      ui_filename = (gchar *) g_value_get_string(value);

      g_rec_mutex_lock(base_plugin_mutex);

      if(base_plugin->ui_filename == ui_filename){
	g_rec_mutex_unlock(base_plugin_mutex);

	return;
      }
      
      if(base_plugin->ui_filename != NULL){
	g_free(base_plugin->ui_filename);
      }

      base_plugin->ui_filename = g_strdup(ui_filename);

      g_rec_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_UI_EFFECT:
    {
      gchar *ui_effect;

      ui_effect = (gchar *) g_value_get_string(value);

      g_rec_mutex_lock(base_plugin_mutex);

      if(base_plugin->ui_effect == ui_effect){
	g_rec_mutex_unlock(base_plugin_mutex);

	return;
      }
      
      if(base_plugin->ui_effect != NULL){
	g_free(base_plugin->ui_effect);
      }

      base_plugin->ui_effect = g_strdup(ui_effect);

      g_rec_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_UI_EFFECT_INDEX:
    {
      guint ui_effect_index;

      ui_effect_index = g_value_get_uint(value);

      g_rec_mutex_lock(base_plugin_mutex);

      base_plugin->ui_effect_index = ui_effect_index;

      g_rec_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_UI_PLUGIN_SO:
    {
      g_rec_mutex_lock(base_plugin_mutex);

      base_plugin->ui_plugin_so = g_value_get_pointer(value);

      g_rec_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_UI_PLUGIN:
    {
      GObject *ui_plugin;

      ui_plugin = g_value_get_object(value);

      g_rec_mutex_lock(base_plugin_mutex);

      if(base_plugin->ui_plugin == ui_plugin){
	g_rec_mutex_unlock(base_plugin_mutex);

	return;
      }

      if(base_plugin->ui_plugin != NULL){
	g_object_unref(base_plugin->ui_plugin);
      }

      if(ui_plugin != NULL){
	g_object_ref(ui_plugin);
      }
      
      base_plugin->ui_plugin = ui_plugin;

      g_rec_mutex_unlock(base_plugin_mutex);
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

  GRecMutex *base_plugin_mutex;

  base_plugin = AGS_BASE_PLUGIN(gobject);

  /* get base plugin mutex */
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(base_plugin);

  switch(prop_id){
  case PROP_UUID:
    {
      g_rec_mutex_lock(base_plugin_mutex);
      
      g_value_set_pointer(value, base_plugin->uuid);

      g_rec_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_FILENAME:
    {
      g_rec_mutex_lock(base_plugin_mutex);

      g_value_set_string(value, base_plugin->filename);

      g_rec_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_EFFECT:
    {
      g_rec_mutex_lock(base_plugin_mutex);

      g_value_set_string(value, base_plugin->effect);

      g_rec_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_PLUGIN_PORT:
    {
      g_rec_mutex_lock(base_plugin_mutex);
      
      g_value_set_pointer(value, g_list_copy_deep(base_plugin->plugin_port,
						  (GCopyFunc) g_object_ref,
						  NULL));

      g_rec_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_EFFECT_INDEX:
    {
      g_rec_mutex_lock(base_plugin_mutex);

      g_value_set_uint(value, base_plugin->effect_index);

      g_rec_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_PLUGIN_SO:
    {
      g_rec_mutex_lock(base_plugin_mutex);
      
      g_value_set_pointer(value, base_plugin->plugin_so);

      g_rec_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_UI_FILENAME:
    {
      g_rec_mutex_lock(base_plugin_mutex);

      g_value_set_string(value, base_plugin->ui_filename);

      g_rec_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_UI_EFFECT:
    {
      g_rec_mutex_lock(base_plugin_mutex);

      g_value_set_string(value, base_plugin->ui_effect);

      g_rec_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_UI_EFFECT_INDEX:
    {
      g_rec_mutex_lock(base_plugin_mutex);

      g_value_set_uint(value, base_plugin->ui_effect_index);

      g_rec_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_UI_PLUGIN_SO:
    {
      g_rec_mutex_lock(base_plugin_mutex);
      
      g_value_set_pointer(value, base_plugin->ui_plugin_so);

      g_rec_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_UI_PLUGIN:
    {
      g_rec_mutex_lock(base_plugin_mutex);

      g_value_set_object(value, base_plugin->ui_plugin);

      g_rec_mutex_unlock(base_plugin_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_base_plugin_dispose(GObject *gobject)
{
  AgsBasePlugin *base_plugin;

  base_plugin = AGS_BASE_PLUGIN(gobject);

  if(base_plugin->ui_plugin != NULL){
    g_object_unref(base_plugin->ui_plugin);

    base_plugin->ui_plugin = NULL;
  }

  /* plugin port */
  if(base_plugin->plugin_port != NULL){
    g_list_free_full(base_plugin->plugin_port,
		     g_object_unref);

    base_plugin->plugin_port = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_base_plugin_parent_class)->dispose(gobject);
}

void
ags_base_plugin_finalize(GObject *gobject)
{
  AgsBasePlugin *base_plugin;

  base_plugin = AGS_BASE_PLUGIN(gobject);

  /* uuid */
  if(base_plugin->uuid != NULL){
    ags_uuid_free(base_plugin->uuid);
  }
  
  /* filename and effect */
  g_free(base_plugin->filename);
  g_free(base_plugin->effect);

  /* UI filename and effect */
  g_free(base_plugin->ui_filename);
  g_free(base_plugin->ui_effect);

  /* plugin port */
  g_list_free_full(base_plugin->plugin_port,
		   g_object_unref);

  /* ui-plugin */
  if(base_plugin->ui_plugin != NULL){
    g_object_unref(base_plugin->ui_plugin);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_base_plugin_parent_class)->finalize(gobject);
}

/**
 * ags_base_plugin_get_obj_mutex:
 * @base_plugin: the #AgsBasePlugin
 * 
 * Get object mutex.
 * 
 * Returns: the #GRecMutex to lock @base_plugin
 * 
 * Since: 3.1.0
 */
GRecMutex*
ags_base_plugin_get_obj_mutex(AgsBasePlugin *base_plugin)
{
  if(!AGS_IS_BASE_PLUGIN(base_plugin)){
    return(NULL);
  }

  return(AGS_BASE_PLUGIN_GET_OBJ_MUTEX(base_plugin));
}

/**
 * ags_base_plugin_test_flags:
 * @base_plugin: the #AgsBasePlugin
 * @flags: the flags
 * 
 * Test @flags to be set on @recall.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_base_plugin_test_flags(AgsBasePlugin *base_plugin, guint flags)
{
  gboolean retval;
  
  GRecMutex *base_plugin_mutex;

  if(!AGS_IS_BASE_PLUGIN(base_plugin)){
    return(FALSE);
  }
  
  /* get base plugin mutex */
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(base_plugin);

  /* test flags */
  g_rec_mutex_lock(base_plugin_mutex);

  retval = ((flags & (base_plugin->flags)) != 0) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(base_plugin_mutex);

  return(retval);
}

/**
 * ags_base_plugin_set_flags:
 * @base_plugin: the #AgsBasePlugin
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 3.0.0
 */
void
ags_base_plugin_set_flags(AgsBasePlugin *base_plugin, guint flags)
{
  GRecMutex *base_plugin_mutex;

  if(!AGS_IS_BASE_PLUGIN(base_plugin)){
    return;
  }
  
  /* get base plugin mutex */
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(base_plugin);

  /* set flags */
  g_rec_mutex_lock(base_plugin_mutex);

  base_plugin->flags |= flags;
  
  g_rec_mutex_unlock(base_plugin_mutex);
}

/**
 * ags_base_plugin_unset_flags:
 * @base_plugin: the #AgsBasePlugin
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 3.0.0
 */
void
ags_base_plugin_unset_flags(AgsBasePlugin *base_plugin, guint flags)
{
  GRecMutex *base_plugin_mutex;

  if(!AGS_IS_BASE_PLUGIN(base_plugin)){
    return;
  }
  
  /* get base plugin mutex */
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(base_plugin);

  /* unset flags */
  g_rec_mutex_lock(base_plugin_mutex);

  base_plugin->flags &= (~flags);
  
  g_rec_mutex_unlock(base_plugin_mutex);
}

/**
 * ags_base_plugin_get_filename:
 * @base_plugin: the #AgsBasePlugin
 * 
 * Get filename.
 * 
 * Returns: the filename
 * 
 * Since: 3.1.0
 */
gchar*
ags_base_plugin_get_filename(AgsBasePlugin *base_plugin)
{
  gchar *filename;

  if(!AGS_IS_BASE_PLUGIN(base_plugin)){
    return(NULL);
  }

  g_object_get(base_plugin,
	       "filename", &filename,
	       NULL);

  return(filename);
}

/**
 * ags_base_plugin_set_filename:
 * @base_plugin: the #AgsBasePlugin
 * @filename: the filename
 * 
 * Set filename.
 * 
 * Since: 3.1.0
 */
void
ags_base_plugin_set_filename(AgsBasePlugin *base_plugin,
			gchar *filename)
{
  if(!AGS_IS_BASE_PLUGIN(base_plugin)){
    return;
  }

  g_object_set(base_plugin,
	       "filename", filename,
	       NULL);
}

/**
 * ags_base_plugin_get_effect:
 * @base_plugin: the #AgsBasePlugin
 * 
 * Get effect.
 * 
 * Returns: the effect
 * 
 * Since: 3.1.0
 */
gchar*
ags_base_plugin_get_effect(AgsBasePlugin *base_plugin)
{
  gchar *effect;

  if(!AGS_IS_BASE_PLUGIN(base_plugin)){
    return(NULL);
  }

  g_object_get(base_plugin,
	       "effect", &effect,
	       NULL);

  return(effect);
}

/**
 * ags_base_plugin_set_effect:
 * @base_plugin: the #AgsBasePlugin
 * @effect: the effect
 * 
 * Set effect.
 * 
 * Since: 3.1.0
 */
void
ags_base_plugin_set_effect(AgsBasePlugin *base_plugin,
			   gchar *effect)
{
  if(!AGS_IS_BASE_PLUGIN(base_plugin)){
    return;
  }

  g_object_set(base_plugin,
	       "effect", effect,
	       NULL);
}

/**
 * ags_base_plugin_get_effect_index:
 * @base_plugin: the #AgsBasePlugin
 * 
 * Get effect index.
 * 
 * Returns: the effect index
 * 
 * Since: 3.1.0
 */
guint
ags_base_plugin_get_effect_index(AgsBasePlugin *base_plugin)
{
  guint effect_index;

  if(!AGS_IS_BASE_PLUGIN(base_plugin)){
    return(NULL);
  }

  g_object_get(base_plugin,
	       "effect-index", &effect_index,
	       NULL);

  return(effect_index);
}

/**
 * ags_base_plugin_set_effect_index:
 * @base_plugin: the #AgsBasePlugin
 * @effect_index: the effect index
 * 
 * Set effect index.
 * 
 * Since: 3.1.0
 */
void
ags_base_plugin_set_effect_index(AgsBasePlugin *base_plugin,
				 guint effect_index)
{
  if(!AGS_IS_BASE_PLUGIN(base_plugin)){
    return;
  }

  g_object_set(base_plugin,
	       "effect-index", effect_index,
	       NULL);
}

/**
 * ags_base_plugin_get_plugin_so:
 * @base_plugin: the #AgsBasePlugin
 * 
 * Get plugin so.
 * 
 * Returns: the plugin so
 * 
 * Since: 3.1.0
 */
gpointer
ags_base_plugin_get_plugin_so(AgsBasePlugin *base_plugin)
{
  gpointer plugin_so;
  
  if(!AGS_IS_BASE_PLUGIN(base_plugin)){
    return(NULL);
  }

  g_object_get(base_plugin,
	       "plugin-so", &plugin_so,
	       NULL);

  return(plugin_so);
}

/**
 * ags_base_plugin_set_plugin_so:
 * @base_plugin: the #AgsBasePlugin
 * @plugin_so: the plugin so
 * 
 * Set plugin_so.
 * 
 * Since: 3.1.0
 */
void
ags_base_plugin_set_plugin_so(AgsBasePlugin *base_plugin,
			      gpointer plugin_so)
{
  if(!AGS_IS_BASE_PLUGIN(base_plugin)){
    return;
  }

  g_object_set(base_plugin,
	       "plugin-so", plugin_so,
	       NULL);
}

/**
 * ags_base_plugin_get_base_plugin_dependency:
 * @base_plugin: the #AgsBasePlugin
 * 
 * Get recall dependency.
 * 
 * Returns: (element-type AgsAudio.PluginPort) (transfer full): the #GList-struct containig #AgsPluginPort
 * 
 * Since: 3.1.0
 */
GList*
ags_base_plugin_get_plugin_port(AgsBasePlugin *base_plugin)
{
  GList *plugin_port;

  if(!AGS_IS_BASE_PLUGIN(base_plugin)){
    return(NULL);
  }

  g_object_get(base_plugin,
	       "recall-dependency", &plugin_port,
	       NULL);

  return(plugin_port);
}

/**
 * ags_base_plugin_set_plugin_port:
 * @base_plugin: the #AgsBasePlugin
 * @plugin_port: (element-type AgsAudio.PluginPort) (transfer full): the #GList-struct containing #AgsPluginPort
 * 
 * Set recall dependency by replacing existing.
 * 
 * Since: 3.1.0
 */
void
ags_base_plugin_set_plugin_port(AgsBasePlugin *base_plugin,
				GList *plugin_port)
{
  GList *start_plugin_port;
  
  GRecMutex *base_plugin_mutex;

  if(!AGS_IS_BASE_PLUGIN(base_plugin)){
    return;
  }

  /* get base_plugin mutex */
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(base_plugin);
    
  g_rec_mutex_lock(base_plugin_mutex);

  start_plugin_port = base_plugin->plugin_port;
  base_plugin->plugin_port = plugin_port;
  
  g_rec_mutex_unlock(base_plugin_mutex);

  g_list_free_full(start_plugin_port,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_base_plugin_get_ui_filename:
 * @base_plugin: the #AgsBasePlugin
 * 
 * Get UI filename.
 * 
 * Returns: the UI filename
 * 
 * Since: 3.1.0
 */
gchar*
ags_base_plugin_get_ui_filename(AgsBasePlugin *base_plugin)
{
  gchar *ui_filename;

  if(!AGS_IS_BASE_PLUGIN(base_plugin)){
    return(NULL);
  }

  g_object_get(base_plugin,
	       "ui-filename", &ui_filename,
	       NULL);

  return(ui_filename);
}

/**
 * ags_base_plugin_set_ui_filename:
 * @base_plugin: the #AgsBasePlugin
 * @ui_filename: the UI filename
 * 
 * Set UI filename.
 * 
 * Since: 3.1.0
 */
void
ags_base_plugin_set_ui_filename(AgsBasePlugin *base_plugin,
				gchar *ui_filename)
{
  if(!AGS_IS_BASE_PLUGIN(base_plugin)){
    return;
  }

  g_object_set(base_plugin,
	       "ui-filename", ui_filename,
	       NULL);
}

/**
 * ags_base_plugin_get_ui_effect:
 * @base_plugin: the #AgsBasePlugin
 * 
 * Get UI effect.
 * 
 * Returns: the UI effect
 * 
 * Since: 3.1.0
 */
gchar*
ags_base_plugin_get_ui_effect(AgsBasePlugin *base_plugin)
{
  gchar *ui_effect;

  if(!AGS_IS_BASE_PLUGIN(base_plugin)){
    return(NULL);
  }

  g_object_get(base_plugin,
	       "ui-effect", &ui_effect,
	       NULL);

  return(ui_effect);
}

/**
 * ags_base_plugin_set_ui_effect:
 * @base_plugin: the #AgsBasePlugin
 * @ui_effect: the UI effect
 * 
 * Set UI effect.
 * 
 * Since: 3.1.0
 */
void
ags_base_plugin_set_ui_effect(AgsBasePlugin *base_plugin,
			      gchar *ui_effect)
{
  if(!AGS_IS_BASE_PLUGIN(base_plugin)){
    return;
  }

  g_object_set(base_plugin,
	       "ui-effect", ui_effect,
	       NULL);
}

/**
 * ags_base_plugin_get_ui_effect_index:
 * @base_plugin: the #AgsBasePlugin
 * 
 * Get UI effect index.
 * 
 * Returns: the UI effect index
 * 
 * Since: 3.1.0
 */
guint
ags_base_plugin_get_ui_effect_index(AgsBasePlugin *base_plugin)
{
  guint ui_effect_index;

  if(!AGS_IS_BASE_PLUGIN(base_plugin)){
    return(NULL);
  }

  g_object_get(base_plugin,
	       "ui-effect-index", &ui_effect_index,
	       NULL);

  return(ui_effect_index);
}

/**
 * ags_base_plugin_set_ui_effect_index:
 * @base_plugin: the #AgsBasePlugin
 * @ui_effect_index: the UI effect index
 * 
 * Set UI effect index.
 * 
 * Since: 3.1.0
 */
void
ags_base_plugin_set_ui_effect_index(AgsBasePlugin *base_plugin,
				    guint ui_effect_index)
{
  if(!AGS_IS_BASE_PLUGIN(base_plugin)){
    return;
  }

  g_object_set(base_plugin,
	       "ui-effect-index", ui_effect_index,
	       NULL);
}

/**
 * ags_base_plugin_get_ui_plugin_so:
 * @base_plugin: the #AgsBasePlugin
 * 
 * Get plugin so.
 * 
 * Returns: the plugin so
 * 
 * Since: 3.1.0
 */
gpointer
ags_base_plugin_get_ui_plugin_so(AgsBasePlugin *base_plugin)
{
  gpointer ui_plugin_so;
  
  if(!AGS_IS_BASE_PLUGIN(base_plugin)){
    return(NULL);
  }

  g_object_get(base_plugin,
	       "ui-plugin-so", &ui_plugin_so,
	       NULL);

  return(ui_plugin_so);
}

/**
 * ags_base_plugin_set_ui_plugin_so:
 * @base_plugin: the #AgsBasePlugin
 * @ui_plugin_so: the plugin so
 * 
 * Set ui_plugin_so.
 * 
 * Since: 3.1.0
 */
void
ags_base_plugin_set_ui_plugin_so(AgsBasePlugin *base_plugin,
			      gpointer ui_plugin_so)
{
  if(!AGS_IS_BASE_PLUGIN(base_plugin)){
    return;
  }

  g_object_set(base_plugin,
	       "ui-plugin-so", ui_plugin_so,
	       NULL);
}

/**
 * ags_base_plugin_find_filename:
 * @base_plugin: (element-type AgsAudio.BasePlugin): the #GList-struct containing #AgsBasePlugin
 * @filename: the filename as string
 * 
 * Find filename in @base_plugin #GList-struct of #AgsBasePlugin.
 *
 * Returns: (element-type AgsAudio.BasePlugin) (transfer none): the next matching #GList-struct
 * 
 * Since: 3.0.0
 */
GList*
ags_base_plugin_find_filename(GList *base_plugin, gchar *filename)
{
  AgsBasePlugin *current_base_plugin;
  
  gboolean success;
  
  GRecMutex *base_plugin_mutex;

  if(filename == NULL){
    return(NULL);
  }

  while(base_plugin != NULL){
    current_base_plugin = AGS_BASE_PLUGIN(base_plugin->data);
    
    /* get base plugin mutex */
    base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(current_base_plugin);

    /* check filename */
    g_rec_mutex_lock(base_plugin_mutex);

    success = (current_base_plugin->filename != NULL &&
	       !g_ascii_strcasecmp(current_base_plugin->filename,
				   filename)) ? TRUE: FALSE;
    
    g_rec_mutex_unlock(base_plugin_mutex);

    if(success){
      return(base_plugin);
    }

    base_plugin = base_plugin->next;
  }

  return(NULL);
}

/**
 * ags_base_plugin_find_effect:
 * @base_plugin: (element-type AgsAudio.BasePlugin): the #GList-struct containing #AgsBasePlugin
 * @filename: the filename as string
 * @effect: the effect as string
 * 
 * Find @filename and @effect in @base_plugin #GList-struct of #AgsBasePlugin.
 *
 * Returns: (element-type AgsAudio.BasePlugin) (transfer none): the next matching #GList-struct
 * 
 * Since: 3.0.0
 */
GList*
ags_base_plugin_find_effect(GList *base_plugin, gchar *filename, gchar *effect)
{
  AgsBasePlugin *current_base_plugin;
  
  gboolean success;
  
  GRecMutex *base_plugin_mutex;

  while(base_plugin != NULL){
    current_base_plugin = AGS_BASE_PLUGIN(base_plugin->data);
    
    /* get base plugin mutex */
    base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(current_base_plugin);

    /* check filename and effect*/
    g_rec_mutex_lock(base_plugin_mutex);

    success = (!g_ascii_strcasecmp(current_base_plugin->filename,
				   filename) &&
	       !g_ascii_strcasecmp(current_base_plugin->effect,
				   effect)) ? TRUE: FALSE;
    
    g_rec_mutex_unlock(base_plugin_mutex);

    if(success){
      return(base_plugin);
    }

    base_plugin = base_plugin->next;
  }

  return(NULL);
}

/**
 * ags_base_plugin_find_ui_effect_index:
 * @base_plugin: (element-type AgsAudio.BasePlugin): the #GList-struct containing #AgsBasePlugin
 * @ui_filename: the UI filename as string
 * @ui_effect_index: the UI effect index
 * 
 * Find @ui_filename and @ui_effect_index in @base_plugin #GList-struct of #AgsBasePlugin.
 *
 * Returns: (element-type AgsAudio.BasePlugin) (transfer none): the next matching #GList-struct
 * 
 * Since: 3.0.0
 */
GList*
ags_base_plugin_find_ui_effect_index(GList *base_plugin, gchar *ui_filename, guint ui_effect_index)
{
  AgsBasePlugin *current_base_plugin;
  
  gboolean success;
  
  GRecMutex *base_plugin_mutex;

  while(base_plugin != NULL){
    current_base_plugin = AGS_BASE_PLUGIN(base_plugin->data);
    
    /* get base plugin mutex */
    base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(current_base_plugin);

    /* check UI filename and effect index */
    g_rec_mutex_lock(base_plugin_mutex);

    success = (!g_ascii_strcasecmp(current_base_plugin->ui_filename,
				   ui_filename) &&
	       current_base_plugin->ui_effect_index == ui_effect_index) ? TRUE: FALSE;
    
    g_rec_mutex_unlock(base_plugin_mutex);

    if(success){
      return(base_plugin);
    }

    base_plugin = base_plugin->next;
  }

  return(NULL);
}

/**
 * ags_base_plugin_sort_func:
 * @a: a #AgsBasePlugin
 * @b: an other #AgsBasePlugin
 *
 * Sort function.
 * 
 * Returns: a value smaller to, equal to or greater to 0 like strcmp
 *
 * Since: 3.0.0
 */
gint
ags_base_plugin_sort_func(gpointer a, gpointer b)
{
  AgsBasePlugin *a_plugin;
  AgsBasePlugin *b_plugin;
    
  gchar *a_effect;
  gchar *b_effect;

  gint retval;
    
  GRecMutex *a_plugin_mutex;
  GRecMutex *b_plugin_mutex;

  /* a and b */
  a_plugin = AGS_BASE_PLUGIN(a);
  b_plugin = AGS_BASE_PLUGIN(b);
    
  /* get base plugin mutex - a and b */
  a_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(a_plugin);
  b_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(b_plugin);

  /* duplicate effect - a */
  g_rec_mutex_lock(a_plugin_mutex);

  a_effect = g_strdup(a_plugin->effect);
    
  g_rec_mutex_unlock(a_plugin_mutex);

  /* duplicate effect - b */
  g_rec_mutex_lock(b_plugin_mutex);

  b_effect = g_strdup(b_plugin->effect);
    
  g_rec_mutex_unlock(b_plugin_mutex);

  if(a_effect == NULL ||
     b_effect == 0){
    return(-1);
  }
    
  /* compare and free */
  retval = strcmp(a_effect,
		  b_effect);

  g_free(a_effect);
  g_free(b_effect);
    
  return(retval);
}

/**
 * ags_base_plugin_sort:
 * @base_plugin: (element-type AgsAudio.BasePlugin) (transfer none): the #GList-struct containing #AgsBasePlugin
 *
 * Sort @base_plugin alphabetically.
 * 
 * Returns: (element-type AgsAudio.BasePlugin) (transfer none): the sorted #GList-struct
 * 
 * Since: 3.0.0
 */
GList*
ags_base_plugin_sort(GList *base_plugin)
{  
  GList *start;
  
  if(base_plugin == NULL){
    return(NULL);
  }
  
  start = NULL;

  while(base_plugin != NULL){
    start = g_list_insert_sorted(start,
				 base_plugin->data,
				 (GCompareFunc) ags_base_plugin_sort_func);

    base_plugin = base_plugin->next;
  }

  return(start);
}

void
ags_base_plugin_apply_port_group_by_prefix(AgsBasePlugin *base_plugin)
{
  //TODO:JK: implement me
}

/**
 * ags_base_plugin_instantiate:
 * @base_plugin: the #AgsBasePlugin
 * @samplerate: the samplerate
 * @buffer_size: the fixed buffer size
 *
 * Instantiate the plugin
 *
 * Returns: the new plugin instance handle
 *
 * Since: 3.0.0
 */
gpointer
ags_base_plugin_instantiate(AgsBasePlugin *base_plugin,
			    guint samplerate, guint buffer_size)
{
  gpointer retval;
  
  g_return_val_if_fail(AGS_IS_BASE_PLUGIN(base_plugin),
		       NULL);
  
  g_object_ref(G_OBJECT(base_plugin));
  g_signal_emit(G_OBJECT(base_plugin),
		base_plugin_signals[INSTANTIATE], 0,
		samplerate, buffer_size,
		&retval);
  g_object_unref(G_OBJECT(base_plugin));

  return(retval);
}

/**
 * ags_base_plugin_instantiate_with_params:
 * @base_plugin: the #AgsBasePlugin
 * @n_params: guint pointer to parameter count
 * @parameter_name: string vector containing parameter names
 * @value: the #GValue-struct array
 *
 * Instantiate the plugin
 *
 * Returns: the new plugin instance handle
 *
 * Since: 3.0.0
 */
gpointer
ags_base_plugin_instantiate_with_params(AgsBasePlugin *base_plugin,
					guint *n_params,
					gchar ***parameter_name,
					GValue **value)
{
  gpointer retval;
  
  g_return_val_if_fail(AGS_IS_BASE_PLUGIN(base_plugin),
		       NULL);
  
  g_object_ref(G_OBJECT(base_plugin));
  g_signal_emit(G_OBJECT(base_plugin),
		base_plugin_signals[INSTANTIATE_WITH_PARAMS], 0,
		n_params, parameter_name, value,
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
 * Since: 3.0.0
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
 * Since: 3.0.0
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
 * Since: 3.0.0
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
 * ags_base_plugin_run:
 * @base_plugin: the #AgsBasePlugin
 * @plugin_handle: the plugin instance handle
 * @seq_event: (type gpointer) (transfer none): the alsa sequencer events
 * @frame_count: the frame counts
 *
 * Deactivat a plugin instance
 *
 * Since: 3.0.0
 */
void
ags_base_plugin_run(AgsBasePlugin *base_plugin,
		    gpointer plugin_handle,
		    snd_seq_event_t *seq_event,
		    guint frame_count)
{
  g_return_if_fail(AGS_IS_BASE_PLUGIN(base_plugin));
  g_object_ref(G_OBJECT(base_plugin));
  g_signal_emit(G_OBJECT(base_plugin),
		base_plugin_signals[RUN], 0,
		plugin_handle,
		seq_event,
		frame_count);
  g_object_unref(G_OBJECT(base_plugin));
}


/**
 * ags_base_plugin_load_plugin:
 * @base_plugin: the #AgsBasePlugin
 *
 * Load the plugin
 *
 * Since: 3.0.0
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
 * Since: 3.0.0
 */
AgsBasePlugin*
ags_base_plugin_new(gchar *filename, gchar *effect, guint effect_index)
{
  AgsBasePlugin *base_plugin;

  base_plugin = (AgsBasePlugin *) g_object_new(AGS_TYPE_BASE_PLUGIN,
					       "filename", filename,
					       "effect", effect,
					       "effect-index", effect_index,
					       NULL);

  return(base_plugin);
}
