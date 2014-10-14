/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/audio/task/ags_open_file.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/file/ags_file_link.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_input.h>

#include <ags/audio/file/ags_audio_file.h>

void ags_open_file_class_init(AgsOpenFileClass *open_file);
void ags_open_file_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_open_file_init(AgsOpenFile *open_file);
void ags_open_file_connect(AgsConnectable *connectable);
void ags_open_file_disconnect(AgsConnectable *connectable);
void ags_open_file_finalize(GObject *gobject);
void ags_open_file_launch(AgsTask *task);

/**
 * SECTION:ags_open_file
 * @short_description: open file object
 * @title: AgsOpenFile
 * @section_id:
 * @include: ags/audio/task/ags_open_file.h
 *
 * The #AgsOpenFile task opens files.
 */

static gpointer ags_open_file_parent_class = NULL;
static AgsConnectableInterface *ags_open_file_parent_connectable_interface;

GType
ags_open_file_get_type()
{
  static GType ags_type_open_file = 0;

  if(!ags_type_open_file){
    static const GTypeInfo ags_open_file_info = {
      sizeof (AgsOpenFileClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_open_file_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOpenFile),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_open_file_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_open_file_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_open_file = g_type_register_static(AGS_TYPE_TASK,
						"AgsOpenFile\0",
						&ags_open_file_info,
						0);

    g_type_add_interface_static(ags_type_open_file,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_open_file);
}

void
ags_open_file_class_init(AgsOpenFileClass *open_file)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_open_file_parent_class = g_type_class_peek_parent(open_file);

  /* GObject */
  gobject = (GObjectClass *) open_file;

  gobject->finalize = ags_open_file_finalize;

  /* AgsTask */
  task = (AgsTaskClass *) open_file;

  task->launch = ags_open_file_launch;
}

void
ags_open_file_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_open_file_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_open_file_connect;
  connectable->disconnect = ags_open_file_disconnect;
}

void
ags_open_file_init(AgsOpenFile *open_file)
{
  open_file->audio = NULL;
  open_file->filenames = NULL;
  open_file->overwrite_channels = FALSE;
  open_file->create_channels = FALSE;
}

void
ags_open_file_connect(AgsConnectable *connectable)
{
  ags_open_file_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_open_file_disconnect(AgsConnectable *connectable)
{
  ags_open_file_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_open_file_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_open_file_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_open_file_launch(AgsTask *task)
{
  AgsOpenFile *open_file;
  AgsAudio *audio;
  AgsChannel *channel, *iter;
  AgsAudioFile *audio_file;
  AgsFileLink *file_link;
  GSList *current;
  GList *audio_signal;
  gchar *current_filename;
  guint i, i_stop;
  GError *error;

  open_file = AGS_OPEN_FILE(task);

  audio = open_file->audio;

  current = open_file->filenames;

  i_stop = 0;
  
  if(open_file->overwrite_channels){
    channel = audio->input;    

    i_stop = audio->input_pads;
  }

  /*  */
  if(open_file->create_channels){
    i_stop = g_slist_length(open_file->filenames);

    if(open_file->overwrite_channels){
      if(i_stop > audio->input_pads){
	ags_audio_set_pads(audio, AGS_TYPE_INPUT,
			   i_stop);
      }

      channel = audio->input;
    }else{
      guint pads_old;

      pads_old = audio->input_pads;

      ags_audio_set_pads(audio, AGS_TYPE_INPUT,
			 audio->input_pads + i_stop);

      channel = ags_channel_pad_nth(audio->input, pads_old);
    }
  }

  for(i = 0; i < i_stop && current != NULL; i++){
    current_filename = (gchar *) current->data;

    audio_file = ags_audio_file_new((gchar *) current_filename,
				    AGS_DEVOUT(audio->devout),
				    0, open_file->audio->audio_channels);

    ags_audio_file_open(audio_file);
    ags_audio_file_read_audio_signal(audio_file);

    iter = channel;
    audio_signal = audio_file->audio_signal;

    while(iter != channel->next_pad && audio_signal != NULL){
      file_link = g_object_new(AGS_TYPE_FILE_LINK,
			       "filename\0", current_filename,
			       NULL);
      g_object_set(G_OBJECT(iter),
		   "file-link", file_link,
		   NULL);

      AGS_AUDIO_SIGNAL(audio_signal->data)->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;

      if(iter->link != NULL){
	error = NULL;

	ags_channel_set_link(iter, NULL,
			     &error);

	if(error != NULL){
	  g_warning(error->message);
	}
      }

      ags_recycling_add_audio_signal(iter->first_recycling,
				     AGS_AUDIO_SIGNAL(audio_signal->data));

      audio_signal = audio_signal->next;
      iter = iter->next;
    }

    channel = channel->next_pad;
    current = current->next;
  }
}

/**
 * ags_open_file_new:
 * @file: the #AgsFile
 * @filenames: the filenames to be opened
 * @overwrite_channels: reset existing #AgsInput
 * @create_channels: inistantiate new #AgsInput, if @overwrite_channell as needed
 * else for sure
 *
 * Creates an #AgsOpenFile.
 *
 * Returns: an new #AgsOpenFile.
 *
 * Since: 0.4
 */
AgsOpenFile*
ags_open_file_new(AgsAudio *audio,
		  GSList *filenames,
		  gboolean overwrite_channels,
		  gboolean create_channels)
{
  AgsOpenFile *open_file;

  open_file = (AgsOpenFile *) g_object_new(AGS_TYPE_OPEN_FILE,
					   NULL);

  open_file->audio = audio;
  open_file->filenames = filenames;
  open_file->overwrite_channels = overwrite_channels;
  open_file->create_channels = create_channels;

  return(open_file);
}
