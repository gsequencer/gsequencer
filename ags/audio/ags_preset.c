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

#include <stdlib.h>
#include <string.h>

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
 * @short_description: presets
 * @title: AgsPreset
 * @section_id:
 * @include: ags/audio/ags_preset.h
 *
 * The #AgsPreset stores presets.
 */

static gpointer ags_preset_parent_class = NULL;

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_SCOPE,
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

    ags_type_preset = g_type_register_static(G_TYPE_OBJECT,
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
   * AgsPreset:audio:
   *
   * The #AgsAudio belonging to.
   * 
   * Since: 0.8.5
   */
  param_spec = g_param_spec_object("audio",
				   i18n_pspec("audio"),
				   i18n_pspec("The audio belonging to"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  /**
   * AgsPreset:scope:
   *
   * The preset's scope.
   * 
   * Since: 0.8.5
   */
  param_spec = g_param_spec_string("scope",
				   i18n_pspec("scope"),
				   i18n_pspec("The preset's scope"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SCOPE,
				  param_spec);

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

  preset->audio = NULL;
  
  preset->scope = NULL;
  preset->preset_name = NULL;

  preset->audio_channel_start = 0;
  preset->audio_channel_end = 0;

  preset->audio_channel_start = 0;
  preset->audio_channel_end = 0;

  preset->pad_start = 0;
  preset->pad_end = 0;
  
  preset->x_start = 0;
  preset->x_end = 0;

  preset->parameter = NULL;
  preset->n_params = 0;
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
  case PROP_AUDIO:
    {
      GObject *audio;

      audio = (GObject *) g_value_get_object(value);

      if(preset->audio == audio){
	return;
      }

      if(preset->audio != NULL){
	g_object_unref(preset->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      preset->audio = audio;
    }
    break;
  case PROP_SCOPE:
    {
      gchar *scope;
      
      scope = g_value_get_string(value);

      if(preset->scope != NULL){
	g_free(preset->scope);
      }
      
      preset->scope = g_strdup(scope);
    }
    break;
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
  case PROP_AUDIO:
    {
      g_value_set_object(value, preset->audio);
    }
    break;
  case PROP_SCOPE:
    {
      g_value_set_string(value,
			 preset->scope);
    }
    break;
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
		       preset->pad_end);
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

void
ags_preset_finalize(GObject *gobject)
{
  AgsPreset *preset;

  preset = AGS_PRESET(gobject);

  if(preset->audio != NULL){
    g_object_unref(preset->audio);
  }

  g_free(preset->scope);
  g_free(preset->preset_name);

  /* call parent */
  G_OBJECT_CLASS(ags_preset_parent_class)->finalize(gobject);
}

/**
 * ags_preset_find_scope:
 * @preset: the #GList-struct containing #AgsPreset
 * @scope: the preset's scope
 * 
 * Find preset's scope in @preset.
 * 
 * Returns: the next matching #AgsPreset
 * 
 * Since: 0.8.5
 */
GList*
ags_preset_find_scope(GList *preset,
		      gchar *scope)
{
  while(preset != NULL){
    if(!g_strcmp0(AGS_PRESET(preset->data)->scope,
		  scope)){
      return(preset);
    }

    preset = preset->next;
  }
  
  return(NULL);
}

/**
 * ags_preset_find_name:
 * @preset: the #GList-struct containing #AgsPreset
 * @preset_name: the preset's name
 * 
 * Find preset name in @preset.
 * 
 * Returns: the next matching #AgsPreset
 * 
 * Since: 0.8.5
 */
GList*
ags_preset_find_name(GList *preset,
		     gchar *preset_name)
{
  while(preset != NULL){
    if(!g_strcmp0(AGS_PRESET(preset->data)->preset_name,
		  preset_name)){
      return(preset);
    }

    preset = preset->next;
  }
  
  return(NULL);
}

/**
 * ags_preset_add_parameter:
 * @preset: the #AgsPreset
 * @param_name: the parameter name
 * @value: the value to add
 *
 * Add parameter to @preset.
 *
 * Since: 0.8.5
 */
gboolean
ags_preset_add_parameter(AgsPreset *preset,
			 gchar *param_name, GValue *value)
{
  guint nth;
  guint i;
  gboolean found;
  
  if(!AGS_IS_PRESET(preset)){
    return(FALSE);
  }

  /* match or allocate */
  found = FALSE;

  if(preset->parameter == NULL){
    preset->parameter = (GParameter *) malloc(sizeof(GParameter));

    preset->parameter[0].name = NULL;
    memset(&(preset->parameter[0].value), 0, sizeof(GValue));

    g_value_init(&(preset->parameter[0].value),
		 G_VALUE_TYPE(value));
    
    preset->n_params += 1;
    nth = 0;
  }else{
    for(i = 0; i < preset->n_params; i++){
      if(!g_strcmp0(preset->parameter[i].name,
		    param_name)){
	nth = i;
	found = TRUE;

	break;
      }
    }

    if(!found){
      preset->parameter = (GParameter *) realloc(preset->parameter,
						 (preset->n_params + 1) * sizeof(GParameter));

      preset->parameter[preset->n_params].name = NULL;
      memset(&(preset->parameter[preset->n_params].value), 0, sizeof(GValue));

      g_value_init(&(preset->parameter[preset->n_params].value),
		   G_VALUE_TYPE(value));
      
      preset->n_params += 1;
      nth = i;
    }
  }

  /* set value */  
  if(preset->parameter[nth].name != NULL){
    g_free(preset->parameter[nth].name);
  }

  preset->parameter[nth].name = g_strdup(param_name);
  g_value_copy(value,
	       &(preset->parameter[nth].value));

  return(!found);
}

/**
 * ags_preset_remove_parameter:
 * @preset: the #AgsPreset
 * @nth: the nth parameter to remove
 * 
 * Remove parameter of @preset.
 * 
 * Since: 0.8.5
 */
void
ags_preset_remove_parameter(AgsPreset *preset,
			    guint nth)
{
  GParameter *parameter;

  if(!AGS_IS_PRESET(preset) ||
     preset->n_params == 0 ||
     nth >= preset->n_params){
    return;
  }

  parameter = preset->parameter;

  if(preset->n_params == 1){
    preset->parameter = NULL;

    preset->n_params = 0;
  }else{
    preset->parameter = (GParameter *) malloc((preset->n_params - 1) * sizeof(GParameter));

    if(nth > 0){
      memcpy(preset->parameter,
	     parameter,
	     nth * sizeof(GParameter));
    }

    if(nth + 1 < preset->n_params){
      memcpy(&(preset->parameter[nth]),
	     &(parameter[nth + 1]),
	     (preset->n_params - (nth + 1)) * sizeof(GParameter));    
    }

    preset->n_params -= 1;
  }
    
  free(parameter);
}

/**
 * ags_preset_get_parameter:
 * @preset: the #AgsPreset
 * @param_name: the parameter name
 * @value: the return location of value
 * @error: the #GError-struct
 * 
 * Get parameter specified by @param_name. If parameter not available
 * the @error is set to indicate the failure.
 *
 * Since: 0.8.5
 */
void
ags_preset_get_parameter(AgsPreset *preset,
			 gchar *param_name, GValue *value,
			 GError **error)
{
  guint i;

  if(!AGS_IS_PRESET(preset)){
    return;
  }

  if(preset->parameter == NULL){
    if(error != NULL){
      g_set_error(error,
		  AGS_PRESET_ERROR,
		  AGS_PRESET_ERROR_NO_SUCH_PARAMETER,
		  "unable to find parameter: %s",
		  param_name);
    }

    return;
  }
  
  for(i = 0; i < preset->n_params; i++){
    if(!g_strcmp0(preset->parameter[i].name,
		  param_name)){
      g_value_copy(&(preset->parameter[i].value),
		   value);
      
      return;
    }
  }
  
  if(error != NULL){
    g_set_error(error,
		AGS_PRESET_ERROR,
		AGS_PRESET_ERROR_NO_SUCH_PARAMETER,
		"unable to find parameter: %s",
		param_name);
  }
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
