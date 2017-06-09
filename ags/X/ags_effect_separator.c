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

#include <ags/X/ags_effect_separator.h>

#include <ags/object/ags_connectable.h>

void ags_effect_separator_class_init(AgsEffectSeparatorClass *effect_separator);
void ags_effect_separator_init(AgsEffectSeparator *effect_separator);
void ags_effect_separator_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_effect_separator_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_effect_separator_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);

/**
 * SECTION:ags_effect_separator
 * @short_description: A effect separator widget
 * @title: AgsEffectSeparator
 * @section_id:
 * @include: ags/widget/ags_effect_separator.h
 *
 * #AgsEffectSeparator is a widget representing an separator of effects.
 */

enum{
  PROP_0,
  PROP_FILENAME,
  PROP_EFFECT,
  PROP_TEXT,
};

static gpointer ags_effect_separator_parent_class = NULL;

GType
ags_effect_separator_get_type(void)
{
  static GType ags_type_effect_separator = 0;

  if(!ags_type_effect_separator){
    static const GTypeInfo ags_effect_separator_info = {
      sizeof(AgsEffectSeparatorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_effect_separator_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsEffectSeparator),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_effect_separator_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_effect_separator_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_effect_separator = g_type_register_static(GTK_TYPE_HBOX,
						       "AgsEffectSeparator", &ags_effect_separator_info,
						       0);

    g_type_add_interface_static(ags_type_effect_separator,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_effect_separator);
}

void
ags_effect_separator_class_init(AgsEffectSeparatorClass *effect_separator)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_effect_separator_parent_class = g_type_class_peek_parent(effect_separator);

  /* GObjectClass */
  gobject = (GObjectClass *) effect_separator;

  gobject->set_property = ags_effect_separator_set_property;
  gobject->get_property = ags_effect_separator_get_property;

  /* properties */
  /**
   * AgsEffectSeparator:filename:
   *
   * The filename.
   * 
   * Since: 0.7.122.19
   */
  param_spec = g_param_spec_string("filename",
				   "filename",
				   "The filename",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  /**
   * AgsEffectSeparator:effect:
   *
   * The effect.
   * 
   * Since: 0.7.122.19
   */
  param_spec = g_param_spec_string("effect",
				   "effect",
				   "The effect",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_EFFECT,
				  param_spec);

  /**
   * AgsEffectSeparator:text:
   *
   * The text.
   * 
   * Since: 0.7.122.19
   */
  param_spec = g_param_spec_string("text",
				   "text",
				   "The text",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TEXT,
				  param_spec);

}

void
ags_effect_separator_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = NULL;
  connectable->disconnect = NULL;
}

void
ags_effect_separator_init(AgsEffectSeparator *effect_separator)
{
  GtkSeparator *separator;
  
  effect_separator->filename = NULL;
  effect_separator->effect = NULL;

  /* heading separator */
  separator = gtk_hseparator_new();
  gtk_box_pack_start(effect_separator,
		     separator,
		     TRUE, TRUE,
		     0);

  /* label */
  effect_separator->label = gtk_label_new(NULL);
  gtk_box_pack_start(effect_separator,
		     effect_separator->label,
		     FALSE, FALSE,
		     0);

  /* trailing separator */
  separator = gtk_hseparator_new();
  gtk_box_pack_start(effect_separator,
		     separator,
		     TRUE, TRUE,
		     0);
}

void
ags_effect_separator_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsEffectSeparator *effect_separator;

  effect_separator = AGS_EFFECT_SEPARATOR(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      gchar *filename;
      
      filename = g_value_get_string(value);

      if(effect_separator->filename != NULL){
	g_free(effect_separator->filename);
      }
      
      effect_separator->filename = g_strdup(filename);
    }
    break;
  case PROP_EFFECT:
    {
      gchar *effect;
      
      effect = g_value_get_string(value);

      if(effect_separator->effect != NULL){
	g_free(effect_separator->effect);
      }
      
      effect_separator->effect = g_strdup(effect);
    }
    break;
  case PROP_TEXT:
    {
      gchar *text;
      
      text = g_value_get_string(value);

      gtk_label_set_text(effect_separator->label,
			 text);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_effect_separator_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsEffectSeparator *effect_separator;

  effect_separator = AGS_EFFECT_SEPARATOR(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      g_value_set_string(value, effect_separator->filename);
    }
    break;
  case PROP_EFFECT:
    {
      g_value_set_string(value, effect_separator->effect);
    }
    break;
  case PROP_TEXT:
    {
      g_value_set_string(value,
			 gtk_label_get_text(effect_separator->label));
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

/**
 * ags_effect_separator_new:
 *
 * Creates an #AgsEffectSeparator. Note, use rather its implementation #AgsVEffectSeparator or
 * #AgsHEffectSeparator.
 *
 * Returns: a new #AgsEffectSeparator
 *
 * Since: 0.7.122.19
 */
AgsEffectSeparator*
ags_effect_separator_new()
{
  AgsEffectSeparator *effect_separator;

  effect_separator = (AgsEffectSeparator *) g_object_new(AGS_TYPE_EFFECT_SEPARATOR,
							 NULL);
  
  return(effect_separator);
}
