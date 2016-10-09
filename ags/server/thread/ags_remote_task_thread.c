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

#include <ags/server/thread/ags_remote_task_thread.h>

#include <ags/object/ags_connectable.h>

void ags_remote_task_thread_class_init(AgsRemoteTaskThreadClass *remote_task_thread);
void ags_remote_task_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_remote_task_thread_init(AgsRemoteTaskThread *remote_task_thread);
void ags_remote_task_thread_connect(AgsConnectable *connectable);
void ags_remote_task_thread_disconnect(AgsConnectable *connectable);
void ags_remote_task_thread_finalize(GObject *gobject);

void ags_remote_task_thread_start(AgsThread *thread);
void ags_remote_task_thread_run(AgsThread *thread);

/**
 * SECTION:ags_remote_task_thread
 * @short_description: returnable thread
 * @title: AgsRemoteTaskThread
 * @section_id:
 * @include: ags/server/thread/ags_remote_task_thread.h
 *
 * The #AgsRemoteTaskThread invokes remote tasks thread-safe.
 */

static gpointer ags_remote_task_thread_parent_class = NULL;
static AgsConnectableInterface *ags_remote_task_thread_parent_connectable_interface;

GType
ags_remote_task_thread_get_type()
{
  static GType ags_type_remote_task_thread = 0;

  if(!ags_type_remote_task_thread){
    static const GTypeInfo ags_remote_task_thread_info = {
      sizeof (AgsRemoteTaskThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_remote_task_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRemoteTaskThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_remote_task_thread_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_remote_task_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_remote_task_thread = g_type_register_static(AGS_TYPE_TASK_THREAD,
							 "AgsRemoteTaskThread\0",
							 &ags_remote_task_thread_info,
							 0);

    g_type_add_interface_static(ags_type_remote_task_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_remote_task_thread);
}

void
ags_remote_task_thread_class_init(AgsRemoteTaskThreadClass *remote_task_thread)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;

  ags_remote_task_thread_parent_class = g_type_class_peek_parent(remote_task_thread);

  /* GObjectClass */
  gobject = (GObjectClass *) remote_task_thread;

  gobject->finalize = ags_remote_task_thread_finalize;

  /* AgsThreadClass */
  thread = (AgsThreadClass *) remote_task_thread;

  thread->start = ags_remote_task_thread_start;
  thread->run = ags_remote_task_thread_run;
}

void
ags_remote_task_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_remote_task_thread_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_remote_task_thread_connect;
  connectable->disconnect = ags_remote_task_thread_disconnect;
}

void
ags_remote_task_thread_init(AgsRemoteTaskThread *remote_task_thread)
{
}

void
ags_remote_task_thread_connect(AgsConnectable *connectable)
{
  ags_remote_task_thread_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_remote_task_thread_disconnect(AgsConnectable *connectable)
{
  ags_remote_task_thread_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_remote_task_thread_finalize(GObject *gobject)
{  
  /* call parent */
  G_OBJECT_CLASS(ags_remote_task_thread_parent_class)->finalize(gobject);
}

void
ags_remote_task_thread_start(AgsThread *thread)
{
  AGS_THREAD_CLASS(ags_remote_task_thread_parent_class)->start(thread);
}

void
ags_remote_task_thread_run(AgsThread *thread)
{
  //TODO:JK: implement me
}

/**
 * ags_remote_task_thread_new:
 * 
 * Instantiate a new #AgsRemoteTaskThread
 *
 * Returns: the new #AgsRemoteTaskThread
 *
 * Since: 1.0.0
 */
AgsRemoteTaskThread*
ags_remote_task_thread_new()
{
  AgsRemoteTaskThread *remote_task_thread;

  remote_task_thread = (AgsRemoteTaskThread *) g_object_new(AGS_TYPE_REMOTE_TASK_THREAD,
							    NULL);
  
  return(remote_task_thread);
}
