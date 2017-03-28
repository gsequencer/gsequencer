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

#include <ags/X/thread/ags_gui_thread.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_application_context.h>
#include <ags/object/ags_soundcard.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_polling_thread.h>
#include <ags/thread/ags_poll_fd.h>
#include <ags/thread/ags_task_completion.h>

#include <gdk/gdk.h>

#include <fontconfig/fontconfig.h>
#include <math.h>

void ags_gui_thread_signal_handler(int signr);

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

void* ags_gui_thread_do_poll_loop(void *ptr);

guint ags_gui_thread_interrupted(AgsThread *thread,
				 int sig,
				 guint time_cycle, guint *time_spent);

void ags_gui_thread_dispatch_callback(AgsPollFd *poll,
				      AgsGuiThread *gui_thread);
void ags_gui_thread_polling_thread_run_callback(AgsThread *thread,
						AgsGuiThread *gui_thread);

/**
 * SECTION:ags_gui_thread
 * @short_description: gui thread
 * @title: AgsGuiThread
 * @section_id:
 * @include: ags/X/thread/ags_gui_thread.h
 *
 * The #AgsGuiThread acts as graphical user interface thread.
 */

static gpointer ags_gui_thread_parent_class = NULL;
static AgsConnectableInterface *ags_gui_thread_parent_connectable_interface;
gboolean ags_gui_ready = FALSE;

__thread struct sigaction ags_gui_thread_sigact;

void
ags_gui_thread_signal_handler(int signr)
{
  if(signr == SIGINT){
    //TODO:JK: do backup
    
    exit(-1);
  }else{
    sigemptyset(&(ags_gui_thread_sigact.sa_mask));
  }
}

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

  //  thread->interrupted = ags_gui_thread_interrupted;
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

  pthread_mutexattr_t attr;

  thread = AGS_THREAD(gui_thread);

  g_atomic_int_or(&(thread->sync_flags),
		  (AGS_THREAD_RESUME_INTERRUPTED | AGS_THREAD_TIMELOCK_RUN)); 
  
  thread->freq = AGS_GUI_THREAD_DEFAULT_JIFFIE;

  /* main context */
  g_atomic_int_set(&(gui_thread->dispatching),
		   FALSE);

  g_cond_init(&(gui_thread->cond));
  g_mutex_init(&(gui_thread->mutex));

  gui_thread->main_context = g_main_context_default();

  gui_thread->cached_poll_array_size = 0;
  gui_thread->cached_poll_array = NULL;

  gui_thread->poll_fd = NULL;

  gui_thread->max_priority = 0;
  
  /* task completion */
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr,
			    PTHREAD_MUTEX_RECURSIVE);

  gui_thread->task_completion_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(gui_thread->task_completion_mutex,
		     &attr);
    
  g_atomic_pointer_set(&(gui_thread->task_completion),
		       NULL);
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
    //    AGS_THREAD_CLASS(ags_gui_thread_parent_class)->start(thread);
  }

  g_atomic_int_or(&(gui_thread->flags),
		  AGS_GUI_THREAD_RUNNING);

  pthread_create(thread->thread, thread->thread_attr,
		 ags_gui_thread_do_poll_loop, thread);
}

void
ags_gui_thread_stop(AgsThread *thread)
{
  AgsGuiThread *gui_thread;

  gui_thread = AGS_GUI_THREAD(thread);

  g_atomic_int_and(&(gui_thread->flags),
		   (~(AGS_GUI_THREAD_RUNNING)));
  
  /*  */
  gdk_flush();
}

void*
ags_gui_thread_do_poll_loop(void *ptr)
{
  AgsGuiThread *gui_thread;
  
  gui_thread = (AgsGuiThread *) ptr;
  
  /* wait for audio loop */
  while(!ags_gui_ready){
    usleep(500000);
  }
  
  /* poll */
  while((AGS_GUI_THREAD_RUNNING & (g_atomic_int_get(&(gui_thread->flags)))) != 0){
    AGS_THREAD_GET_CLASS(gui_thread)->run(gui_thread);
  }
  
  pthread_exit(NULL);
}

