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

#include <ags/thread/ags_gui_thread.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_devout.h>

#include <math.h>

void ags_gui_thread_class_init(AgsGuiThreadClass *gui_thread);
void ags_gui_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_gui_thread_init(AgsGuiThread *gui_thread);
void ags_gui_thread_connect(AgsConnectable *connectable);
void ags_gui_thread_disconnect(AgsConnectable *connectable);
void ags_gui_thread_finalize(GObject *gobject);

void ags_gui_thread_start(AgsThread *thread);
void ags_gui_thread_run(AgsThread *thread);
void ags_gui_thread_stop(AgsThread *thread);

static gpointer ags_gui_thread_parent_class = NULL;
static AgsConnectableInterface *ags_gui_thread_parent_connectable_interface;

#define NSEC_PER_SEC    (1000000000) /* The number of nsecs per sec. */

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
  gui_thread->main_loop = g_main_loop_new(NULL, FALSE);

  gui_thread->frequency = 1.0 / (double) AGS_GUI_THREAD_DEFAULT_JIFFIE;
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

  gui_thread->iter = 0.0;

  /*  */
  GDK_THREADS_LEAVE();

  /*  */
  AGS_THREAD_CLASS(ags_gui_thread_parent_class)->start(thread);
}

void
ags_gui_thread_run(AgsThread *thread)
{
  AgsAudioLoop *audio_loop;
  AgsGuiThread *gui_thread;
  AgsTaskThread *task_thread;
  GMainLoop *main_loop;
  GMainContext *main_context;
  GCond cond;
  GMutex mutex;
  guint i, i_stop;

  gui_thread = AGS_GUI_THREAD(thread);
  audio_loop = AGS_AUDIO_LOOP(thread->parent);
  task_thread = AGS_TASK_THREAD(audio_loop->task_thread);

  /*  */
  g_main_loop_ref(gui_thread->main_loop);

  main_loop = gui_thread->main_loop;
  main_context = g_main_context_default();

  g_cond_init(&cond);
  g_mutex_init(&mutex);

  /*  */
  i_stop = (guint) floor(1.0 / gui_thread->frequency * ((double) AGS_DEVOUT_DEFAULT_SAMPLERATE / (double) AGS_DEVOUT_DEFAULT_BUFFER_SIZE));

  if(i_stop >= 1){
    struct timespec start, stop, current, reserved;

    /* calculate timing */
    clock_gettime(CLOCK_MONOTONIC ,&start);

    reserved.tv_sec = 0;
    reserved.tv_nsec = NSEC_PER_SEC / gui_thread->frequency;

    stop.tv_sec = start.tv_sec;
    stop.tv_nsec = start.tv_nsec + ((i_stop - 1) * NSEC_PER_SEC / gui_thread->frequency);

    while(current.tv_nsec >= NSEC_PER_SEC) {
      stop.tv_nsec -= NSEC_PER_SEC;
      stop.tv_sec++;
    }

    /*  */
    for(i = 0; i < i_stop; i++){
      if(!g_main_context_acquire(main_context)){
	gboolean got_ownership = FALSE;

	while(!got_ownership){
	  got_ownership = g_main_context_wait(main_context,
					      &cond,
					      &mutex);
	}
      }

      if(!pthread_mutex_trylock(&(task_thread->launch_mutex))){
	g_main_context_iteration(main_context, FALSE);

	pthread_mutex_unlock(&(task_thread->launch_mutex));
      }

      g_main_context_release(main_context);

      /* do timing */
      clock_gettime(CLOCK_MONOTONIC ,&current);

      if(current.tv_sec > stop.tv_sec ||
	 current.tv_nsec > stop.tv_nsec && current.tv_sec == stop.tv_sec){
	break;
      }
    }
  }else{
    struct timespec wait;
    guint iter_val;

    wait.tv_sec = 0;
    wait.tv_nsec = round(1000000000 * ((double) AGS_DEVOUT_DEFAULT_BUFFER_SIZE  / (double) AGS_DEVOUT_DEFAULT_SAMPLERATE));

    iter_val = 1.0 / gui_thread->frequency * ((double) AGS_DEVOUT_DEFAULT_SAMPLERATE / (double) AGS_DEVOUT_DEFAULT_BUFFER_SIZE);

    /*  */
    if(gui_thread->iter > 1.0){
      if(!g_main_context_acquire(main_context)){
	gboolean got_ownership = FALSE;

	while(!got_ownership){
	  got_ownership = g_main_context_wait(main_context,
					      &cond,
					      &mutex);
	}
      }

      pthread_mutex_lock(&(task_thread->launch_mutex));

      g_main_context_iteration(main_context, FALSE);

      pthread_mutex_unlock(&(task_thread->launch_mutex));

      g_main_context_release(main_context);

      gui_thread->iter = 0.0;
    }else{
      gui_thread->iter += iter_val;
      nanosleep(&wait, NULL);
    }
  }

  g_main_loop_unref(gui_thread->main_loop);
}

void
ags_gui_thread_stop(AgsThread *thread)
{
  /*  */
  AGS_THREAD_CLASS(ags_gui_thread_parent_class)->stop(thread);  

  /*  */
  //  GDK_THREADS_ENTER();
  //  gdk_flush();
}

AgsGuiThread*
ags_gui_thread_new()
{
  AgsGuiThread *gui_thread;
  
  gui_thread = (AgsGuiThread *) g_object_new(AGS_TYPE_GUI_THREAD,
					     NULL);

  return(gui_thread);
}
