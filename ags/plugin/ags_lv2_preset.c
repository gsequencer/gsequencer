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

#include <ags/plugin/ags_lv2_preset.h>

#include <ags/plugin/ags_lv2_plugin.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_lv2_preset_class_init(AgsLv2PresetClass *lv2_preset);
void ags_lv2_preset_init(AgsLv2Preset *lv2_preset);
void ags_lv2_preset_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_lv2_preset_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_lv2_preset_finalize(GObject *gobject);

/**
 * SECTION:ags_lv2_preset
 * @short_description: LV2 presets
 * @title: AgsLv2Preset
 * @section_id:
 * @include: ags/plugin/ags_lv2_preset.h
 *
 * The #AgsLv2Preset handles LV2 presets mapped to a bank.
 */

enum{
  PROP_0,
  PROP_LV2_PLUGIN,
  PROP_URI,
  PROP_APPLIES_TO,
  PROP_BANK,
  PROP_PRESET_LABEL,
  PROP_MANIFEST,
  PROP_TURTLE,
  PROP_PORT_PRESET,
};

static gpointer ags_lv2_preset_parent_class = NULL;

GType
ags_lv2_preset_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_lv2_preset = 0;

    static const GTypeInfo ags_lv2_preset_info = {
      sizeof (AgsLv2PresetClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_lv2_preset_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLv2Preset),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_lv2_preset_init,
    };
    
    ags_type_lv2_preset = g_type_register_static(G_TYPE_OBJECT,
						 "AgsLv2Preset",
						 &ags_lv2_preset_info,
						 0);    

    g_once_init_leave(&g_define_type_id__volatile, ags_type_lv2_preset);
  }

  return g_define_type_id__volatile;
}

GType
ags_lv2_preset_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_LV2_PRESET_CONNECTED, "AGS_LV2_PRESET_CONNECTED", "lv2-preset-connected" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsLv2PresetFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
}

void
ags_lv2_preset_class_init(AgsLv2PresetClass *lv2_preset)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_lv2_preset_parent_class = g_type_class_peek_parent(lv2_preset);

  /* GObject */
  gobject = (GObjectClass *) lv2_preset;

  gobject->set_property = ags_lv2_preset_set_property;
  gobject->get_property = ags_lv2_preset_get_property;

  gobject->finalize = ags_lv2_preset_finalize;

  /* properties */
  /**
   * AgsLv2Preset:lv2-plugin:
   *
   * The assigned lv2 plugin.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("lv2-plugin",
				   i18n_pspec("lv2 plugin of the preset"),
				   i18n_pspec("The lv2 plugin this preset is located in"),
				   AGS_TYPE_LV2_PLUGIN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LV2_PLUGIN,
				  param_spec);

  /**
   * AgsLv2Preset:uri:
   *
   * The assigned uri.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("uri",
				   i18n_pspec("uri of the preset"),
				   i18n_pspec("The uri this preset is located in"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_URI,
				  param_spec);

  /**
   * AgsLv2Preset:applies-to:
   *
   * The assigned applies to.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("applies-to",
				   i18n_pspec("applies to plugin"),
				   i18n_pspec("The applies to plugin"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLIES_TO,
				  param_spec);

  /**
   * AgsLv2Preset:bank:
   *
   * The assigned bank.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("bank",
				   i18n_pspec("bank of the preset"),
				   i18n_pspec("The bank this preset is assigned with"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BANK,
				  param_spec);

  /**
   * AgsLv2Preset:preset-label:
   *
   * The preset label.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("preset-label",
				   i18n_pspec("preset label"),
				   i18n_pspec("The preset label"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PRESET_LABEL,
				  param_spec);

  /**
   * AgsLv2Preset:manifest:
   *
   * The assigned manifest.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("manifest",
				   i18n_pspec("manifest of the preset"),
				   i18n_pspec("The manifest this preset is referred by"),
				   AGS_TYPE_TURTLE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MANIFEST,
				  param_spec);

  /**
   * AgsLv2Preset:turtle:
   *
   * The assigned turtle.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("turtle",
				   i18n_pspec("turtle of the preset"),
				   i18n_pspec("The turtle this preset is located in"),
				   AGS_TYPE_TURTLE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TURTLE,
				  param_spec);
  
  /**
   * AgsLv2Preset:port-preset:
   *
   * The port preset.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("port-preset",
				    i18n_pspec("port-preset of the preset"),
				    i18n_pspec("The port-preset this preset is located in"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT_PRESET,
				  param_spec);
}

void
ags_lv2_preset_init(AgsLv2Preset *lv2_preset)
{
  lv2_preset->flags = 0;

  /* add lv2 preset mutex */
  g_rec_mutex_init(&(lv2_preset->obj_mutex));
  
  lv2_preset->lv2_plugin = NULL;

  lv2_preset->uri = NULL;

  lv2_preset->applies_to = NULL;

  lv2_preset->bank = NULL;
  lv2_preset->preset_label = NULL;

  lv2_preset->manifest = NULL;
  lv2_preset->turtle = NULL;

  lv2_preset->port_preset = NULL;
}