void
ags_gui_thread_run(AgsThread *thread)
{
  AgsGuiThread *gui_thread;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsPollingThread *polling_thread;
  AgsPollFd *poll_fd;

  GMainContext *main_context;

  GPollFD *fds = NULL;  

  GList *list, *list_start;
  
  gint nfds, allocated_nfds;
  gint timeout;

  gboolean some_ready;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  auto void ags_gui_thread_complete_task();
  
  void ags_gui_thread_complete_task()
  {
    GList *list, *list_next, *list_start;

    pthread_mutex_lock(gui_thread->task_completion_mutex);
    
    list_start = 
      list = g_atomic_pointer_get(&(gui_thread->task_completion));
    g_atomic_pointer_set(&(gui_thread->task_completion),
			 NULL);
    
    pthread_mutex_unlock(gui_thread->task_completion_mutex);
    
    while(list != NULL){
      list_next = list->next;
      
      if((AGS_TASK_COMPLETION_READY & (g_atomic_int_get(&(AGS_TASK_COMPLETION(list->data)->flags)))) != 0){
	ags_task_completion_complete(AGS_TASK_COMPLETION(list->data));
      }

      list = list_next;
    }

    g_list_free(list_start);
  }

  /*  */
  gui_thread = AGS_GUI_THREAD(thread);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
    
  /* real-time setup */  
  main_loop = ags_thread_get_toplevel(thread);
  polling_thread = (AgsPollingThread *) ags_thread_find_type(main_loop,
							     AGS_TYPE_POLLING_THREAD);
  
  main_context = gui_thread->main_context;

  if((AGS_THREAD_RT_SETUP & (g_atomic_int_get(&(thread->flags)))) == 0){
    struct sched_param param;

    sigset_t sigmask;
    
    /* Declare ourself as a real time task */
    param.sched_priority = AGS_GUI_THREAD_RT_PRIORITY;
      
    if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
      perror("sched_setscheduler failed\0");
    }

    g_atomic_int_or(&(thread->flags),
		    AGS_THREAD_RT_SETUP);

    ags_gui_thread_sigact.sa_handler = ags_gui_thread_signal_handler;

    sigemptyset(&ags_gui_thread_sigact.sa_mask);
    ags_gui_thread_sigact.sa_flags = 0;

    sigaction(SIGIO, &ags_gui_thread_sigact, (struct sigaction *) NULL);
    
    /* push default thread */
    if(!g_main_context_acquire(main_context)){
      gboolean got_ownership = FALSE;

      g_mutex_lock(&(gui_thread->mutex));
    
      while(!got_ownership){
	got_ownership = g_main_context_wait(main_context,
					    &(gui_thread->cond),
					    &(gui_thread->mutex));
      }

      g_mutex_unlock(&(gui_thread->mutex));
    }
    
    g_main_context_push_thread_default(main_context);

    g_signal_connect(polling_thread, "run\0",
		     G_CALLBACK(ags_gui_thread_polling_thread_run_callback), gui_thread);


    g_main_context_release(main_context);
  }
  
  if((AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(thread->flags)))) != 0){
    return;
  }

  /* acquire main context */
  if(!g_main_context_acquire(main_context)){
    gboolean got_ownership = FALSE;

    g_mutex_lock(&(gui_thread->mutex));
    
    while(!got_ownership){
      got_ownership = g_main_context_wait(main_context,
					  &(gui_thread->cond),
					  &(gui_thread->mutex));
    }

    g_mutex_unlock(&(gui_thread->mutex));
  }
  
  /*  */
  allocated_nfds = gui_thread->cached_poll_array_size;
  fds = gui_thread->cached_poll_array;

  /* query new */
  g_main_context_prepare(main_context, &gui_thread->max_priority);

  timeout = 4;
  
  while((nfds = g_main_context_query(main_context, gui_thread->max_priority, &timeout, fds,
				     allocated_nfds)) > allocated_nfds){
    g_free (fds);
    gui_thread->cached_poll_array_size = allocated_nfds = nfds;
    gui_thread->cached_poll_array = fds = g_new(GPollFD, nfds);
  }

  /* dispatch */  
  some_ready = g_main_context_check(main_context, gui_thread->max_priority, gui_thread->cached_poll_array, gui_thread->cached_poll_array_size);

  g_main_context_dispatch(main_context);
  
  if(g_atomic_int_get(&(gui_thread->dispatching)) == TRUE){
    g_atomic_int_set(&(gui_thread->dispatching),
		     FALSE);
  }
    
  ags_gui_thread_complete_task();  
  
  g_main_context_release(main_context);

  gdk_threads_enter();

  //  pango_fc_font_map_cache_clear(pango_cairo_font_map_get_default());
  //  pango_cairo_font_map_set_default(NULL);

  //  cairo_debug_reset_static_data();
  //  FcFini();
  
  gdk_threads_leave();
}

