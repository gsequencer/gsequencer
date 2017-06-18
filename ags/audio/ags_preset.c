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

#include <ags/audio/ags_preset.h>

#include <ags/i18n.h>

void ags_preset_class_init(AgsPresetClass *preset);
void ags_preset_init (AgsPreset *preset);
void ags_preset_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec);
void ags_preset_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec);
void ags_preset_finalize(GObject *gobject);

/**
 * SECTION:ags_preset
 * @short_description: audio connections
 * @title: AgsPreset
 * @section_id:
 * @include: ags/audio/ags_preset.h
 *
 * The #AgsPreset specifies your audio connections.
 */

static gpointer ags_preset_parent_class = NULL;

enum{
  PROP_0,
  PROP_PRESET_NAME,
  PROP_AUDIO_CHANNEL_START,
  PROP_AUDIO_CHANNEL_END,
  PROP_PAD_START,
  PROP_PAD_END,
  PROP_X_START,
  PROP_X_END,
};

GType
ags_preset_get_type (void)
{
  static GType ags_type_preset = 0;

  if(!ags_type_preset){
    static const GTypeInfo ags_preset_info = {
      sizeof (AgsPresetClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_preset_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPreset),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_preset_init,
    };

    ags_type_preset = g_type_register_static(AGS_TYPE_CONNECTION,
					     "AgsPreset",
					     &ags_preset_info,
					     0);
  }

  return (ags_type_preset);
}

