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

#include <ags/X/task/ags_simple_file_read.h>

#include <ags/object/ags_connectable.h>

void ags_simple_file_read_class_init(AgsSimpleFileReadClass *simple_file_read);
void ags_simple_file_read_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_simple_file_read_init(AgsSimpleFileRead *simple_file_read);
void ags_simple_file_read_connect(AgsConnectable *connectable);
void ags_simple_file_read_disconnect(AgsConnectable *connectable);
void ags_simple_file_read_finalize(GObject *gobject);

void ags_simple_file_read_launch(AgsTask *task);

/**
 * SECTION:ags_simple_file_read
 * @short_description: task to read simple file format
 * @title: AgsConfig
 * @section_id:
 * @include: ags/X/task/ags_simple_file_read.h
 *
 * #AgsSimpleFileRead lets you read the simple file format as a task.
 */

static gpointer ags_simple_file_read_parent_class = NULL;
static AgsConnectableInterface *ags_simple_file_read_parent_connectable_interface;

GType
ags_simple_file_read_get_type()
{
  static GType ags_type_simple_file_read = 0;

  if(!ags_type_simple_file_read){
    static const GTypeInfo ags_simple_file_read_info = {
      sizeof (AgsSimpleFileReadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_simple_file_read_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSimpleFileRead),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_simple_file_read_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_simple_file_read_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_simple_file_read = g_type_register_static(AGS_TYPE_TASK,
						       "AgsSimpleFileRead\0",
						       &ags_simple_file_read_info,
						       0);

    g_type_add_interface_static(ags_type_simple_file_read,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_simple_file_read);
}

void
ags_simple_file_read_class_init(AgsSimpleFileReadClass *simple_file_read)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_simple_file_read_parent_class = g_type_class_peek_parent(simple_file_read);

  /* gobject */
  gobject = (GObjectClass *) simple_file_read;

  gobject->finalize = ags_simple_file_read_finalize;

  /* task */
  task = (AgsTaskClass *) simple_file_read;

  task->launch = ags_simple_file_read_launch;
}

void
ags_simple_file_read_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_simple_file_read_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_simple_file_read_connect;
  connectable->disconnect = ags_simple_file_read_disconnect;
}

void
ags_simple_file_read_init(AgsSimpleFileRead *simple_file_read)
{
  simple_file_read->simple_file = NULL;
}

void
ags_simple_file_read_connect(AgsConnectable *connectable)
{
  ags_simple_file_read_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_simple_file_read_disconnect(AgsConnectable *connectable)
{
  ags_simple_file_read_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_simple_file_read_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_simple_file_read_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_simple_file_read_launch(AgsTask *task)
{  
  gdk_threads_enter();

  ags_simple_file_read(AGS_SIMPLE_FILE_READ(task)->simple_file);
  ags_simple_file_close(AGS_SIMPLE_FILE_READ(task)->simple_file);

  gdk_threads_leave();
}

/**
 * ags_simple_file_read_new:
 * @simple_file: the #AgsSimpleFile
 *
 * Create a new task to read the simple file format.
 *
 * Returns: the #AgsSimpleFileRead task
 * 
 * Since: 0.7.42
 */
AgsSimpleFileRead*
ags_simple_file_read_new(AgsSimpleFile *simple_file)
{
  AgsSimpleFileRead *simple_file_read;

  simple_file_read = (AgsSimpleFileRead *) g_object_new(AGS_TYPE_SIMPLE_FILE_READ,
							NULL);
  
  simple_file_read->simple_file = simple_file;

  return(simple_file_read);
}
