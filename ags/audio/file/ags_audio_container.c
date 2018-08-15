/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/audio/file/ags_audio_container.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_wave.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/file/ags_sound_resource.h>

#include <ags/i18n.h>

void ags_audio_container_class_init(AgsAudioContainerClass *audio_container);
void ags_audio_container_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_container_init(AgsAudioContainer *audio_container);
void ags_audio_container_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_audio_container_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_audio_container_finalize(GObject *object);

enum{
  PROP_0,
  PROP_SOUNDCARD,
  PROP_FILENAME,
};

/**
 * SECTION:ags_audio_container
 * @short_description: Audio container input/output
 * @title: AgsAudioContainer
 * @section_id:
 * @include: ags/audio/file/ags_audio_container.h
 *
 * #AgsAudioContainer is the base object to read/write audio data.
 */

static gpointer ags_audio_container_parent_class = NULL;
static AgsConnectableInterface *ags_audio_container_parent_connectable_interface;

GType
ags_audio_container_get_type()
{
  static GType ags_type_audio_container = 0;

  if(!ags_type_audio_container){
    static const GTypeInfo ags_audio_container_info = {
      sizeof (AgsAudioContainerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_container_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioContainer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_container_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_audio_container_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_audio_container = g_type_register_static(G_TYPE_OBJECT,
						      "AgsAudioContainer",
						      &ags_audio_container_info,
						      0);

    g_type_add_interface_static(ags_type_audio_container,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_audio_container);
}

void
ags_audio_container_class_init(AgsAudioContainerClass *audio_container)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_audio_container_parent_class = g_type_class_peek_parent(audio_container);

  gobject = (GObjectClass *) audio_container;

  gobject->set_property = ags_audio_container_set_property;
  gobject->get_property = ags_audio_container_get_property;

  gobject->finalize = ags_audio_container_finalize;
  
  /* properties */
  /**
   * AgsAudioContainer:soundcard:
   *
   * The assigned soundcard.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("soundcard",
				   i18n_pspec("soundcard of audio container"),
				   i18n_pspec("The soundcard what audio container has it's presets"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /**
   * AgsAudioContainer:filename:
   *
   * The assigned filename.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("filename",
				   i18n_pspec("filename of audio container"),
				   i18n_pspec("The filename of audio container"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);
}

void
ags_audio_container_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_audio_container_parent_connectable_interface = g_type_interface_peek_parent(connectable);
}

void
ags_audio_container_init(AgsAudioContainer *audio_container)
{
  AgsConfig *config;
  
  audio_container->soundcard = NULL;

  audio_container->filename = NULL;
}

void
ags_audio_container_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsAudioContainer *audio_container;

  audio_container = AGS_AUDIO_CONTAINER(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;
      
      soundcard = (GObject *) g_value_get_object(value);

      if(soundcard == ((GObject *) audio_container->soundcard)){
	return;
      }

      if(audio_container->soundcard != NULL){
	g_object_unref(audio_container->soundcard);
      }
      
      if(soundcard != NULL){
	g_object_ref(G_OBJECT(soundcard));
      }
      
      audio_container->soundcard = (GObject *) soundcard;
    }
    break;
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = (gchar *) g_value_get_string(value);

      if(audio_container->filename == filename){
	return;
      }
      
      if(audio_container->filename != NULL){
	g_free(audio_container->filename);
      }

      audio_container->filename = g_strdup(filename);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_container_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsAudioContainer *audio_container;

  audio_container = AGS_AUDIO_CONTAINER(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      g_value_set_object(value, audio_container->soundcard);
    }
    break;
  case PROP_FILENAME:
    {
      g_value_set_string(value, audio_container->filename);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_container_finalize(GObject *gobject)
{
  AgsAudioContainer *audio_container;

  audio_container = AGS_AUDIO_CONTAINER(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_audio_container_parent_class)->finalize(gobject);
}

gboolean
ags_audio_container_check_suffix(gchar *filename)
{
  //TODO:JK: implement me
}

void
ags_audio_container_add_audio_signal(AgsAudioContainer *audio_container, GObject *audio_signal)
{
  //TODO:JK: implement me
}

void
ags_audio_container_remove_audio_signal(AgsAudioContainer *audio_container, GObject *audio_signal)
{
  //TODO:JK: implement me
}

void
ags_audio_container_add_wave(AgsAudioContainer *audio_container, GObject *wave)
{
  //TODO:JK: implement me
}

void
ags_audio_container_remove_wave(AgsAudioContainer *audio_container, GObject *wave)
{
  //TODO:JK: implement me
}

gboolean
ags_audio_container_open(AgsAudioContainer *audio_container)
{
  //TODO:JK: implement me
}

gboolean
ags_audio_container_open_from_data(AgsAudioContainer *audio_container, gchar *data)
{
  //TODO:JK: implement me
}

gboolean
ags_audio_container_rw_open(AgsAudioContainer *audio_container,
			    gboolean create)
{
  //TODO:JK: implement me
}

void
ags_audio_container_close(AgsAudioContainer *audio_container)
{
  //TODO:JK: implement me
}

void*
ags_audio_container_read(AgsAudioContainer *audio_container,
			 guint audio_channel,
			 guint format,
			 GError **error)
{
  //TODO:JK: implement me
}

GList*
ags_audio_container_read_audio_signal(AgsAudioContainer *audio_container)
{
  //TODO:JK: implement me
}

GList*
ags_audio_container_read_wave(AgsAudioContainer *audio_container,
			      guint64 x_offset,
			      gdouble delay, guint attack)
{
  //TODO:JK: implement me
}

void
ags_audio_container_seek(AgsAudioContainer *audio_container, guint frames, gint whence)
{
  //TODO:JK: implement me
}

void
ags_audio_container_write(AgsAudioContainer *audio_container,
			  void *buffer, guint buffer_size,
			  guint format)
{
  //TODO:JK: implement me
}

void
ags_audio_container_flush(AgsAudioContainer *audio_container)
{
  //TODO:JK: implement me
}

/**
 * ags_audio_container_new:
 * @filename: the filename
 * @preset: the preset
 * @instrument: the instrument
 * @sample: the sample
 * @soundcard: defaults of #AgsSoundcard
 * @audio_channel: the audio channel to read
 *
 * Create a new instance of #AgsAudioContainer.
 *
 * Returns: the new #AgsAudioContainer.
 *
 * Since: 2.0.0
 */
AgsAudioContainer*
ags_audio_container_new(gchar *filename,
			gchar *preset,
			gchar *instrument,
			gchar *sample,
			GObject *soundcard,
			gint audio_channel)
{
  AgsAudioContainer *audio_container;

  audio_container = (AgsAudioContainer *) g_object_new(AGS_TYPE_AUDIO_CONTAINER,
						       "filename", filename,
						       "soundcard", soundcard,
						       "audio-channel", audio_channel,
						       NULL);

  return(audio_container);
}
