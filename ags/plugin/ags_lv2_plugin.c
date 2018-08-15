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

#include <ags/plugin/ags_lv2_plugin.h>

#include <ags/libags.h>

#include <ags/plugin/ags_lv2_plugin.h>
#include <ags/plugin/ags_lv2_preset.h>
#include <ags/plugin/ags_lv2_manager.h>
#include <ags/plugin/ags_lv2_event_manager.h>
#include <ags/plugin/ags_lv2_uri_map_manager.h>
#include <ags/plugin/ags_lv2_log_manager.h>
#include <ags/plugin/ags_lv2_worker_manager.h>
#include <ags/plugin/ags_lv2_worker.h>
#include <ags/plugin/ags_lv2_urid_manager.h>
#include <ags/plugin/ags_lv2_option_manager.h>
#include <ags/plugin/ags_plugin_port.h>

#include <ags/audio/midi/ags_midi_buffer_util.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <math.h>

#include <ags/i18n.h>

void ags_lv2_plugin_class_init(AgsLv2PluginClass *lv2_plugin);
void ags_lv2_plugin_init (AgsLv2Plugin *lv2_plugin);
void ags_lv2_plugin_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_lv2_plugin_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_lv2_plugin_dispose(GObject *gobject);
void ags_lv2_plugin_finalize(GObject *gobject);

gpointer ags_lv2_plugin_instantiate(AgsBasePlugin *base_plugin,
				    guint samplerate);
void ags_lv2_plugin_connect_port(AgsBasePlugin *base_plugin,
				 gpointer plugin_handle,
				 guint port_index,
				 gpointer data_location);
void ags_lv2_plugin_activate(AgsBasePlugin *base_plugin,
			     gpointer plugin_handle);
void ags_lv2_plugin_deactivate(AgsBasePlugin *base_plugin,
			       gpointer plugin_handle);
void ags_lv2_plugin_run(AgsBasePlugin *base_plugin,
			gpointer plugin_handle,
			snd_seq_event_t *seq_event,
			guint frame_count);
void ags_lv2_plugin_load_plugin(AgsBasePlugin *base_plugin);

void ags_lv2_plugin_real_change_program(AgsLv2Plugin *lv2_plugin,
					gpointer ladspa_handle,
					guint bank_index,
					guint program_index);

/**
 * SECTION:ags_lv2_plugin
 * @short_description: The lv2 plugin class
 * @title: AgsLv2Plugin
 * @section_id:
 * @include: ags/plugin/ags_lv2_plugin.h
 *
 * The #AgsLv2Plugin loads/unloads a Lv2 plugin.
 */

enum{
  PROP_0,
  PROP_PNAME,
  PROP_URI,
  PROP_UI_URI,
  PROP_MANIFEST,
  PROP_TURTLE,
  PROP_DOAP_NAME,
  PROP_FOAF_NAME,
  PROP_FOAF_HOMEPAGE,
  PROP_FOAF_MBOX,
  PROP_PRESET,
};

enum{
  CHANGE_PROGRAM,
  LAST_SIGNAL,
};

static gpointer ags_lv2_plugin_parent_class = NULL;
static guint lv2_plugin_signals[LAST_SIGNAL];

GType
ags_lv2_plugin_get_type (void)
{
  static GType ags_type_lv2_plugin = 0;

  if(!ags_type_lv2_plugin){
    static const GTypeInfo ags_lv2_plugin_info = {
      sizeof (AgsLv2PluginClass),
      NULL, /* lv2_init */
      NULL, /* lv2_finalize */
      (GClassInitFunc) ags_lv2_plugin_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLv2Plugin),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_lv2_plugin_init,
    };

    ags_type_lv2_plugin = g_type_register_static(AGS_TYPE_BASE_PLUGIN,
						 "AgsLv2Plugin",
						 &ags_lv2_plugin_info,
						 0);
  }

  return (ags_type_lv2_plugin);
}

void
ags_lv2_plugin_class_init(AgsLv2PluginClass *lv2_plugin)
{
  AgsBasePluginClass *base_plugin;

  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_lv2_plugin_parent_class = g_type_class_peek_parent(lv2_plugin);

  /* GObjectClass */
  gobject = (GObjectClass *) lv2_plugin;

  gobject->set_property = ags_lv2_plugin_set_property;
  gobject->get_property = ags_lv2_plugin_get_property;

  gobject->dispose = ags_lv2_plugin_dispose;
  gobject->finalize = ags_lv2_plugin_finalize;

  /* properties */
  /**
   * AgsLv2Plugin:pname:
   *
   * The assigned pname.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("pname",
				   i18n_pspec("pname of the plugin"),
				   i18n_pspec("The pname this plugin is associated with"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PNAME,
				  param_spec);

  /**
   * AgsLv2Plugin:uri:
   *
   * The assigned uri.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("uri",
				   i18n_pspec("uri of the plugin"),
				   i18n_pspec("The uri this plugin is located in"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_URI,
				  param_spec);

  /**
   * AgsLv2Plugin:ui-uri:
   *
   * The assigned ui-uri.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("ui-uri",
				   i18n_pspec("ui-uri of the plugin"),
				   i18n_pspec("The ui-uri this plugin has"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_UI_URI,
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
   * AgsLv2Plugin:turtle:
   *
   * The assigned turtle.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("turtle",
				   i18n_pspec("turtle of the plugin"),
				   i18n_pspec("The turtle this plugin is located in"),
				   AGS_TYPE_TURTLE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TURTLE,
				  param_spec);

  /**
   * AgsLv2Plugin:doap-name:
   *
   * The assigned doap name.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("doap-name",
				   i18n_pspec("doap name"),
				   i18n_pspec("The doap name"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DOAP_NAME,
				  param_spec);

  /**
   * AgsLv2Plugin:foaf-name:
   *
   * The assigned foaf name.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("foaf-name",
				   i18n_pspec("foaf name"),
				   i18n_pspec("The foaf name"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FOAF_NAME,
				  param_spec);

  /**
   * AgsLv2Plugin:foaf-homepage:
   *
   * The assigned foaf homepage.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("foaf-homepage",
				   i18n_pspec("foaf homepage"),
				   i18n_pspec("The foaf homepage"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FOAF_HOMEPAGE,
				  param_spec);

  /**
   * AgsLv2Plugin:foaf-mbox:
   *
   * The assigned foaf mbox.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("foaf-mbox",
				   i18n_pspec("foaf mbox"),
				   i18n_pspec("The foaf mbox"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FOAF_MBOX,
				  param_spec);

  /**
   * AgsLv2Plugin:preset:
   *
   * The assigned preset.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("preset",
				    i18n_pspec("preset of the plugin"),
				    i18n_pspec("The preset of this plugin"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PRESET,
				  param_spec);
  
  /* AgsBasePluginClass */
  base_plugin = (AgsBasePluginClass *) lv2_plugin;

  base_plugin->instantiate = ags_lv2_plugin_instantiate;

  base_plugin->connect_port = ags_lv2_plugin_connect_port;

  base_plugin->activate = ags_lv2_plugin_activate;
  base_plugin->deactivate = ags_lv2_plugin_deactivate;

  base_plugin->run = ags_lv2_plugin_run;

  base_plugin->load_plugin = ags_lv2_plugin_load_plugin;

  /* AgsLv2PluginClass */
  lv2_plugin->change_program = ags_lv2_plugin_real_change_program;
  
  /**
   * AgsLv2Plugin::change-program:
   * @lv2_plugin: the plugin to change-program
   * @lv2_handle: the Lv2 handle
   * @bank: the bank number
   * @program: the program number
   *
   * The ::change-program signal creates a new instance of plugin.
   *
   * Since: 2.0.0
   */
  lv2_plugin_signals[CHANGE_PROGRAM] =
    g_signal_new("change-program",
		 G_TYPE_FROM_CLASS (lv2_plugin),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsLv2PluginClass, change_program),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__POINTER_UINT_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_POINTER,
		 G_TYPE_UINT,
		 G_TYPE_UINT);
}

