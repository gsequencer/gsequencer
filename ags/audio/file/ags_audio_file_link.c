/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/audio/file/ags_audio_file_link.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_audio_signal.h>

#include <ags/audio/file/ags_audio_file.h>

#include <libxml/xpath.h>
#include <libxml/tree.h>

#include <ags/i18n.h>

void ags_audio_file_link_class_init(AgsAudioFileLinkClass *audio_file_link);
void ags_audio_file_link_init(AgsAudioFileLink *audio_file_link);
void ags_audio_file_link_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_audio_file_link_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_audio_file_link_finalize(GObject *gobject);

/**
 * SECTION:ags_audio_file_link
 * @short_description: link objects read of file.
 * @title: AgsAudioFileLink
 * @section_id:
 * @include: ags/file/ags_audio_file_link.h
 *
 * The #AgsAudioFileLink links read objects of file.
 */

enum{
  PROP_0,
  PROP_PRESET,
  PROP_INSTRUMENT,
  PROP_SAMPLE,
  PROP_AUDIO_CHANNEL,
  PROP_TIMESTAMP,
};

static gpointer ags_audio_file_link_parent_class = NULL;

GType
ags_audio_file_link_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_audio_file_link = 0;

    static const GTypeInfo ags_audio_file_link_info = {
      sizeof (AgsAudioFileLinkClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_file_link_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioFileLink),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_file_link_init,
    };

    ags_type_audio_file_link = g_type_register_static(AGS_TYPE_FILE_LINK,
						      "AgsAudioFileLink",
						      &ags_audio_file_link_info,
						      0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_audio_file_link);
  }

  return g_define_type_id__volatile;
}

