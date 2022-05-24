/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_preset = 0;

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

    g_once_init_leave(&g_define_type_id__volatile, ags_type_preset);
  }

  return g_define_type_id__volatile;
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
  preset->connectable_flags = 0;

  /* preset mutex */
  g_rec_mutex_init(&(preset->obj_mutex));

  /* common fields */
  preset->audio = NULL;
  
  preset->scope = NULL;
  preset->preset_name = NULL;

  /* insets */
  preset->audio_channel_start = 0;
  preset->audio_channel_end = 0;

  preset->audio_channel_start = 0;
  preset->audio_channel_end = 0;

  preset->pad_start = 0;
  preset->pad_end = 0;
  
  preset->x_start = 0;
  preset->x_end = 0;

  /* preset value */
  preset->n_params = 0;
  preset->parameter_name = NULL;
  preset->value = NULL;
}

void
ags_preset_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsPreset *preset;

  GRecMutex *preset_mutex;

  preset = AGS_PRESET(gobject);

  /* get preset mutex */
  preset_mutex = AGS_PRESET_GET_OBJ_MUTEX(preset);

  switch(prop_id){
  case PROP_AUDIO:
    {
      GObject *audio;

      audio = (GObject *) g_value_get_object(value);

      g_rec_mutex_lock(preset_mutex);

      if(preset->audio == audio){
	g_rec_mutex_unlock(preset_mutex);

	return;
      }

      if(preset->audio != NULL){
	g_object_unref(preset->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      preset->audio = audio;

      g_rec_mutex_unlock(preset_mutex);
    }
    break;
  case PROP_SCOPE:
    {
      gchar *scope;
      
      scope = g_value_get_string(value);

      g_rec_mutex_lock(preset_mutex);

      if(preset->scope != NULL){
	g_free(preset->scope);
      }
      
      preset->scope = g_strdup(scope);

      g_rec_mutex_unlock(preset_mutex);
    }
    break;
  case PROP_PRESET_NAME:
    {
      gchar *preset_name;
      
      preset_name = g_value_get_string(value);

      g_rec_mutex_lock(preset_mutex);

      if(preset->preset_name != NULL){
	g_free(preset->preset_name);
      }
      
      preset->preset_name = g_strdup(preset_name);

      g_rec_mutex_unlock(preset_mutex);
    }
    break;
  case PROP_AUDIO_CHANNEL_START:
    {
      g_rec_mutex_lock(preset_mutex);

      preset->audio_channel_start = g_value_get_uint(value);

      g_rec_mutex_unlock(preset_mutex);
    }
    break;
  case PROP_AUDIO_CHANNEL_END:
    {
      g_rec_mutex_lock(preset_mutex);

      preset->audio_channel_end = g_value_get_uint(value);

      g_rec_mutex_unlock(preset_mutex);
    }
    break;
  case PROP_PAD_START:
    {
      g_rec_mutex_lock(preset_mutex);

      preset->pad_start = g_value_get_uint(value);

      g_rec_mutex_unlock(preset_mutex);
    }
    break;
  case PROP_PAD_END:
    {
      g_rec_mutex_lock(preset_mutex);

      preset->pad_end = g_value_get_uint(value);

      g_rec_mutex_unlock(preset_mutex);
    }
    break;
  case PROP_X_START:
    {
      g_rec_mutex_lock(preset_mutex);

      preset->x_start = g_value_get_uint(value);

      g_rec_mutex_unlock(preset_mutex);
    }
    break;
  case PROP_X_END:
    {
      g_rec_mutex_lock(preset_mutex);

      preset->x_end = g_value_get_uint(value);

      g_rec_mutex_unlock(preset_mutex);
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

  GRecMutex *preset_mutex;

  preset = AGS_PRESET(gobject);

  /* get preset mutex */
  preset_mutex = AGS_PRESET_GET_OBJ_MUTEX(preset);

  switch(prop_id){
  case PROP_AUDIO:
    {
      g_rec_mutex_lock(preset_mutex);

      g_value_set_object(value, preset->audio);

      g_rec_mutex_unlock(preset_mutex);
    }
    break;
  case PROP_SCOPE:
    {
      g_rec_mutex_lock(preset_mutex);

      g_value_set_string(value,
			 preset->scope);

      g_rec_mutex_unlock(preset_mutex);
    }
    break;
  case PROP_PRESET_NAME:
    {
      g_rec_mutex_lock(preset_mutex);

      g_value_set_string(value,
			 preset->preset_name);

      g_rec_mutex_unlock(preset_mutex);
    }
    break;
  case PROP_AUDIO_CHANNEL_START:
    {
      g_rec_mutex_lock(preset_mutex);

      g_value_set_uint(value,
		       preset->audio_channel_start);

      g_rec_mutex_unlock(preset_mutex);
    }
    break;
  case PROP_AUDIO_CHANNEL_END:
    {
      g_rec_mutex_lock(preset_mutex);

      g_value_set_uint(value,
		       preset->audio_channel_end);

      g_rec_mutex_unlock(preset_mutex);
    }
    break;
  case PROP_PAD_START:
    {
      g_rec_mutex_lock(preset_mutex);

      g_value_set_uint(value,
		       preset->pad_start);

      g_rec_mutex_unlock(preset_mutex);
    }
    break;
  case PROP_PAD_END:
    {
      g_rec_mutex_lock(preset_mutex);

      g_value_set_uint(value,
		       preset->pad_end);

      g_rec_mutex_unlock(preset_mutex);
    }
    break;
  case PROP_X_START:
    {
      g_rec_mutex_lock(preset_mutex);

      g_value_set_uint(value,
		       preset->x_start);

      g_rec_mutex_unlock(preset_mutex);
    }
    break;
  case PROP_X_END:
    {
      g_rec_mutex_lock(preset_mutex);

      g_value_set_uint(value,
		       preset->x_end);

      g_rec_mutex_unlock(preset_mutex);
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

  guint i;
  
  preset = AGS_PRESET(gobject);

  if(preset->audio != NULL){
    g_object_unref(preset->audio);
  }

  g_free(preset->scope);
  g_free(preset->preset_name);

  /* paramenter name */
  g_strfreev(preset->parameter_name);
  
  /* unset value */
  for(i = 0; i < preset->n_params; i++){
    g_value_unset(&(preset->value[i]));
  }

  g_free(preset->value);
  
  /* call parent */
  G_OBJECT_CLASS(ags_preset_parent_class)->finalize(gobject);
}

/**
 * ags_preset_get_obj_mutex:
 * @preset: the #AgsPreset
 * 
 * Get object mutex.
 * 
 * Returns: the #GRecMutex to lock @preset
 * 
 * Since: 3.1.0
 */
GRecMutex*
ags_preset_get_obj_mutex(AgsPreset *preset)
{
  if(!AGS_IS_PRESET(preset)){
    return(NULL);
  }

  return(AGS_PRESET_GET_OBJ_MUTEX(preset));
}

/**
 * ags_preset_test_flags:
 * @preset: the #AgsPreset
 * @flags: the flags
 *
 * Test @flags to be set on @preset.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_preset_test_flags(AgsPreset *preset, guint flags)
{
  gboolean retval;  
  
  GRecMutex *preset_mutex;

  if(!AGS_IS_PRESET(preset)){
    return(FALSE);
  }

  /* get preset mutex */
  preset_mutex = AGS_PRESET_GET_OBJ_MUTEX(preset);

  /* test */
  g_rec_mutex_lock(preset_mutex);

  retval = (flags & (preset->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(preset_mutex);

  return(retval);
}

/**
 * ags_preset_set_flags:
 * @preset: the #AgsPreset
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 3.0.0
 */
void
ags_preset_set_flags(AgsPreset *preset, guint flags)
{
  GRecMutex *preset_mutex;

  if(!AGS_IS_PRESET(preset)){
    return;
  }

  /* get preset mutex */
  preset_mutex = AGS_PRESET_GET_OBJ_MUTEX(preset);

  /* set flags */
  g_rec_mutex_lock(preset_mutex);

  preset->flags |= flags;

  g_rec_mutex_unlock(preset_mutex);
}

/**
 * ags_preset_unset_flags:
 * @preset: the #AgsPreset
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 3.0.0
 */
void
ags_preset_unset_flags(AgsPreset *preset, guint flags)
{
  GRecMutex *preset_mutex;

  if(!AGS_IS_PRESET(preset)){
    return;
  }

  /* get preset mutex */
  preset_mutex = AGS_PRESET_GET_OBJ_MUTEX(preset);

  /* set flags */
  g_rec_mutex_lock(preset_mutex);

  preset->flags &= (~flags);

  g_rec_mutex_unlock(preset_mutex);
}

/**
 * ags_preset_get_audio:
 * @preset: the #AgsPreset
 * 
 * Get audio.
 * 
 * Returns: (transfer full): the #AgsAudio
 * 
 * Since: 3.1.0
 */
GObject*
ags_preset_get_audio(AgsPreset *preset)
{
  GObject *audio;

  if(!AGS_IS_PRESET(preset)){
    return(NULL);
  }

  g_object_get(preset,
	       "audio", &audio,
	       NULL);

  return(audio);
}

/**
 * ags_preset_set_audio:
 * @preset: the #AgsPreset
 * @audio: the #AgsAudio
 * 
 * Set audio.
 * 
 * Since: 3.1.0
 */
void
ags_preset_set_audio(AgsPreset *preset, GObject *audio)
{
  if(!AGS_IS_PRESET(preset)){
    return;
  }

  g_object_set(preset,
	       "audio", audio,
	       NULL);
}

/**
 * ags_preset_get_scope:
 * @preset: the #AgsPreset
 * 
 * Get scope.
 * 
 * Returns: the scope
 * 
 * Since: 3.1.0
 */
gchar*
ags_preset_get_scope(AgsPreset *preset)
{
  gchar *scope;

  if(!AGS_IS_PRESET(preset)){
    return(NULL);
  }

  g_object_get(preset,
	       "scope", &scope,
	       NULL);

  return(scope);
}

/**
 * ags_preset_set_scope:
 * @preset: the #AgsPreset
 * @scope: the scope
 * 
 * Set scope.
 * 
 * Since: 3.1.0
 */
void
ags_preset_set_scope(AgsPreset *preset,
		     gchar *scope)
{
  if(!AGS_IS_PRESET(preset)){
    return;
  }

  g_object_set(preset,
	       "scope", scope,
	       NULL);
}

/**
 * ags_preset_get_preset_name:
 * @preset: the #AgsPreset
 * 
 * Get preset name.
 * 
 * Returns: the preset name
 * 
 * Since: 3.1.0
 */
gchar*
ags_preset_get_preset_name(AgsPreset *preset)
{
  gchar *preset_name;

  if(!AGS_IS_PRESET(preset)){
    return(NULL);
  }

  g_object_get(preset,
	       "preset-name", &preset_name,
	       NULL);

  return(preset_name);
}

/**
 * ags_preset_set_preset_name:
 * @preset: the #AgsPreset
 * @preset_name: the preset name
 * 
 * Set preset name.
 * 
 * Since: 3.1.0
 */
void
ags_preset_set_preset_name(AgsPreset *preset,
			   gchar *preset_name)
{
  if(!AGS_IS_PRESET(preset)){
    return;
  }

  g_object_set(preset,
	       "preset-name", preset_name,
	       NULL);
}

/**
 * ags_preset_get_audio_channel_start:
 * @preset: the #AgsPreset
 *
 * Gets audio channel start.
 * 
 * Returns: the audio channel start
 * 
 * Since: 3.1.0
 */
guint
ags_preset_get_audio_channel_start(AgsPreset *preset)
{
  guint audio_channel_start;
  
  if(!AGS_IS_PRESET(preset)){
    return(0);
  }

  g_object_get(preset,
	       "audio-channel-start", &audio_channel_start,
	       NULL);

  return(audio_channel_start);
}

/**
 * ags_preset_set_audio_channel_start:
 * @preset: the #AgsPreset
 * @audio_channel_start: the audio channel start
 *
 * Sets audio channel start.
 * 
 * Since: 3.1.0
 */
void
ags_preset_set_audio_channel_start(AgsPreset *preset, guint audio_channel_start)
{
  if(!AGS_IS_PRESET(preset)){
    return;
  }

  g_object_set(preset,
	       "audio-channel-start", audio_channel_start,
	       NULL);
}

/**
 * ags_preset_get_audio_channel_end:
 * @preset: the #AgsPreset
 *
 * Gets audio channel end.
 * 
 * Returns: the audio channel end
 * 
 * Since: 3.1.0
 */
guint
ags_preset_get_audio_channel_end(AgsPreset *preset)
{
  guint audio_channel_end;
  
  if(!AGS_IS_PRESET(preset)){
    return(0);
  }

  g_object_get(preset,
	       "audio-channel-end", &audio_channel_end,
	       NULL);

  return(audio_channel_end);
}

/**
 * ags_preset_set_audio_channel_end:
 * @preset: the #AgsPreset
 * @audio_channel_end: the audio channel end
 *
 * Sets audio channel end.
 * 
 * Since: 3.1.0
 */
void
ags_preset_set_audio_channel_end(AgsPreset *preset, guint audio_channel_end)
{
  if(!AGS_IS_PRESET(preset)){
    return;
  }

  g_object_set(preset,
	       "audio-channel-end", audio_channel_end,
	       NULL);
}

/**
 * ags_preset_get_pad_start:
 * @preset: the #AgsPreset
 *
 * Gets pad start.
 * 
 * Returns: the pad start
 * 
 * Since: 3.1.0
 */
guint
ags_preset_get_pad_start(AgsPreset *preset)
{
  guint pad_start;
  
  if(!AGS_IS_PRESET(preset)){
    return(0);
  }

  g_object_get(preset,
	       "pad-start", &pad_start,
	       NULL);

  return(pad_start);
}

/**
 * ags_preset_set_pad_start:
 * @preset: the #AgsPreset
 * @pad_start: the pad start
 *
 * Sets pad start.
 * 
 * Since: 3.1.0
 */
void
ags_preset_set_pad_start(AgsPreset *preset, guint pad_start)
{
  if(!AGS_IS_PRESET(preset)){
    return;
  }

  g_object_set(preset,
	       "pad-start", pad_start,
	       NULL);
}

/**
 * ags_preset_get_pad_end:
 * @preset: the #AgsPreset
 *
 * Gets pad end.
 * 
 * Returns: the pad end
 * 
 * Since: 3.1.0
 */
guint
ags_preset_get_pad_end(AgsPreset *preset)
{
  guint pad_end;
  
  if(!AGS_IS_PRESET(preset)){
    return(0);
  }

  g_object_get(preset,
	       "pad-end", &pad_end,
	       NULL);

  return(pad_end);
}

/**
 * ags_preset_set_pad_end:
 * @preset: the #AgsPreset
 * @pad_end: the pad end
 *
 * Sets pad end.
 * 
 * Since: 3.1.0
 */
void
ags_preset_set_pad_end(AgsPreset *preset, guint pad_end)
{
  if(!AGS_IS_PRESET(preset)){
    return;
  }

  g_object_set(preset,
	       "pad-end", pad_end,
	       NULL);
}

/**
 * ags_preset_get_x_start:
 * @preset: the #AgsPreset
 *
 * Gets x start.
 * 
 * Returns: the x start
 * 
 * Since: 3.1.0
 */
guint
ags_preset_get_x_start(AgsPreset *preset)
{
  guint x_start;
  
  if(!AGS_IS_PRESET(preset)){
    return(0);
  }

  g_object_get(preset,
	       "x-start", &x_start,
	       NULL);

  return(x_start);
}

/**
 * ags_preset_set_x_start:
 * @preset: the #AgsPreset
 * @x_start: the x start
 *
 * Sets x start.
 * 
 * Since: 3.1.0
 */
void
ags_preset_set_x_start(AgsPreset *preset, guint x_start)
{
  if(!AGS_IS_PRESET(preset)){
    return;
  }

  g_object_set(preset,
	       "x-start", x_start,
	       NULL);
}

/**
 * ags_preset_get_x_end:
 * @preset: the #AgsPreset
 *
 * Gets x end.
 * 
 * Returns: the x end
 * 
 * Since: 3.1.0
 */
guint
ags_preset_get_x_end(AgsPreset *preset)
{
  guint x_end;
  
  if(!AGS_IS_PRESET(preset)){
    return(0);
  }

  g_object_get(preset,
	       "x-end", &x_end,
	       NULL);

  return(x_end);
}

/**
 * ags_preset_set_x_end:
 * @preset: the #AgsPreset
 * @x_end: the x end
 *
 * Sets x end.
 * 
 * Since: 3.1.0
 */
void
ags_preset_set_x_end(AgsPreset *preset, guint x_end)
{
  if(!AGS_IS_PRESET(preset)){
    return;
  }

  g_object_set(preset,
	       "x-end", x_end,
	       NULL);
}

/**
 * ags_preset_find_scope:
 * @preset: (element-type AgsAudio.Preset) (transfer none): the #GList-struct containing #AgsPreset
 * @scope: the preset's scope
 * 
 * Find preset's scope in @preset.
 * 
 * Returns: (element-type AgsAudio.Preset) (transfer none): the next matching #AgsPreset
 * 
 * Since: 3.0.0
 */
GList*
ags_preset_find_scope(GList *preset,
		      gchar *scope)
{
  GRecMutex *preset_mutex;

  while(preset != NULL){
    /* get preset mutex */
    preset_mutex = AGS_PRESET_GET_OBJ_MUTEX(preset->data);

    /* compare scope */
    g_rec_mutex_lock(preset_mutex);
    
    if(!g_strcmp0(AGS_PRESET(preset->data)->scope,
		  scope)){
      g_rec_mutex_unlock(preset_mutex);
      
      return(preset);
    }

    g_rec_mutex_unlock(preset_mutex);

    /* iterate */
    preset = preset->next;
  }
  
  return(NULL);
}

/**
 * ags_preset_find_name:
 * @preset: (element-type AgsAudio.Preset) (transfer none): the #GList-struct containing #AgsPreset
 * @preset_name: the preset's name
 * 
 * Find preset name in @preset.
 * 
 * Returns: (element-type AgsAudio.Preset) (transfer none): the next matching #AgsPreset
 * 
 * Since: 3.0.0
 */
GList*
ags_preset_find_name(GList *preset,
		     gchar *preset_name)
{
  GRecMutex *preset_mutex;

  while(preset != NULL){
    /* get preset mutex */
    preset_mutex = AGS_PRESET_GET_OBJ_MUTEX(preset->data);

    /* compare scope */
    g_rec_mutex_lock(preset_mutex);
    
    if(!g_strcmp0(AGS_PRESET(preset->data)->preset_name,
		  preset_name)){
      g_rec_mutex_unlock(preset_mutex);
      
      return(preset);
    }

    g_rec_mutex_unlock(preset_mutex);

    /* iterate */
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
 * Since: 3.0.0
 */
gboolean
ags_preset_add_parameter(AgsPreset *preset,
			 gchar *param_name, GValue *value)
{
  guint nth;
  guint i;
  gboolean found;
  
  GRecMutex *preset_mutex;

  if(!AGS_IS_PRESET(preset)){
    return(FALSE);
  }

  /* get preset mutex */
  preset_mutex = AGS_PRESET_GET_OBJ_MUTEX(preset);

  /* match or allocate */
  g_rec_mutex_lock(preset_mutex);

  found = FALSE;

  if(preset->parameter_name == NULL){
    preset->parameter_name = (gchar **) malloc(2 * sizeof(gchar *));

    preset->parameter_name[0] = NULL;
    preset->parameter_name[1] = NULL;

    preset->value = g_new0(GValue,
			   1);
    g_value_init(&(preset->value[0]),
		 G_VALUE_TYPE(value));
    
    preset->n_params += 1;
    nth = 0;
  }else{
    for(i = 0; i < preset->n_params; i++){
      if(!g_strcmp0(preset->parameter_name[i],
		    param_name)){
	nth = i;
	found = TRUE;

	break;
      }
    }

    if(!found){
      preset->parameter_name = (gchar **) realloc(preset->parameter_name,
						  (preset->n_params + 2) * sizeof(gchar *));

      preset->parameter_name[preset->n_params] = NULL;
      preset->parameter_name[preset->n_params + 1] = NULL;

      preset->value = g_renew(GValue,
			      preset->value,
			      preset->n_params + 1);
      
      memset(&(preset->value[preset->n_params]), 0, sizeof(GValue));

      g_value_init(&(preset->value[preset->n_params]),
		   G_VALUE_TYPE(value));
      
      preset->n_params += 1;
      nth = i;
    }
  }

  /* set value */  
  g_free(preset->parameter_name[nth]);

  preset->parameter_name[nth] = g_strdup(param_name);
  g_value_copy(value,
	       &(preset->value[nth]));

  g_rec_mutex_unlock(preset_mutex);

  return(!found);
}

/**
 * ags_preset_remove_parameter:
 * @preset: the #AgsPreset
 * @nth: the nth parameter to remove
 * 
 * Remove parameter of @preset.
 * 
 * Since: 3.0.0
 */
void
ags_preset_remove_parameter(AgsPreset *preset,
			    guint nth)
{
  GValue *value;

  gchar **parameter_name;

  guint i;
  
  GRecMutex *preset_mutex;

  if(!AGS_IS_PRESET(preset)){
    return;
  }

  /* get preset mutex */
  preset_mutex = AGS_PRESET_GET_OBJ_MUTEX(preset);

  /* check boundaries */
  g_rec_mutex_lock(preset_mutex);
  
  if(preset->n_params == 0 ||
     nth >= preset->n_params){
    g_rec_mutex_unlock(preset_mutex);

    return;
  }
  
  parameter_name = preset->parameter_name;
  value = preset->value;

  if(preset->n_params == 1){    
    preset->parameter_name = NULL;
    preset->n_params = 0;
  }else{
    preset->parameter_name = (gchar **) malloc((preset->n_params) * sizeof(gchar *));
    preset->value = g_new(GValue,
			  preset->n_params - 1);
    
    if(nth > 0){
      memcpy(preset->parameter_name,
	     parameter_name,
	     nth * sizeof(gchar *));

      memcpy(preset->value,
	     value,
	     nth * sizeof(GValue));
    }

    if(nth + 1 < preset->n_params){
      memcpy(&(preset->parameter_name[nth]),
	     &(parameter_name[nth + 1]),
	     (preset->n_params - (nth + 1)) * sizeof(gchar *));    

      memcpy(&(preset->value[nth]),
	     &(value[nth + 1]),
	     (preset->n_params - (nth + 1)) * sizeof(GValue));
    }

    preset->n_params -= 1;

    preset->parameter_name[preset->n_params] = NULL;
  }

  /* free old preset array */
  g_free(parameter_name[nth]);
  g_free(parameter_name);

  for(i = 0; i < preset->n_params + 1; i++){
    g_value_unset(&(value[i]));
  }
  
  g_free(value);
  
  g_rec_mutex_unlock(preset_mutex);
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
 * Since: 3.0.0
 */
void
ags_preset_get_parameter(AgsPreset *preset,
			 gchar *param_name, GValue *value,
			 GError **error)
{
  guint i;
  gboolean success;
  
  GRecMutex *preset_mutex;

  if(!AGS_IS_PRESET(preset)){
    return;
  }

  /* get preset mutex */
  preset_mutex = AGS_PRESET_GET_OBJ_MUTEX(preset);

  /* find */
  g_rec_mutex_lock(preset_mutex);

  success = FALSE;
  
  if(preset->parameter_name != NULL){
    for(i = 0; i < preset->n_params; i++){
      if(!g_strcmp0(preset->parameter_name[i],
		    param_name)){
	g_value_copy(&(preset->value[i]),
		     value);

	success = TRUE;      

	break;
      }
    }
  }

  g_rec_mutex_unlock(preset_mutex);
  
  /* report error */
  if(!success && error != NULL){
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
 * Create a new instance of #AgsPreset
 *
 * Returns: the new #AgsPreset
 *
 * Since: 3.0.0
 */
AgsPreset*
ags_preset_new()
{
  AgsPreset *preset;

  preset = (AgsPreset *) g_object_new(AGS_TYPE_PRESET,
				      NULL);

  return(preset);
}
