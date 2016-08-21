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

#include <ags/X/thread/ags_polling_thread.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_application_context.h>

#include <math.h>

void ags_polling_thread_class_init(AgsPollingThreadClass *polling_thread);
void ags_polling_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_polling_thread_init(AgsPollingThread *polling_thread);
void ags_polling_thread_connect(AgsConnectable *connectable);
void ags_polling_thread_disconnect(AgsConnectable *connectable);
void ags_polling_thread_finalize(GObject *gobject);

void ags_polling_thread_start(AgsThread *thread);
void ags_polling_thread_run(AgsThread *thread);
void ags_polling_thread_stop(AgsThread *thread);

/**
 * SECTION:ags_polling_thread
 * @short_description: polling thread
 * @title: AgsPollingThread
 * @section_id:
 * @include: ags/thread/ags_polling_thread.h
 *
 * The #AgsPollingThread polls on the given file descriptors.
 */

static gpointer ags_polling_thread_parent_class = NULL;
static AgsConnectableInterface *ags_polling_thread_parent_connectable_interface;

GType
ags_polling_thread_get_type()
{
  static GType ags_type_polling_thread = 0;

  if(!ags_type_polling_thread){
    static const GTypeInfo ags_polling_thread_info = {
      sizeof (AgsPollingThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_polling_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPollingThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_polling_thread_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_polling_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_polling_thread = g_type_register_static(AGS_TYPE_THREAD,
						     "AgsPollingThread\0",
						     &ags_polling_thread_info,
						     0);
    
    g_type_add_interface_static(ags_type_polling_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_polling_thread);
}

void
ags_polling_thread_class_init(AgsPollingThreadClass *polling_thread)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;

  ags_polling_thread_parent_class = g_type_class_peek_parent(polling_thread);

  /* GObject */
  gobject = (GObjectClass *) polling_thread;

  gobject->finalize = ags_polling_thread_finalize;

  /* AgsThread */
  thread = (AgsThreadClass *) polling_thread;

  thread->start = ags_polling_thread_start;
  thread->run = ags_polling_thread_run;
  thread->stop = ags_polling_thread_stop;
}

void
ags_polling_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_polling_thread_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_polling_thread_connect;
  connectable->disconnect = ags_polling_thread_disconnect;
}

void
ags_polling_thread_init(AgsPollingThread *polling_thread)
{
  /* empty */
}

void
ags_polling_thread_connect(AgsConnectable *connectable)
{
  ags_polling_thread_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_polling_thread_disconnect(AgsConnectable *connectable)
{
  ags_polling_thread_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_polling_thread_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_polling_thread_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_polling_thread_start(AgsThread *thread)
{
  AgsPollingThread *polling_thread;

  /*  */
  polling_thread = AGS_POLLING_THREAD(thread);

  /*  */
  if((AGS_THREAD_SINGLE_LOOP & (g_atomic_int_get(&(thread->flags)))) == 0){
    AGS_THREAD_CLASS(ags_polling_thread_parent_class)->start(thread);
  }
}

void
ags_polling_thread_run(AgsThread *thread)
{
  AgsPollingThread *polling_thread;

  AgsThread *main_loop;

  //TODO:JK: implement me
}

void
ags_polling_thread_stop(AgsThread *thread)
{
  /*  */
  AGS_THREAD_CLASS(ags_polling_thread_parent_class)->stop(thread);  
}

/**
 * ags_polling_thread_new:
 *
 * Create a new #AgsPollingThread.
 *
 * Returns: the new #AgsPollingThread
 *
 * Since: 0.7.50
 */
AgsPollingThread*
ags_polling_thread_new()
{
  AgsPollingThread *polling_thread;
  
  polling_thread = (AgsPollingThread *) g_object_new(AGS_TYPE_POLLING_THREAD,
						     NULL);

  return(polling_thread);
}
