/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#include <ags/thread/ags_returnable_thread.h>

#include <ags-lib/object/ags_connectable.h>

void ags_returnable_thread_class_init(AgsReturnableThreadClass *returnable_thread);
void ags_returnable_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_returnable_thread_init(AgsReturnableThread *returnable_thread);
void ags_returnable_thread_connect(AgsConnectable *connectable);
void ags_returnable_thread_disconnect(AgsConnectable *connectable);
void ags_returnable_thread_finalize(GObject *gobject);

void ags_returnable_thread_start(AgsThread *thread);
void ags_returnable_thread_stop(AgsThread *thread);
void ags_returnable_thread_run(AgsThread *thread);

enum{
  SAFE_RUN,
  LAST_SIGNAL,
};

static gpointer ags_returnable_thread_parent_class = NULL;
static AgsConnectableInterface *ags_returnable_thread_parent_connectable_interface;
static guint returnable_thread_signals[LAST_SIGNAL];

GType
ags_returnable_thread_get_type()
{
  static GType ags_type_returnable_thread = 0;

  if(!ags_type_returnable_thread){
    static const GTypeInfo ags_returnable_thread_info = {
      sizeof (AgsReturnableThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_returnable_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsReturnableThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_returnable_thread_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_returnable_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_returnable_thread = g_type_register_static(AGS_TYPE_THREAD,
							"AgsReturnableThread\0",
							&ags_returnable_thread_info,
							0);

    g_type_add_interface_static(ags_type_returnable_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_returnable_thread);
}

void
ags_returnable_thread_class_init(AgsReturnableThreadClass *returnable_thread)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;

  ags_returnable_thread_parent_class = g_type_class_peek_parent(returnable_thread);

  /* GObjectClass */
  gobject = (GObjectClass *) returnable_thread;

  gobject->finalize = ags_returnable_thread_finalize;

  /* AgsThreadClass */
  thread->start = ags_returnable_thread_start;
  thread->stop = ags_returnable_thread_stop;
  thread->run = ags_returnable_thread_run;

  /* AgsReturnableThreadClass */
  returnable_thread->safe_run = NULL;

  /* signals */
  returnable_thread_signals[SAFE_RUN] =
    g_signal_new("safe-run\0",
		 G_TYPE_FROM_CLASS (returnable_thread),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsReturnableThreadClass, safe_run),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

}

void
ags_returnable_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_returnable_thread_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_returnable_thread_connect;
  connectable->disconnect = ags_returnable_thread_disconnect;
}

void
ags_returnable_thread_init(AgsReturnableThread *returnable_thread)
{
  g_atomic_int_set(&(returnable_thread->flags),
		   0);

  pthread_mutex_init(&(returnable_thread->reset_mutex), NULL);
  g_atomic_pointer_set(&(returnable_thread->safe_data),
		       NULL);
}

void
ags_returnable_thread_connect(AgsConnectable *connectable)
{
  ags_returnable_thread_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_returnable_thread_disconnect(AgsConnectable *connectable)
{
  ags_returnable_thread_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_returnable_thread_finalize(GObject *gobject)
{
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_returnable_thread_parent_class)->finalize(gobject);
}

void
ags_returnable_thread_start(AgsThread *thread)
{
  if((AGS_THREAD_RUNNING & (g_atomic_int_get(&thread->flags))) == 0){
    g_atomic_int_or(&(thread->flags),
		    AGS_THREAD_RUNNING);
    AGS_THREAD_CLASS(ags_returnable_thread_parent_class)->start(thread);
  }else{
    ags_thread_resume(thread);
  }
}

void
ags_returnable_thread_stop(AgsThread *thread)
{
  g_atomic_int_or(&(thread->flags),
		  AGS_THREAD_WAIT_0);
  
  ags_thread_suspend(thread);
}

void
ags_returnable_thread_run(AgsThread *thread)
{
  AgsReturnableThread *returnable_thread;

  returnable_thread = AGS_RETURNABLE_THREAD(thread);

  pthread_mutex_lock(&(returnable_thread->reset_mutex));

  ags_returnable_thread_safe_run(returnable_thread);

  if((AGS_RETURNABLE_THREAD_RESET & (g_atomic_int_get(&(returnable_thread->flags)))) != 0){
    AGS_RETURNABLE_THREAD_GET_CLASS(thread)->safe_run = NULL;
  }

  pthread_mutex_unlock(&(returnable_thread->reset_mutex));


  g_atomic_int_or(&(thread->flags),
		  AGS_THREAD_WAIT_0);
}

void
ags_returnable_thread_safe_run(AgsReturnableThread *returnable_thread)
{
  g_return_if_fail(AGS_IS_RETURNABLE_THREAD(returnable_thread));

  g_object_ref(G_OBJECT(returnable_thread));
  g_signal_emit(G_OBJECT(returnable_thread),
		returnable_thread_signals[SAFE_RUN], 0);
  g_object_unref(G_OBJECT(returnable_thread));
}

AgsReturnableThread*
ags_returnable_thread_new()
{
  AgsReturnableThread *returnable_thread;

  returnable_thread = (AgsReturnableThread *) g_object_new(AGS_TYPE_RETURNABLE_THREAD,
							   NULL);

  return(returnable_thread);
}