guint
ags_gui_thread_interrupted(AgsThread *thread,
			   int sig,
			   guint time_cycle, guint *time_spent)
{
  AgsGuiThread *gui_thread;

  gui_thread = (AgsGuiThread *) thread;
  
  if((AGS_THREAD_INTERRUPTED & (g_atomic_int_get(&(thread->sync_flags)))) == 0){
    g_atomic_int_or(&(thread->sync_flags),
    		    AGS_THREAD_INTERRUPTED);
    
    if(g_atomic_int_get(&(gui_thread->dispatching))){      
      pthread_kill(*(thread->thread),
		   SIGIO);

#ifdef AGS_PTHREAD_SUSPEND
      pthread_suspend(thread->thread);
#else
      pthread_kill(*(thread->thread), AGS_THREAD_SUSPEND_SIG);
#endif
    }
  }

  return(0);
}

void
ags_gui_thread_suspend(AgsThread *thread)
{
  gboolean success;
  gboolean critical_region;

  success = pthread_mutex_trylock(thread->suspend_mutex);
  critical_region = g_atomic_int_get(&(thread->critical_region));

  if(success || critical_region){
    if(success){
      pthread_mutex_unlock(thread->suspend_mutex);
    }
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
    if(success){
      pthread_mutex_unlock(thread->suspend_mutex);
    }
  }
}

void
ags_gui_thread_dispatch_callback(AgsPollFd *poll_fd,
				 AgsGuiThread *gui_thread)
{
  AgsThread *thread;

  thread = (AgsThread *) gui_thread;
  g_atomic_int_set(&(gui_thread->dispatching),
		   TRUE);

  if((AGS_THREAD_INTERRUPTED & (g_atomic_int_get(&(thread->sync_flags)))) != 0){
    g_atomic_int_and(&(thread->sync_flags),
		     (~AGS_THREAD_INTERRUPTED));

#ifdef AGS_PTHREAD_RESUME
    //    pthread_resume(thread->thread);
#else
    //    pthread_kill(*(thread->thread), AGS_THREAD_RESUME_SIG);
#endif
  }
}

void
ags_gui_thread_polling_thread_run_callback(AgsThread *thread,
					   AgsGuiThread *gui_thread)
{

  AgsPollingThread *polling_thread;

  AgsPollFd *poll_fd;
  
  GPollFD *fds = NULL;  

  GList *list, *list_next;

  gint position;
  guint i;

  polling_thread = (AgsPollingThread *) thread;
  
  fds = gui_thread->cached_poll_array;
  
  /* add new poll fd */
  for(i = 0; i < gui_thread->cached_poll_array_size; i++){
    pthread_mutex_lock(polling_thread->fd_mutex);

    position = ags_polling_thread_fd_position(polling_thread,
					      fds[i].fd);

    pthread_mutex_unlock(polling_thread->fd_mutex);
    
    if(position < 0){
      poll_fd = ags_poll_fd_new();
      
      poll_fd->fd = fds[i].fd;
      poll_fd->poll_fd = &(fds[i]);

      poll_fd->delay = 5.0;
      
      //      g_signal_connect(poll_fd, "dispatch\0",
      //	       G_CALLBACK(ags_gui_thread_dispatch_callback), gui_thread);
      
      ags_polling_thread_add_poll_fd(polling_thread,
				     (GObject *) poll_fd);
      
      /* add poll fd to gui thread */
      gui_thread->poll_fd = g_list_prepend(gui_thread->poll_fd,
					   poll_fd);
    }
  }

  /* remove old poll fd */
  list = gui_thread->poll_fd;
  
  while(list != NULL){
    gboolean found;

    list_next = list->next;
    found = FALSE;
    
    for(i = 0; i < gui_thread->cached_poll_array_size; i++){
      if(AGS_POLL_FD(list->data)->fd == fds[i].fd){
	found = TRUE;

	break;
      }
    }

    if(!found){
      ags_polling_thread_remove_poll_fd(polling_thread,
					list->data);
      gui_thread->poll_fd = g_list_remove(gui_thread->poll_fd,
					  list->data);
      g_object_unref(list->data);
    }
    
    list = list_next;
  }
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
