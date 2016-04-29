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

#include <ags/thread/ags_generic_main_loop.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_main_loop.h>

void ags_generic_main_loop_class_init(AgsGenericMainLoopClass *generic_main_loop);
void ags_generic_main_loop_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_generic_main_loop_main_loop_interface_init(AgsMainLoopInterface *main_loop);
void ags_generic_main_loop_init(AgsGenericMainLoop *generic_main_loop);
void ags_generic_main_loop_connect(AgsConnectable *connectable);
void ags_generic_main_loop_disconnect(AgsConnectable *connectable);
pthread_mutex_t* ags_generic_main_loop_get_tree_lock(AgsMainLoop *main_loop);
void ags_generic_main_loop_set_async_queue(AgsMainLoop *main_loop, GObject *async_queue);
GObject* ags_generic_main_loop_get_async_queue(AgsMainLoop *main_loop);
void ags_generic_main_loop_set_tic(AgsMainLoop *main_loop, guint tic);
guint ags_generic_main_loop_get_tic(AgsMainLoop *main_loop);
void ags_generic_main_loop_set_last_sync(AgsMainLoop *main_loop, guint last_sync);
guint ags_generic_main_loop_get_last_sync(AgsMainLoop *main_loop);
void ags_generic_main_loop_finalize(GObject *gobject);

void ags_generic_main_loop_start(AgsThread *thread);

/**
 * SECTION:ags_generic_main_loop
 * @short_description: generic loop
 * @title: AgsGenericMainLoop
 * @section_id:
 * @include: ags/thread/ags_generic_main_loop.h
 *
 * The #AgsGenericMainLoop is suitable as #AgsMainLoop and does
 * generic processing.
 */

static gpointer ags_generic_main_loop_parent_class = NULL;
static AgsConnectableInterface *ags_generic_main_loop_parent_connectable_interface;

GType
ags_generic_main_loop_get_type()
{
  static GType ags_type_generic_main_loop = 0;

  if(!ags_type_generic_main_loop){
    static const GTypeInfo ags_generic_main_loop_info = {
      sizeof (AgsGenericMainLoopClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_generic_main_loop_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsGenericMainLoop),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_generic_main_loop_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_generic_main_loop_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_main_loop_interface_info = {
      (GInterfaceInitFunc) ags_generic_main_loop_main_loop_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_generic_main_loop = g_type_register_static(AGS_TYPE_THREAD,
						 "AgsGenericMainLoop\0",
						 &ags_generic_main_loop_info,
						 0);
    
    g_type_add_interface_static(ags_type_generic_main_loop,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_generic_main_loop,
				AGS_TYPE_MAIN_LOOP,
				&ags_main_loop_interface_info);
  }
  
  return (ags_type_generic_main_loop);
}

void
ags_generic_main_loop_class_init(AgsGenericMainLoopClass *generic_main_loop)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;

  ags_generic_main_loop_parent_class = g_type_class_peek_parent(generic_main_loop);

  /* GObject */
  gobject = (GObjectClass *) generic_main_loop;

  gobject->finalize = ags_generic_main_loop_finalize;

  /* AgsThread */
  thread = (AgsThreadClass *) generic_main_loop;
  
  thread->start = ags_generic_main_loop_start;

  /* AgsGenericMainLoop */
}

void
ags_generic_main_loop_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_generic_main_loop_parent_connectable_interface = g_type_interface_peek_parent(connectable);
  
  connectable->connect = ags_generic_main_loop_connect;
  connectable->disconnect = ags_generic_main_loop_disconnect;
}

void
ags_generic_main_loop_main_loop_interface_init(AgsMainLoopInterface *main_loop)
{
  main_loop->get_tree_lock = ags_generic_main_loop_get_tree_lock;
  main_loop->set_async_queue = ags_generic_main_loop_set_async_queue;
  main_loop->get_async_queue = ags_generic_main_loop_get_async_queue;
  main_loop->set_tic = ags_generic_main_loop_set_tic;
  main_loop->get_tic = ags_generic_main_loop_get_tic;
  main_loop->set_last_sync = ags_generic_main_loop_set_last_sync;
  main_loop->get_last_sync = ags_generic_main_loop_get_last_sync;
}