void
ags_preset_class_init(AgsPresetClass *preset)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_preset_parent_class = g_type_class_peek_parent(preset);

  /* GObjectClass */
  gobject = (GObjectClass *) preset;

  gobject->set_property = ags_preset_set_property;
  gobject->get_property = ags_preset_get_property;

  gobject->finalize = ags_preset_finalize;

  /* properties */
  /**
   * AgsPreset:preset-name:
   *
   * The preset name.
   * 
   * Since: 0.8.5
   */
  param_spec = g_param_spec_string("preset-name",
				   i18n_pspec("preset-name"),
				   i18n_pspec("The preset name"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PRESET_NAME,
				  param_spec);
  
  /**
   * AgsPreset:audio-channel-start:
   *
   * The start audio channel to apply.
   * 
   * Since: 0.8.5
   */
  param_spec = g_param_spec_uint("audio-channel-start",
				 i18n_pspec("audio-channel-start"),
				 i18n_pspec("The start audio channel to apply"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNEL_START,
				  param_spec);

  /**
   * AgsPreset:audio-channel-end:
   *
   * The end audio channel to apply.
   * 
   * Since: 0.8.5
   */
  param_spec = g_param_spec_uint("audio-channel-end",
				 i18n_pspec("audio-channel-end"),
				 i18n_pspec("The end audio channel to apply"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNEL_END,
				  param_spec);

  /**
   * AgsPreset:pad-start:
   *
   * The start pad to apply.
   * 
   * Since: 0.8.5
   */
  param_spec = g_param_spec_uint("pad-start",
				 i18n_pspec("pad-start"),
				 i18n_pspec("The start pad to apply"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PAD_START,
				  param_spec);

  /**
   * AgsPreset:pad-end:
   *
   * The end pad to apply.
   * 
   * Since: 0.8.5
   */
  param_spec = g_param_spec_uint("pad-end",
				 i18n_pspec("pad-end"),
				 i18n_pspec("The end pad to apply"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PAD_END,
				  param_spec);

  /**
   * AgsPreset:x-start:
   *
   * The start x to apply.
   * 
   * Since: 0.8.5
   */
  param_spec = g_param_spec_uint("x-start",
				 i18n_pspec("x-start"),
				 i18n_pspec("The start x to apply"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_X_START,
				  param_spec);

  /**
   * AgsPreset:x-end:
   *
   * The end x to apply.
   * 
   * Since: 0.8.5
   */
  param_spec = g_param_spec_uint("x-end",
				 i18n_pspec("x-end"),
				 i18n_pspec("The end x to apply"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_X_END,
				  param_spec);
}

GQuark
ags_preset_error_quark()
{
  return(g_quark_from_static_string("ags-preset-error-quark"));
}

void
ags_preset_init(AgsPreset *preset)
{
  preset->flags = 0;

  preset->preset_name = NULL;

  preset->audio_channel_start = 0;
  preset->audio_channel_end = 0;

  preset->audio_channel_start = 0;
  preset->audio_channel_end = 0;

  preset->pad_start = 0;
  preset->pad_end = 0;
  
  preset->x_start = 0;
  preset->x_end = 0;
}

void
ags_preset_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsPreset *preset;

  preset = AGS_PRESET(gobject);

  switch(prop_id){
  case PROP_PRESET_NAME:
    {
      gchar *preset_name;
      
      preset_name = g_value_get_string(value);

      if(preset->preset_name != NULL){
	g_free(preset->preset_name);
      }
      
      preset->preset_name = g_strdup(preset_name);
    }
    break;
  case PROP_AUDIO_CHANNEL_START:
    {
      preset->audio_channel_start = g_value_get_uint(value);
    }
    break;
  case PROP_AUDIO_CHANNEL_END:
    {
      preset->audio_channel_end = g_value_get_uint(value);
    }
    break;
  case PROP_PAD_START:
    {
      preset->pad_start = g_value_get_uint(value);
    }
    break;
  case PROP_PAD_END:
    {
      preset->pad_end = g_value_get_uint(value);
    }
    break;
  case PROP_X_START:
    {
      preset->x_start = g_value_get_uint(value);
    }
    break;
  case PROP_X_END:
    {
      preset->x_end = g_value_get_uint(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
  }
}

void
ags_preset_get_property(GObject *gobject,
			guint prop_id,
			GValue *value,
			GParamSpec *param_spec)
{
  AgsPreset *preset;

  preset = AGS_PRESET(gobject);

  switch(prop_id){
  case PROP_PRESET_NAME:
    {
      g_value_set_string(value,
			 preset->preset_name);
    }
    break;
  case PROP_AUDIO_CHANNEL_START:
    {
      g_value_set_uint(value,
		       preset->audio_channel_start);
    }
    break;
  case PROP_AUDIO_CHANNEL_END:
    {
      g_value_set_uint(value,
		       preset->audio_channel_end);
    }
    break;
  case PROP_PAD_START:
    {
      g_value_set_uint(value,
		       preset->pad_start);
    }
    break;
  case PROP_PAD_END:
    {
      g_value_set_uint(value,
		       preset->end);
    }
    break;
  case PROP_X_START:
    {
      g_value_set_uint(value,
		       preset->x_start);
    }
    break;
  case PROP_X_END:
    {
      g_value_set_uint(value,
		       preset->x_end);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
  }
}

GList*
ags_preset_find_name(GList *preset,
		     gchar *preset_name)
{
  //TODO:JK: implement me
  
  return(NULL);
}

void
ags_preset_add_parameter(AgsPreset *preset,
			 gchar *param_name, GValue *value)
{
  //TODO:JK: implement me
}

void
ags_preset_remove_parameter(AgsPreset *preset,
			    guint nth)
{
  //TODO:JK: implement me
}

void
ags_preset_get_parameter(AgsPreset *preset,
			 gchar *param_name, GValue *value,
			 GError *error)
{
  //TODO:JK: implement me
}

/**
 * ags_preset_new:
 *
 * Creates an #AgsPreset
 *
 * Returns: a new #AgsPreset
 *
 * Since: 0.8.5
 */
AgsPreset*
ags_preset_new()
{
  AgsPreset *preset;

  preset = (AgsPreset *) g_object_new(AGS_TYPE_PRESET,
				      NULL);

  return(preset);
}