void
ags_lv2_preset_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsLv2Preset *lv2_preset;

  GRecMutex *lv2_preset_mutex;

  lv2_preset = AGS_LV2_PRESET(gobject);

  /* get base plugin mutex */
  lv2_preset_mutex = AGS_LV2_PRESET_GET_OBJ_MUTEX(lv2_preset);

  switch(prop_id){
  case PROP_LV2_PLUGIN:
    {
      AgsLv2Plugin *lv2_plugin;

      lv2_plugin = (AgsLv2Plugin *) g_value_get_object(value);

      g_rec_mutex_lock(lv2_preset_mutex);
      
      if(lv2_preset->lv2_plugin == (GObject *) lv2_plugin){
	g_rec_mutex_unlock(lv2_preset_mutex);

	return;
      }

      if(lv2_preset->lv2_plugin != NULL){
	g_object_unref(lv2_preset->lv2_plugin);
      }

      if(lv2_plugin != NULL){
	g_object_ref(lv2_plugin);
      }
      
      lv2_preset->lv2_plugin = lv2_plugin;

      g_rec_mutex_unlock(lv2_preset_mutex);
    }
    break;
  case PROP_URI:
    {
      gchar *uri;

      uri = (gchar *) g_value_get_string(value);

      g_rec_mutex_lock(lv2_preset_mutex);

      if(lv2_preset->uri == uri){
	g_rec_mutex_unlock(lv2_preset_mutex);

	return;
      }
      
      if(lv2_preset->uri != NULL){
	g_free(lv2_preset->uri);
      }

      lv2_preset->uri = g_strdup(uri);

      g_rec_mutex_unlock(lv2_preset_mutex);
    }
    break;
  case PROP_APPLIES_TO:
    {
      gchar *applies_to;

      applies_to = (gchar *) g_value_get_string(value);

      g_rec_mutex_lock(lv2_preset_mutex);

      if(lv2_preset->applies_to == applies_to){
	g_rec_mutex_unlock(lv2_preset_mutex);

	return;
      }
      
      if(lv2_preset->applies_to != NULL){
	g_free(lv2_preset->applies_to);
      }

      lv2_preset->applies_to = g_strdup(applies_to);

      g_rec_mutex_unlock(lv2_preset_mutex);
    }
    break;
  case PROP_BANK:
    {
      gchar *bank;

      bank = (gchar *) g_value_get_string(value);

      g_rec_mutex_lock(lv2_preset_mutex);

      if(lv2_preset->bank == bank){
	g_rec_mutex_unlock(lv2_preset_mutex);

	return;
      }
      
      if(lv2_preset->bank != NULL){
	g_free(lv2_preset->bank);
      }

      lv2_preset->bank = g_strdup(bank);

      g_rec_mutex_unlock(lv2_preset_mutex);
    }
    break;
  case PROP_PRESET_LABEL:
    {
      gchar *preset_label;

      preset_label = (gchar *) g_value_get_string(value);

      g_rec_mutex_lock(lv2_preset_mutex);

      if(lv2_preset->preset_label == preset_label){
	g_rec_mutex_unlock(lv2_preset_mutex);

	return;
      }
      
      if(lv2_preset->preset_label != NULL){
	g_free(lv2_preset->preset_label);
      }

      lv2_preset->preset_label = g_strdup(preset_label);

      g_rec_mutex_unlock(lv2_preset_mutex);
    }
    break;
  case PROP_MANIFEST:
    {
      AgsTurtle *manifest;

      manifest = (AgsTurtle *) g_value_get_object(value);

      g_rec_mutex_lock(lv2_preset_mutex);

      if(lv2_preset->manifest == manifest){
	g_rec_mutex_unlock(lv2_preset_mutex);

	return;
      }

      if(lv2_preset->manifest != NULL){
	g_object_unref(lv2_preset->manifest);
      }

      if(manifest != NULL){
	g_object_ref(manifest);
      }
      
      lv2_preset->manifest = manifest;

      g_rec_mutex_unlock(lv2_preset_mutex);
    }
    break;
  case PROP_TURTLE:
    {
      AgsTurtle *turtle;

      turtle = (AgsTurtle *) g_value_get_object(value);

      g_rec_mutex_lock(lv2_preset_mutex);

      if(lv2_preset->turtle == turtle){
	g_rec_mutex_unlock(lv2_preset_mutex);

	return;
      }

      if(lv2_preset->turtle != NULL){
	g_object_unref(lv2_preset->turtle);
      }

      if(turtle != NULL){
	g_object_ref(turtle);
      }
      
      lv2_preset->turtle = turtle;

      g_rec_mutex_unlock(lv2_preset_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_lv2_preset_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsLv2Preset *lv2_preset;

  GRecMutex *lv2_preset_mutex;

  lv2_preset = AGS_LV2_PRESET(gobject);

  /* get base plugin mutex */
  lv2_preset_mutex = AGS_LV2_PRESET_GET_OBJ_MUTEX(lv2_preset);

  switch(prop_id){
  case PROP_LV2_PLUGIN:
    {
      g_rec_mutex_lock(lv2_preset_mutex);

      g_value_set_object(value, lv2_preset->lv2_plugin);

      g_rec_mutex_unlock(lv2_preset_mutex);
    }
    break;
  case PROP_URI:
    {
      g_rec_mutex_lock(lv2_preset_mutex);

      g_value_set_string(value, lv2_preset->uri);

      g_rec_mutex_unlock(lv2_preset_mutex);
    }
    break;
  case PROP_APPLIES_TO:
    {
      g_rec_mutex_lock(lv2_preset_mutex);

      g_value_set_string(value, lv2_preset->applies_to);

      g_rec_mutex_unlock(lv2_preset_mutex);
    }
    break;
  case PROP_BANK:
    {
      g_rec_mutex_lock(lv2_preset_mutex);

      g_value_set_string(value, lv2_preset->bank);

      g_rec_mutex_unlock(lv2_preset_mutex);
    }
    break;
  case PROP_PRESET_LABEL:
    {
      g_rec_mutex_lock(lv2_preset_mutex);

      g_value_set_string(value, lv2_preset->preset_label);

      g_rec_mutex_unlock(lv2_preset_mutex);
    }
    break;
  case PROP_MANIFEST:
    {
      g_rec_mutex_lock(lv2_preset_mutex);

      g_value_set_object(value, lv2_preset->manifest);

      g_rec_mutex_unlock(lv2_preset_mutex);
    }
    break;
  case PROP_TURTLE:
    {
      g_rec_mutex_lock(lv2_preset_mutex);

      g_value_set_object(value, lv2_preset->turtle);

      g_rec_mutex_unlock(lv2_preset_mutex);
    }
    break;
  case PROP_PORT_PRESET:
    {
      g_rec_mutex_lock(lv2_preset_mutex);
      
      g_value_set_pointer(value, g_list_copy(lv2_preset->port_preset));

      g_rec_mutex_unlock(lv2_preset_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_lv2_preset_finalize(GObject *gobject)
{
  AgsLv2Preset *lv2_preset;

  lv2_preset = AGS_LV2_PRESET(gobject);

  /* lv2 plugin */
  if(lv2_preset->lv2_plugin != NULL){
    g_object_unref(lv2_preset->lv2_plugin);
  }

  /* uri */
  if(lv2_preset->uri != NULL){
    free(lv2_preset->uri);
  }

  /* applies to */
  if(lv2_preset->applies_to != NULL){
    free(lv2_preset->applies_to);
  }

  /* bank and preset label */
  if(lv2_preset->bank != NULL){
    free(lv2_preset->bank);
  }
  
  if(lv2_preset->preset_label != NULL){
    free(lv2_preset->preset_label);
  }

  /* turtle */
  if(lv2_preset->turtle != NULL){
    g_object_unref(lv2_preset->turtle);
  }

  /* port preset */
  if(lv2_preset->port_preset != NULL){
    g_list_free_full(lv2_preset->port_preset,
		     (GDestroyNotify) ags_lv2_port_preset_free);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_lv2_preset_parent_class)->finalize(gobject);
}

/**
 * ags_lv2_port_preset_alloc:
 * @port_symbol: the port symbol
 * @port_type: the port type
 * 
 * Allocated #AgsLv2PortPreset-struct.
 * 
 * Returns: (type gpointer) (transfer none): the new #AgsLv2PortPreset-struct.
 * 
 * Since: 3.0.0
 */
AgsLv2PortPreset*
ags_lv2_port_preset_alloc(gchar *port_symbol,
			  GType port_type)
{
  AgsLv2PortPreset *lv2_port_preset;

  lv2_port_preset = (AgsLv2PortPreset *) malloc(sizeof(AgsLv2PortPreset));

  lv2_port_preset->port_symbol = port_symbol;

  lv2_port_preset->port_value = g_new0(GValue,
				       1);
  
  if(port_type != G_TYPE_NONE){
    g_value_init(lv2_port_preset->port_value,
		 port_type);
  }

  return(lv2_port_preset);
}

/**
 * ags_lv2_port_preset_free:
 * @lv2_port_preset: (type gpointer) (transfer none): the #AgsLv2PortPreset-struct
 * 
 * Free @lv2_port_preset.
 * 
 * Since: 3.0.0
 */
void
ags_lv2_port_preset_free(AgsLv2PortPreset *lv2_port_preset)
{
  if(lv2_port_preset == NULL){
    return;
  }
  
  if(lv2_port_preset->port_symbol != NULL){
    free(lv2_port_preset->port_symbol);
  }
  
  if(lv2_port_preset->port_value != NULL){
    g_value_unset(lv2_port_preset->port_value);

    free(lv2_port_preset->port_value);
  }
  
  free(lv2_port_preset);
}

/**
 * ags_lv2_preset_parse_turtle:
 * @lv2_preset: the #AgsLv2Preset
 * 
 * Parse @lv2_preset.
 * 
 * Since: 3.0.0
 */
void
ags_lv2_preset_parse_turtle(AgsLv2Preset *lv2_preset)
{
  //NOTE:JK: deprecated
}

/**
 * ags_lv2_preset_find_preset_uri:
 * @lv2_preset: (element-type AgsAudio.Lv2Preset) (transfer none): the #GList-struct containing #AgsLv2Preset
 * @preset_uri: the preset URI
 * 
 * Find @preset_uri within @lv2_preset.
 * 
 * Returns: (element-type AgsAudio.Lv2Preset) (transfer none): the matching #GList-struct containing #AgsLv2Preset
 * 
 * Since: 3.0.0
 */
GList*
ags_lv2_preset_find_preset_uri(GList *lv2_preset,
			       gchar *preset_uri)
{
  if(preset_uri == NULL){
    return(NULL);
  }

  while(lv2_preset != NULL){
    if(AGS_LV2_PRESET(lv2_preset->data)->uri != NULL &&
       !g_ascii_strcasecmp(preset_uri,
			   AGS_LV2_PRESET(lv2_preset->data)->uri)){
      return(lv2_preset);
    }
    
    lv2_preset = lv2_preset->next;
  }
  
  return(NULL);
}

/**
 * ags_lv2_preset_find_preset_label:
 * @lv2_preset: (element-type AgsAudio.Lv2Preset) (transfer none): the #GList-struct containing #AgsLv2Preset
 * @preset_label: the preset label
 * 
 * Find @preset_label within @lv2_preset.
 * 
 * Returns: (element-type AgsAudio.Lv2Preset) (transfer none): the matching #GList-struct containing #AgsLv2Preset
 * 
 * Since: 3.0.0
 */
GList*
ags_lv2_preset_find_preset_label(GList *lv2_preset,
				 gchar *preset_label)
{
  if(preset_label == NULL){
    return(NULL);
  }

  while(lv2_preset != NULL){
    if(!g_strcmp0(preset_label,
		  AGS_LV2_PRESET(lv2_preset->data)->preset_label)){
      return(lv2_preset);
    }

    lv2_preset = lv2_preset->next;
  }
  
  return(NULL);
}

/**
 * ags_lv2_preset_new:
 * @lv2_plugin: an #AgsLv2Plugin
 * @turtle: the #AgsTurtle
 * @uri: the URI as string
 * 
 * Create a new instance of #AgsLv2Preset.
 *
 * Returns: the new #AgsLv2Preset
 *
 * Since: 3.0.0
 */ 
AgsLv2Preset*
ags_lv2_preset_new(GObject *lv2_plugin,
		   AgsTurtle *turtle,
		   gchar *uri)
{
  AgsLv2Preset *lv2_preset;

  lv2_preset = (AgsLv2Preset *) g_object_new(AGS_TYPE_LV2_PRESET,
					     "lv2-plugin", lv2_plugin,
					     "turtle", turtle,
					     "uri", uri,
					     NULL);

  return(lv2_preset);
}
