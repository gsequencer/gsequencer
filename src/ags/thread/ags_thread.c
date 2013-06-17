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

#include <ags/thread/ags_thread.h>

void ags_thread_class_init(AgsThreadClass *thread);
void ags_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_thread_init(AgsThread *thread);
void ags_thread_connect(AgsConnectable *connectable);
void ags_thread_disconnect(AgsConnectable *connectable);
void ags_thread_finalize(GObject *gobject);
void ags_thread_launch(AgsTask *task);

void ags_thread_real_start(AgsThread *thread);
void ags_thread_real_run(AgsThread *thread);

static gpointer ags_thread_parent_class = NULL;
static AgsConnectableInterface *ags_thread_parent_connectable_interface;

GType
ags_thread_get_type()
{
  static GType ags_type_thread = 0;

  if(!ags_type_thread){
    static const GTypeInfo ags_thread_info = {
      sizeof (AgsThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_thread_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_thread = g_type_register_static(AGS_TYPE_TASK,
					     "AgsThread\0",
					     &ags_thread_info,
					     0);
    
    g_type_add_interface_static(ags_type_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_thread);
}

void
ags_thread_class_init(AgsThreadClass *thread)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_thread_parent_class = g_type_class_peek_parent(thread);

  /* GObject */
  gobject = (GObjectClass *) thread;

  gobject->finalize = ags_thread_finalize;

  /* AgsTask */
  task = (AgsTaskClass *) thread;
}

void
ags_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_thread_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_thread_connect;
  connectable->disconnect = ags_thread_disconnect;
}

void
ags_thread_init(AgsThread *thread)
{
  thread->flags = 0;

  thread->parent = NULL;
  thread->next = NULL;
  thread->prev = NULL;
  thread->children = NULL;

  thread->data = NULL;
}

void
ags_thread_connect(AgsConnectable *connectable)
{
  ags_thread_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_thread_disconnect(AgsConnectable *connectable)
{
  ags_thread_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_thread_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_thread_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_thread_real_start(AgsThread *thread)
{
  //TODO:JK: implement me
}

void
ags_thread_start(AgsThread *thread)
{
  //TODO:JK: implement me
}

void
ags_thread_real_run(AgsThread *thread)
{
  //TODO:JK: implement me
}

void
ags_thread_run(AgsThread *thread)
{
  //TODO:JK: implement me
}

AgsThread*
ags_thread_new(GObject *data)
{
  AgsThread *thread;

  thread = (AgsThread *) g_object_new(AGS_TYPE_THREAD,
				      NULL);

  thread->data = data;

  return(thread);
}
