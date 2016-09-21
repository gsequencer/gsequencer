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
 * @include: ags/object/ags_lv2ui_plugin.h
 *
 * The #AgsLv2uiPlugin loads/unloads a Lv2ui plugin.
 */

enum{
  PROP_0,
  PROP_URI,
  PROP_TURTLE,
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
						   "AgsLv2uiPlugin\0",
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

  gobject->finalize = ags_lv2ui_plugin_finalize;


  /**
   * AgsLv2uiPlugin:turtle:
   *
   * The assigned turtle.
   * 
   * Since: 0.7.6
   */
  param_spec = g_param_spec_object("turtle\0",
				   "turtle of the plugin\0",
				   "The turtle this plugin is located in\0",
				   AGS_TYPE_TURTLE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TURTLE,
				  param_spec);

  /**
   * AgsLv2uiPlugin:uri:
   *
   * The assigned uri.
   * 
   * Since: 0.7.6
   */
  param_spec = g_param_spec_string("uri\0",
				   "uri of the plugin\0",
				   "The uri this plugin is located in\0",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_URI,
				  param_spec);

  
  /* AgsBasePluginClass */
  base_plugin = (AgsBasePluginClass *) lv2ui_plugin;

  base_plugin->load_plugin = ags_lv2ui_plugin_load_plugin;
}

void
ags_lv2ui_plugin_init(AgsLv2uiPlugin *lv2ui_plugin)
{
  lv2ui_plugin->flags = 0;
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
  case PROP_URI:
    {
      gchar *uri;

      uri = (gchar *) g_value_get_string(value);

      if(lv2ui_plugin->uri == uri){
	return;
      }
      
      if(lv2ui_plugin->uri != NULL){
	g_free(lv2ui_plugin->uri);
      }

      lv2ui_plugin->uri = g_strdup(uri);
    }
    break;
  case PROP_TURTLE:
    {
      AgsTurtle *turtle;

      turtle = g_value_get_object(value);

      if(lv2ui_plugin->turtle == turtle){
	return;
      }

      if(lv2ui_plugin->turtle != NULL){
	g_object_unref(lv2ui_plugin->turtle);
      }

      if(turtle != NULL){
	g_object_ref(turtle);
      }
      
      lv2ui_plugin->turtle = turtle;
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
  case PROP_URI:
    g_value_set_string(value, lv2ui_plugin->uri);
    break;
  case PROP_TURTLE:
    g_value_set_object(value, lv2ui_plugin->turtle);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_lv2ui_plugin_finalize(GObject *gobject)
{
  AgsLv2uiPlugin *lv2ui_plugin;

  lv2ui_plugin = AGS_LV2UI_PLUGIN(gobject);
}

void
ags_lv2ui_plugin_load_plugin(AgsBasePlugin *base_plugin)
{
}

/**
 * ags_lv2ui_plugin_new:
 * @turtle: the turtle
 * @filename: the plugin .so
 * @effect: the effect's string representation
 * @uri: the effect's uri
 * @effect_index: the effect's index
 *
 * Creates an #AgsLv2uiPlugin
 *
 * Returns: a new #AgsLv2uiPlugin
 *
 * Since: 0.7.6
 */
AgsLv2uiPlugin*
ags_lv2ui_plugin_new(AgsTurtle *turtle, gchar *filename, gchar *effect, gchar *uri, guint effect_index)
{
  AgsLv2uiPlugin *lv2ui_plugin;

  lv2ui_plugin = (AgsLv2uiPlugin *) g_object_new(AGS_TYPE_LV2UI_PLUGIN,
						 "turtle\0", turtle,
						 "filename\0", filename,
						 "effect\0", effect,
						 "uri\0", uri,
						 "effect-index\0", effect_index,
						 NULL);

  return(lv2ui_plugin);
}