void
ags_audio_file_link_class_init(AgsAudioFileLinkClass *audio_file_link)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_audio_file_link_parent_class = g_type_class_peek_parent(audio_file_link);

  /* GObjectClass */
  gobject = (GObjectClass *) audio_file_link;

  gobject->set_property = ags_audio_file_link_set_property;
  gobject->get_property = ags_audio_file_link_get_property;

  gobject->finalize = ags_audio_file_link_finalize;

  /* properties */
  /**
   * AgsAudioFileLink:preset:
   *
   * The assigned preset.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("preset",
				   i18n_pspec("the preset"),
				   i18n_pspec("The preset to locate the file"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PRESET,
				  param_spec);

  /**
   * AgsAudioFileLink:instrument:
   *
   * The assigned instrument.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("instrument",
				   i18n_pspec("the instrument"),
				   i18n_pspec("The instrument to locate the file"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INSTRUMENT,
				  param_spec);

  /**
   * AgsAudioFileLink:sample:
   *
   * The assigned sample.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("sample",
				   i18n_pspec("the sample"),
				   i18n_pspec("The sample to locate the file"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLE,
				  param_spec);

  /**
   * AgsAudioFileLink:audio-channel:
   *
   * The assigned audio channel.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("audio-channel",
				 i18n_pspec("audio channel to read"),
				 i18n_pspec("The selected audio channel to read"),
				 0, 256,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNEL,
				  param_spec);

  /**
   * AgsAudioFileLink:timestamp:
   *
   * The assigned timestamp.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("timestamp",
				   i18n_pspec("timestamp"),
				   i18n_pspec("The timestamp"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TIMESTAMP,
				  param_spec);
}

void
ags_audio_file_link_init(AgsAudioFileLink *audio_file_link)
{
  audio_file_link->preset = NULL;
  audio_file_link->instrument = NULL;
  audio_file_link->sample = NULL;
  
  audio_file_link->audio_channel = 0;
  
  audio_file_link->timestamp = NULL;
}

void
ags_audio_file_link_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsAudioFileLink *audio_file_link;

  GRecMutex *file_link_mutex;

  audio_file_link = AGS_AUDIO_FILE_LINK(gobject);

  /* get file link mutex */
  file_link_mutex = AGS_FILE_LINK_GET_OBJ_MUTEX(audio_file_link);
  
  switch(prop_id){
  case PROP_PRESET:
    {
      char *preset;

      preset = (char *) g_value_get_string(value);

      g_rec_mutex_lock(file_link_mutex);

      if(preset == audio_file_link->preset){
	g_rec_mutex_unlock(file_link_mutex);

	return;
      }

      if(audio_file_link->preset != NULL){
	free(audio_file_link->preset);
      }
	
      audio_file_link->preset = g_strdup(preset);

      g_rec_mutex_unlock(file_link_mutex);
    }
    break;
  case PROP_INSTRUMENT:
    {
      char *instrument;

      instrument = (char *) g_value_get_string(value);

      g_rec_mutex_lock(file_link_mutex);

      if(instrument == audio_file_link->instrument){
	g_rec_mutex_unlock(file_link_mutex);

	return;
      }

      if(audio_file_link->instrument != NULL){
	free(audio_file_link->instrument);
      }
	
      audio_file_link->instrument = g_strdup(instrument);

      g_rec_mutex_unlock(file_link_mutex);
    }
    break;
  case PROP_SAMPLE:
    {
      char *sample;

      sample = (char *) g_value_get_string(value);

      g_rec_mutex_lock(file_link_mutex);

      if(sample == audio_file_link->sample){
	g_rec_mutex_unlock(file_link_mutex);

	return;
      }

      if(audio_file_link->sample != NULL){
	free(audio_file_link->sample);
      }
	
      audio_file_link->sample = g_strdup(sample);

      g_rec_mutex_unlock(file_link_mutex);
    }
    break;
  case PROP_AUDIO_CHANNEL:
    {
      g_rec_mutex_lock(file_link_mutex);

      audio_file_link->audio_channel = g_value_get_uint(value);

      g_rec_mutex_unlock(file_link_mutex);
    }
    break;
  case PROP_TIMESTAMP:
    {
      GObject *timestamp;

      timestamp = (GObject *) g_value_get_object(value);

      g_rec_mutex_lock(file_link_mutex);

      if((AgsTimestamp *) timestamp == audio_file_link->timestamp){
	g_rec_mutex_unlock(file_link_mutex);

	return;
      }

      if(audio_file_link->timestamp != NULL){
	g_object_unref(audio_file_link->timestamp);
      }

      if(timestamp != NULL){
	g_object_ref(timestamp);
      }

      audio_file_link->timestamp = (AgsTimestamp *) timestamp;

      g_rec_mutex_unlock(file_link_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_file_link_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsAudioFileLink *audio_file_link;

  GRecMutex *file_link_mutex;

  audio_file_link = AGS_AUDIO_FILE_LINK(gobject);

  /* get file link mutex */
  file_link_mutex = AGS_FILE_LINK_GET_OBJ_MUTEX(audio_file_link);
  
  switch(prop_id){
  case PROP_PRESET:
    {
      g_rec_mutex_lock(file_link_mutex);

      g_value_set_string(value, audio_file_link->preset);

      g_rec_mutex_unlock(file_link_mutex);
    }
    break;
  case PROP_INSTRUMENT:
    {
      g_rec_mutex_lock(file_link_mutex);

      g_value_set_string(value, audio_file_link->instrument);

      g_rec_mutex_unlock(file_link_mutex);
    }
    break;
  case PROP_SAMPLE:
    {
      g_rec_mutex_lock(file_link_mutex);

      g_value_set_string(value, audio_file_link->sample);

      g_rec_mutex_unlock(file_link_mutex);
    }
    break;
  case PROP_AUDIO_CHANNEL:
    {
      g_rec_mutex_lock(file_link_mutex);

      g_value_set_uint(value, audio_file_link->audio_channel);

      g_rec_mutex_unlock(file_link_mutex);
    }
    break;
  case PROP_TIMESTAMP:
    {
      g_rec_mutex_lock(file_link_mutex);

      g_value_set_object(value, audio_file_link->timestamp);

      g_rec_mutex_unlock(file_link_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_file_link_finalize(GObject *gobject)
{
  AgsAudioFileLink *audio_file_link;

  audio_file_link = AGS_AUDIO_FILE_LINK(gobject);

  if(audio_file_link->timestamp != NULL){
    g_object_unref(audio_file_link->timestamp);
  }
}

/**
 * ags_audio_file_link_new:
 *
 * Create a new instance of #AgsAudioFileLink
 *
 * Returns: the new #AgsAudioFileLink
 *
 * Since: 3.0.0
 */
AgsAudioFileLink*
ags_audio_file_link_new()
{
  AgsAudioFileLink *audio_file_link;

  audio_file_link = (AgsAudioFileLink *) g_object_new(AGS_TYPE_AUDIO_FILE_LINK,
						      NULL);

  return(audio_file_link);
}
