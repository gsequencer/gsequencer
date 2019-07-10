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

#include <ags/plugin/ags_lv2ui_plugin.h>

#include <ags/lib/ags_string_util.h>

#include <ags/plugin/ags_lv2_plugin.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <math.h>

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
						  gchar **parameter_name,
						  GValue *value);

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
				   "GUI URI of the plugin",
				   "The GUI URI this plugin is located in",
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
				   "manifest of the plugin",
				   "The manifest this plugin is located in",
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
				   "GUI turtle of the plugin",
				   "The GUI turtle this plugin is located in",
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
				   "LV2 plugin of the plugin",
				   "The LV2 plugin this plugin is located in",
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
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
  base_plugin_mutex = AGS_BASE_PLUGIN(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

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
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
  base_plugin_mutex = AGS_BASE_PLUGIN(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

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

gpointer
ags_lv2ui_plugin_instantiate_with_params(AgsBasePlugin *base_plugin,
					 guint *n_params,
					 gchar **parameter_name,
					 GValue *value)
{
  AgsLv2uiPlugin *lv2ui_plugin;
  
  AgsConfig *config;
  
  void *plugin_so;
  LV2UI_DescriptorFunction lv2ui_descriptor;
  LV2UI_Descriptor *plugin_descriptor;    

  LV2UI_Handle *ui_handle;
  
  LV2_URI_Map_Feature *uri_map_feature;
  
  LV2_Worker_Schedule_Handle worker_handle;
  LV2_Worker_Schedule *worker_schedule;

  LV2_Log_Log *log_feature;

  LV2_Event_Feature *event_feature;

  LV2_URID_Map *urid_map;
  LV2_URID_Unmap *urid_unmap;

  LV2_Options_Interface *options_interface;
  LV2_Options_Option *options;
  
  LV2_Feature **feature;

  gchar *filename;
  char *path;  
  gchar *str;
  
  float *ptr_samplerate;
  float *ptr_buffer_size;

  uint32_t ui_effect_index;
  guint conf_buffer_size;
  guint conf_samplerate;
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
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
  base_plugin_mutex = base_plugin->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(base_plugin_mutex);

  plugin_so = base_plugin->plugin_so;
  ui_effect_index = 0;

  if(plugin_so == NULL){
    g_message("open %s", base_plugin->filename);
    
    plugin_so = dlopen(base_plugin->filename,
		       RTLD_NOW);
    
    g_object_set(lv2_plugin,
		 "ui-plugin-so", plugin_so,
		 NULL);

    base_plugin->ui_plugin_handle = 
      lv2ui_descriptor = (LV2_Descriptor_Function) dlsym(plugin_so,
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

  feature = lv2_plugin->feature;
  
  path = g_path_get_dirname(base_plugin->filename);

  ui_effect_index = base_plugin->ui_effect_index;
  
  pthread_mutex_unlock(base_plugin_mutex);

  if(plugin_so == NULL){
    return(NULL);
  }

  /* get some config values */
  initial_call = FALSE;
  
  config = ags_config_get_instance();

  conf_samplerate = ags_soundcard_helper_config_get_samplerate(config);
  conf_buffer_size = ags_soundcard_helper_config_get_buffer_size(config);

  worker_handle = NULL;

  if(feature == NULL){    
    initial_call = TRUE;

    /**/
    total_feature = 8;
    nth = 0;  

    feature = (LV2_Feature **) malloc(total_feature * sizeof(LV2_Feature *));
  
    /* URI map feature */  
    uri_map_feature = (LV2_URI_Map_Feature *) malloc(sizeof(LV2_URI_Map_Feature));
    uri_map_feature->callback_data = NULL;
    uri_map_feature->uri_to_id = ags_lv2_uri_map_manager_uri_to_id;
  
    feature[nth] = (LV2_Feature *) malloc(sizeof(LV2_Feature));
    feature[nth]->URI = LV2_URI_MAP_URI;
    feature[nth]->data = uri_map_feature;

    nth++;
  
    /* worker feature */
    if(ags_lv2_plugin_test_flags(lv2_plugin, AGS_LV2_PLUGIN_NEEDS_WORKER)){
      worker_handle = ags_lv2_worker_manager_pull_worker(ags_lv2_worker_manager_get_instance());
  
      worker_schedule = (LV2_Worker_Schedule *) malloc(sizeof(LV2_Worker_Schedule));
      worker_schedule->handle = worker_handle;
      worker_schedule->schedule_work = ags_lv2_worker_schedule_work;
  
      feature[nth] = (LV2_Feature *) malloc(sizeof(LV2_Feature));
      feature[nth]->URI = LV2_WORKER__schedule;
      feature[nth]->data = worker_schedule;

      nth++;
    }
  
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

    /* Options interface */
    options_interface = (LV2_Options_Interface *) malloc(sizeof(LV2_Options_Interface));
    options_interface->set = ags_lv2_option_manager_lv2_options_set;
    options_interface->get = ags_lv2_option_manager_lv2_options_get;
  
    feature[nth] = (LV2_Feature *) malloc(sizeof(LV2_Feature));
    feature[nth]->URI = LV2_OPTIONS_URI;
    feature[nth]->data = options_interface;

    nth++;

    /* terminate */
    for(; nth < total_feature; nth++){
      feature[nth] = NULL;
    }
  
    pthread_mutex_lock(base_plugin_mutex);

    lv2_plugin->feature = feature;
    
    pthread_mutex_unlock(base_plugin_mutex);
  }
  
  if(plugin_so != NULL){
    lv2_descriptor = (LV2_Descriptor_Function) dlsym(plugin_so,
						     "lv2_descriptor");

    if(dlerror() == NULL && lv2_descriptor){
      pthread_mutex_lock(base_plugin_mutex);
      
      base_plugin->plugin_descriptor = 
	plugin_descriptor = lv2_descriptor(effect_index);

      instantiate = plugin_descriptor->instantiate;
      
      pthread_mutex_unlock(base_plugin_mutex);
    }
  }

  /* alloc handle and path */
  lv2ui_handle = (LV2UI_Handle *) malloc(sizeof(LV2UI_Handle));

  /* instantiate */
  lv2ui_handle[0] = instantiate(plugin_descriptor,
				lv2ui_plugin->gui_uri,
				bundle_path,
				parent_widget,
				&plugin_widget,
				feature);
  
  if(initial_call){
    /* some options */
    options = (LV2_Options_Option *) malloc(6 * sizeof(LV2_Options_Option));

    /* samplerate */
    options[0].context = LV2_OPTIONS_INSTANCE;
    options[0].subject = 0;
    options[0].key = ags_lv2_urid_manager_lookup(ags_lv2_urid_manager_get_instance(),
						 LV2_PARAMETERS__sampleRate);

    ptr_samplerate = (float *) malloc(sizeof(float));
    ptr_samplerate[0] = conf_samplerate;
  
    options[0].size = sizeof(float);
    options[0].type = ags_lv2_urid_manager_lookup(ags_lv2_urid_manager_get_instance(),
						  LV2_ATOM__Float);
    options[0].value = ptr_samplerate;
  
    /* min-block-length */
    options[1].context = LV2_OPTIONS_INSTANCE;
    options[1].subject = 0;
    options[1].key = ags_lv2_urid_manager_lookup(ags_lv2_urid_manager_get_instance(),
						 LV2_BUF_SIZE__minBlockLength);

    ptr_buffer_size = (float *) malloc(sizeof(float));
    ptr_buffer_size[0] = conf_buffer_size;
  
    options[1].size = sizeof(float);
    options[1].type = ags_lv2_urid_manager_lookup(ags_lv2_urid_manager_get_instance(),
						  LV2_ATOM__Int);
    options[1].value = ptr_buffer_size;

    /* max-block-length */
    options[2].context = LV2_OPTIONS_INSTANCE;
    options[2].subject = 0;
    options[2].key = ags_lv2_urid_manager_lookup(ags_lv2_urid_manager_get_instance(),
						 LV2_BUF_SIZE__maxBlockLength);

    ptr_buffer_size = (float *) malloc(sizeof(float));
    ptr_buffer_size[0] = conf_buffer_size;

    options[2].size = sizeof(float);
    options[2].type = ags_lv2_urid_manager_lookup(ags_lv2_urid_manager_get_instance(),
						  LV2_ATOM__Int);
    options[2].value = ptr_buffer_size;

    /* bounded-block-length */
    options[3].context = LV2_OPTIONS_INSTANCE;
    options[3].subject = 0;
    options[3].key = ags_lv2_urid_manager_lookup(ags_lv2_urid_manager_get_instance(),
						 LV2_BUF_SIZE__boundedBlockLength);

    ptr_buffer_size = (float *) malloc(sizeof(float));
    ptr_buffer_size[0] = conf_buffer_size;

    options[3].size = sizeof(float);
    options[3].type = ags_lv2_urid_manager_lookup(ags_lv2_urid_manager_get_instance(),
						  LV2_ATOM__Int);
    options[3].value = ptr_buffer_size;

    /* fixed-block-length */
    options[4].context = LV2_OPTIONS_INSTANCE;
    options[4].subject = 0;
    options[4].key = ags_lv2_urid_manager_lookup(ags_lv2_urid_manager_get_instance(),
						 LV2_BUF_SIZE__fixedBlockLength);

    ptr_buffer_size = (float *) malloc(sizeof(float));
    ptr_buffer_size[0] = conf_buffer_size;

    options[4].size = sizeof(float);
    options[4].type = ags_lv2_urid_manager_lookup(ags_lv2_urid_manager_get_instance(),
						  LV2_ATOM__Int);
    options[4].value = ptr_buffer_size;
    
    /* instance */
    options[5].context = LV2_OPTIONS_INSTANCE;
    options[5].subject = 0;
    options[5].key = 0;
    
    options[5].size = 0;
    options[5].type = 0;
    options[5].value = NULL;
    
    /* set options */
    ags_lv2_option_manager_lv2_options_set(lv2_handle[0],
					   options);
  }
  
  /*  */  
  if(worker_handle != NULL){
    if(plugin_descriptor->extension_data != NULL){
      AGS_LV2_WORKER(worker_handle)->worker_interface = plugin_descriptor->extension_data("http://lv2plug.in/ns/ext/worker#interface");
    }
    
    g_object_set(worker_handle,
		 "handle", lv2_handle[0],
		 NULL);
  }

  g_free(path);
  
  return(lv2_handle);
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
