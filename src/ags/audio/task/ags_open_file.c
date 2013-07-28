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

void ags_open_file_class_init(AgsOpenFileClass *open_file);
void ags_open_file_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_open_file_init(AgsOpenFile *open_file);
void ags_open_file_connect(AgsConnectable *connectable);
void ags_open_file_disconnect(AgsConnectable *connectable);
void ags_open_file_finalize(GObject *gobject);
void ags_open_file_launch(AgsTask *task);

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

  open_file = AGS_OPEN_FILE(task);

  ags_audio_file_open(open_file->audio,
		      open_file->filenames,
		      open_file->overwrite_channels,
		      open_file->create_channels);
}

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
