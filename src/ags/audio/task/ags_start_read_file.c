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

#include <ags/audio/task/ags_start_read_file.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

void ags_start_read_file_class_init(AgsStartReadFileClass *start_read_file);
void ags_start_read_file_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_start_read_file_init(AgsStartReadFile *start_read_file);
void ags_start_read_file_connect(AgsConnectable *connectable);
void ags_start_read_file_disconnect(AgsConnectable *connectable);
void ags_start_read_file_finalize(GObject *gobject);

void ags_start_read_file_launch(AgsTask *task);

/**
 * SECTION:ags_start_read_file
 * @short_description: start devout object
 * @title: AgsStartReadFile
 * @section_id:
 * @include: ags/audio/task/ags_start_read_file.h
 *
 * The #AgsStartReadFile task starts devout.
 */

static gpointer ags_start_read_file_parent_class = NULL;
static AgsConnectableInterface *ags_start_read_file_parent_connectable_interface;

GType
ags_start_read_file_get_type()
{
  static GType ags_type_start_read_file = 0;

  if(!ags_type_start_read_file){
    static const GTypeInfo ags_start_read_file_info = {
      sizeof (AgsStartReadFileClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_start_read_file_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsStartReadFile),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_start_read_file_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_start_read_file_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_start_read_file = g_type_register_static(AGS_TYPE_TASK,
						      "AgsStartReadFile\0",
						      &ags_start_read_file_info,
						      0);

    g_type_add_interface_static(ags_type_start_read_file,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_start_read_file);
}

void
ags_start_read_file_class_init(AgsStartReadFileClass *start_read_file)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_start_read_file_parent_class = g_type_class_peek_parent(start_read_file);

  /* gobject */
  gobject = (GObjectClass *) start_read_file;

  gobject->finalize = ags_start_read_file_finalize;

  /* task */
  task = (AgsTaskClass *) start_read_file;

  task->launch = ags_start_read_file_launch;
}

void
ags_start_read_file_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_start_read_file_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_start_read_file_connect;
  connectable->disconnect = ags_start_read_file_disconnect;
}

void
ags_start_read_file_init(AgsStartReadFile *start_read_file)
{
  start_read_file->file = NULL;
}

void
ags_start_read_file_connect(AgsConnectable *connectable)
{
  ags_start_read_file_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_start_read_file_disconnect(AgsConnectable *connectable)
{
  ags_start_read_file_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_start_read_file_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_start_read_file_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_start_read_file_launch(AgsTask *task)
{
  AgsStartReadFile *start_read_file;

  start_read_file = AGS_START_READ_FILE(task);

  g_message("starting\0");
  
  ags_file_read_start(start_read_file->file);

  g_message("started\0");

  ags_file_close(start_read_file->file);
  g_object_unref(start_read_file->file);
}

/**
 * ags_start_read_file_new:
 * @file: the #AgsFile
 *
 * Creates an #AgsStartReadFile.
 *
 * Returns: an new #AgsStartReadFile.
 *
 * Since: 0.4
 */
AgsStartReadFile*
ags_start_read_file_new(AgsFile *file)
{
  AgsStartReadFile *start_read_file;

  start_read_file = (AgsStartReadFile *) g_object_new(AGS_TYPE_START_READ_FILE,
						      NULL);

  start_read_file->file = file;

  return(start_read_file);
}
