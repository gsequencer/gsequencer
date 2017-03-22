/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/object/ags_connectable.h>

#include <ags/plugin/ags_lv2_plugin.h>

void ags_lv2_preset_class_init(AgsLv2PresetClass *lv2_preset);
void ags_lv2_preset_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_lv2_preset_init(AgsLv2Preset *lv2_preset);
void ags_lv2_preset_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_lv2_preset_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_lv2_preset_connect(AgsConnectable *connectable);
void ags_lv2_preset_disconnect(AgsConnectable *connectable);
void ags_lv2_preset_finalize(GObject *gobject);

/**
 * SECTION:ags_lv2_preset
 * @short_description: task thread
 * @title: AgsLv2Preset
 * @section_id:
 * @include: ags/plugin/ags_lv2_preset.h
 *
 * The #AgsLv2Preset handles LV2 presets mapped to a bank.
 */

enum{
  PROP_0,
  PROP_LV2_PLUGIN,
  PROP_BANK,
  PROP_TURTLE,
};

static gpointer ags_lv2_preset_parent_class = NULL;
static AgsConnectableInterface *ags_lv2_preset_parent_connectable_interface;

GType
ags_lv2_preset_get_type()
{
  static GType ags_type_lv2_preset = 0;

  if(!ags_type_lv2_preset){
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

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_lv2_preset_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_lv2_preset = g_type_register_static(G_TYPE_OBJECT,
						 "AgsLv2Preset\0",
						 &ags_lv2_preset_info,
						 0);
    
    g_type_add_interface_static(ags_type_lv2_preset,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_lv2_preset);
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
   * Since: 0.7.122.8
   */
  param_spec = g_param_spec_object("lv2-plugin\0",
				   "lv2 plugin of the preset\0",
				   "The lv2 plugin this preset is located in\0",
				   AGS_TYPE_LV2_PLUGIN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LV2_PLUGIN,
				  param_spec);

  /**
   * AgsLv2Preset:bank:
   *
   * The assigned bank.
   * 
   * Since: 0.7.122.8
   */
  param_spec = g_param_spec_string("bank\0",
				   "bank of the preset\0",
				   "The bank this preset is assigned with\0",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BANK,
				  param_spec);

  /**
   * AgsLv2Preset:turtle:
   *
   * The assigned turtle.
   * 
   * Since: 0.7.122.8
   */
  param_spec = g_param_spec_object("turtle\0",
				   "turtle of the preset\0",
				   "The turtle this preset is located in\0",
				   AGS_TYPE_TURTLE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TURTLE,
				  param_spec);
}

void
ags_lv2_preset_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_lv2_preset_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_lv2_preset_connect;
  connectable->disconnect = ags_lv2_preset_disconnect;
}

void
ags_lv2_preset_init(AgsLv2Preset *lv2_preset)
{
  lv2_preset->flags = 0;

  lv2_preset->lv2_plugin = NULL;

  lv2_preset->bank = NULL;

  lv2_preset->turtle = NULL;
}


void
ags_lv2_preset_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsLv2Preset *lv2_preset;

  lv2_preset = AGS_LV2_PRESET(gobject);

  switch(prop_id){
  case PROP_LV2_PLUGIN:
    {
      AgsLv2Plugin *lv2_plugin;

      lv2_plugin = (AgsLv2Plugin *) g_value_get_object(value);

      if(lv2_preset->lv2_plugin == lv2_plugin){
	return;
      }

      if(lv2_preset->lv2_plugin != NULL){
	g_object_unref(lv2_preset->lv2_plugin);
      }

      if(lv2_plugin != NULL){
	g_object_ref(lv2_plugin);
      }
      
      lv2_preset->lv2_plugin = lv2_plugin;
    }
    break;
  case PROP_BANK:
    {
      gchar *bank;

      bank = (gchar *) g_value_get_string(value);

      if(lv2_preset->bank == bank){
	return;
      }
      
      if(lv2_preset->bank != NULL){
	g_free(lv2_preset->bank);
      }

      lv2_preset->bank = g_strdup(bank);
    }
    break;
  case PROP_TURTLE:
    {
      AgsTurtle *turtle;

      turtle = (AgsTurtle *) g_value_get_object(value);

      if(lv2_preset->turtle == turtle){
	return;
      }

      if(lv2_preset->turtle != NULL){
	g_object_unref(lv2_preset->turtle);
      }

      if(turtle != NULL){
	g_object_ref(turtle);
      }
      
      lv2_preset->turtle = turtle;
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

  lv2_preset = AGS_LV2_PRESET(gobject);

  switch(prop_id){
  case PROP_LV2_PLUGIN:
    {
      g_value_set_object(value, lv2_preset->lv2_plugin);
    }
    break;
  case PROP_BANK:
    {
      g_value_set_string(value, lv2_preset->bank);
    }
    break;
  case PROP_TURTLE:
    {
      g_value_set_object(value, lv2_preset->turtle);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_lv2_preset_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_lv2_preset_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_lv2_preset_finalize(GObject *gobject)
{
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_lv2_preset_parent_class)->finalize(gobject);
}

void
ags_lv2_preset_parse_turtle(AgsLv2Preset *lv2_preset)
{
  if(lv2_preset == NULL ||
     lv2_preset->turtle == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_lv2_preset_new:
 * @lv2_plugin: an #AgsLv2Plugin
 * @turtle: the #AgsTurtle
 *
 * Create a new instance of #AgsLv2Preset.
 *
 * Returns: the new #AgsLv2Preset
 *
 * Since: 0.7.122.8
 */ 
AgsLv2Preset*
ags_lv2_preset_new(GObject *lv2_plugin,
		   AgsTurtle *turtle)
{
  AgsLv2Preset *lv2_preset;

  lv2_preset = (AgsLv2Preset *) g_object_new(AGS_TYPE_LV2_PRESET,
					     "lv2-plugin\0", lv2_plugin,
					     "turtle\0", turtle,
					     NULL);

  return(lv2_preset);
}