void
ags_generic_main_loop_init(AgsGenericMainLoop *generic_main_loop)
{
  AgsThread *thread;

  /* calculate frequency */
  thread = (AgsThread *) generic_main_loop;
  
  thread->freq = AGS_GENERIC_MAIN_LOOP_DEFAULT_JIFFIE;

  g_atomic_int_set(&(generic_main_loop->tic), 0);
  g_atomic_int_set(&(generic_main_loop->last_sync), 0);

  generic_main_loop->application_context = NULL;
  
  /* tree lock mutex */
  pthread_mutexattr_init(&(generic_main_loop->tree_lock_mutexattr));
  pthread_mutexattr_settype(&(generic_main_loop->tree_lock_mutexattr), PTHREAD_MUTEX_RECURSIVE);

  generic_main_loop->tree_lock = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(generic_main_loop->tree_lock, &(generic_main_loop->tree_lock_mutexattr));
}

void
ags_generic_main_loop_connect(AgsConnectable *connectable)
{
  ags_generic_main_loop_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_generic_main_loop_disconnect(AgsConnectable *connectable)
{
  ags_generic_main_loop_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

pthread_mutex_t*
ags_generic_main_loop_get_tree_lock(AgsMainLoop *main_loop)
{
  return(AGS_GENERIC_MAIN_LOOP(main_loop)->tree_lock);
}

void
ags_generic_main_loop_set_async_queue(AgsMainLoop *main_loop, GObject *async_queue)
{
  AGS_GENERIC_MAIN_LOOP(main_loop)->async_queue = async_queue;
}

GObject*
ags_generic_main_loop_get_async_queue(AgsMainLoop *main_loop)
{
  return(AGS_GENERIC_MAIN_LOOP(main_loop)->async_queue);
}

void
ags_generic_main_loop_set_tic(AgsMainLoop *main_loop, guint tic)
{
  g_atomic_int_set(&(AGS_GENERIC_MAIN_LOOP(main_loop)->tic),
		   tic);
}

guint
ags_generic_main_loop_get_tic(AgsMainLoop *main_loop)
{
  return(g_atomic_int_get(&(AGS_GENERIC_MAIN_LOOP(main_loop)->tic)));
}

void
ags_generic_main_loop_set_last_sync(AgsMainLoop *main_loop, guint last_sync)
{
  g_atomic_int_set(&(AGS_GENERIC_MAIN_LOOP(main_loop)->last_sync),
		   last_sync);
}

guint
ags_generic_main_loop_get_last_sync(AgsMainLoop *main_loop)
{
  gint val;

  val = g_atomic_int_get(&(AGS_GENERIC_MAIN_LOOP(main_loop)->last_sync));

  return(val);
}

void
ags_generic_main_loop_finalize(GObject *gobject)
{
  AgsGenericMainLoop *generic_main_loop;

  generic_main_loop = AGS_GENERIC_MAIN_LOOP(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_generic_main_loop_parent_class)->finalize(gobject);
}

void
ags_generic_main_loop_start(AgsThread *thread)
{
  AgsGenericMainLoop *generic_main_loop;

  generic_main_loop = AGS_GENERIC_MAIN_LOOP(thread);

  if((AGS_THREAD_SINGLE_LOOP & (g_atomic_int_get(&(thread->flags)))) == 0){
    /*  */
    AGS_THREAD_CLASS(ags_generic_main_loop_parent_class)->start(thread);
  }
}
 
/**
 * ags_generic_main_loop_new:
 * @application_context: the #AgsMain
 *
 * Create a new #AgsGenericMainLoop.
 *
 * Returns: the new #AgsGenericMainLoop
 *
 * Since: 0.7.11
 */
AgsGenericMainLoop*
ags_generic_main_loop_new(GObject *application_context)
{
  AgsGenericMainLoop *generic_main_loop;

  generic_main_loop = (AgsGenericMainLoop *) g_object_new(AGS_TYPE_GENERIC_MAIN_LOOP,
					     NULL);

  if(application_context != NULL){
    g_object_ref(G_OBJECT(application_context));
    generic_main_loop->application_context = application_context;
  }

  return(generic_main_loop);
}
