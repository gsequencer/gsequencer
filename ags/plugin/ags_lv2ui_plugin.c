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
 */

#include <ags/plugin/ags_lv2ui_plugin.h>

#include <ags/lib/ags_string_util.h>

#include <ags/plugin/ags_lv2_plugin.h>
#include <ags/plugin/ags_lv2_event_manager.h>
#include <ags/plugin/ags_lv2_uri_map_manager.h>
#include <ags/plugin/ags_lv2_log_manager.h>
#include <ags/plugin/ags_lv2_urid_manager.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <math.h>

#include <lv2.h>
#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>
#include <lv2/lv2plug.in/ns/ext/instance-access/instance-access.h>

#include <ags/i18n.h>

void ags_lv2ui_plugin_class_init(AgsLv2uiPluginClass *lv2ui_plugin);
void ags_lv2ui_plugin_init (AgsLv2uiPlugin *lv2ui_plugin);
void ags_lv2ui_plugin_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_lv2ui_plugin_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_lv2ui_plugin_dispose(GObject *gobject);
void ags_lv2ui_plugin_finalize(GObject *gobject);

gpointer ags_lv2ui_plugin_instantiate_with_params(AgsBasePlugin *base_plugin,
						  guint *n_params,
						  gchar ***parameter_name,
						  GValue **value);

void ags_lv2ui_plugin_connect_port(AgsBasePlugin *base_plugin,
				   gpointer plugin_handle,
				   guint port_index,
				   gpointer data_location);
void ags_lv2ui_plugin_activate(AgsBasePlugin *base_plugin,
			       gpointer plugin_handle);
void ags_lv2ui_plugin_deactivate(AgsBasePlugin *base_plugin,
				 gpointer plugin_handle);
void ags_lv2ui_plugin_run(AgsBasePlugin *base_plugin,
			  gpointer plugin_handle,
			  snd_seq_event_t *seq_event,
			  guint frame_count);
void ags_lv2ui_plugin_load_plugin(AgsBasePlugin *base_plugin);

/**
 * SECTION:ags_lv2ui_plugin
 * @short_description: The lv2ui plugin class
 * @title: AgsLv2uiPlugin
 * @section_id:
 * @include: ags/plugin/ags_lv2ui_plugin.h
 *
 * The #AgsLv2uiPlugin loads/unloads a Lv2ui plugin.
 */

enum{
  PROP_0,
  PROP_GUI_FILENAME,
  PROP_GUI_URI,
  PROP_MANIFEST,
  PROP_GUI_TURTLE,
  PROP_LV2_PLUGIN,
};

static gpointer ags_lv2ui_plugin_parent_class = NULL;

GType
ags_lv2ui_plugin_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_lv2ui_plugin = 0;

    static const GTypeInfo ags_lv2ui_plugin_info = {
      sizeof (AgsLv2uiPluginClass),
      NULL, /* lv2ui_init */
      NULL, /* lv2ui_finalize */
      (GClassInitFunc) ags_lv2ui_plugin_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLv2uiPlugin),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_lv2ui_plugin_init,
    };

    ags_type_lv2ui_plugin = g_type_register_static(AGS_TYPE_BASE_PLUGIN,
						   "AgsLv2uiPlugin",
						   &ags_lv2ui_plugin_info,
						   0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_lv2ui_plugin);
  }

  return g_define_type_id__volatile;
}

