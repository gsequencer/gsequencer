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

#include <ags/thread/ags_gui_thread.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/audio/ags_devout.h>

#include <ags/thread/ags_task_completion.h>

#include <math.h>

void ags_gui_thread_class_init(AgsGuiThreadClass *gui_thread);
void ags_gui_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_gui_thread_init(AgsGuiThread *gui_thread);
void ags_gui_thread_connect(AgsConnectable *connectable);
void ags_gui_thread_disconnect(AgsConnectable *connectable);
void ags_gui_thread_finalize(GObject *gobject);

void ags_gui_thread_start(AgsThread *thread);
void ags_gui_thread_run(AgsThread *thread);
void ags_gui_thread_suspend(AgsThread *thread);
void ags_gui_thread_resume(AgsThread *thread);
void ags_gui_thread_stop(AgsThread *thread);

void ags_gui_thread_suspend_handler(int sig);

/**
 * SECTION:ags_gui_thread
 * @short_description: gui thread
 * @title: AgsGuiThread
 * @section_id:
 * @include: ags/thread/ags_gui_thread.h
 *
 * The #AgsGuiThread acts as graphical user interface thread.
 */

static gpointer ags_gui_thread_parent_class = NULL;
static AgsConnectableInterface *ags_gui_thread_parent_connectable_interface;

