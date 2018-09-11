/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

static pthread_mutex_t ags_base_plugin_class_mutex = PTHREAD_MUTEX_INITIALIZER;

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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * AgsBasePlugin:plugin-port:
   *
   * The assigned #GList-struct containing #AgsPluginPort
   * 
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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

  base_plugin->connect_port = NULL;
  
  base_plugin->activate = NULL;
  base_plugin->deactivate = NULL;

  base_plugin->run = NULL;

  base_plugin->load_plugin = NULL;
  
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
   * Since: 2.0.0
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
   * AgsBasePlugin::connect-port:
   * @base_plugin: the plugin to connect-port
   * @plugin_handle: the plugin handle
   * @port_index: the port index
   * @data_location: the data location
   *
   * The ::connect-port signal creates a new instance of plugin.
   * 
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * AgsBasePlugin::load_plugin:
   * @base_plugin: the plugin to load_plugin
   *
   * The ::load_plugin signal creates a new instance of plugin.
   * 
   * Since: 2.0.0
   */
  base_plugin_signals[LOAD_PLUGIN] =
    g_signal_new("load_plugin",
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
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  base_plugin->flags = 0;

  /* add base plugin mutex */
  base_plugin->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  base_plugin->obj_mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

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

  base_plugin->ui_plugin = NULL;
}