void
ags_lv2_plugin_init(AgsLv2Plugin *lv2_plugin)
{
  lv2_plugin->flags = 0;

  lv2_plugin->pname = NULL;
  lv2_plugin->uri = NULL;
  lv2_plugin->ui_uri = NULL;

  lv2_plugin->manifest = NULL;
  lv2_plugin->turtle = NULL;
  
  lv2_plugin->doap_name = NULL;
  lv2_plugin->foaf_name = NULL;
  lv2_plugin->foaf_homepage = NULL;
  lv2_plugin->foaf_mbox = NULL;

  lv2_plugin->feature = NULL;

  lv2_plugin->program = NULL;

  lv2_plugin->preset = NULL;
}

void
ags_lv2_plugin_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsLv2Plugin *lv2_plugin;

  pthread_mutex_t *base_plugin_mutex;

  lv2_plugin = AGS_LV2_PLUGIN(gobject);

  /* get base plugin mutex */
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
  base_plugin_mutex = AGS_BASE_PLUGIN(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

  switch(prop_id){
  case PROP_PNAME:
    {
      gchar *pname;

      pname = (gchar *) g_value_get_string(value);

      pthread_mutex_lock(base_plugin_mutex);

      if(lv2_plugin->pname == pname){
	pthread_mutex_unlock(base_plugin_mutex);

	return;
      }
      
      if(lv2_plugin->pname != NULL){
	g_free(lv2_plugin->pname);
      }

      lv2_plugin->pname = g_strdup(pname);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_URI:
    {
      gchar *uri;

      uri = (gchar *) g_value_get_string(value);

      pthread_mutex_lock(base_plugin_mutex);

      if(lv2_plugin->uri == uri){
	pthread_mutex_unlock(base_plugin_mutex);

	return;
      }
      
      if(lv2_plugin->uri != NULL){
	g_free(lv2_plugin->uri);
      }

      lv2_plugin->uri = g_strdup(uri);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_UI_URI:
    {
      gchar *ui_uri;

      ui_uri = (gchar *) g_value_get_string(value);

      pthread_mutex_lock(base_plugin_mutex);

      if(lv2_plugin->ui_uri == ui_uri){
	pthread_mutex_unlock(base_plugin_mutex);

	return;
      }
      
      if(lv2_plugin->ui_uri != NULL){
	g_free(lv2_plugin->ui_uri);
      }

      lv2_plugin->ui_uri = g_strdup(ui_uri);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_MANIFEST:
    {
      AgsTurtle *manifest;

      manifest = (AgsTurtle *) g_value_get_object(value);

      pthread_mutex_lock(base_plugin_mutex);

      if(lv2_plugin->manifest == manifest){
	pthread_mutex_unlock(base_plugin_mutex);

	return;
      }

      if(lv2_plugin->manifest != NULL){
	g_object_unref(lv2_plugin->manifest);
      }

      if(manifest != NULL){
	g_object_ref(manifest);
      }
      
      lv2_plugin->manifest = manifest;

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_TURTLE:
    {
      AgsTurtle *turtle;

      turtle = (AgsTurtle *) g_value_get_object(value);

      pthread_mutex_lock(base_plugin_mutex);

      if(lv2_plugin->turtle == turtle){
	pthread_mutex_unlock(base_plugin_mutex);

	return;
      }

      if(lv2_plugin->turtle != NULL){
	g_object_unref(lv2_plugin->turtle);
      }

      if(turtle != NULL){
	g_object_ref(turtle);
      }
      
      lv2_plugin->turtle = turtle;

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_DOAP_NAME:
    {
      gchar *doap_name;

      doap_name = (gchar *) g_value_get_string(value);

      pthread_mutex_lock(base_plugin_mutex);

      if(lv2_plugin->doap_name == doap_name){
	pthread_mutex_unlock(base_plugin_mutex);

	return;
      }
      
      if(lv2_plugin->doap_name != NULL){
	g_free(lv2_plugin->doap_name);
      }

      lv2_plugin->doap_name = g_strdup(doap_name);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_FOAF_NAME:
    {
      gchar *foaf_name;

      foaf_name = (gchar *) g_value_get_string(value);

      pthread_mutex_lock(base_plugin_mutex);

      if(lv2_plugin->foaf_name == foaf_name){
	pthread_mutex_unlock(base_plugin_mutex);

	return;
      }
      
      if(lv2_plugin->foaf_name != NULL){
	g_free(lv2_plugin->foaf_name);
      }

      lv2_plugin->foaf_name = g_strdup(foaf_name);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_FOAF_HOMEPAGE:
    {
      gchar *foaf_homepage;

      foaf_homepage = (gchar *) g_value_get_string(value);

      pthread_mutex_lock(base_plugin_mutex);

      if(lv2_plugin->foaf_homepage == foaf_homepage){
	pthread_mutex_unlock(base_plugin_mutex);

	return;
      }
      
      if(lv2_plugin->foaf_homepage != NULL){
	g_free(lv2_plugin->foaf_homepage);
      }

      lv2_plugin->foaf_homepage = g_strdup(foaf_homepage);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_FOAF_MBOX:
    {
      gchar *foaf_mbox;

      foaf_mbox = (gchar *) g_value_get_string(value);

      pthread_mutex_lock(base_plugin_mutex);

      if(lv2_plugin->foaf_mbox == foaf_mbox){
	pthread_mutex_unlock(base_plugin_mutex);

	return;
      }
      
      if(lv2_plugin->foaf_mbox != NULL){
	g_free(lv2_plugin->foaf_mbox);
      }

      lv2_plugin->foaf_mbox = g_strdup(foaf_mbox);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_PRESET:
    {
      AgsLv2Preset *lv2_preset;

      lv2_preset = g_value_get_pointer(value);

      pthread_mutex_lock(base_plugin_mutex);

      if(lv2_preset == NULL ||
	 g_list_find(lv2_plugin->preset, lv2_preset) != NULL){
	pthread_mutex_unlock(base_plugin_mutex);
      
	return;
      }

      lv2_plugin->preset = g_list_append(lv2_plugin->preset,
					 lv2_preset);
      g_object_ref(lv2_preset);
      
      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_lv2_plugin_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsLv2Plugin *lv2_plugin;

  pthread_mutex_t *base_plugin_mutex;

  lv2_plugin = AGS_LV2_PLUGIN(gobject);

  /* get base plugin mutex */
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
  base_plugin_mutex = AGS_BASE_PLUGIN(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

  switch(prop_id){
  case PROP_PNAME:
    {
      pthread_mutex_lock(base_plugin_mutex);

      g_value_set_string(value, lv2_plugin->pname);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_URI:
    {
      pthread_mutex_lock(base_plugin_mutex);

      g_value_set_string(value, lv2_plugin->uri);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_UI_URI:
    {
      pthread_mutex_lock(base_plugin_mutex);

      g_value_set_string(value, lv2_plugin->ui_uri);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_MANIFEST:
    {
      pthread_mutex_lock(base_plugin_mutex);

      g_value_set_object(value, lv2_plugin->manifest);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_TURTLE:
    {
      pthread_mutex_lock(base_plugin_mutex);

      g_value_set_object(value, lv2_plugin->turtle);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_DOAP_NAME:
    {
      pthread_mutex_lock(base_plugin_mutex);

      g_value_set_string(value, lv2_plugin->doap_name);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_FOAF_NAME:
    {
      pthread_mutex_lock(base_plugin_mutex);

      g_value_set_string(value, lv2_plugin->foaf_name);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_FOAF_HOMEPAGE:
    {
      pthread_mutex_lock(base_plugin_mutex);

      g_value_set_string(value, lv2_plugin->foaf_homepage);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_FOAF_MBOX:
    {
      pthread_mutex_lock(base_plugin_mutex);

      g_value_set_string(value, lv2_plugin->foaf_mbox);

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  case PROP_PRESET:
    {
      pthread_mutex_lock(base_plugin_mutex);

      g_value_set_pointer(value, g_list_copy(lv2_plugin->preset));

      pthread_mutex_unlock(base_plugin_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_lv2_plugin_dispose(GObject *gobject)
{
  AgsLv2Plugin *lv2_plugin;

  lv2_plugin = AGS_LV2_PLUGIN(gobject);

  if(lv2_plugin->manifest != NULL){
    g_object_unref(lv2_plugin->manifest);

    lv2_plugin->manifest = NULL;
  }
  
  if(lv2_plugin->turtle != NULL){
    g_object_unref(lv2_plugin->turtle);

    lv2_plugin->turtle = NULL;
  }

  if(lv2_plugin->preset != NULL){
    g_list_free_full(lv2_plugin->preset,
		     g_object_unref);

    lv2_plugin->preset = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_lv2_plugin_parent_class)->dispose(gobject);
}

void
ags_lv2_plugin_finalize(GObject *gobject)
{
  AgsLv2Plugin *lv2_plugin;

  lv2_plugin = AGS_LV2_PLUGIN(gobject);

  g_free(lv2_plugin->uri);
  g_free(lv2_plugin->ui_uri);

  if(lv2_plugin->manifest != NULL){
    g_object_unref(lv2_plugin->manifest);
  }
  
  if(lv2_plugin->turtle != NULL){
    g_object_unref(lv2_plugin->turtle);
  }

  if(lv2_plugin->feature != NULL){
    guint i;

    for(i = 0; lv2_plugin->feature[i] != NULL; i++){
      free(lv2_plugin->feature[i]->data);
      free(lv2_plugin->feature[i]);
    }
    
    free(lv2_plugin->feature);
  }

  if(lv2_plugin->preset != NULL){
    g_list_free_full(lv2_plugin->preset,
		     g_object_unref);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_lv2_plugin_parent_class)->finalize(gobject);
}

gpointer
ags_lv2_plugin_instantiate(AgsBasePlugin *base_plugin,
			   guint samplerate)
{
  AgsLv2Plugin *lv2_plugin;
  
  AgsConfig *config;
  
  void *plugin_so;
  LV2_Descriptor_Function lv2_descriptor;
  LV2_Descriptor *plugin_descriptor;

  LV2_Handle *lv2_handle;
  
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

  uint32_t effect_index;
  guint conf_buffer_size;
  guint conf_samplerate;
  double rate;
  guint total_feature;
  guint nth;
  gboolean initial_call;
  
  LV2_Handle (*instantiate)(const struct _LV2_Descriptor * descriptor,
			    double sample_rate,
			    const char *bundle_path,
			    const LV2_Feature *const *features);

  pthread_mutex_t *base_plugin_mutex;
  
  lv2_plugin = AGS_LV2_PLUGIN(base_plugin);

  /* get base plugin mutex */
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
  base_plugin_mutex = base_plugin->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

  //  xmlSaveFormatFileEnc("-", lv2_plugin->turtle->doc, "UTF-8", 1);

  /* get some fields */
  pthread_mutex_lock(base_plugin_mutex);

  plugin_so = base_plugin->plugin_so;

  feature = lv2_plugin->feature;

  path = g_strndup(base_plugin->filename,
		   rindex(base_plugin->filename, '/') - base_plugin->filename + 1);

  effect_index = base_plugin->effect_index;
  
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
  lv2_handle = (LV2_Handle *) malloc(sizeof(LV2_Handle));

  /* instantiate */
  rate = (double) samplerate;
  lv2_handle[0] = instantiate(plugin_descriptor,
			      rate,
			      path,
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
    
    /* terminate */
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
    g_object_set(worker_handle,
		 "handle", lv2_handle[0],
		 NULL);
  }

  free(path);
  
  return(lv2_handle);
}

void
ags_lv2_plugin_connect_port(AgsBasePlugin *base_plugin,
			    gpointer plugin_handle,
			    guint port_index,
			    gpointer data_location)
{
  void (*connect_port)(LV2_Handle instance,
		       uint32_t port,
		       void *data_location);

  pthread_mutex_t *base_plugin_mutex;

  /* get base plugin mutex */
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
  base_plugin_mutex = base_plugin->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(base_plugin_mutex);

  connect_port = AGS_LV2_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->connect_port;

  pthread_mutex_unlock(base_plugin_mutex);
  
  /* connect port */
  connect_port((LV2_Handle) plugin_handle,
	       (uint32_t) port_index,
	       (float *) data_location);
}

void
ags_lv2_plugin_activate(AgsBasePlugin *base_plugin,
			gpointer plugin_handle)
{
  void (*activate)(LV2_Handle instance);
  
  pthread_mutex_t *base_plugin_mutex;

  /* get base plugin mutex */
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
  base_plugin_mutex = base_plugin->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(base_plugin_mutex);

  activate = AGS_LV2_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->activate;
  
  pthread_mutex_unlock(base_plugin_mutex);

  /* activate */
  if(activate != NULL){
    activate((LV2_Handle) plugin_handle);
  }
}

void
ags_lv2_plugin_deactivate(AgsBasePlugin *base_plugin,
			  gpointer plugin_handle)
{
  void (*deactivate)(LV2_Handle instance);
  
  pthread_mutex_t *base_plugin_mutex;

  /* get base plugin mutex */
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
  base_plugin_mutex = base_plugin->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(base_plugin_mutex);

  deactivate = AGS_LV2_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->deactivate;
  
  pthread_mutex_unlock(base_plugin_mutex);

  /* deactivate */
  if(deactivate != NULL){
    deactivate((LV2_Handle) plugin_handle);
  }
}

void
ags_lv2_plugin_run(AgsBasePlugin *base_plugin,
		   gpointer plugin_handle,
		   snd_seq_event_t *seq_event,
		   guint frame_count)
{
  void (*run)(LV2_Handle instance,
	      uint32_t sample_count);
  
  pthread_mutex_t *base_plugin_mutex;

  /* get base plugin mutex */
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
  base_plugin_mutex = base_plugin->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(base_plugin_mutex);

  run = AGS_LV2_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->run;
  
  pthread_mutex_unlock(base_plugin_mutex);

  /* run */
  run((LV2_Handle) plugin_handle,
      (uint32_t) frame_count);
}

void
ags_lv2_plugin_load_plugin(AgsBasePlugin *base_plugin)
{
  AgsLv2Plugin *lv2_plugin;  
  AgsPluginPort *current_plugin_port;

  AgsTurtle *turtle;
  
  GList *plugin_port;
  GList *metadata_list;
  GList *instrument_list;
  GList *extension_list;
  GList *feature_list;
  GList *ui_list;
  GList *port_list;
  
  gchar *escaped_effect;
  gchar *xpath;
  gchar *uri;

  gdouble lower_value, upper_value;
  guint i;
  gboolean found_port;
  
  LV2_Descriptor_Function lv2_descriptor;
  LV2_Descriptor *plugin_descriptor;

  uint32_t effect_index;
    
  pthread_mutex_t *base_plugin_mutex;
  
  lv2_plugin = AGS_LV2_PLUGIN(base_plugin);  

  /* get base plugin mutex */
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
  base_plugin_mutex = base_plugin->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

  /* dlopen */
  pthread_mutex_lock(base_plugin_mutex);

  base_plugin->plugin_so = dlopen(base_plugin->filename,
				  RTLD_NOW);

  if(base_plugin->plugin_so == NULL){
    g_warning("ags_lv2_plugin.c - failed to load static object file");
    
    dlerror();

    pthread_mutex_unlock(base_plugin_mutex);
    
    return;
  }

  lv2_descriptor = (LV2_Descriptor_Function) dlsym(base_plugin->plugin_so,
						   "lv2_descriptor");

  turtle = lv2_plugin->turtle;
  
  pthread_mutex_unlock(base_plugin_mutex);
  
  if(dlerror() == NULL && lv2_descriptor){
    xmlNode *triple_node;
    xmlNode *port_node;
    xmlNode *current;

    AgsUUID *uuid;
    
    GList *list, *list_start;

    pthread_mutex_lock(base_plugin_mutex);
    
    uuid = ags_uuid_copy(base_plugin->uuid);
    
    escaped_effect = ags_string_util_escape_single_quote(base_plugin->effect);

    pthread_mutex_unlock(base_plugin_mutex);

    /* retrieve triple node */
    triple_node = g_hash_table_lookup(ags_lv2_manager_get_instance()->current_plugin_node,
				      base_plugin->uuid);
    ags_uuid_free(uuid);
    
    if(triple_node == NULL){
      g_warning("rdf-triple not found");
      
      return;
    }
    
    /* name metadata */
    xpath = ".//rdf-pname-ln[text()='doap:name']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list][1]//rdf-string";    
    metadata_list = ags_turtle_find_xpath_with_context_node(turtle,
							    xpath,
							    triple_node);

    if(metadata_list != NULL){
      g_object_set(lv2_plugin,
		   "doap-name", xmlNodeGetContent((xmlNode *) metadata_list->data),
		   NULL);

      g_list_free(metadata_list);
    }

    /* author metadata */
    xpath = ".//rdf-pname-ln[text()='foaf:name']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list][1]//rdf-string";
    metadata_list = ags_turtle_find_xpath_with_context_node(turtle,
							    xpath,
							    triple_node);

    if(metadata_list != NULL){
      g_object_set(lv2_plugin,
		   "foaf-name", xmlNodeGetContent((xmlNode *) metadata_list->data),
		   NULL);

      g_list_free(metadata_list);
    }

    /* homepage metadata */
    xpath = ".//rdf-pname-ln[text()='foaf:homepage']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list][1]//rdf-string";
    metadata_list = ags_turtle_find_xpath_with_context_node(turtle,
							    xpath,
							    triple_node);

    if(metadata_list != NULL){
      g_object_set(lv2_plugin,
		   "foaf-homepage", xmlNodeGetContent((xmlNode *) metadata_list->data),
		   NULL);

      g_list_free(metadata_list);
    }

    /* mbox metadata */
    xpath = ".//rdf-pname-ln[text()='foaf:mbox']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list][1]//rdf-string";
    metadata_list = ags_turtle_find_xpath_with_context_node(turtle,
							    xpath,
							    triple_node);

    if(metadata_list != NULL){
      g_object_set(lv2_plugin,
		   "foaf-mbox", xmlNodeGetContent((xmlNode *) metadata_list->data),
		   NULL);
    
      g_list_free(metadata_list);
    }

    /* check programs interface */
    xpath = ".//rdf-pname-ln[text()='lv2:extensiondata']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list][1]//rdf-iriref[text()='<http://kxstudio.sf.net/ns/lv2ext/programs#Interface>']";

    extension_list = ags_turtle_find_xpath_with_context_node(turtle,
							     xpath,
							     triple_node);

    if(extension_list != NULL){
      ags_lv2_plugin_set_flags(lv2_plugin, AGS_LV2_PLUGIN_HAS_PROGRAM_INTERFACE);
      
      g_list_free(extension_list);
    }
    
    /* check needs worker */
    xpath = ".//rdf-pname-ln[text()='lv2:requiredFeature']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list][1]//rdf-pname-ln[text()='lv2worker:schedule']";

    feature_list = ags_turtle_find_xpath_with_context_node(turtle,
							   xpath,
							   triple_node);

    if(feature_list != NULL){
      ags_lv2_plugin_set_flags(lv2_plugin, AGS_LV2_PLUGIN_NEEDS_WORKER);
      
      g_list_free(feature_list);
    }

    /* check if is synthesizer */
    xpath = ".//rdf-verb[@verb='a']/following-sibling::*[self::rdf-object-list]//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':instrumentplugin') + 1) = ':instrumentplugin']";
    instrument_list = ags_turtle_find_xpath_with_context_node(turtle,
							      xpath,
							      triple_node);

    if(instrument_list != NULL){
      ags_lv2_plugin_set_flags(lv2_plugin, AGS_LV2_PLUGIN_IS_SYNTHESIZER);

      g_list_free(instrument_list);
    }

    /* check UI */
    xpath = ".//rdf-pname-ln[text()='uiext:ui']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list][1]//rdf-iriref";

    ui_list = ags_turtle_find_xpath_with_context_node(turtle,
						      xpath,
						      triple_node);

    if(ui_list != NULL){
      gchar *ui_uri;

      ui_uri = xmlNodeGetContent(ui_list->data);

      if(strlen(ui_uri) > 2){
	ui_uri = g_strndup(ui_uri + 1,
			   strlen(ui_uri) - 2);
      }

      g_object_set(lv2_plugin,
		   "ui-uri", ui_uri,
		   NULL);
      
      g_message("*** found UI <%s> ***", ui_uri);

      g_list_free(ui_list);
    }

    /* load ports */
    xpath = ".//rdf-verb//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':port') + 1) = ':port']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list]/rdf-object[.//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':index') + 1) = ':index']]";
    port_list = ags_turtle_find_xpath_with_context_node(turtle,
							xpath,
							triple_node);

    /* AgsPluginPort */
    plugin_port = NULL;

    while(port_list != NULL){
      current_plugin_port = ags_plugin_port_new();
      g_object_ref(current_plugin_port);
      
      found_port = FALSE;

      g_value_init(current_plugin_port->upper_value,
		   G_TYPE_FLOAT);
      g_value_init(current_plugin_port->lower_value,
		   G_TYPE_FLOAT);
      g_value_init(current_plugin_port->default_value,
		   G_TYPE_FLOAT);

      g_value_set_float(current_plugin_port->upper_value,
			0.0);
      
      port_node = (xmlNode *) port_list->data;

      //      g_message("%s", port_node->name);
      
      /* load flags - control */
      xpath = g_ascii_strdown(".//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':ControlPort') + 1) = ':ControlPort']",
			      -1);

      list = ags_turtle_find_xpath_with_context_node(turtle,
						     xpath,
						     port_node);

      if(list != NULL){
	current_plugin_port->flags |= AGS_PLUGIN_PORT_CONTROL;
	found_port = TRUE;

	g_list_free(list);
      }

      /* load flags - output */
      xpath = g_ascii_strdown(".//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':OutputPort') + 1) = ':OutputPort']",
			      -1);

      list = ags_turtle_find_xpath_with_context_node(turtle,
						     xpath,
						     port_node);

      if(list != NULL){
	current_plugin_port->flags |= AGS_PLUGIN_PORT_OUTPUT;
	found_port = TRUE;

	g_list_free(list);
      }

      /* load flags - input */
      xpath = g_ascii_strdown(".//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':InputPort') + 1) = ':InputPort']",
			      -1);

      list = ags_turtle_find_xpath_with_context_node(turtle,
						     xpath,
						     port_node);

      if(list != NULL){
	current_plugin_port->flags |= AGS_PLUGIN_PORT_INPUT;
	found_port = TRUE;

	g_list_free(list);
      }

      /* load flags - audio */
      xpath = g_ascii_strdown(".//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':AudioPort') + 1) = ':AudioPort']",
			      -1);

      list = ags_turtle_find_xpath_with_context_node(turtle,
						     xpath,
						     port_node);

      if(list != NULL){
	current_plugin_port->flags |= AGS_PLUGIN_PORT_AUDIO;
	found_port = TRUE;
	
	g_list_free(list);
      }

      /* load flags - event */
      xpath = g_ascii_strdown(".//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':EventPort') + 1) = ':EventPort']",
			      -1);

      list = ags_turtle_find_xpath_with_context_node(turtle,
						     xpath,
						     port_node);

      if(list != NULL){
	current_plugin_port->flags |= AGS_PLUGIN_PORT_EVENT;
	found_port = TRUE;

	g_list_free(list);
      }

      /* load flags - atom */
      xpath = g_ascii_strdown(".//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':AtomPort') + 1) = ':AtomPort']",
			      -1);

      list = ags_turtle_find_xpath_with_context_node(turtle,
						     xpath,
						     port_node);

      if(list != NULL){
	current_plugin_port->flags |= AGS_PLUGIN_PORT_ATOM;
	found_port = TRUE;
	
	g_list_free(list);
      }

      /* load index */
      xpath = g_ascii_strdown(".//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':index') + 1) = ':index']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list][1]//rdf-numeric",
			      -1);
      list = ags_turtle_find_xpath_with_context_node(turtle,
						     xpath,
						     port_node);

      if(list != NULL){
	current = (xmlNode *) list->data;

       	current_plugin_port->port_index = g_ascii_strtoull(xmlNodeGetContent(current),
							   NULL,
							   10);
	found_port = TRUE;
	
	g_list_free(list);
      }else{
	g_warning("ags_lv2_plugin.c - no port index found");
      }
      
      if(!found_port){
	g_object_unref(current_plugin_port);
	
	port_list = port_list->next;
	
	continue;
      }

      /* properties */
      xpath = g_ascii_strdown(".//rdf-verb//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':portProperty') + 1) = ':portProperty']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list][1]//rdf-pname-ln",
			      -1);
      list_start =
	list = ags_turtle_find_xpath_with_context_node(turtle,
						       xpath,
						       port_node);
      
      while(list != NULL){
	current = (xmlNode *) list->data;
	
	if(g_str_has_suffix(xmlNodeGetContent(current),
			    ":toggled")){
	  current_plugin_port->flags |= AGS_PLUGIN_PORT_TOGGLED;
	}else if(g_str_has_suffix(xmlNodeGetContent(current),
				  ":enumeration")){
	  current_plugin_port->flags |= AGS_PLUGIN_PORT_ENUMERATION;
	}else if(g_str_has_suffix(xmlNodeGetContent(current),
				  ":logarithmic")){
	  current_plugin_port->flags |= AGS_PLUGIN_PORT_LOGARITHMIC;
	}else if(g_str_has_suffix(xmlNodeGetContent(current),
				  ":integer")){
	  current_plugin_port->flags |= AGS_PLUGIN_PORT_INTEGER;
	}

	list = list->next;
      }

      if(list_start != NULL){
	g_list_free(list_start);
      }
      
      /* load name */
      xpath = g_ascii_strdown(".//rdf-object-list//rdf-string[ancestor::*[self::rdf-object-list][1]/preceding-sibling::*[self::rdf-verb][1]//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':name') + 1) = ':name']]",
			      -1);
      list = ags_turtle_find_xpath_with_context_node(turtle,
						     xpath,
						     port_node);

      if(list != NULL){
	gchar *str;
	
	current = (xmlNode *) list->data;
	str = xmlNodeGetContent(current);

	if(strlen(str) > 2){
	  str = g_strndup(str + 1,
			  strlen(str) - 2);
	  
	  g_object_set(current_plugin_port,
		       "port-name", str,
		       NULL);

	  g_free(str);
	}

	g_list_free(list);
      }
      
      /* load symbol */
      xpath = g_ascii_strdown(".//rdf-object-list//rdf-string[ancestor::*[self::rdf-object-list][1]/preceding-sibling::*[self::rdf-verb][1]//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':symbol') + 1) = ':symbol']]",
			      -1);
      list = ags_turtle_find_xpath_with_context_node(turtle,
						     xpath,
						     port_node);

      if(list != NULL){
	gchar *str;
	
	current = (xmlNode *) list->data;
	str = xmlNodeGetContent(current);

	if(strlen(str) > 2){
	  str = g_strndup(str + 1,
			  strlen(str) - 2);
	  
	  g_object_set(current_plugin_port,
		       "port-symbol", str,
		       NULL);

	  g_free(str);
	}

	g_list_free(list);
      }

      /* load scale points and value */
      xpath = g_ascii_strdown(".//rdf-verb//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':scalePoint') + 1) = ':scalePoint']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list][1]//rdf-string",
			      -1);
      list_start = 
	list = ags_turtle_find_xpath_with_context_node(turtle,
						       xpath,
						       port_node);

      if(list != NULL){
	current_plugin_port->scale_steps = g_list_length(list) - 1;

	current_plugin_port->scale_point = malloc((current_plugin_port->scale_steps + 2) * sizeof(gchar *));
      
	for(i = 0; list != NULL; i++){
	  gchar *str;

	  current_plugin_port->scale_point[i] = NULL;
	  
	  current = (xmlNode *) list->data;
	  str = xmlNodeGetContent(current);
	
	  if(strlen(str) > 2){
	    current_plugin_port->scale_point[i] = g_strndup(str + 1,
							     strlen(str) - 2);
	  }

	  list = list->next;
	}
	
	current_plugin_port->scale_point[i] = NULL;
	  
	if(list_start != NULL){
	  g_list_free(list_start);
	}

	xpath = g_ascii_strdown(".//rdf-verb//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':scalePoint') + 1) = ':scalePoint']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list][1]//rdf-numeric",
				-1);
	list_start = 
	  list = ags_turtle_find_xpath_with_context_node(turtle,
							 xpath,
							 port_node);

	current_plugin_port->scale_value = malloc((g_list_length(list_start)) * sizeof(gdouble));
	
	for(i = 0; list != NULL; i++){
	  gchar *str;
	
	  current = (xmlNode *) list->data;
	  str = xmlNodeGetContent(current);
	
	  current_plugin_port->scale_value[i] = g_ascii_strtod(str,
							       NULL);
	
	  list = list->next;
	}

	if(list_start != NULL){
	  g_list_free(list_start);
	}
      }else{
	current_plugin_port->scale_steps = 0;

	current_plugin_port->scale_point = NULL;
	current_plugin_port->scale_value = NULL;
      }      

      /* minimum */
      xpath = g_ascii_strdown(".//rdf-verb//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':minimum') + 1) = ':minimum']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list][1]//rdf-numeric",
			      -1);
      list = ags_turtle_find_xpath_with_context_node(turtle,
						     xpath,
						     port_node);
      lower_value = 0.0;
      
      if(list != NULL){
	current = (xmlNode *) list->data;

	lower_value = g_ascii_strtod(xmlNodeGetContent(current),
				     NULL);
	
	g_value_set_float(current_plugin_port->lower_value,
			  lower_value);

	g_list_free(list);
      }

      /* maximum */
      xpath = g_ascii_strdown(".//rdf-verb//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':maximum') + 1) = ':maximum']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list][1]//rdf-numeric",
			      -1);
      list = ags_turtle_find_xpath_with_context_node(turtle,
						     xpath,
						     port_node);
      upper_value = 1.0;
      
      if(list != NULL){
	current = (xmlNode *) list->data;

	upper_value = g_ascii_strtod(xmlNodeGetContent(current),
				     NULL);
	
	g_value_set_float(current_plugin_port->upper_value,
			  upper_value);

	g_list_free(list);
      }

      /* check scale steps */
      if((AGS_PLUGIN_PORT_INTEGER & (current_plugin_port->flags)) != 0 &&
	 current_plugin_port->scale_steps == 0){
	if(upper_value > 0.0 &&
	   lower_value >= 0.0){
	  current_plugin_port->scale_steps = upper_value - lower_value;
	}else if(upper_value > 0 &&
		 lower_value < 0){
	  current_plugin_port->scale_steps = upper_value + (-1 * (lower_value));
	}else{
	  current_plugin_port->scale_steps = -1 * (lower_value - upper_value);
	}	
      }
      
      /* default */
      xpath = g_ascii_strdown(".//rdf-verb//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':default') + 1) = ':default']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list][1]//rdf-numeric",
			      -1);
      list = ags_turtle_find_xpath_with_context_node(turtle,
						     xpath,
						     port_node);

      if(list != NULL){
	current = (xmlNode *) list->data;

	g_value_set_float(current_plugin_port->default_value,
			  g_ascii_strtod(xmlNodeGetContent(current),
					 NULL));

	g_list_free(list);
      }

      plugin_port = g_list_prepend(plugin_port,
				   current_plugin_port);
      
      /* iterate */
      port_list = port_list->next;
    }

    if(port_list != NULL){
      g_list_free(port_list);
    }

    /* set plugin port */
    pthread_mutex_lock(base_plugin_mutex);

    base_plugin->plugin_port = g_list_reverse(plugin_port);

    pthread_mutex_unlock(base_plugin_mutex);
  }
}

/**
 * ags_lv2_plugin_test_flags:
 * @lv2_plugin: the #AgsLv2Plugin
 * @flags: the flags
 * 
 * Test @flags to be set on @recall.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 * 
 * Since: 2.0.0
 */
gboolean
ags_lv2_plugin_test_flags(AgsLv2Plugin *lv2_plugin, guint flags)
{
  gboolean retval;
  
  pthread_mutex_t *base_plugin_mutex;

  if(!AGS_IS_LV2_PLUGIN(lv2_plugin)){
    return(FALSE);
  }
  
  /* get base plugin mutex */
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
  base_plugin_mutex = AGS_BASE_PLUGIN(lv2_plugin)->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

  /* test flags */
  pthread_mutex_lock(base_plugin_mutex);

  retval = ((flags & (lv2_plugin->flags)) != 0) ? TRUE: FALSE;
  
  pthread_mutex_unlock(base_plugin_mutex);

  return(retval);
}

/**
 * ags_lv2_plugin_set_flags:
 * @lv2_plugin: the #AgsLv2Plugin
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 2.0.0
 */
void
ags_lv2_plugin_set_flags(AgsLv2Plugin *lv2_plugin, guint flags)
{
  pthread_mutex_t *base_plugin_mutex;

  if(!AGS_IS_LV2_PLUGIN(lv2_plugin)){
    return;
  }
  
  /* get base plugin mutex */
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
  base_plugin_mutex = AGS_BASE_PLUGIN(lv2_plugin)->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

  /* set flags */
  pthread_mutex_lock(base_plugin_mutex);

  lv2_plugin->flags |= flags;
  
  pthread_mutex_unlock(base_plugin_mutex);
}

/**
 * ags_lv2_plugin_unset_flags:
 * @lv2_plugin: the #AgsLv2Plugin
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 2.0.0
 */
void
ags_lv2_plugin_unset_flags(AgsLv2Plugin *lv2_plugin, guint flags)
{
  pthread_mutex_t *base_plugin_mutex;

  if(!AGS_IS_LV2_PLUGIN(lv2_plugin)){
    return;
  }
  
  /* get base plugin mutex */
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
  base_plugin_mutex = AGS_BASE_PLUGIN(lv2_plugin)->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

  /* unset flags */
  pthread_mutex_lock(base_plugin_mutex);

  lv2_plugin->flags &= (~flags);
  
  pthread_mutex_unlock(base_plugin_mutex);
}

/**
 * ags_lv2_plugin_alloc_event_buffer:
 * @buffer_size: the allocated size
 *
 * Allocates a LV2_Event_Buffer
 * 
 * Returns: the new event buffer
 * 
 * Since: 2.0.0
 */
void*
ags_lv2_plugin_alloc_event_buffer(guint buffer_size)
{
  void *event_buffer;
  
  uint32_t padded_buffer_size;

  if(buffer_size > G_MAXUINT16){
    return(NULL);
  }
  
  if(buffer_size < 8){
    padded_buffer_size = 8;
  }else{
    padded_buffer_size = buffer_size;
  }
    
  event_buffer = (void *) malloc(padded_buffer_size + sizeof(LV2_Event_Buffer));
  memset(event_buffer, 0, padded_buffer_size + sizeof(LV2_Event_Buffer));

  AGS_LV2_EVENT_BUFFER(event_buffer)->data = event_buffer + sizeof(LV2_Event_Buffer);

  AGS_LV2_EVENT_BUFFER(event_buffer)->header_size = sizeof(LV2_Event_Buffer);

  AGS_LV2_EVENT_BUFFER(event_buffer)->stamp_type = 0;
  AGS_LV2_EVENT_BUFFER(event_buffer)->capacity = padded_buffer_size;

  AGS_LV2_EVENT_BUFFER(event_buffer)->event_count = 0;
  AGS_LV2_EVENT_BUFFER(event_buffer)->size = 0;

  return(event_buffer);
}

/**
 * ags_lv2_plugin_concat_event_buffer:
 * @buffer0: the first buffer
 * @...: %NULL terminated variadict arguments
 *
 * Concats the event buffers.
 * 
 * Returns: The newly allocated event buffer
 * 
 * Since: 2.0.0
 */
void*
ags_lv2_plugin_concat_event_buffer(void *buffer0, ...)
{
  void *buffer;
  void *current;
  
  va_list ap;

  guint buffer_length, prev_length;

  buffer_length = AGS_LV2_EVENT_BUFFER(buffer0)->capacity + sizeof(LV2_Event_Buffer);

  buffer = (void *) malloc(buffer_length);
  memcpy(buffer, buffer0, buffer_length);
  
  va_start(ap, buffer0);

  while((current = va_arg(ap, void*)) != NULL){
    prev_length = buffer_length;
    buffer_length += (AGS_LV2_EVENT_BUFFER(current)->capacity + sizeof(LV2_Event_Buffer));
    
    buffer = (void *) realloc(buffer,
			      buffer_length);
    memcpy(buffer + prev_length, current, buffer_length - prev_length);
  }

  va_end(ap);

  return(buffer);
}

/**
 * ags_lv2_plugin_event_buffer_alloc:
 * @buffer_size: the data's buffer size
 * 
 * Allocate LV2_Event_Buffer struct.
 * 
 * Returns: a new allocated LV2_Event_Buffer
 * 
 * Since: 2.0.0
 */
LV2_Event_Buffer*
ags_lv2_plugin_event_buffer_alloc(guint buffer_size)
{
  LV2_Event_Buffer *event_buffer;

  void *data;
  
  uint32_t padded_buffer_size;

  if(buffer_size > G_MAXUINT16){
    return(NULL);
  }
  
  if(buffer_size < 8){
    padded_buffer_size = 8;
  }else{
    padded_buffer_size = buffer_size;
  }
    
  event_buffer = (LV2_Event_Buffer *) malloc(sizeof(LV2_Event_Buffer));

  data = (void *) malloc(sizeof(LV2_Event) + padded_buffer_size * sizeof(uint8_t));
  memset(data, 0, sizeof(LV2_Event) + padded_buffer_size * sizeof(uint8_t));
  event_buffer->data = data;

  event_buffer->header_size = sizeof(LV2_Event_Buffer);

  event_buffer->stamp_type = 0;
  event_buffer->capacity = padded_buffer_size;

  event_buffer->event_count = 0;
  event_buffer->size = 0;

  return(event_buffer);
}

/**
 * ags_lv2_plugin_event_buffer_realloc_data:
 * @event_buffer: the LV2_Event_Buffer struct
 * @buffer_size: the data's buffer size
 * 
 * Reallocate LV2_Event_Buffer struct's data field.
 * 
 * Since: 2.0.0
 */
void
ags_lv2_plugin_event_buffer_realloc_data(LV2_Event_Buffer *event_buffer,
					 guint buffer_size)
{
  void *data;
  
  uint32_t padded_buffer_size;

  if(buffer_size > G_MAXUINT16){
    return(NULL);
  }
  
  if(buffer_size < 8){
    padded_buffer_size = 8;
  }else{
    padded_buffer_size = buffer_size;
  }

  data = event_buffer->data;
  data = (void *) realloc(data,
			  sizeof(LV2_Event) + padded_buffer_size * sizeof(uint8_t));

  event_buffer->data = data;
}

/**
 * ags_lv2_plugin_event_buffer_concat:
 * @event_buffer: the first buffer
 * @...: %NULL terminated variadict arguments
 *
 * Concats the event buffers.
 * 
 * Returns: The newly allocated event buffer
 * 
 * Since: 2.0.0
 */
LV2_Event_Buffer*
ags_lv2_plugin_event_buffer_concat(LV2_Event_Buffer *event_buffer, ...)
{
  LV2_Event_Buffer *buffer;
  LV2_Event_Buffer *current;

  void *offset;
  
  va_list ap;

  guint i;

  buffer = (LV2_Event_Buffer *) malloc(sizeof(LV2_Event_Buffer));  
  memcpy(buffer, event_buffer, sizeof(LV2_Event_Buffer));
  
  va_start(ap, event_buffer);
  i = 1;

  while(TRUE){
    current = va_arg(ap, LV2_Event_Buffer *);

    if(current == NULL){
      break;
    }
    
    buffer = (LV2_Event_Buffer *) realloc(buffer,
					  (i + 1) * sizeof(LV2_Event_Buffer));
    offset = buffer;
    offset += (i * sizeof(LV2_Event_Buffer));
    memcpy(offset, current, sizeof(LV2_Event_Buffer));

    i++;
  }

  va_end(ap);

  return(buffer);
}

/**
 * ags_lv2_plugin_event_buffer_append_midi:
 * @event_buffer: the event buffer
 * @buffer_size: the event buffer size
 * @events: the events to write
 * @event_count: the number of events to write
 *
 * Append MIDI data to event buffer.
 *
 * Returns: %TRUE on success otherwise %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_lv2_plugin_event_buffer_append_midi(void *event_buffer,
					guint buffer_size,
					snd_seq_event_t *events,
					guint event_count)
{
  void *offset;
  
  unsigned char midi_buffer[8];

  guint padded_buffer_size;
  guint count;
  guint i;
  gboolean success;

  /* find offset */
  offset = AGS_LV2_EVENT_BUFFER(event_buffer)->data;
  offset += AGS_LV2_EVENT_BUFFER(event_buffer)->size;
  
  /* append midi */
  success = TRUE;

  for(i = 0; i < event_count; i++){
    if(offset >= (void *) AGS_LV2_EVENT_BUFFER(event_buffer)->data + buffer_size){
      return(FALSE);
    }

    /* decode midi sequencer event */
    count = ags_midi_buffer_util_decode(midi_buffer,
					&(events[i]));
    
    if(count < 8){
      padded_buffer_size = 8;
    }else{
      padded_buffer_size = count;
    }
    
    if(AGS_LV2_EVENT_BUFFER(event_buffer)->size + padded_buffer_size >= buffer_size){      
      return(FALSE);
    }
    
    AGS_LV2_EVENT_BUFFER(event_buffer)->size += (padded_buffer_size + sizeof(LV2_Event));
    AGS_LV2_EVENT_BUFFER(event_buffer)->event_count += 1;
    
    AGS_LV2_EVENT(offset)->frames = 0;
    AGS_LV2_EVENT(offset)->subframes = 0;
    AGS_LV2_EVENT(offset)->type = ags_lv2_uri_map_manager_uri_to_id(NULL,
								    LV2_EVENT_URI,
								    LV2_MIDI__MidiEvent);
    AGS_LV2_EVENT(offset)->size = count;
      
    memcpy(offset + sizeof(LV2_Event), midi_buffer, count * sizeof(unsigned char));
      
    offset += (padded_buffer_size + sizeof(LV2_Event));
  }

  return(success);
}

/**
 * ags_lv2_plugin_event_buffer_remove_midi:
 * @event_buffer: the event buffer
 * @buffer_size: the event buffer size
 * @note: the key to remove
 *
 * Remove MIDI data from event buffer.
 *
 * Returns: %TRUE on success otherwise %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_lv2_plugin_event_buffer_remove_midi(void *event_buffer,
					guint buffer_size,
					guint note)
{
  void *offset;

  guint padded_buffer_size;
  guint count;
  guint i;
  gboolean success;

  /* find offset */
  offset = AGS_LV2_EVENT_BUFFER(event_buffer)->data;
  
  /* append midi */
  success = FALSE;

  while(offset < offset + AGS_LV2_EVENT_BUFFER(event_buffer)->size){
    if(((unsigned char *) (offset + sizeof(LV2_Event)))[1] == (0x7f & note)){
      success = TRUE;
      
      break;
    }
    
    count = AGS_LV2_EVENT(offset)->size;
    
    if(count < 8){
      padded_buffer_size = 8;
    }else{
      padded_buffer_size = count;
    }

    offset += (padded_buffer_size + sizeof(LV2_Event));
  }
  
  if(success){
    count = AGS_LV2_EVENT(offset)->size;
    
    if(count < 8){
      padded_buffer_size = 8;
    }else{
      padded_buffer_size = count;
    }

    memmove(offset,
	    offset + (padded_buffer_size + sizeof(LV2_Event)),
	    ((void *) AGS_LV2_EVENT_BUFFER(event_buffer)->data + buffer_size) - (offset + padded_buffer_size + sizeof(LV2_Event)));
    
    memset(AGS_LV2_EVENT_BUFFER(event_buffer)->data + buffer_size - (padded_buffer_size + sizeof(LV2_Event)),
	   0,
	   (padded_buffer_size + sizeof(LV2_Event)));
    
    AGS_LV2_EVENT_BUFFER(event_buffer)->size -= (padded_buffer_size + sizeof(LV2_Event));
    AGS_LV2_EVENT_BUFFER(event_buffer)->event_count -= 1;
  }
  
  return(success);
}

/**
 * ags_lv2_plugin_clear_event_buffer:
 * @event_buffer: the event buffer
 * @buffer_size: size of @event_buffer
 *
 * Clear the event buffer.
 *
 * Since: 2.0.0 
 */
void
ags_lv2_plugin_clear_event_buffer(void *event_buffer,
				  guint buffer_size)
{
  void *offset;

  guint padded_buffer_size;

  offset = AGS_LV2_EVENT_BUFFER(event_buffer)->data;
  
  if(buffer_size < 8){
    padded_buffer_size = 8;
  }else{
    padded_buffer_size = buffer_size;
  }
  
  memset(offset, 0, padded_buffer_size);
}

/**
 * ags_lv2_plugin_alloc_atom_sequence:
 * @sequence_size: the requested size
 *
 * Allocates a LV2_Atom_Sequence
 * 
 * Returns: the new atom sequence
 * 
 * Since: 2.0.0
 */
void*
ags_lv2_plugin_alloc_atom_sequence(guint sequence_size)
{
  LV2_Atom_Sequence *aseq;
  LV2_Atom_Event *aev;

  aseq = (LV2_Atom_Sequence *) malloc(sizeof(LV2_Atom_Sequence) + sequence_size);

  aseq->atom.size = sizeof(LV2_Atom_Sequence_Body);
  aseq->atom.type = ags_lv2_urid_manager_map(NULL,
					     LV2_ATOM__Sequence);

  aseq->body.unit = ags_lv2_urid_manager_map(NULL,
					     LV2_MIDI__MidiEvent);
  aseq->body.pad = 0;

  aev = (LV2_Atom_Event*) ((char*) LV2_ATOM_CONTENTS(LV2_Atom_Sequence, aseq));
  aev->body.size = 0;  
  aev->body.type = 0;
  
  return(aseq);
}

/**
 * ags_lv2_plugin_atom_sequence_append_midi:
 * @atom_sequence: the atom sequence
 * @sequence_size: the atom sequence size
 * @events: the events to write
 * @event_count: the number of events to write
 *
 * Append MIDI data to atom sequence.
 *
 * Returns: %TRUE on success otherwise %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_lv2_plugin_atom_sequence_append_midi(void *atom_sequence,
					 guint sequence_size,
					 snd_seq_event_t *events,
					 guint event_count)
{
  AgsLv2UriMapManager *uri_map_manager;
  
  LV2_Atom_Sequence *aseq;
  LV2_Atom_Event *aev;
  
  unsigned char midi_buffer[8];

  guint count, size;
  guint padded_size;
  guint i;
  gboolean success;

  aseq = (LV2_Atom_Sequence *) atom_sequence;
  
  /* find offset */
  aev = (LV2_Atom_Event*) ((char*) LV2_ATOM_CONTENTS(LV2_Atom_Sequence, aseq));
  
  while((void *) aev < atom_sequence + sequence_size){
    if(aev->body.size == 0){
      break;
    }
    
    size = aev->body.size;
    aev += ((size + 7) & (~7));
  }
  
  /* append midi */
  success = TRUE;

  for(i = 0; i < event_count; i++){
    if((void *) aev >= atom_sequence + sequence_size){
      return(FALSE);
    }
  
    /* decode midi sequencer event */    
    if((count = ags_midi_buffer_util_decode(midi_buffer,
					    &(events[i]))) <= 8){
      aev->time.frames = 0;

      aev->body.size = count;
      aev->body.type = ags_lv2_urid_manager_map(NULL,
						LV2_MIDI__MidiEvent);

      memcpy(LV2_ATOM_BODY(&(aev->body)), midi_buffer, count * sizeof(char));

      aseq->atom.size += ((count + 7) & (~7));
      
      aev += ((count + 7) & (~7));
    }else{
      success = FALSE;

      break;
    }
  }

  /* set last empty */
  aev->body.size = 0;  
  aev->body.type = 0;
  
  return(success);
}

/**
 * ags_lv2_plugin_atom_sequence_remove_midi:
 * @atom_sequence: the atom sequence
 * @sequence_size: the atom sequence size
 * @note: the key to remove
 *
 * Remove MIDI data from atom sequence.
 *
 * Returns: %TRUE on success otherwise %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_lv2_plugin_atom_sequence_remove_midi(void *atom_sequence,
					 guint sequence_size,
					 guint note)
{
  AgsLv2UriMapManager *uri_map_manager;
  
  LV2_Atom_Sequence *aseq;
  LV2_Atom_Event *aev, *current_aev;
  
  unsigned char midi_buffer[8];

  guint count, size, current_size;
  guint padded_size;
  guint i;
  gboolean success;

  aseq = (LV2_Atom_Sequence *) atom_sequence;
  
  /* find offset */
  aev = (LV2_Atom_Event*) ((char*) LV2_ATOM_CONTENTS(LV2_Atom_Sequence, aseq));

  success = FALSE;
  
  while((void *) aev < atom_sequence + sequence_size){
    if(aev->body.size == 0){
      break;
    }

    if(((unsigned char *) LV2_ATOM_BODY(&(aev->body)))[1] == (0x7f & note)){
      current_aev = aev;
      current_size = aev->body.size;
      
      success = TRUE;

      break;
    }
    
    size = aev->body.size;
    aev += ((size + 7) & (~7));
  }
  
  /* remove midi */
  if(success){
    /* set empty */
    current_aev->body.size = 0;  
    current_aev->body.type = 0;

    memmove(current_aev,
	    current_aev + ((current_size + 7) & (~7)),
	    (atom_sequence + sequence_size) - (((void *) current_aev) + ((current_size + 7) & (~7))));
    
    memset(atom_sequence + sequence_size - ((current_size + 7) & (~7)),
	   0,
	   ((current_size + 7) & (~7)));

    aseq->atom.size -= ((current_size + 7) & (~7));    
  }
  
  return(success);
}

/**
 * ags_lv2_plugin_clear_atom_sequence:
 * @atom_sequence: the atom sequence
 * @sequence_size: size of @atom_sequence
 *
 * Clear the atom sequence.
 *
 * Since: 2.0.0 
 */
void
ags_lv2_plugin_clear_atom_sequence(void *atom_sequence,
				   guint sequence_size)
{
  memset(atom_sequence, 0, sequence_size);
}

/**
 * ags_lv2_plugin_find_pname:
 * @lv2_plugin: a #GList-struct containig #AgsLv2Plugin
 * @pname: the pname to find
 * 
 * Find pname in @lv2_plugin #GList-struct
 * 
 * Returns: the matching #GList-struct containing #AgsLv2Plugin
 * 
 * Since: 2.0.0
 */
GList*
ags_lv2_plugin_find_pname(GList *lv2_plugin,
			  gchar *pname)
{
  gboolean success;
  
  pthread_mutex_t *base_plugin_mutex;

  if(pname == NULL){
    return(NULL);
  }

  while(lv2_plugin != NULL){
    /* get base plugin mutex */
    pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
    base_plugin_mutex = AGS_BASE_PLUGIN(lv2_plugin->data)->obj_mutex;
  
    pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

    /* check pname */
    pthread_mutex_lock(base_plugin_mutex);
    
    success = (AGS_LV2_PLUGIN(lv2_plugin->data)->pname != NULL &&
	       !g_ascii_strcasecmp(pname,
				   AGS_LV2_PLUGIN(lv2_plugin->data)->pname)) ? TRUE: FALSE;

    pthread_mutex_unlock(base_plugin_mutex);
    
    if(success){
      return(lv2_plugin);
    }
    
    lv2_plugin = lv2_plugin->next;
  }

  return(NULL);
}

void
ags_lv2_plugin_real_change_program(AgsLv2Plugin *lv2_plugin,
				   gpointer lv2_handle,
				   guint bank_index,
				   guint program_index)
{
  LV2_Descriptor *plugin_descriptor;
  LV2_Programs_Interface *program_interface;

  plugin_descriptor = AGS_BASE_PLUGIN(lv2_plugin)->plugin_descriptor;
  
  if(plugin_descriptor != NULL &&
     plugin_descriptor->extension_data != NULL &&
     (program_interface = plugin_descriptor->extension_data(LV2_PROGRAMS__Interface)) != NULL){
    program_interface->select_program(((LV2_Handle *) lv2_handle)[0],
				      (uint32_t) bank_index,
				      (uint32_t) program_index);
  }
}

/**
 * ags_lv2_plugin_change_program:
 * @lv2_plugin: the #AgsLv2Plugin
 * @lv2_handle: the lv2 handle
 * @bank_index: the bank index
 * @program_index: the program index
 * 
 * Change program of @lv2_handle.
 * 
 * Since: 2.0.0
 */
void
ags_lv2_plugin_change_program(AgsLv2Plugin *lv2_plugin,
			      gpointer lv2_handle,
			      guint bank_index,
			      guint program_index)
{
  g_return_if_fail(AGS_IS_LV2_PLUGIN(lv2_plugin));
  g_object_ref(G_OBJECT(lv2_plugin));
  g_signal_emit(G_OBJECT(lv2_plugin),
		lv2_plugin_signals[CHANGE_PROGRAM], 0,
		lv2_handle,
		bank_index,
		program_index);
  g_object_unref(G_OBJECT(lv2_plugin));
}

/**
 * ags_lv2_plugin_new:
 * @turtle: the turtle
 * @filename: the plugin .so
 * @effect: the effect's string representation
 * @uri: the effect's uri
 * @effect_index: the effect's index
 *
 * Creates an #AgsLv2Plugin
 *
 * Returns: a new #AgsLv2Plugin
 *
 * Since: 2.0.0
 */
AgsLv2Plugin*
ags_lv2_plugin_new(AgsTurtle *turtle, gchar *filename, gchar *effect, gchar *uri, guint effect_index)
{
  AgsLv2Plugin *lv2_plugin;

  lv2_plugin = (AgsLv2Plugin *) g_object_new(AGS_TYPE_LV2_PLUGIN,
					     "turtle", turtle,
					     "filename", filename,
					     "effect", effect,
					     "uri", uri,
					     "effect-index", effect_index,
					     NULL);

  return(lv2_plugin);
}
