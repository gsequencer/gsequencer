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

#include <ags/audio/task/ags_set_audio_channels.h>

#include <ags/i18n.h>

void ags_set_audio_channels_class_init(AgsSetAudioChannelsClass *set_audio_channels);
void ags_set_audio_channels_init(AgsSetAudioChannels *set_audio_channels);
void ags_set_audio_channels_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_set_audio_channels_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_set_audio_channels_dispose(GObject *gobject);
void ags_set_audio_channels_finalize(GObject *gobject);

void ags_set_audio_channels_launch(AgsTask *task);

/**
 * SECTION:ags_set_audio_channels
 * @short_description: resizes audio channels
 * @title: AgsSetAudioChannels
 * @section_id:
 * @include: ags/audio/task/ags_set_audio_channels.h
 *
 * The #AgsSetAudioChannels task resizes audio channels of #AgsSoundcard.
 */

static gpointer ags_set_audio_channels_parent_class = NULL;

enum{
  PROP_0,
  PROP_SOUNDCARD,
  PROP_AUDIO_CHANNELS,
};

GType
ags_set_audio_channels_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_set_audio_channels = 0;

    static const GTypeInfo ags_set_audio_channels_info = {
      sizeof(AgsSetAudioChannelsClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_set_audio_channels_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSetAudioChannels),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_set_audio_channels_init,
    };

    ags_type_set_audio_channels = g_type_register_static(AGS_TYPE_TASK,
							 "AgsSetAudioChannels",
							 &ags_set_audio_channels_info,
							 0);

    g_once_init_leave(&g_define_type_id__static, ags_type_set_audio_channels);
  }

  return g_define_type_id__static;
}

void
ags_set_audio_channels_class_init(AgsSetAudioChannelsClass *set_audio_channels)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;

  ags_set_audio_channels_parent_class = g_type_class_peek_parent(set_audio_channels);

  /* gobject */
  gobject = (GObjectClass *) set_audio_channels;

  gobject->set_property = ags_set_audio_channels_set_property;
  gobject->get_property = ags_set_audio_channels_get_property;

  gobject->dispose = ags_set_audio_channels_dispose;
  gobject->finalize = ags_set_audio_channels_finalize;
  
  /* properties */
  /**
   * AgsSetAudioChannels:soundcard:
   *
   * The assigned #AgsSoundcard instance.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("soundcard",
				   i18n_pspec("soundcard of set audio channels"),
				   i18n_pspec("The soundcard of set audio channels"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /**
   * AgsSetAudioChannels:audio-channels:
   *
   * The count of audio channels to apply to soundcard.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("audio-channels",
				 i18n_pspec("audio channels"),
				 i18n_pspec("The count of audio channels"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNELS,
				  param_spec);

  /* task */
  task = (AgsTaskClass *) set_audio_channels;

  task->launch = ags_set_audio_channels_launch;
}

void
ags_set_audio_channels_init(AgsSetAudioChannels *set_audio_channels)
{
  set_audio_channels->soundcard = NULL;
  set_audio_channels->audio_channels = 0;
}

void
ags_set_audio_channels_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec)
{
  AgsSetAudioChannels *set_audio_channels;

  set_audio_channels = AGS_SET_AUDIO_CHANNELS(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = (GObject *) g_value_get_object(value);

      if(set_audio_channels->soundcard == (GObject *) soundcard){
	return;
      }

      if(set_audio_channels->soundcard != NULL){
	g_object_unref(set_audio_channels->soundcard);
      }

      if(soundcard != NULL){
	g_object_ref(soundcard);
      }

      set_audio_channels->soundcard = (GObject *) soundcard;
    }
    break;
  case PROP_AUDIO_CHANNELS:
    {
      set_audio_channels->audio_channels = g_value_get_uint(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_set_audio_channels_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec)
{
  AgsSetAudioChannels *set_audio_channels;

  set_audio_channels = AGS_SET_AUDIO_CHANNELS(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      g_value_set_object(value, set_audio_channels->soundcard);
    }
    break;
  case PROP_AUDIO_CHANNELS:
    {
      g_value_set_uint(value, set_audio_channels->audio_channels);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_set_audio_channels_dispose(GObject *gobject)
{
  AgsSetAudioChannels *set_audio_channels;

  set_audio_channels = AGS_SET_AUDIO_CHANNELS(gobject);
  
  if(set_audio_channels->soundcard != NULL){
    g_object_unref(set_audio_channels->soundcard);

    set_audio_channels->soundcard = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_set_audio_channels_parent_class)->dispose(gobject);
}

void
ags_set_audio_channels_finalize(GObject *gobject)
{
  AgsSetAudioChannels *set_audio_channels;

  set_audio_channels = AGS_SET_AUDIO_CHANNELS(gobject);
  
  if(set_audio_channels->soundcard != NULL){
    g_object_unref(set_audio_channels->soundcard);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_set_audio_channels_parent_class)->finalize(gobject);
}

void
ags_set_audio_channels_launch(AgsTask *task)
{
  AgsSetAudioChannels *set_audio_channels;

  guint channels;
  guint samplerate;
  guint buffer_size;
  guint format;

  set_audio_channels = AGS_SET_AUDIO_CHANNELS(task);

  g_return_if_fail(AGS_IS_SOUNDCARD(set_audio_channels->soundcard));
  
  /* set audio channels */
  ags_soundcard_get_presets(AGS_SOUNDCARD(set_audio_channels->soundcard),
			    &channels,
			    &samplerate,
			    &buffer_size,
			    &format);
  
  ags_soundcard_set_presets(AGS_SOUNDCARD(set_audio_channels->soundcard),
			    set_audio_channels->audio_channels,
			    samplerate,
			    buffer_size,
			    format);
}

/**
 * ags_set_audio_channels_new:
 * @soundcard: the #AgsSoundcard to reset
 * @audio_channels: the new count of audio channels
 *
 * Create a new instance of #AgsSetAudioChannels.
 *
 * Returns: the new #AgsSetAudioChannels.
 *
 * Since: 3.0.0
 */
AgsSetAudioChannels*
ags_set_audio_channels_new(GObject *soundcard, guint audio_channels)
{
  AgsSetAudioChannels *set_audio_channels;

  set_audio_channels = (AgsSetAudioChannels *) g_object_new(AGS_TYPE_SET_AUDIO_CHANNELS,
							    "soundcard", soundcard,
							    "audio-channels", audio_channels,
							    NULL);

  return(set_audio_channels);
}