void
ags_lv2ui_plugin_class_init(AgsLv2uiPluginClass *lv2ui_plugin)
{
  AgsBasePluginClass *base_plugin;

  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_lv2ui_plugin_parent_class = g_type_class_peek_parent(lv2ui_plugin);

  /* GObjectClass */
  gobject = (GObjectClass *) lv2ui_plugin;

  gobject->set_property = ags_lv2ui_plugin_set_property;
  gobject->get_property = ags_lv2ui_plugin_get_property;

  gobject->dispose = ags_lv2ui_plugin_dispose;
  gobject->finalize = ags_lv2ui_plugin_finalize;

  /* properties */
  /**
   * AgsLv2uiPlugin:gui-uri:
   *
   * The assigned GUI URI.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("gui-uri",
				   i18n_pspec("GUI URI of the plugin"),
				   i18n_pspec("The GUI URI this plugin is located in"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_GUI_URI,
				  param_spec);
  
  /**
   * AgsLv2Plugin:manifest:
   *
   * The assigned manifest.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("manifest",
				   i18n_pspec("manifest of the plugin"),
				   i18n_pspec("The manifest this plugin is located in"),
				   AGS_TYPE_TURTLE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MANIFEST,
				  param_spec);

  /**
   * AgsLv2uiPlugin:gui-turtle:
   *
   * The assigned GUI turtle.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("gui-turtle",
				   i18n_pspec("GUI turtle of the plugin"),
				   i18n_pspec("The GUI turtle this plugin is located in"),
				   AGS_TYPE_TURTLE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_GUI_TURTLE,
				  param_spec);
    
  /**
   * AgsLv2uiPlugin:lv2-plugin:
   *
   * The assigned LV2 plugin.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("lv2-plugin",
				   i18n_pspec("LV2 plugin of the plugin"),
				   i18n_pspec("The LV2 plugin this plugin is located in"),
				   AGS_TYPE_LV2_PLUGIN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LV2_PLUGIN,
				  param_spec);

  /* AgsBasePluginClass */
  base_plugin = (AgsBasePluginClass *) lv2ui_plugin;

  base_plugin->instantiate_with_params = ags_lv2ui_plugin_instantiate_with_params;
  base_plugin->load_plugin = ags_lv2ui_plugin_load_plugin;
}

void
ags_lv2ui_plugin_init(AgsLv2uiPlugin *lv2ui_plugin)
{
  lv2ui_plugin->flags = 0;

  lv2ui_plugin->gui_uri = NULL;

  lv2ui_plugin->manifest = NULL;
  lv2ui_plugin->gui_turtle = NULL;

  lv2ui_plugin->lv2_plugin = NULL;

  lv2ui_plugin->feature = NULL;
}

