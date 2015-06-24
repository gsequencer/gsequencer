/* This file is part of GSequencer.
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

#include <ags/audio/task/ags_open_single_file.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/file/ags_file_link.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>

#include <ags/audio/file/ags_audio_file.h>

void ags_open_single_file_class_init(AgsOpenSingleFileClass *open_single_file);
void ags_open_single_file_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_open_single_file_init(AgsOpenSingleFile *open_single_file);
void ags_open_single_file_connect(AgsConnectable *connectable);
void ags_open_single_file_disconnect(AgsConnectable *connectable);
void ags_open_single_file_finalize(GObject *gobject);
void ags_open_single_file_launch(AgsTask *task);

static gpointer ags_open_single_file_parent_class = NULL;
static AgsConnectableInterface *ags_open_single_file_parent_connectable_interface;

GType
ags_open_single_file_get_type()
{
  static GType ags_type_open_single_file = 0;

  if(!ags_type_open_single_file){
    static const GTypeInfo ags_open_single_file_info = {
      sizeof (AgsOpenSingleFileClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_open_single_file_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOpenSingleFile),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_open_single_file_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_open_single_file_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_open_single_file = g_type_register_static(AGS_TYPE_TASK,
						       "AgsOpenSingleFile\0",
						       &ags_open_single_file_info,
						       0);

    g_type_add_interface_static(ags_type_open_single_file,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_open_single_file);
}

void
ags_open_single_file_class_init(AgsOpenSingleFileClass *open_single_file)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_open_single_file_parent_class = g_type_class_peek_parent(open_single_file);

  /* GObject */
  gobject = (GObjectClass *) open_single_file;

  gobject->finalize = ags_open_single_file_finalize;

  /* AgsTask */
  task = (AgsTaskClass *) open_single_file;

  task->launch = ags_open_single_file_launch;
}

void
ags_open_single_file_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_open_single_file_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_open_single_file_connect;
  connectable->disconnect = ags_open_single_file_disconnect;
}

void
ags_open_single_file_init(AgsOpenSingleFile *open_single_file)
{
  open_single_file->channel = NULL;
  open_single_file->devout = NULL;
  open_single_file->filename = NULL;
  open_single_file->start_channel = 0;
  open_single_file->audio_channels = 0;
}

void
ags_open_single_file_connect(AgsConnectable *connectable)
{
  ags_open_single_file_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_open_single_file_disconnect(AgsConnectable *connectable)
{
  ags_open_single_file_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_open_single_file_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_open_single_file_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_open_single_file_launch(AgsTask *task)
{
  AgsOpenSingleFile *open_single_file;
  AgsChannel *channel;
  AgsFileLink *file_link;
  AgsAudioSignal *old_template;
  AgsAudioFile *audio_file;
  GList *audio_signal;
  guint i;
  GError *error;

  open_single_file = AGS_OPEN_SINGLE_FILE(task);

  /* open audio file and read audio signal */
  audio_file = ags_audio_file_new(open_single_file->filename,
				  open_single_file->devout,
				  open_single_file->start_channel, open_single_file->audio_channels);

  ags_audio_file_open(audio_file);
  ags_audio_file_read_audio_signal(audio_file);

  /* iterate channels */
  channel = open_single_file->channel;
  audio_signal = audio_file->audio_signal;

  for(i = 0; i < open_single_file->audio_channels; i++){
    /* unset link */
    if(channel->link != NULL){
      error = NULL;
      ags_channel_set_link(channel, NULL,
			   &error);

      if(error != NULL){
	g_warning("%s\0", error->message);
      }
    }

    file_link = g_object_new(AGS_TYPE_FILE_LINK,
			     "filename\0", open_single_file->filename,
			     NULL);
    g_object_set(G_OBJECT(channel),
		 "file-link\0", file_link,
		 NULL);

    /* mark as template */
    AGS_AUDIO_SIGNAL(audio_signal->data)->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;

    /* old source */
    old_template = ags_audio_signal_get_template(channel->first_recycling->audio_signal);
    
    /* add as template */
    ags_recycling_add_audio_signal(channel->first_recycling,
				   AGS_AUDIO_SIGNAL(audio_signal->data));

    /* remove old template */
    ags_recycling_remove_audio_signal(channel->first_recycling,
				      old_template);

    /* iterate */
    channel = channel->next;
    audio_signal = audio_signal->next;
  }

  g_object_unref(audio_file);
}

AgsOpenSingleFile*
ags_open_single_file_new(AgsChannel *channel,
			 AgsDevout *devout,
			 gchar *filename,
			 guint start_channel,
			 guint audio_channels)
{
  AgsOpenSingleFile *open_single_file;

  open_single_file = (AgsOpenSingleFile *) g_object_new(AGS_TYPE_OPEN_SINGLE_FILE,
							NULL);

  open_single_file->channel = channel;
  open_single_file->devout = devout;
  open_single_file->filename = filename;
  open_single_file->start_channel = start_channel;
  open_single_file->audio_channels = audio_channels;

  return(open_single_file);
}