GType
ags_gui_thread_get_type()
{
  static GType ags_type_gui_thread = 0;

  if(!ags_type_gui_thread){
    static const GTypeInfo ags_gui_thread_info = {
      sizeof (AgsGuiThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_gui_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsGuiThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_gui_thread_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_gui_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_gui_thread = g_type_register_static(AGS_TYPE_THREAD,
						    "AgsGuiThread\0",
						    &ags_gui_thread_info,
						    0);
    
    g_type_add_interface_static(ags_type_gui_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_gui_thread);
}

void
ags_gui_thread_class_init(AgsGuiThreadClass *gui_thread)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;

  ags_gui_thread_parent_class = g_type_class_peek_parent(gui_thread);

  /* GObject */
  gobject = (GObjectClass *) gui_thread;

  gobject->finalize = ags_gui_thread_finalize;

  /* AgsThread */
  thread = (AgsThreadClass *) gui_thread;

  thread->start = ags_gui_thread_start;
  thread->run = ags_gui_thread_run;
  thread->suspend = ags_gui_thread_suspend;
  thread->resume = ags_gui_thread_resume;
  thread->stop = ags_gui_thread_stop;
}

void
ags_gui_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_gui_thread_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_gui_thread_connect;
  connectable->disconnect = ags_gui_thread_disconnect;
}

void
ags_gui_thread_init(AgsGuiThread *gui_thread)
{
  AgsThread *thread;

  thread = AGS_THREAD(gui_thread);

  //  g_atomic_int_or(&(thread->flags),
  //		  AGS_THREAD_TIMELOCK_RUN);
  
  thread->freq = AGS_GUI_THREAD_DEFAULT_JIFFIE;

  g_cond_init(&(gui_thread->cond));
  g_mutex_init(&(gui_thread->mutex));

  gui_thread->task_completion = NULL;
}

void
ags_gui_thread_connect(AgsConnectable *connectable)
{
  ags_gui_thread_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_gui_thread_disconnect(AgsConnectable *connectable)
{
  ags_gui_thread_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_gui_thread_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_gui_thread_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_gui_thread_start(AgsThread *thread)
{
  AgsGuiThread *gui_thread;

  /*  */
  gui_thread = AGS_GUI_THREAD(thread);

  /*  */
  if((AGS_THREAD_SINGLE_LOOP & (g_atomic_int_get(&(thread->flags)))) == 0){
    AGS_THREAD_CLASS(ags_gui_thread_parent_class)->start(thread);

    //    ags_thread_start(gui_thread->gui_task_thread);
  }
}

void
ags_gui_thread_run(AgsThread *thread)
{
  AgsAudioLoop *audio_loop;
  AgsGuiThread *gui_thread;
  AgsTaskThread *task_thread;
  GMainContext *main_context;
  int success;

  auto void ags_gui_thread_do_gtk_iteration();
  auto void ags_gui_thread_complete_task();
  
  void ags_gui_thread_do_gtk_iteration(){

    if(!g_main_context_acquire(main_context)){
      gboolean got_ownership = FALSE;

      while(!got_ownership){
	got_ownership = g_main_context_wait(main_context,
					    &(gui_thread->cond),
					    &(gui_thread->mutex));
      }
    }

    /*  */
    //    success = pthread_mutex_trylock(thread->suspend_mutex);
    success = FALSE;
    
    if(success){
      g_atomic_int_set(&(thread->critical_region),
		       TRUE);
    }

    /*  */
    pthread_mutex_lock(audio_loop->recall_mutex);

    if(success){
      /*  */
      pthread_mutex_unlock(thread->suspend_mutex);
    }else{
      //      g_atomic_int_set(&(thread->critical_region),
      //	       TRUE);
    }

    /*  */
    gdk_threads_enter();
    gdk_threads_leave();

    g_main_context_iteration(main_context, FALSE);

    /*  */
    //    success = pthread_mutex_trylock(thread->suspend_mutex);
      
    /*  */
    pthread_mutex_unlock(audio_loop->recall_mutex);

    /*  */
    //    g_atomic_int_set(&(thread->critical_region),
    //		     FALSE);

    if(success){
      pthread_mutex_unlock(thread->suspend_mutex);
    }

    g_main_context_release(main_context);
  }

  void ags_gui_thread_complete_task()
  {
    GList *list, *list_next;

    list = gui_thread->task_completion;

    while(list != NULL){
      list_next = list->next;
      
      if((AGS_TASK_COMPLETION_READY & (g_atomic_int_get(&(AGS_TASK_COMPLETION(list->data)->flags)))) != 0){
	ags_task_completion_complete(AGS_TASK_COMPLETION(list->data));

	gui_thread->task_completion = g_list_remove(gui_thread->task_completion,
						    list->data);
      }

      list = list_next;
    }
  }
  
  gui_thread = AGS_GUI_THREAD(thread);
  audio_loop = AGS_AUDIO_LOOP(thread->parent);
  task_thread = AGS_TASK_THREAD(audio_loop->task_thread);

  /*  */
  main_context = g_main_context_default();

  ags_gui_thread_do_gtk_iteration();

  ags_gui_thread_complete_task();  
}

void
ags_gui_thread_suspend(AgsThread *thread)
{
  gboolean success;
  gboolean critical_region;

  success = pthread_mutex_trylock(thread->suspend_mutex);
  critical_region = g_atomic_int_get(&(thread->critical_region));

  if(success || critical_region){
    AgsAudioLoop *audio_loop;
    AgsGuiThread *gui_thread;
    AgsTaskThread *task_thread;
    
    if(success){
      pthread_mutex_unlock(thread->suspend_mutex);
    }

    gui_thread = AGS_GUI_THREAD(thread);
    audio_loop = AGS_AUDIO_LOOP(thread->parent);
    task_thread = AGS_TASK_THREAD(audio_loop->task_thread);  

    pthread_mutex_unlock(task_thread->launch_mutex);
  }
}

void
ags_gui_thread_resume(AgsThread *thread)
{
  gboolean success;
  gboolean critical_region;

  success = pthread_mutex_trylock(thread->suspend_mutex);
  critical_region = g_atomic_int_get(&(thread->critical_region));

  if(success || critical_region){
    AgsAudioLoop *audio_loop;
    AgsGuiThread *gui_thread;
    AgsTaskThread *task_thread;

    if(success){
      pthread_mutex_unlock(thread->suspend_mutex);
    }

    gui_thread = AGS_GUI_THREAD(thread);
    audio_loop = AGS_AUDIO_LOOP(thread->parent);
    task_thread = AGS_TASK_THREAD(audio_loop->task_thread);  

    pthread_mutex_lock(task_thread->launch_mutex);
  }
}

void
ags_gui_thread_stop(AgsThread *thread)
{
  /*  */
  AGS_THREAD_CLASS(ags_gui_thread_parent_class)->stop(thread);  

  /*  */
  gdk_flush();
}

/**
 * ags_gui_thread_new:
 *
 * Create a new #AgsGuiThread.
 *
 * Returns: the new #AgsGuiThread
 *
 * Since: 0.4
 */
AgsGuiThread*
ags_gui_thread_new()
{
  AgsGuiThread *gui_thread;
  
  gui_thread = (AgsGuiThread *) g_object_new(AGS_TYPE_GUI_THREAD,
					     NULL);

  return(gui_thread);
}