void
ags_base_plugin_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsBasePlugin *base_plugin;

  pthread_mutex_t *base_plugin_mutex;

  base_plugin = AGS_BASE_PLUGIN(gobject);

  /* get base plugin mutex */
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
  base_plugin_mutex = base_plugin->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

  switch(prop_id){
  case PROP_UUID:
    {
      pthread_mutex_lock(base_plugin_mutex);

      base_plugin->uuid = (AgsUUID *) g_value_get_pointer(value);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = (gchar *) g_value_get_string(value);

      pthread_mutex_lock(base_plugin_mutex);

      if(base_plugin->filename == filename){
	pthread_mutex_unlock(base_plugin_mutex);
	
	return;
      }
      
      if(base_plugin->filename != NULL){
	g_free(base_plugin->filename);
      }

      base_plugin->filename = g_strdup(filename);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_EFFECT:
    {
      gchar *effect;

      effect = (gchar *) g_value_get_string(value);

      pthread_mutex_lock(base_plugin_mutex);

      if(base_plugin->effect == effect){
	pthread_mutex_unlock(base_plugin_mutex);

	return;
      }
      
      if(base_plugin->effect != NULL){
	g_free(base_plugin->effect);
      }

      base_plugin->effect = g_strdup(effect);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_PLUGIN_PORT:
    {
      AgsPluginPort *plugin_port;

      plugin_port = (AgsPluginPort *) g_value_get_pointer(value);

      pthread_mutex_lock(base_plugin_mutex);

      if(!AGS_IS_PLUGIN_PORT(plugin_port) ||
	 g_list_find(base_plugin->plugin_port, plugin_port) != NULL){
	pthread_mutex_unlock(base_plugin_mutex);
	
	return;
      }

      g_object_ref(plugin_port);
      base_plugin->plugin_port = g_list_prepend(base_plugin->plugin_port,
						plugin_port);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_EFFECT_INDEX:
    {
      guint effect_index;

      effect_index = g_value_get_uint(value);

      pthread_mutex_lock(base_plugin_mutex);

      base_plugin->effect_index = effect_index;

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_PLUGIN_SO:
    {
      pthread_mutex_lock(base_plugin_mutex);

      base_plugin->plugin_so = g_value_get_pointer(value);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_UI_FILENAME:
    {
      gchar *ui_filename;

      ui_filename = (gchar *) g_value_get_string(value);

      pthread_mutex_lock(base_plugin_mutex);

      if(base_plugin->ui_filename == ui_filename){
	pthread_mutex_unlock(base_plugin_mutex);

	return;
      }
      
      if(base_plugin->ui_filename != NULL){
	g_free(base_plugin->ui_filename);
      }

      base_plugin->ui_filename = g_strdup(ui_filename);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_UI_EFFECT:
    {
      gchar *ui_effect;

      ui_effect = (gchar *) g_value_get_string(value);

      pthread_mutex_lock(base_plugin_mutex);

      if(base_plugin->ui_effect == ui_effect){
	pthread_mutex_unlock(base_plugin_mutex);

	return;
      }
      
      if(base_plugin->ui_effect != NULL){
	g_free(base_plugin->ui_effect);
      }

      base_plugin->ui_effect = g_strdup(ui_effect);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_UI_EFFECT_INDEX:
    {
      guint ui_effect_index;

      ui_effect_index = g_value_get_uint(value);

      pthread_mutex_lock(base_plugin_mutex);

      base_plugin->ui_effect_index = ui_effect_index;

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_UI_PLUGIN_SO:
    {
      pthread_mutex_lock(base_plugin_mutex);

      base_plugin->ui_plugin_so = g_value_get_pointer(value);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_UI_PLUGIN:
    {
      GObject *ui_plugin;

      ui_plugin = g_value_get_object(value);

      pthread_mutex_lock(base_plugin_mutex);

      if(base_plugin->ui_plugin == ui_plugin){
	pthread_mutex_unlock(base_plugin_mutex);

	return;
      }

      if(base_plugin->ui_plugin != NULL){
	g_object_unref(base_plugin->ui_plugin);
      }

      if(ui_plugin != NULL){
	g_object_ref(ui_plugin);
      }
      
      base_plugin->ui_plugin = ui_plugin;

      pthread_mutex_unlock(base_plugin_mutex);
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

  pthread_mutex_t *base_plugin_mutex;

  base_plugin = AGS_BASE_PLUGIN(gobject);

  /* get base plugin mutex */
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
  base_plugin_mutex = base_plugin->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

  switch(prop_id){
  case PROP_UUID:
    {
      pthread_mutex_lock(base_plugin_mutex);
      
      g_value_set_pointer(value, base_plugin->uuid);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_FILENAME:
    {
      pthread_mutex_lock(base_plugin_mutex);

      g_value_set_string(value, base_plugin->filename);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_EFFECT:
    {
      pthread_mutex_lock(base_plugin_mutex);

      g_value_set_string(value, base_plugin->effect);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_PLUGIN_PORT:
    {
      pthread_mutex_lock(base_plugin_mutex);
      
      g_value_set_pointer(value, g_list_copy(base_plugin->plugin_port));

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_EFFECT_INDEX:
    {
      pthread_mutex_lock(base_plugin_mutex);

      g_value_set_uint(value, base_plugin->effect_index);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_PLUGIN_SO:
    {
      pthread_mutex_lock(base_plugin_mutex);
      
      g_value_set_pointer(value, base_plugin->plugin_so);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_UI_FILENAME:
    {
      pthread_mutex_lock(base_plugin_mutex);

      g_value_set_string(value, base_plugin->ui_filename);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_UI_EFFECT:
    {
      pthread_mutex_lock(base_plugin_mutex);

      g_value_set_string(value, base_plugin->ui_effect);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_UI_EFFECT_INDEX:
    {
      pthread_mutex_lock(base_plugin_mutex);

      g_value_set_uint(value, base_plugin->ui_effect_index);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_UI_PLUGIN_SO:
    {
      pthread_mutex_lock(base_plugin_mutex);
      
      g_value_set_pointer(value, base_plugin->ui_plugin_so);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_UI_PLUGIN:
    {
      pthread_mutex_lock(base_plugin_mutex);

      g_value_set_object(value, base_plugin->ui_plugin);

      pthread_mutex_unlock(base_plugin_mutex);
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

  /* destroy object mutex */
  pthread_mutex_destroy(base_plugin->obj_mutex);
  free(base_plugin->obj_mutex);

  pthread_mutexattr_destroy(base_plugin->obj_mutexattr);
  free(base_plugin->obj_mutexattr);

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
 * ags_base_plugin_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_base_plugin_get_class_mutex()
{
  return(&ags_base_plugin_class_mutex);
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
 * Since: 2.0.0
 */
gboolean
ags_base_plugin_test_flags(AgsBasePlugin *base_plugin, guint flags)
{
  gboolean retval;
  
  pthread_mutex_t *base_plugin_mutex;

  if(!AGS_IS_BASE_PLUGIN(base_plugin)){
    return(FALSE);
  }
  
  /* get base plugin mutex */
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
  base_plugin_mutex = base_plugin->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

  /* test flags */
  pthread_mutex_lock(base_plugin_mutex);

  retval = ((flags & (base_plugin->flags)) != 0) ? TRUE: FALSE;
  
  pthread_mutex_unlock(base_plugin_mutex);

  return(retval);
}

/**
 * ags_base_plugin_set_flags:
 * @base_plugin: the #AgsBasePlugin
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 2.0.0
 */
void
ags_base_plugin_set_flags(AgsBasePlugin *base_plugin, guint flags)
{
  pthread_mutex_t *base_plugin_mutex;

  if(!AGS_IS_BASE_PLUGIN(base_plugin)){
    return;
  }
  
  /* get base plugin mutex */
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
  base_plugin_mutex = base_plugin->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

  /* set flags */
  pthread_mutex_lock(base_plugin_mutex);

  base_plugin->flags |= flags;
  
  pthread_mutex_unlock(base_plugin_mutex);
}

/**
 * ags_base_plugin_unset_flags:
 * @base_plugin: the #AgsBasePlugin
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 2.0.0
 */
void
ags_base_plugin_unset_flags(AgsBasePlugin *base_plugin, guint flags)
{
  pthread_mutex_t *base_plugin_mutex;

  if(!AGS_IS_BASE_PLUGIN(base_plugin)){
    return;
  }
  
  /* get base plugin mutex */
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
  base_plugin_mutex = base_plugin->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

  /* unset flags */
  pthread_mutex_lock(base_plugin_mutex);

  base_plugin->flags &= (~flags);
  
  pthread_mutex_unlock(base_plugin_mutex);
}

/**
 * ags_base_plugin_find_filename:
 * @base_plugin: the #GList-struct containing #AgsBasePlugin
 * @filename: the filename as string
 * 
 * Find filename in @base_plugin #GList-struct of #AgsBasePlugin.
 *
 * Returns: the next matching #GList-struct
 * 
 * Since: 2.0.0
 */
GList*
ags_base_plugin_find_filename(GList *base_plugin, gchar *filename)
{
  AgsBasePlugin *current_base_plugin;
  
  gboolean success;
  
  pthread_mutex_t *base_plugin_mutex;

  while(base_plugin != NULL){
    current_base_plugin = AGS_BASE_PLUGIN(base_plugin->data);
    
    /* get base plugin mutex */
    pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
    base_plugin_mutex = current_base_plugin->obj_mutex;
  
    pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

    /* check filename */
    pthread_mutex_lock(base_plugin_mutex);

    success = (!g_ascii_strcasecmp(current_base_plugin->filename,
				   filename)) ? TRUE: FALSE;
    
    pthread_mutex_unlock(base_plugin_mutex);

    if(success){
      return(base_plugin);
    }

    base_plugin = base_plugin->next;
  }

  return(NULL);
}

/**
 * ags_base_plugin_find_effect:
 * @base_plugin: the #GList-struct containing #AgsBasePlugin
 * @filename: the filename as string
 * @effect: the effect as string
 * 
 * Find @filename and @effect in @base_plugin #GList-struct of #AgsBasePlugin.
 *
 * Returns: the next matching #GList-struct
 * 
 * Since: 2.0.0
 */
GList*
ags_base_plugin_find_effect(GList *base_plugin, gchar *filename, gchar *effect)
{
  AgsBasePlugin *current_base_plugin;
  
  gboolean success;
  
  pthread_mutex_t *base_plugin_mutex;

  while(base_plugin != NULL){
    current_base_plugin = AGS_BASE_PLUGIN(base_plugin->data);
    
    /* get base plugin mutex */
    pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
    base_plugin_mutex = current_base_plugin->obj_mutex;
  
    pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

    /* check filename and effect*/
    pthread_mutex_lock(base_plugin_mutex);

    success = (!g_ascii_strcasecmp(current_base_plugin->filename,
				   filename) &&
	       !g_ascii_strcasecmp(current_base_plugin->effect,
				   effect)) ? TRUE: FALSE;
    
    pthread_mutex_unlock(base_plugin_mutex);

    if(success){
      return(base_plugin);
    }

    base_plugin = base_plugin->next;
  }

  return(NULL);
}

/**
 * ags_base_plugin_find_ui_effect_index:
 * @base_plugin: the #GList-struct containing #AgsBasePlugin
 * @ui_filename: the UI filename as string
 * @ui_effect_index: the UI effect index
 * 
 * Find @ui_filename and @ui_effect_index in @base_plugin #GList-struct of #AgsBasePlugin.
 *
 * Returns: the next matching #GList-struct
 * 
 * Since: 2.0.0
 */
GList*
ags_base_plugin_find_ui_effect_index(GList *base_plugin, gchar *ui_filename, guint ui_effect_index)
{
  AgsBasePlugin *current_base_plugin;
  
  gboolean success;
  
  pthread_mutex_t *base_plugin_mutex;

  while(base_plugin != NULL){
    current_base_plugin = AGS_BASE_PLUGIN(base_plugin->data);
    
    /* get base plugin mutex */
    pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
    base_plugin_mutex = current_base_plugin->obj_mutex;
  
    pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

    /* check UI filename and effect index */
    pthread_mutex_lock(base_plugin_mutex);

    success = (!g_ascii_strcasecmp(current_base_plugin->ui_filename,
				   ui_filename) &&
	       current_base_plugin->ui_effect_index == ui_effect_index) ? TRUE: FALSE;
    
    pthread_mutex_unlock(base_plugin_mutex);

    if(success){
      return(base_plugin);
    }

    base_plugin = base_plugin->next;
  }

  return(NULL);
}

/**
 * ags_base_plugin_sort:
 * @base_plugin: the #GList-struct containing #AgsBasePlugin
 *
 * Sort @base_plugin alphabetically.
 * 
 * Returns: the sorted #GList-struct
 * 
 * Since: 2.0.0
 */
GList*
ags_base_plugin_sort(GList *base_plugin)
{  
  GList *start;
  
  auto gint ags_base_plugin_sort_compare_function(gpointer a, gpointer b);

  gint ags_base_plugin_sort_compare_function(gpointer a, gpointer b){
    AgsBasePlugin *a_plugin;
    AgsBasePlugin *b_plugin;
    
    gchar *a_effect;
    gchar *b_effect;

    gint retval;
    
    pthread_mutex_t *a_plugin_mutex;
    pthread_mutex_t *b_plugin_mutex;

    /* a and b */
    a_plugin = AGS_BASE_PLUGIN(a);
    b_plugin = AGS_BASE_PLUGIN(b);
    
    /* get base plugin mutex - a and b */
    pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
    a_plugin_mutex = a_plugin->obj_mutex;
    b_plugin_mutex = b_plugin->obj_mutex;
  
    pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

    /* duplicate effect - a */
    pthread_mutex_lock(a_plugin_mutex);

    a_effect = g_strdup(a_plugin->effect);
    
    pthread_mutex_unlock(a_plugin_mutex);

    /* duplicate effect - b */
    pthread_mutex_lock(b_plugin_mutex);

    b_effect = g_strdup(b_plugin->effect);
    
    pthread_mutex_unlock(b_plugin_mutex);

    /* compare and free */
    retval = strcmp(a_effect,
		    b_effect);

    g_free(a_effect);
    g_free(b_effect);
    
    return(retval);
  }

  if(base_plugin == NULL){
    return(NULL);
  }
  
  start = NULL;

  while(base_plugin != NULL){
    start = g_list_insert_sorted(start,
				 base_plugin->data,
				 (GCompareFunc) ags_base_plugin_sort_compare_function);

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
 * Since: 2.0.0
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
 * ags_base_plugin_connect_port:
 * @base_plugin: the #AgsBasePlugin
 * @plugin_handle: the plugin instance handle
 * @port_index: the port's index to connect
 * @data_location: the data location to connect
 *
 * Connect a plugin instance.
 *
 * Since: 2.0.0
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
 * Since: 2.0.0
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
 * Since: 2.0.0
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
 * @seq_event: the alsa sequencer events
 * @frame_count: the frame counts
 *
 * Deactivat a plugin instance
 *
 * Since: 2.0.0
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
 * Since: 2.0.0
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
 * Since: 2.0.0
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