void
ags_lv2ui_plugin_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsLv2uiPlugin *lv2ui_plugin;

  pthread_mutex_t *base_plugin_mutex;

  lv2ui_plugin = AGS_LV2UI_PLUGIN(gobject);

  /* get base plugin mutex */
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(lv2ui_plugin);

  switch(prop_id){
  case PROP_GUI_URI:
    {
      gchar *gui_uri;

      gui_uri = (gchar *) g_value_get_string(value);

      pthread_mutex_lock(base_plugin_mutex);

      if(lv2ui_plugin->gui_uri == gui_uri){
	pthread_mutex_unlock(base_plugin_mutex);

	return;
      }
      
      if(lv2ui_plugin->gui_uri != NULL){
	g_free(lv2ui_plugin->gui_uri);
      }

      lv2ui_plugin->gui_uri = g_strdup(gui_uri);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_MANIFEST:
    {
      AgsTurtle *manifest;

      manifest = (AgsTurtle *) g_value_get_object(value);

      pthread_mutex_lock(base_plugin_mutex);

      if(lv2ui_plugin->manifest == manifest){
	pthread_mutex_unlock(base_plugin_mutex);

	return;
      }

      if(lv2ui_plugin->manifest != NULL){
	g_object_unref(lv2ui_plugin->manifest);
      }

      if(manifest != NULL){
	g_object_ref(manifest);
      }
      
      lv2ui_plugin->manifest = manifest;

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_GUI_TURTLE:
    {
      AgsTurtle *gui_turtle;

      gui_turtle = (AgsTurtle *) g_value_get_object(value);

      pthread_mutex_lock(base_plugin_mutex);

      if(lv2ui_plugin->gui_turtle == gui_turtle){
	pthread_mutex_unlock(base_plugin_mutex);

	return;
      }

      if(lv2ui_plugin->gui_turtle != NULL){
	g_object_unref(lv2ui_plugin->gui_turtle);
      }

      if(gui_turtle != NULL){
	g_object_ref(gui_turtle);
      }
      
      lv2ui_plugin->gui_turtle = gui_turtle;

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_LV2_PLUGIN:
    {
      GObject *lv2_plugin;

      lv2_plugin = (GObject *) g_value_get_object(value);

      pthread_mutex_lock(base_plugin_mutex);

      if(lv2ui_plugin->lv2_plugin == lv2_plugin){
	pthread_mutex_unlock(base_plugin_mutex);

	return;
      }

      if(lv2ui_plugin->lv2_plugin != NULL){
	g_object_unref(lv2ui_plugin->lv2_plugin);
      }

      if(lv2_plugin != NULL){
	g_object_ref(lv2_plugin);
      }
      
      lv2ui_plugin->lv2_plugin = lv2_plugin;

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_lv2ui_plugin_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsLv2uiPlugin *lv2ui_plugin;

  pthread_mutex_t *base_plugin_mutex;

  lv2ui_plugin = AGS_LV2UI_PLUGIN(gobject);

  /* get base plugin mutex */
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(lv2ui_plugin);

  switch(prop_id){
  case PROP_GUI_URI:
    {
      pthread_mutex_lock(base_plugin_mutex);

      g_value_set_string(value, lv2ui_plugin->gui_uri);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_MANIFEST:
    {
      pthread_mutex_lock(base_plugin_mutex);

      g_value_set_object(value, lv2ui_plugin->manifest);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_GUI_TURTLE:
    {
      pthread_mutex_lock(base_plugin_mutex);

      g_value_set_object(value, lv2ui_plugin->gui_turtle);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_LV2_PLUGIN:
    {
      pthread_mutex_lock(base_plugin_mutex);

      g_value_set_object(value, lv2ui_plugin->lv2_plugin);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_lv2ui_plugin_dispose(GObject *gobject)
{
  AgsLv2uiPlugin *lv2ui_plugin;

  lv2ui_plugin = AGS_LV2UI_PLUGIN(gobject);

  if(lv2ui_plugin->manifest != NULL){
    g_object_unref(lv2ui_plugin->manifest);

    lv2ui_plugin->manifest = NULL;
  }

  if(lv2ui_plugin->gui_turtle != NULL){
    g_object_unref(lv2ui_plugin->gui_turtle);

    lv2ui_plugin->gui_turtle = NULL;
  }

  if(lv2ui_plugin->lv2_plugin != NULL){
    g_object_unref(lv2ui_plugin->lv2_plugin);

    lv2ui_plugin->lv2_plugin = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_lv2ui_plugin_parent_class)->dispose(gobject);
}

void
ags_lv2ui_plugin_finalize(GObject *gobject)
{
  AgsLv2uiPlugin *lv2ui_plugin;

  lv2ui_plugin = AGS_LV2UI_PLUGIN(gobject);

  g_free(lv2ui_plugin->gui_uri);

  if(lv2ui_plugin->manifest != NULL){
    g_object_unref(lv2ui_plugin->manifest);
  }

  if(lv2ui_plugin->gui_turtle != NULL){
    g_object_unref(lv2ui_plugin->gui_turtle);
  }

  if(lv2ui_plugin->lv2_plugin != NULL){
    g_object_unref(lv2ui_plugin->lv2_plugin);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_lv2ui_plugin_parent_class)->finalize(gobject);
}

void
ags_lv2ui_plugin_load_plugin(AgsBasePlugin *base_plugin)
{
  //NOTE:JK: deprecated  
}

gpointer
ags_lv2ui_plugin_instantiate_with_params(AgsBasePlugin *base_plugin,
					 guint *n_params,
					 gchar ***parameter_name,
					 GValue **value)
{
  AgsLv2uiPlugin *lv2ui_plugin;
    
  LV2UI_Controller controller;
  LV2UI_Widget widget;
  
  void *ui_plugin_so;
  LV2UI_DescriptorFunction lv2ui_descriptor;
  LV2UI_Descriptor *plugin_descriptor;    

  LV2_Handle *lv2_handle;
  LV2UI_Handle *ui_handle;
    
  LV2_URI_Map_Feature *uri_map_feature;
  
  LV2_Log_Log *log_feature;

  LV2_Event_Feature *event_feature;

  LV2_URID_Map *urid_map;
  LV2_URID_Unmap *urid_unmap;

  LV2UI_Write_Function write_function;
  
  LV2_Feature **feature;

  gchar *uri;
  gchar *filename;
  char *path;  
  gchar *str;
  
  float *ptr_samplerate;
  float *ptr_buffer_size;

  uint32_t ui_effect_index;
  guint local_n_params;
  guint total_feature;
  guint nth;
  guint i;
  gboolean initial_call;

  LV2UI_Handle (*instantiate)(const struct _LV2UI_Descriptor* descriptor,
			      const char *plugin_uri,
			      const char *bundle_path,
			      LV2UI_Write_Function write_function,
			      LV2UI_Controller controller,
			      LV2UI_Widget *widget,
			      const LV2_Feature *const *features);
  
  pthread_mutex_t *base_plugin_mutex;
  
  lv2ui_plugin = AGS_LV2UI_PLUGIN(base_plugin);
  
  /* get base plugin mutex */
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(base_plugin);

  local_n_params = 0;
  
  if(n_params == NULL){
    n_params = &local_n_params;
  }

  /* get some fields */
  pthread_mutex_lock(base_plugin_mutex);

  ui_plugin_so = base_plugin->ui_plugin_so;
  ui_effect_index = 0;

  if(ui_plugin_so == NULL){
    g_message("open %s", base_plugin->ui_filename);
    
    ui_plugin_so = dlopen(base_plugin->ui_filename,
			  RTLD_NOW);
    
    g_object_set(lv2ui_plugin,
		 "ui-plugin-so", ui_plugin_so,
		 NULL);

    base_plugin->ui_plugin_handle = 
      lv2ui_descriptor = (LV2UI_DescriptorFunction) dlsym(ui_plugin_so,
							  "lv2ui_descriptor");

    if(dlerror() == NULL && lv2_descriptor){
      for(i = 0; (plugin_descriptor = lv2ui_descriptor((uint32_t) i)) != NULL; i++){
	if(!g_ascii_strcasecmp(plugin_descriptor->URI,
			       lv2ui_plugin->gui_uri)){
	  base_plugin->ui_plugin_descriptor = plugin_descriptor;
	  ui_effect_index = i;

	  g_object_set(lv2ui_plugin,
		       "ui-effect-index", ui_effect_index,
		       NULL);
	   
	  break;
	}
      }
    }
  }
  
  lv2ui_descriptor = base_plugin->ui_plugin_handle;
  plugin_descriptor = base_plugin->ui_plugin_descriptor;

  feature = lv2ui_plugin->feature;
  
  path = g_path_get_dirname(base_plugin->ui_filename);

  ui_effect_index = base_plugin->ui_effect_index;
  
  pthread_mutex_unlock(base_plugin_mutex);

  if(ui_plugin_so == NULL){
    g_free(path);
    
    return(NULL);
  }

  /* check args */
  uri = NULL;

  controller = NULL;  
  widget = NULL;

  write_function = NULL;

  lv2_handle = NULL;
  
  if(n_params != NULL &&
     parameter_name != NULL &&
     parameter_name[0] != NULL &&
     value != NULL &&
     value[0] != NULL){
    for(i = 0; i < n_params[0] && parameter_name[0][i] != NULL; i++){
      if(!g_ascii_strncasecmp(parameter_name[0][i],
			      "uri",
			      4)){
	uri = g_value_get_pointer(&(value[0][i]));
      }else if(!g_ascii_strncasecmp(parameter_name[0][i],
			      "controller",
			      11)){
	controller = g_value_get_pointer(&(value[0][i]));
      }else if(!g_ascii_strncasecmp(parameter_name[0][i],
				    "write-function",
				    15)){
	write_function = g_value_get_pointer(&(value[0][i]));
      }else if(!g_ascii_strncasecmp(parameter_name[0][i],
				    "instance",
				    9)){
	lv2_handle = g_value_get_pointer(&(value[0][i]));
      }
    }
  }
  
  /* features */
  initial_call = FALSE;

  if(feature == NULL){    
    initial_call = TRUE;

    /**/
    total_feature = 9;
    nth = 0;  

    feature = (LV2_Feature **) malloc(total_feature * sizeof(LV2_Feature *));
    
    /* idle interface */
    feature[nth] = (LV2_Feature *) malloc(sizeof(LV2_Feature));    
    feature[nth]->URI = LV2_UI__idleInterface;
    feature[nth]->data = NULL;

    nth++;
      
    /* show interface */
    feature[nth] = (LV2_Feature *) malloc(sizeof(LV2_Feature));
    feature[nth]->URI = LV2_UI__showInterface;
    feature[nth]->data = NULL;

    nth++;  
  
    /* instance access */
    feature[nth] = (LV2_Feature *) malloc(sizeof(LV2_Feature));
    feature[nth]->URI = LV2_INSTANCE_ACCESS_URI;
    feature[nth]->data = NULL;

    if(lv2_handle != NULL){
      feature[nth]->data = lv2_handle[0];
    }
    
    nth++;  

    /* URI map feature */  
    uri_map_feature = (LV2_URI_Map_Feature *) malloc(sizeof(LV2_URI_Map_Feature));
    uri_map_feature->callback_data = NULL;
    uri_map_feature->uri_to_id = ags_lv2_uri_map_manager_uri_to_id;
  
    feature[nth] = (LV2_Feature *) malloc(sizeof(LV2_Feature));
    feature[nth]->URI = LV2_URI_MAP_URI;
    feature[nth]->data = uri_map_feature;

    nth++;
  
    /* log feature */
    log_feature = (LV2_Log_Log *) malloc(sizeof(LV2_Log_Log));
  
    log_feature->handle = NULL;
    log_feature->printf = ags_lv2_log_manager_printf;
    log_feature->vprintf = ags_lv2_log_manager_vprintf;

    feature[nth] = (LV2_Feature *) malloc(sizeof(LV2_Feature));
    feature[nth]->URI = LV2_LOG__log;
    feature[nth]->data = log_feature;

    nth++;
  
    /* event feature */
    event_feature = (LV2_Event_Feature *) malloc(sizeof(LV2_Event_Feature));
  
    event_feature->callback_data = NULL;
    event_feature->lv2_event_ref = ags_lv2_event_manager_lv2_event_ref;
    event_feature->lv2_event_unref = ags_lv2_event_manager_lv2_event_unref;
  
    feature[nth] = (LV2_Feature *) malloc(sizeof(LV2_Feature));
    feature[nth]->URI = LV2_EVENT_URI;
    feature[nth]->data = event_feature;

    nth++;
    
    /* URID map feature */
    urid_map = (LV2_URID_Map *) malloc(sizeof(LV2_URID_Map));
    urid_map->handle = NULL;
    urid_map->map = ags_lv2_urid_manager_map;
  
    feature[nth] = (LV2_Feature *) malloc(sizeof(LV2_Feature));
    feature[nth]->URI = LV2_URID_MAP_URI;
    feature[nth]->data = urid_map;

    nth++;
  
    /* URID unmap feature */
    urid_unmap = (LV2_URID_Unmap *) malloc(sizeof(LV2_URID_Unmap));
    urid_unmap->handle = NULL;
    urid_unmap->unmap = ags_lv2_urid_manager_unmap;
  
    feature[nth] = (LV2_Feature *) malloc(sizeof(LV2_Feature));
    feature[nth]->URI = LV2_URID_UNMAP_URI;
    feature[nth]->data = urid_unmap;

    nth++;

    /* terminate */
    for(; nth < total_feature; nth++){
      feature[nth] = NULL;
    }
  
    pthread_mutex_lock(base_plugin_mutex);

    lv2ui_plugin->feature = feature;
    
    pthread_mutex_unlock(base_plugin_mutex);
  }
  
  instantiate = NULL;
  
  if(plugin_descriptor != NULL){
    pthread_mutex_lock(base_plugin_mutex);
    
    instantiate = plugin_descriptor->instantiate;
      
    pthread_mutex_unlock(base_plugin_mutex);
  }

  /* instantiate */
  ui_handle = (LV2UI_Handle *) malloc(sizeof(LV2UI_Handle));
  
  ui_handle[0] = instantiate(plugin_descriptor,
			     uri,
			     path,
			     write_function,
			     controller,
			     &widget,
			     feature);
  
  g_message("LV2UI handle = %p", ui_handle[0]);
  
  if(parameter_name != NULL &&
     value != NULL){
    if(n_params[0] == 0){
      parameter_name[0] = (gchar **) malloc(2 * sizeof(gchar *));
      value[0] = g_new0(GValue,
			1);
    }else{
      parameter_name[0] = (gchar **) realloc(parameter_name[0],
					     (n_params[0] + 2) * sizeof(gchar *));
      value[0] = g_renew(GValue,
			 value[0],
			 n_params[0] + 1);
    }

    parameter_name[0][n_params[0]] = g_strdup("widget");
    g_value_init(&(value[0][n_params[0]]),
		 G_TYPE_POINTER);
    g_value_set_pointer(&(value[0][n_params[0]]), widget);
  
    parameter_name[0][n_params[0] + 1] = NULL;

    n_params[0] += 1;
  }
  
  return(ui_handle);
}

/**
 * ags_lv2ui_plugin_test_flags:
 * @lv2ui_plugin: the #AgsLv2uiPlugin
 * @flags: the flags
 * 
 * Test @flags to be set on @recall.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 * 
 * Since: 2.2.18
 */
gboolean
ags_lv2ui_plugin_test_flags(AgsLv2uiPlugin *lv2ui_plugin, guint flags)
{
  gboolean retval;
  
  pthread_mutex_t *base_plugin_mutex;

  if(!AGS_IS_LV2UI_PLUGIN(lv2ui_plugin)){
    return(FALSE);
  }
  
  /* get base plugin mutex */
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(lv2ui_plugin);

  /* test flags */
  pthread_mutex_lock(base_plugin_mutex);

  retval = ((flags & (lv2ui_plugin->flags)) != 0) ? TRUE: FALSE;
  
  pthread_mutex_unlock(base_plugin_mutex);

  return(retval);
}

/**
 * ags_lv2ui_plugin_set_flags:
 * @lv2ui_plugin: the #AgsLv2uiPlugin
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 2.2.18
 */
void
ags_lv2ui_plugin_set_flags(AgsLv2uiPlugin *lv2ui_plugin, guint flags)
{
  pthread_mutex_t *base_plugin_mutex;

  if(!AGS_IS_LV2UI_PLUGIN(lv2ui_plugin)){
    return;
  }
  
  /* get base plugin mutex */
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(lv2ui_plugin);

  /* set flags */
  pthread_mutex_lock(base_plugin_mutex);

  lv2ui_plugin->flags |= flags;
  
  pthread_mutex_unlock(base_plugin_mutex);
}

/**
 * ags_lv2ui_plugin_unset_flags:
 * @lv2ui_plugin: the #AgsLv2uiPlugin
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 2.2.18
 */
void
ags_lv2ui_plugin_unset_flags(AgsLv2uiPlugin *lv2ui_plugin, guint flags)
{
  pthread_mutex_t *base_plugin_mutex;

  if(!AGS_IS_LV2UI_PLUGIN(lv2ui_plugin)){
    return;
  }
  
  /* get base plugin mutex */
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(lv2ui_plugin);

  /* unset flags */
  pthread_mutex_lock(base_plugin_mutex);

  lv2ui_plugin->flags &= (~flags);
  
  pthread_mutex_unlock(base_plugin_mutex);
}

/**
 * ags_lv2ui_plugin_find_gui_uri:
 * @lv2ui_plugin: the #GList-struct containing #AgsLv2uiPlugin
 * @gui_uri: the gui-uri as string
 * 
 * Find next matching gui-uri in @lv2ui_plugin.
 * 
 * Returns: the next matching #GList-struct
 * 
 * Since: 2.0.0
 */
GList*
ags_lv2ui_plugin_find_gui_uri(GList *lv2ui_plugin,
			      gchar *gui_uri)
{
  if(gui_uri == NULL){
    return(NULL);
  }
  
  while(lv2ui_plugin != NULL){
    if(AGS_LV2UI_PLUGIN(lv2ui_plugin->data)->gui_uri != NULL &&
       !g_ascii_strcasecmp(AGS_LV2UI_PLUGIN(lv2ui_plugin->data)->gui_uri,
			   gui_uri)){
      return(lv2ui_plugin);
    }

    lv2ui_plugin = lv2ui_plugin->next;
  }

  return(NULL);
}

/**
 * ags_lv2ui_plugin_new:
 * @gui_turtle: the #AgsTurtle
 * @filename: the plugin .so
 * @effect: the effect's string representation
 * @gui_uri: the effect's gui_uri
 * @effect_index: the effect's index
 *
 * Creates an #AgsLv2uiPlugin
 *
 * Returns: a new #AgsLv2uiPlugin
 *
 * Since: 2.0.0
 */
AgsLv2uiPlugin*
ags_lv2ui_plugin_new(AgsTurtle *gui_turtle,
		     gchar *filename,
		     gchar *effect,
		     gchar *gui_uri,
		     guint effect_index)
{
  AgsLv2uiPlugin *lv2ui_plugin;

  lv2ui_plugin = (AgsLv2uiPlugin *) g_object_new(AGS_TYPE_LV2UI_PLUGIN,
						 "gui-turtle", gui_turtle,
						 "filename", filename,
						 "effect", effect,
						 "gui-uri", gui_uri,
						 "effect-index", effect_index,
						 NULL);

  return(lv2ui_plugin);
}
