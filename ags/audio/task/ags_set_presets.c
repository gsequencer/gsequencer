/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/audio/task/ags_set_presets.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_audio_signal.h>

#include <ags/i18n.h>

void ags_set_presets_class_init(AgsSetPresetsClass *set_presets);
void ags_set_presets_init(AgsSetPresets *set_presets);
void ags_set_presets_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_set_presets_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_set_presets_dispose(GObject *gobject);
void ags_set_presets_finalize(GObject *gobject);

void ags_set_presets_launch(AgsTask *task);

void ags_set_presets_audio_signal(AgsSetPresets *set_presets, AgsAudioSignal *audio_signal);
void ags_set_presets_recycling(AgsSetPresets *set_presets, AgsRecycling *recycling);
void ags_set_presets_channel(AgsSetPresets *set_presets, AgsChannel *channel);
void ags_set_presets_audio(AgsSetPresets *set_presets, AgsAudio *audio);
void ags_set_presets_soundcard(AgsSetPresets *set_presets, GObject *soundcard);

/**
 * SECTION:ags_set_presets
 * @short_description: modify presets
 * @title: AgsSetPresets
 * @section_id:
 * @include: ags/audio/task/ags_set_presets.h
 *
 * The #AgsSetPresets task modifies presets of scope.
 */

static gpointer ags_set_presets_parent_class = NULL;

enum{
  PROP_0,
  PROP_SCOPE,
  PROP_BUFFER_SIZE,
  PROP_FORMAT,
  PROP_SAMPLERATE,
};

GType
ags_set_presets_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_set_presets = 0;

    static const GTypeInfo ags_set_presets_info = {
      sizeof(AgsSetPresetsClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_set_presets_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSetPresets),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_set_presets_init,
    };

    ags_type_set_presets = g_type_register_static(AGS_TYPE_TASK,
						  "AgsSetPresets",
						  &ags_set_presets_info,
						  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_set_presets);
  }

  return g_define_type_id__volatile;
}

