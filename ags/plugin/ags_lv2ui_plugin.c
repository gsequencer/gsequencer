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

gpointer ags_lv2ui_plugin_instantiate(AgsBasePlugin *base_plugin,
				      guint samplerate);
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
  static GType ags_type_lv2ui_plugin = 0;

  if(!ags_type_lv2ui_plugin){
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
  }

  return (ags_type_lv2ui_plugin);
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
   * Since: 0.7.127
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
   * Since: 0.7.6
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
   * Since: 0.7.127
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
   * Since: 0.7.127
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

  lv2ui_plugin = AGS_LV2UI_PLUGIN(gobject);

  switch(prop_id){
  case PROP_GUI_URI:
    {
      gchar *gui_uri;

      gui_uri = (gchar *) g_value_get_string(value);

      if(lv2ui_plugin->gui_uri == gui_uri){
	return;
      }
      
      if(lv2ui_plugin->gui_uri != NULL){
	g_free(lv2ui_plugin->gui_uri);
      }

      lv2ui_plugin->gui_uri = g_strdup(gui_uri);
    }
    break;
  case PROP_MANIFEST:
    {
      AgsTurtle *manifest;

      manifest = (AgsTurtle *) g_value_get_object(value);

      if(lv2ui_plugin->manifest == manifest){
	return;
      }

      if(lv2ui_plugin->manifest != NULL){
	g_object_unref(lv2ui_plugin->manifest);
      }

      if(manifest != NULL){
	g_object_ref(manifest);
      }
      
      lv2ui_plugin->manifest = manifest;
    }
    break;
  case PROP_GUI_TURTLE:
    {
      AgsTurtle *gui_turtle;

      gui_turtle = (AgsTurtle *) g_value_get_object(value);

      if(lv2ui_plugin->gui_turtle == gui_turtle){
	return;
      }

      if(lv2ui_plugin->gui_turtle != NULL){
	g_object_unref(lv2ui_plugin->gui_turtle);
      }

      if(gui_turtle != NULL){
	g_object_ref(gui_turtle);
      }
      
      lv2ui_plugin->gui_turtle = gui_turtle;
    }
    break;
  case PROP_LV2_PLUGIN:
    {
      GObject *lv2_plugin;

      lv2_plugin = (GObject *) g_value_get_object(value);

      if(lv2ui_plugin->lv2_plugin == lv2_plugin){
	return;
      }

      if(lv2ui_plugin->lv2_plugin != NULL){
	g_object_unref(lv2ui_plugin->lv2_plugin);
      }

      if(lv2_plugin != NULL){
	g_object_ref(lv2_plugin);
      }
      
      lv2ui_plugin->lv2_plugin = lv2_plugin;
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

  lv2ui_plugin = AGS_LV2UI_PLUGIN(gobject);

  switch(prop_id){
  case PROP_GUI_URI:
    {
      g_value_set_string(value, lv2ui_plugin->gui_uri);
    }
    break;
  case PROP_MANIFEST:
    {
      g_value_set_object(value, lv2ui_plugin->manifest);
    }
    break;
  case PROP_GUI_TURTLE:
    {
      g_value_set_object(value, lv2ui_plugin->gui_turtle);
    }
    break;
  case PROP_LV2_PLUGIN:
    {
      g_value_set_object(value, lv2ui_plugin->lv2_plugin);
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
  //TODO:JK: implement me  
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
 * Since: 0.7.127
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
 * Since: 0.7.6
 */
AgsLv2uiPlugin*
ags_lv2ui_plugin_new(AgsTurtle *gui_turtle, gchar *filename, gchar *effect, gchar *gui_uri, guint effect_index)
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
