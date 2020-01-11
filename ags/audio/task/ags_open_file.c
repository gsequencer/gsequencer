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

#include <ags/audio/task/ags_open_file.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_input.h>

#include <ags/audio/file/ags_audio_file_link.h>
#include <ags/audio/file/ags_audio_file.h>

#include <ags/i18n.h>

void ags_open_file_class_init(AgsOpenFileClass *open_file);
void ags_open_file_init(AgsOpenFile *open_file);
void ags_open_file_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec);
void ags_open_file_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec);
void ags_open_file_dispose(GObject *gobject);
void ags_open_file_finalize(GObject *gobject);

void ags_open_file_launch(AgsTask *task);

/**
 * SECTION:ags_open_file
 * @short_description: open file task
 * @title: AgsOpenFile
 * @section_id:
 * @include: ags/audio/task/ags_open_file.h
 *
 * The #AgsOpenFile task opens files.
 */

static gpointer ags_open_file_parent_class = NULL;

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_FILENAME,
  PROP_OVERWRITE_CHANNELS,
  PROP_CREATE_CHANNELS,
};

GType
ags_open_file_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_open_file = 0;

    static const GTypeInfo ags_open_file_info = {
      sizeof(AgsOpenFileClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_open_file_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsOpenFile),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_open_file_init,
    };

    ags_type_open_file = g_type_register_static(AGS_TYPE_TASK,
						"AgsOpenFile",
						&ags_open_file_info,
						0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_open_file);
  }

  return g_define_type_id__volatile;
}

void
ags_open_file_class_init(AgsOpenFileClass *open_file)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;

  ags_open_file_parent_class = g_type_class_peek_parent(open_file);

  /* GObject */
  gobject = (GObjectClass *) open_file;

  gobject->set_property = ags_open_file_set_property;
  gobject->get_property = ags_open_file_get_property;

  gobject->dispose = ags_open_file_dispose;
  gobject->finalize = ags_open_file_finalize;

  /* properties */
  /**
   * AgsOpenFile:audio:
   *
   * The assigned #AgsAudio
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("audio",
				   i18n_pspec("audio of open file"),
				   i18n_pspec("The audio of open file task"),
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);
  
  /**
   * AgsOpenFile:filename:
   *
   * The assigned #GSList-struct providing filename as string
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("filename",
				    i18n_pspec("filename of open file"),
				    i18n_pspec("The filename of open file task"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);
  
  /**
   * AgsOpenFile:overwrite-channels:
   *
   * As open files overwrite #AgsChannel.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_boolean("overwrite-channels",
				     i18n_pspec("open file overwriting channels"),
				     i18n_pspec("Do overwrite channels as open files"),
				     FALSE,
				     G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OVERWRITE_CHANNELS,
				  param_spec);

  /**
   * AgsOpenFile:create-channels:
   *
   * As open files create #AgsChannel.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_boolean("create-channels",
				     i18n_pspec("open file creating channels"),
				     i18n_pspec("Do create channels as open files"),
				     FALSE,
				     G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CREATE_CHANNELS,
				  param_spec);

  /* AgsTask */
  task = (AgsTaskClass *) open_file;

  task->launch = ags_open_file_launch;
}

void
ags_open_file_init(AgsOpenFile *open_file)
{
  open_file->audio = NULL;
  open_file->filename = NULL;
  open_file->overwrite_channels = FALSE;
  open_file->create_channels = FALSE;
}

void
ags_open_file_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec)
{
  AgsOpenFile *open_file;

  open_file = AGS_OPEN_FILE(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      if(open_file->audio == audio){
	return;
      }

      if(open_file->audio != NULL){
	g_object_unref(open_file->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      open_file->audio = audio;
    }
    break;
  case PROP_FILENAME:
    {
      GSList *filename;
      
      filename = g_value_get_pointer(value);

      if(open_file->filename == filename){
	return;
      }

      if(open_file->filename != NULL){
	g_slist_free(open_file->filename);
      }
      
      open_file->filename = g_slist_copy(filename);
    }
    break;
  case PROP_OVERWRITE_CHANNELS:
    {
      open_file->overwrite_channels = g_value_get_boolean(value);
    }
    break;
  case PROP_CREATE_CHANNELS:
    {
      open_file->create_channels = g_value_get_boolean(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_open_file_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec)
{
  AgsOpenFile *open_file;

  open_file = AGS_OPEN_FILE(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      g_value_set_object(value, open_file->audio);
    }
    break;
  case PROP_FILENAME:
    {
      g_value_set_pointer(value,
			  g_slist_copy(open_file->filename));
    }
    break;
  case PROP_OVERWRITE_CHANNELS:
    {
      g_value_set_boolean(value, open_file->overwrite_channels);
    }
    break;
  case PROP_CREATE_CHANNELS:
    {
      g_value_set_boolean(value, open_file->create_channels);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_open_file_dispose(GObject *gobject)
{
  AgsOpenFile *open_file;

  open_file = AGS_OPEN_FILE(gobject);

  if(open_file->audio != NULL){
    g_object_unref(open_file->audio);

    open_file->audio = NULL;
  }

  if(open_file->filename != NULL){
    g_slist_free(open_file->filename);

    open_file->filename = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_open_file_parent_class)->dispose(gobject);
}

void
ags_open_file_finalize(GObject *gobject)
{
  AgsOpenFile *open_file;

  open_file = AGS_OPEN_FILE(gobject);

  if(open_file->audio != NULL){
    g_object_unref(open_file->audio);
  }

  if(open_file->filename != NULL){
    g_slist_free(open_file->filename);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_open_file_parent_class)->finalize(gobject);
}

void
ags_open_file_launch(AgsTask *task)
{
  AgsOpenFile *open_file;

  open_file = AGS_OPEN_FILE(task);

  ags_audio_open_audio_file_as_channel(open_file->audio,
				       open_file->filename,
				       open_file->overwrite_channels,
				       open_file->create_channels);
}

/**
 * ags_open_file_new:
 * @audio: the #AgsAudio
 * @filename: (element-type utf8): the filename to be opened
 * @overwrite_channels: reset existing #AgsInput
 * @create_channels: instantiate new #AgsInput
 *
 * Create a new instance of #AgsOpenFile.
 *
 * Returns: the new #AgsOpenFile.
 *
 * Since: 3.0.0
 */
AgsOpenFile*
ags_open_file_new(AgsAudio *audio,
		  GSList *filename,
		  gboolean overwrite_channels,
		  gboolean create_channels)
{
  AgsOpenFile *open_file;

  open_file = (AgsOpenFile *) g_object_new(AGS_TYPE_OPEN_FILE,
					   "audio", audio,
					   "filename", filename,
					   "overwrite-channels", overwrite_channels,
					   "create-channels", create_channels,
					   NULL);

  return(open_file);
}
