/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2016 Joël Krähemann
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

#include <ags/thread/ags_gui_loop.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/object/ags_main_loop.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/audio/ags_config.h>

#include <fontconfig/fontconfig.h>

void ags_gui_loop_class_init(AgsGuiLoopClass *gui_loop);
void ags_gui_loop_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_gui_loop_main_loop_interface_init(AgsMainLoopInterface *main_loop);
void ags_gui_loop_init(AgsGuiLoop *gui_loop);
void ags_gui_loop_finalize(GObject *gobject);
void ags_gui_loop_connect(AgsConnectable *connectable);
void ags_gui_loop_disconnect(AgsConnectable *connectable);
pthread_mutex_t* ags_gui_loop_get_tree_lock(AgsMainLoop *main_loop);
void ags_gui_loop_set_async_queue(AgsMainLoop *main_loop, GObject *async_queue);
GObject* ags_gui_loop_get_async_queue(AgsMainLoop *main_loop);
void ags_gui_loop_set_tic(AgsMainLoop *main_loop, guint tic);
guint ags_gui_loop_get_tic(AgsMainLoop *main_loop);
void ags_gui_loop_set_last_sync(AgsMainLoop *main_loop, guint last_sync);
guint ags_gui_loop_get_last_sync(AgsMainLoop *main_loop);

void ags_gui_loop_start(AgsThread *thread);
void ags_gui_loop_run(AgsThread *thread);

/**
 * SECTION:ags_gui_loop
 * @short_description: gui loop
 * @title: AgsGuiLoop
 * @section_id:
 * @include: ags/thread/ags_gui_loop.h
 *
 * The #AgsGuiLoop is suitable as #AgsMainLoop and does
 * gui processing.
 */

static gpointer ags_gui_loop_parent_class = NULL;
static AgsConnectableInterface *ags_gui_loop_parent_connectable_interface;

extern pthread_mutex_t ags_application_mutex;
extern AgsConfig *config;

GType
ags_gui_loop_get_type()
{
  static GType ags_type_gui_loop = 0;

  if(!ags_type_gui_loop){
    static const GTypeInfo ags_gui_loop_info = {
      sizeof (AgsGuiLoopClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_gui_loop_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsGuiLoop),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_gui_loop_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_gui_loop_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_main_loop_interface_info = {
      (GInterfaceInitFunc) ags_gui_loop_main_loop_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_gui_loop = g_type_register_static(AGS_TYPE_THREAD,
					       "AgsGuiLoop\0",
					       &ags_gui_loop_info,
					       0);
    
    g_type_add_interface_static(ags_type_gui_loop,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_gui_loop,
				AGS_TYPE_MAIN_LOOP,
				&ags_main_loop_interface_info);
  }
  
  return (ags_type_gui_loop);
}

void
ags_gui_loop_class_init(AgsGuiLoopClass *gui_loop)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;
  GParamSpec *param_spec;

  ags_gui_loop_parent_class = g_type_class_peek_parent(gui_loop);

  /* GObject */
  gobject = (GObjectClass *) gui_loop;

  gobject->finalize = ags_gui_loop_finalize;

  /* AgsThread */
  thread = (AgsThreadClass *) gui_loop;
  
  thread->start = ags_gui_loop_start;
  thread->run = ags_gui_loop_run;

  /* AgsGuiLoop */
}

void
ags_gui_loop_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_gui_loop_parent_connectable_interface = g_type_interface_peek_parent(connectable);
  
  connectable->connect = ags_gui_loop_connect;
  connectable->disconnect = ags_gui_loop_disconnect;
}

void
ags_gui_loop_main_loop_interface_init(AgsMainLoopInterface *main_loop)
{
  main_loop->get_tree_lock = ags_gui_loop_get_tree_lock;
  main_loop->set_async_queue = ags_gui_loop_set_async_queue;
  main_loop->get_async_queue = ags_gui_loop_get_async_queue;
  main_loop->set_tic = ags_gui_loop_set_tic;
  main_loop->get_tic = ags_gui_loop_get_tic;
  main_loop->set_last_sync = ags_gui_loop_set_last_sync;
  main_loop->get_last_sync = ags_gui_loop_get_last_sync;
}

