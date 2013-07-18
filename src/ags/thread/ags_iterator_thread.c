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

#include <ags/thread/ags_iterator_thread.h>

void ags_iterator_thread_class_init(AgsIteratorThreadClass *iterator_thread);
void ags_iterator_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_iterator_thread_init(AgsIteratorThread *iterator_thread);
void ags_iterator_thread_connect(AgsConnectable *connectable);
void ags_iterator_thread_disconnect(AgsConnectable *connectable);
void ags_iterator_thread_finalize(GObject *gobject);

void ags_iterator_thread_start(AgsThread *thread);

static gpointer ags_iterator_thread_parent_class = NULL;
static AgsConnectableInterface *ags_iterator_thread_parent_connectable_interface;

GType
ags_iterator_thread_get_type()
{
  static GType ags_type_iterator_thread = 0;

  if(!ags_type_iterator_thread){
    static const GTypeInfo ags_iterator_thread_info = {
      sizeof (AgsIteratorThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_iterator_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsIteratorThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_iterator_thread_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_iterator_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_iterator_thread = g_type_register_static(AGS_TYPE_THREAD,
						      "AgsIteratorThread\0",
						      &ags_iterator_thread_info,
						      0);
    
    g_type_add_interface_static(ags_type_iterator_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_iterator_thread);
}

void
ags_iterator_thread_class_init(AgsIteratorThreadClass *iterator_thread)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;

  ags_iterator_thread_parent_class = g_type_class_peek_parent(iterator_thread);

  /* GObject */
  gobject = (GObjectClass *) iterator_thread;

  gobject->finalize = ags_iterator_thread_finalize;

  /* AgsThread */
  thread = (AgsThreadClass *) iterator_thread;

  thread->start = ags_iterator_thread_start;
}

void
ags_iterator_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_iterator_thread_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_iterator_thread_connect;
  connectable->disconnect = ags_iterator_thread_disconnect;
}

void
ags_iterator_thread_init(AgsIteratorThread *iterator_thread)
{
  iterator_thread->flags = 0;
}

void
ags_iterator_thread_connect(AgsConnectable *connectable)
{
  AgsThread *thread;

  thread = AGS_THREAD(connectable);

  ags_iterator_thread_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_iterator_thread_disconnect(AgsConnectable *connectable)
{
  ags_iterator_thread_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_iterator_thread_finalize(GObject *gobject)
{
  AgsIteratorThread *iterator_thread;

  iterator_thread = AGS_ITERATOR_THREAD(gobject);

  /*  */
  G_OBJECT_CLASS(ags_iterator_thread_parent_class)->finalize(gobject);
}

void
ags_iterator_thread_start(AgsThread *thread)
{
  AGS_THREAD_CLASS(ags_iterator_thread_parent_class)->start(thread);
}

AgsIteratorThread*
ags_iterator_thread_new(GObject *iterator)
{
  AgsIteratorThread *iterator_thread;
  
  iterator_thread = (AgsIteratorThread *) g_object_new(AGS_TYPE_ITERATOR_THREAD,
						       NULL);

  iterator_thread->iterator = iterator;

  return(iterator_thread);
}