void
ags_set_presets_class_init(AgsSetPresetsClass *set_presets)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;

  ags_set_presets_parent_class = g_type_class_peek_parent(set_presets);

  /* gobject */
  gobject = (GObjectClass *) set_presets;

  gobject->set_property = ags_set_presets_set_property;
  gobject->get_property = ags_set_presets_get_property;

  gobject->dispose = ags_set_presets_dispose;
  gobject->finalize = ags_set_presets_finalize;

  /* properties */
  /**
   * AgsSetPresets:scope:
   *
   * The assigned #GObject as scope.
   * 
   * Since: 5.3.1
   */
  param_spec = g_param_spec_object("scope",
				   i18n_pspec("scope of set presets"),
				   i18n_pspec("The scope of set presets"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SCOPE,
				  param_spec);

  /**
   * AgsSetPresets:buffer-size:
   *
   * The buffer size to apply to scope.
   * 
   * Since: 5.3.1
   */
  param_spec = g_param_spec_uint("buffer-size",
				 i18n_pspec("buffer size"),
				 i18n_pspec("The buffer size to apply"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);

  /**
   * AgsSetPresets:format:
   *
   * The format to apply to scope.
   * 
   * Since: 5.3.1
   */
  param_spec = g_param_spec_uint("format",
				 i18n_pspec("format"),
				 i18n_pspec("The format to apply"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FORMAT,
				  param_spec);

  /**
   * AgsSetPresets:samplerate:
   *
   * The samplerate to apply to scope.
   * 
   * Since: 5.3.1
   */
  param_spec = g_param_spec_uint("samplerate",
				 i18n_pspec("samplerate"),
				 i18n_pspec("The samplerate to apply"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /* task */
  task = (AgsTaskClass *) set_presets;

  task->launch = ags_set_presets_launch;
}

void
ags_set_presets_init(AgsSetPresets *set_presets)
{
  set_presets->scope = NULL;
  
  set_presets->buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  set_presets->format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  set_presets->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
}

void
ags_set_presets_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsSetPresets *set_presets;

  set_presets = AGS_SET_PRESETS(gobject);

  switch(prop_id){
  case PROP_SCOPE:
    {
      GObject *scope;

      scope = (GObject *) g_value_get_object(value);

      if(set_presets->scope == (GObject *) scope){
	return;
      }

      if(set_presets->scope != NULL){
	g_object_unref(set_presets->scope);
      }

      if(scope != NULL){
	g_object_ref(scope);
      }

      set_presets->scope = (GObject *) scope;
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      set_presets->buffer_size = g_value_get_uint(value);
    }
    break;
  case PROP_FORMAT:
    {
      set_presets->format = g_value_get_uint(value);
    }
    break;
  case PROP_SAMPLERATE:
    {
      set_presets->samplerate = g_value_get_uint(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_set_presets_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsSetPresets *set_presets;

  set_presets = AGS_SET_PRESETS(gobject);

  switch(prop_id){
  case PROP_SCOPE:
    {
      g_value_set_object(value, set_presets->scope);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_value_set_uint(value, set_presets->buffer_size);
    }
    break;
  case PROP_FORMAT:
    {
      g_value_set_uint(value, set_presets->format);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_value_set_uint(value, set_presets->samplerate);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_set_presets_dispose(GObject *gobject)
{
  AgsSetPresets *set_presets;

  set_presets = AGS_SET_PRESETS(gobject);

  if(set_presets->scope != NULL){
    g_object_unref(set_presets->scope);

    set_presets->scope = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_set_presets_parent_class)->dispose(gobject);
}

void
ags_set_presets_finalize(GObject *gobject)
{
  AgsSetPresets *set_presets;

  set_presets = AGS_SET_PRESETS(gobject);

  if(set_presets->scope != NULL){
    g_object_unref(set_presets->scope);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_set_presets_parent_class)->finalize(gobject);
}

void
ags_set_presets_launch(AgsTask *task)
{
  AgsSetPresets *set_presets;
  GObject *scope;

  set_presets = AGS_SET_PRESETS(task);

  scope = set_presets->scope;

  if(AGS_IS_SOUNDCARD(scope)){
    ags_set_presets_soundcard(set_presets, scope);
  }else if(AGS_IS_AUDIO(scope)){
    ags_set_presets_audio(set_presets, AGS_AUDIO(scope));
  }else if(AGS_IS_CHANNEL(scope)){
    ags_set_presets_channel(set_presets, AGS_CHANNEL(scope));
  }else if(AGS_IS_RECYCLING(scope)){
    ags_set_presets_recycling(set_presets, AGS_RECYCLING(scope));
  }else if(AGS_IS_AUDIO_SIGNAL(scope)){
    ags_set_presets_audio_signal(set_presets, AGS_AUDIO_SIGNAL(scope));
  }
}

void
ags_set_presets_audio_signal(AgsSetPresets *set_presets, AgsAudioSignal *audio_signal)
{
  g_object_set(audio_signal,
	       "samplerate", set_presets->samplerate,
	       "buffer-size", set_presets->buffer_size,
	       "format", set_presets->format,
	       NULL);
}

void
ags_set_presets_recycling(AgsSetPresets *set_presets, AgsRecycling *recycling)
{
  g_object_set(recycling,
	       "samplerate", set_presets->samplerate,
	       "buffer-size", set_presets->buffer_size,
	       "format", set_presets->format,
	       NULL);
}

void
ags_set_presets_channel(AgsSetPresets *set_presets, AgsChannel *channel)
{
  AgsChannel *link;
  AgsRecycling *recycling;

  GRecMutex *channel_mutex;

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

  g_rec_mutex_lock(channel_mutex);

  link = channel->link;
  recycling = channel->first_recycling;

  channel->samplerate = set_presets->samplerate;
  channel->buffer_size = set_presets->buffer_size;
  channel->format = set_presets->format;

  g_rec_mutex_unlock(channel_mutex);

  if(link == NULL &&
     recycling != NULL){
    g_object_set(recycling,
		 "samplerate", set_presets->samplerate,
		 NULL);
  }
}

void
ags_set_presets_audio(AgsSetPresets *set_presets, AgsAudio *audio)
{
  GRecMutex *audio_mutex;

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  g_rec_mutex_lock(audio_mutex);

  audio->samplerate = set_presets->samplerate;
  audio->buffer_size = set_presets->buffer_size;
  audio->format = set_presets->format;

  g_rec_mutex_unlock(audio_mutex);
}

void
ags_set_presets_soundcard(AgsSetPresets *set_presets, GObject *soundcard)
{
  guint orig_channels;
  guint orig_samplerate;
  guint orig_buffer_size;
  guint orig_format;
  
  /* set buffer size */
  ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			    &orig_channels,
			    &orig_samplerate,
			    &orig_buffer_size,
			    &orig_format);

  ags_soundcard_set_presets(AGS_SOUNDCARD(soundcard),
			    orig_channels,
			    set_presets->samplerate,
			    set_presets->buffer_size,
			    set_presets->format);
}

/**
 * ags_set_presets_new:
 * @scope: the #AgsSoundcard to reset
 * @buffer_size: the new buffer size
 * @format: the new format
 * @samplerate: the new samplerate
 *
 * Create a new instance of #AgsSetPresets.
 *
 * Returns: the new #AgsSetPresets
 *
 * Since: 5.3.1
 */
AgsSetPresets*
ags_set_presets_new(GObject *scope,
		    guint buffer_size,
		    AgsSoundcardFormat format,
		    guint samplerate)
{
  AgsSetPresets *set_presets;

  set_presets = (AgsSetPresets *) g_object_new(AGS_TYPE_SET_PRESETS,
					       "scope", scope,
					       "buffer-size", buffer_size,
					       "format", format,
					       "samplerate", samplerate,
					       NULL);

  return(set_presets);
}