void
ags_gui_loop_init(AgsGuiLoop *gui_loop)
{
  AgsThread *thread;

  thread = (AgsThread *) gui_loop;
  
  thread->freq = AGS_GUI_LOOP_DEFAULT_JIFFIE;
  
  gui_loop->flags = 0;

  g_atomic_int_set(&(gui_loop->tic), 0);
  g_atomic_int_set(&(gui_loop->last_sync), 0);

  gui_loop->ags_main = NULL;

  /* tree lock mutex */
  pthread_mutexattr_init(&(gui_loop->tree_lock_mutexattr));
  pthread_mutexattr_settype(&(gui_loop->tree_lock_mutexattr), PTHREAD_MUTEX_RECURSIVE);

  gui_loop->tree_lock = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(gui_loop->tree_lock, &(gui_loop->tree_lock_mutexattr));
}

void
ags_gui_loop_finalize(GObject *gobject)
{
  AgsGuiLoop *gui_loop;

  gui_loop = AGS_GUI_LOOP(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_gui_loop_parent_class)->finalize(gobject);
}

void
ags_gui_loop_connect(AgsConnectable *connectable)
{
  ags_gui_loop_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_gui_loop_disconnect(AgsConnectable *connectable)
{
  ags_gui_loop_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

pthread_mutex_t*
ags_gui_loop_get_tree_lock(AgsMainLoop *main_loop)
{
  return(AGS_GUI_LOOP(main_loop)->tree_lock);
}

void
ags_gui_loop_set_async_queue(AgsMainLoop *main_loop, GObject *async_queue)
{
  AGS_GUI_LOOP(main_loop)->async_queue = async_queue;
}

GObject*
ags_gui_loop_get_async_queue(AgsMainLoop *main_loop)
{
  return(AGS_GUI_LOOP(main_loop)->async_queue);
}

void
ags_gui_loop_set_tic(AgsMainLoop *main_loop, guint tic)
{
  g_atomic_int_set(&(AGS_GUI_LOOP(main_loop)->tic),
		   tic);
}

guint
ags_gui_loop_get_tic(AgsMainLoop *main_loop)
{
  return(g_atomic_int_get(&(AGS_GUI_LOOP(main_loop)->tic)));
}

void
ags_gui_loop_set_last_sync(AgsMainLoop *main_loop, guint last_sync)
{
  g_atomic_int_set(&(AGS_GUI_LOOP(main_loop)->last_sync),
		   last_sync);
}

guint
ags_gui_loop_get_last_sync(AgsMainLoop *main_loop)
{
  gint val;

  val = g_atomic_int_get(&(AGS_GUI_LOOP(main_loop)->last_sync));

  return(val);
}

void
ags_gui_loop_start(AgsThread *thread)
{
  AgsGuiLoop *gui_loop;

  gui_loop = AGS_GUI_LOOP(thread);

  if((AGS_THREAD_SINGLE_LOOP & (thread->flags)) == 0){
    /*  */
    AGS_THREAD_CLASS(ags_gui_loop_parent_class)->start(thread);
  }
}

void
ags_gui_loop_run(AgsThread *thread)
{
  //TODO:JK: implement me
}

/**
 * ags_gui_loop_new:
 * @devout: the #AgsDevout
 * @ags_main: the #AgsMain
 *
 * Create a new #AgsGuiLoop.
 *
 * Returns: the new #AgsGuiLoop
 *
 * Since: 0.4
 */
AgsGuiLoop*
ags_gui_loop_new(GObject *devout, GObject *ags_main)
{
  AgsGuiLoop *gui_loop;

  gui_loop = (AgsGuiLoop *) g_object_new(AGS_TYPE_GUI_LOOP,
					 "devout\0", devout,
					 NULL);

  if(ags_main != NULL){
    g_object_ref(G_OBJECT(ags_main));
    gui_loop->ags_main = ags_main;
  }

  return(gui_loop);
}
