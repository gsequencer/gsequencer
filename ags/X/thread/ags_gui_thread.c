/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/lib/ags_log.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_main_loop.h>
#include <ags/object/ags_soundcard.h>

#include <ags/thread/ags_concurrency_provider.h>
#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif
#include <ags/thread/ags_single_thread.h>
#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_polling_thread.h>
#include <ags/thread/ags_poll_fd.h>
#include <ags/thread/ags_autosave_thread.h>
#include <ags/thread/ags_task_thread.h>
#include <ags/thread/ags_task_completion.h>
#include <ags/thread/ags_thread_pool.h>

#include <ags/server/ags_server.h>

#include <ags/plugin/ags_ladspa_manager.h>
#include <ags/plugin/ags_dssi_manager.h>
#include <ags/plugin/ags_lv2_manager.h>
#include <ags/plugin/ags_lv2_worker_manager.h>
#include <ags/plugin/ags_lv2ui_manager.h>

#include <ags/audio/ags_sound_provider.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_soundcard_thread.h>
#include <ags/audio/thread/ags_export_thread.h>

#include <ags/X/ags_xorg_application_context.h>
#include <ags/X/ags_window.h>

#include <ags/X/file/ags_simple_file.h>

#include <ags/X/thread/ags_gui_thread.h>
#include <ags/X/thread/ags_simple_autosave_thread.h>

#include <ags/X/task/ags_simple_file_read.h>

#include <ags/config.h>

#ifdef AGS_WITH_LIBINSTPATCH
#include <libinstpatch/libinstpatch.h>
#endif

#include <libxml/parser.h>
#include <libxml/xlink.h>
#include <libxml/xpath.h>
#include <libxml/valid.h>
#include <libxml/xmlIO.h>
#include <libxml/xmlmemory.h>
#include <libxml/xmlsave.h>

#include <pthread.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>

#include <stdlib.h>

#include <locale.h>

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
void ags_gui_thread_suspend(AgsThread *thread);
void ags_gui_thread_resume(AgsThread *thread);
void ags_gui_thread_stop(AgsThread *thread);

void ags_gui_thread_dispatch_callback(AgsPollFd *poll,
				      AgsGuiThread *gui_thread);
void ags_gui_thread_polling_thread_run_callback(AgsThread *thread,
						AgsGuiThread *gui_thread);

static void ags_gui_thread_sigact_create();
struct sigaction* ags_gui_thread_get_sigact();


guint ags_gui_thread_interrupted(AgsThread *thread,
				 int sig,
				 guint time_cycle, guint *time_spent);
static void ags_gui_thread_dispatch_mutex_create();

gboolean ags_gui_thread_animation_prepare(GSource *source,
					  gint *timeout_);
gboolean ags_gui_thread_animation_check(GSource *source);
gboolean ags_gui_thread_animation_dispatch(GSource *source,
					   GSourceFunc callback,
					   gpointer user_data);

gboolean ags_gui_thread_sync_task_prepare(GSource *source,
					  gint *timeout_);
gboolean ags_gui_thread_sync_task_check(GSource *source);
gboolean ags_gui_thread_sync_task_dispatch(GSource *source,
					   GSourceFunc callback,
					   gpointer user_data);

gboolean ags_gui_thread_task_prepare(GSource *source,
				     gint *timeout_);
gboolean ags_gui_thread_task_check(GSource *source);
gboolean ags_gui_thread_task_dispatch(GSource *source,
				      GSourceFunc callback,
				      gpointer user_data);

gboolean ags_gui_thread_do_animation_callback(GtkWidget *widget, GdkEventExpose *event,
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

static pthread_mutex_t* gui_dispatch_mutex = NULL;

/* Key for the thread-specific AgsGuiThread */
static pthread_key_t ags_gui_thread_sigact_key;

/* Once-only initialisation of the key */
static pthread_once_t ags_gui_thread_sigact_key_once = PTHREAD_ONCE_INIT;

//TODO:JK: implement get functions
#ifndef AGS_USE_TIMER
extern sigset_t ags_wait_mask;
extern struct sigaction ags_sigact;
#else
extern sigset_t ags_timer_mask;

extern struct sigaction ags_sigact_timer;

extern struct sigevent ags_sev_timer;
extern struct itimerspec its;
#endif

void
ags_gui_thread_signal_handler(int signr)
{
  if(signr == SIGINT){
    //TODO:JK: do backup
    
    exit(-1);
  }else{
    sigemptyset(&(ags_gui_thread_get_sigact()->sa_mask));
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
						    "AgsGuiThread",
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

  pthread_mutexattr_t *attr;

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
  attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

  gui_thread->task_completion_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(gui_thread->task_completion_mutex,
		     attr);

  g_atomic_pointer_set(&(gui_thread->task_completion),
		       NULL);

  /*  */
  gui_thread->dispatch_mutex = ags_gui_thread_get_dispatch_mutex();
  
  /* task */
  gui_thread->task_schedule_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(gui_thread->task_schedule_mutex,
		     NULL);

  gui_thread->nth_message = 0;
  gui_thread->animation_source = NULL;

  gui_thread->queued_sync = 0;
  gui_thread->sync_source = NULL;

  gui_thread->collected_task = NULL;
  gui_thread->task_source = NULL;
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
ags_gui_thread_complete_task(AgsGuiThread *gui_thread)
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

void*
ags_gui_thread_do_poll_loop(void *ptr)
{
  AgsGuiThread *gui_thread;
  AgsTaskThread *task_thread;
  AgsPollingThread *polling_thread;
  AgsThread *thread;
  
  AgsXorgApplicationContext *xorg_application_context;

  GMainContext *main_context;
  GSourceFuncs task_funcs;
  GSourceFuncs sync_funcs;
  GSourceFuncs animation_funcs;

  struct timespec idle = {
    0,
    4000000,
  };
  
  guint i;
  
  gui_thread = (AgsGuiThread *) ptr;
  thread = (AgsThread *) ptr;
  
  xorg_application_context = ags_application_context_get_instance();

  main_context = gui_thread->main_context;

  /* notify start */
  pthread_mutex_lock(thread->start_mutex);
      
  g_atomic_int_set(&(thread->start_done),
		   TRUE);    
      
  if(g_atomic_int_get(&(thread->start_wait)) == TRUE){
    pthread_cond_broadcast(thread->start_cond);
  }
      
  pthread_mutex_unlock(thread->start_mutex);

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

  g_main_context_release(main_context);

  /* animation functions */
  animation_funcs.prepare = ags_gui_thread_animation_prepare;
  animation_funcs.check = ags_gui_thread_animation_check;
  animation_funcs.dispatch = ags_gui_thread_animation_dispatch;

  gui_thread->animation_source = g_source_new(&animation_funcs,
					      sizeof(GSource));
  g_source_attach(gui_thread->animation_source,
		  main_context);

  /* sync functions */
#if 0
  sync_funcs.prepare = ags_gui_thread_sync_task_prepare;
  sync_funcs.check = ags_gui_thread_sync_task_check;
  sync_funcs.dispatch = ags_gui_thread_sync_task_dispatch;

  gui_thread->sync_source = g_source_new(&sync_funcs,
					 sizeof(GSource));
  g_source_attach(gui_thread->sync_source,
		  main_context);
#endif
  
  /* task functions */
  task_funcs.prepare = ags_gui_thread_task_prepare;
  task_funcs.check = ags_gui_thread_task_check;
  task_funcs.dispatch = ags_gui_thread_task_dispatch;

  gui_thread->task_source = g_source_new(&task_funcs,
					 sizeof(GSource));
  g_source_attach(gui_thread->task_source,
		  main_context);

  /* wait for audio loop */
  while(g_atomic_int_get(&(xorg_application_context->gui_ready)) == 0){
    usleep(500000);
  }

  task_thread = ags_thread_find_type(AGS_APPLICATION_CONTEXT(xorg_application_context)->main_loop,
				     AGS_TYPE_TASK_THREAD);

  polling_thread = (AgsPollingThread *) ags_thread_find_type(AGS_APPLICATION_CONTEXT(xorg_application_context)->main_loop,
							     AGS_TYPE_POLLING_THREAD);
  
  /* poll */
  g_signal_connect(polling_thread, "run",
		   G_CALLBACK(ags_gui_thread_polling_thread_run_callback), gui_thread);
  
  while((AGS_GUI_THREAD_RUNNING & (g_atomic_int_get(&(gui_thread->flags)))) != 0){
    nanosleep(&idle,
	      NULL);
    
    pthread_mutex_lock(task_thread->launch_mutex);
    
    AGS_THREAD_GET_CLASS(gui_thread)->run(gui_thread);

    pthread_mutex_unlock(task_thread->launch_mutex);
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
#ifdef AGS_WITH_RT
    struct sched_param param;

    sigset_t sigmask;
    
    /* Declare ourself as a real time task */
    param.sched_priority = AGS_GUI_THREAD_RT_PRIORITY;
      
    if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
      perror("sched_setscheduler failed");
    }
#endif
    
    g_atomic_int_or(&(thread->flags),
		    AGS_THREAD_RT_SETUP);

    ags_gui_thread_get_sigact()->sa_handler = ags_gui_thread_signal_handler;

    sigemptyset(&ags_gui_thread_get_sigact()->sa_mask);
    ags_gui_thread_get_sigact()->sa_flags = 0;

    sigaction(SIGIO, ags_gui_thread_get_sigact(), (struct sigaction *) NULL);    
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

  gdk_threads_enter();
  
  g_main_context_dispatch(main_context);

  gdk_threads_leave();
  
  if(g_atomic_int_get(&(gui_thread->dispatching)) == TRUE){
    g_atomic_int_set(&(gui_thread->dispatching),
		     FALSE);
  }
    
  ags_gui_thread_complete_task(gui_thread);  
  
  g_main_context_release(main_context);

  gdk_threads_enter();

  //  pango_fc_font_map_cache_clear(pango_cairo_font_map_get_default());
  //  pango_cairo_font_map_set_default(NULL);

  //  cairo_debug_reset_static_data();
  //  FcFini();
  
  gdk_threads_leave();
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
ags_gui_thread_stop(AgsThread *thread)
{
  AgsGuiThread *gui_thread;

  gui_thread = AGS_GUI_THREAD(thread);

  g_atomic_int_and(&(gui_thread->flags),
		   (~(AGS_GUI_THREAD_RUNNING)));
  
  /*  */
  gdk_flush();
}

static void
ags_gui_thread_sigact_create()
{
  pthread_key_create(&ags_gui_thread_sigact_key, NULL);
  pthread_setspecific(ags_gui_thread_sigact_key, (struct sigaction *) malloc(sizeof(struct sigaction)));
}

struct sigaction*
ags_gui_thread_get_sigact()
{
  return((AgsThread *) pthread_getspecific(ags_gui_thread_sigact_key));
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

#if 0
#ifdef AGS_PTHREAD_RESUME
    pthread_resume(thread->thread);
#else
    pthread_kill(*(thread->thread), AGS_THREAD_RESUME_SIG);
#endif
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

  if((AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(AGS_THREAD(gui_thread)->flags)))) != 0){
    return;
  }

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
      
      //      g_signal_connect(poll_fd, "dispatch",
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

void
ags_gui_init(int *argc, char ***argv)
{
  ags_gui_thread_get_dispatch_mutex();

  gdk_threads_set_lock_functions(ags_gui_thread_enter,
				 ags_gui_thread_leave);
}

void
ags_gui_thread_enter()
{
  pthread_mutex_lock(ags_gui_thread_get_dispatch_mutex());
}

void
ags_gui_thread_leave()
{
  pthread_mutex_unlock(ags_gui_thread_get_dispatch_mutex());
}

static void
ags_gui_thread_dispatch_mutex_create()
{
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);
  
  mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  gui_dispatch_mutex = mutex;
}

void
ags_gui_thread_show_file_error(AgsGuiThread *gui_thread,
			       gchar *filename,
			       GError *error)
{
  GtkDialog *dialog;
      
  g_warning("could not parse file %s", filename);
      
  dialog = gtk_message_dialog_new(NULL,
				  0,
				  GTK_MESSAGE_WARNING,
				  GTK_BUTTONS_OK,
				  "Failed to open '%s'",
				  filename);
  gtk_widget_show_all((GtkWidget *) dialog);
  g_signal_connect(dialog, "response",
		   G_CALLBACK(gtk_main_quit), NULL);
  gtk_main();
}

void
ags_gui_thread_launch(AgsGuiThread *gui_thread,
		      gboolean single_thread)
{
  AgsApplicationContext *application_context;
  
  AgsMutexManager *mutex_manager;
  AgsThread *audio_loop, *task_thread;

  AgsConfig *config;

  GList *start_queue;  

  pthread_mutex_t *application_mutex;

  application_context = ags_application_context_get_instance();

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
    
  /* get threads, thread pool and config */
  pthread_mutex_lock(application_mutex);
  
  audio_loop = (AgsThread *) application_context->main_loop;

  config = application_context->config;
  
  pthread_mutex_unlock(application_mutex);

  task_thread = ags_thread_find_type(audio_loop,
				     AGS_TYPE_TASK_THREAD);

  start_queue = NULL;

  /* autosave thread */
  if(!g_strcmp0(ags_config_get_value(config,
				     AGS_CONFIG_GENERIC,
				     "autosave-thread"),
		"true")){
    pthread_mutex_lock(audio_loop->start_mutex);
    
    start_queue = g_atomic_pointer_get(&(audio_loop->start_queue));
    start_queue = g_list_prepend(start_queue,
				 task_thread);
    
    g_atomic_pointer_set(&(audio_loop->start_queue),
			 start_queue);
    
    pthread_mutex_unlock(audio_loop->start_mutex);
  }
}

void
ags_gui_thread_launch_filename(AgsGuiThread *gui_thread,
			       gchar *filename,
			       gboolean single_thread)
{
  AgsApplicationContext *application_context;

  AgsMutexManager *mutex_manager;
  AgsThread *audio_loop, *task_thread;

  AgsConfig *config;

  GList *start_queue;  

  pthread_mutex_t *application_mutex;

  application_context = ags_application_context_get_instance();

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
    
  /* get threads, thread pool and config */
  pthread_mutex_lock(application_mutex);

  audio_loop = (AgsThread *) application_context->main_loop;
  task_thread = (AgsThread *) application_context->task_thread;

  config = application_context->config;
  
  pthread_mutex_unlock(application_mutex);

  /* open file */
  if(g_strcmp0(ags_config_get_value(config,
				    AGS_CONFIG_GENERIC,
				    "simple-file"),
	       "false")){
    AgsSimpleFile *simple_file;
    
    AgsSimpleFileRead *simple_file_read;

    AgsGuiThread *gui_thread;
    
    GError *error;

    gui_thread = ags_thread_find_type(audio_loop,
				      AGS_TYPE_GUI_THREAD);

    simple_file = (AgsSimpleFile *) g_object_new(AGS_TYPE_SIMPLE_FILE,
						 "application-context", application_context,
						 "filename", filename,
						 NULL);
    error = NULL;
    ags_simple_file_open(simple_file,
			 &error);

    if(error != NULL){
      ags_gui_thread_show_file_error(gui_thread,
				     filename,
				     error);
      ags_application_context_quit(application_context);
    }
          
    /* autosave thread */
    if(!g_strcmp0(ags_config_get_value(config,
				       AGS_CONFIG_GENERIC,
				       "autosave-thread"),
		  "true")){
      pthread_mutex_lock(audio_loop->start_mutex);

      start_queue = g_atomic_pointer_get(&(audio_loop->start_queue));
      start_queue = g_list_prepend(start_queue,
				   task_thread);

      g_atomic_pointer_set(&(audio_loop->start_queue),
			   start_queue);
	
      pthread_mutex_unlock(audio_loop->start_mutex);
    }
    
    /* now start read task */
    simple_file_read = ags_simple_file_read_new(simple_file);
    ags_gui_thread_schedule_task(gui_thread,
				 simple_file_read);
  }
}

void
ags_gui_thread_timer_start(AgsGuiThread *gui_thread,
			   void *timer_id)
{
#ifdef AGS_USE_TIMER
  struct itimerspec value;

  /* Start the timer */
  its.it_value.tv_sec = 0;
  its.it_value.tv_nsec = NSEC_PER_SEC / AGS_THREAD(gui_thread)->max_precision;
  its.it_interval.tv_sec = its.it_value.tv_sec;
  its.it_interval.tv_nsec = its.it_value.tv_nsec;

  if(timer_settime(timer_id, 0, &its, NULL) == -1){
    perror("timer_settime");
    exit(EXIT_FAILURE);
    
  }

  if(sigprocmask(SIG_UNBLOCK, &ags_timer_mask, NULL) == -1){
    perror("sigprocmask");
    exit(EXIT_FAILURE);
  }
#endif
}

void
ags_gui_thread_timer_launch(AgsGuiThread *gui_thread,
			    void *timer_id,
			    gboolean single_thread)
{
  AgsApplicationContext *application_context;
  
  AgsMutexManager *mutex_manager;
  AgsThread *audio_loop, *task_thread;

  AgsConfig *config;

  GList *start_queue;  

  pthread_mutex_t *application_mutex;

  application_context = ags_application_context_get_instance();

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get threads, thread pool and config */
  pthread_mutex_lock(application_mutex);
  
  audio_loop = (AgsThread *) application_context->main_loop;

  config = application_context->config;
  
  pthread_mutex_unlock(application_mutex);

  task_thread = ags_thread_find_type(audio_loop,
				     AGS_TYPE_TASK_THREAD);
  
  /* autosave thread */
  if(!g_strcmp0(ags_config_get_value(config,
				     AGS_CONFIG_GENERIC,
				     "autosave-thread"),
		"true")){
    pthread_mutex_lock(audio_loop->start_mutex);

    start_queue = g_atomic_pointer_get(&(audio_loop->start_queue));
    start_queue = g_list_prepend(start_queue,
				 task_thread);

    g_atomic_pointer_set(&(audio_loop->start_queue),
			 start_queue);
	
    pthread_mutex_unlock(audio_loop->start_mutex);
  }
}

void
ags_gui_thread_timer_launch_filename(AgsGuiThread *gui_thread,
				     void *timer_id, gchar *filename,
				     gboolean single_thread)
{
  AgsApplicationContext *application_context;
  
  AgsMutexManager *mutex_manager;

  AgsThread *audio_loop, *task_thread;

  AgsConfig *config;
      
  GList *start_queue;

  pthread_mutex_t *application_mutex;

  application_context = ags_application_context_get_instance();
  
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get thread, thread pool and config */
  pthread_mutex_lock(application_mutex);

  audio_loop = (AgsThread *) application_context->main_loop;
  task_thread = (AgsThread *) application_context->task_thread;

  config = application_context->config;
  
  pthread_mutex_unlock(application_mutex);

  /* open file */
  if(g_strcmp0(ags_config_get_value(config,
				    AGS_CONFIG_GENERIC,
				    "simple-file"),
	       "false")){
    AgsSimpleFile *simple_file;

    AgsSimpleFileRead *simple_file_read;

    GError *error;    

    simple_file = (AgsSimpleFile *) g_object_new(AGS_TYPE_SIMPLE_FILE,
						 "application-context", application_context,
						 "filename", filename,
						 NULL);
    error = NULL;
    ags_simple_file_open(simple_file,
			 &error);

    if(error != NULL){
      ags_gui_thread_show_file_error(gui_thread,
				     filename,
				     error);
      exit(-1);
    }

    /* Start the timer */
    ags_gui_thread_timer_start(gui_thread,
			       timer_id);
    
    /* autosave thread */
    if(!g_strcmp0(ags_config_get_value(config,
				       AGS_CONFIG_GENERIC,
				       "autosave-thread"),
		  "true")){
      pthread_mutex_lock(audio_loop->start_mutex);

      start_queue = g_atomic_pointer_get(&(audio_loop->start_queue));
      start_queue = g_list_prepend(start_queue,
				   task_thread);

      g_atomic_pointer_set(&(audio_loop->start_queue),
			   start_queue);
	
      pthread_mutex_unlock(audio_loop->start_mutex);
    }
    
    /* now start read task */
    simple_file_read = ags_simple_file_read_new(simple_file);
    ags_task_thread_append_task((AgsTaskThread *) task_thread,
				(AgsTask *) simple_file_read);
  }
}


pthread_mutex_t*
ags_gui_thread_get_dispatch_mutex()
{
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&mutex);
  
  if(gui_dispatch_mutex == NULL){
    ags_gui_thread_dispatch_mutex_create();
  }

  pthread_mutex_unlock(&mutex);
  
  return(gui_dispatch_mutex);
}

gboolean
ags_gui_thread_animation_prepare(GSource *source,
				 gint *timeout_)
{
  AgsXorgApplicationContext *xorg_application_context;

  AgsGuiThread *gui_thread;
  
  AgsThread *main_loop;

  AgsLog *log;

  guint nth;
  
  xorg_application_context = ags_application_context_get_instance();
  main_loop = AGS_APPLICATION_CONTEXT(xorg_application_context)->main_loop;
  
  gui_thread = ags_thread_find_type(main_loop,
				    AGS_TYPE_GUI_THREAD);

  log = ags_log_get_instance();
  
  pthread_mutex_lock(log->mutex);
  
  nth = g_list_length(g_atomic_pointer_get(&(log->messages)));

  pthread_mutex_unlock(log->mutex);

  if(nth > gui_thread->nth_message ||
     !g_atomic_int_get(&(xorg_application_context->show_animation))){
    if(timeout_ != NULL){
      *timeout_ = 0;
    }

    return(TRUE);
  }else{
    if(timeout_ != NULL){
      *timeout_ = 0;
    }

    return(FALSE);
  }
}

gboolean
ags_gui_thread_animation_check(GSource *source)
{
  AgsXorgApplicationContext *xorg_application_context;

  AgsGuiThread *gui_thread;
  
  AgsThread *main_loop;

  AgsLog *log;

  guint nth;

  xorg_application_context = ags_application_context_get_instance();
  main_loop = AGS_APPLICATION_CONTEXT(xorg_application_context)->main_loop;
  
  gui_thread = ags_thread_find_type(main_loop,
				    AGS_TYPE_GUI_THREAD);

  log = ags_log_get_instance();
  
  pthread_mutex_lock(log->mutex);
  
  nth = g_list_length(g_atomic_pointer_get(&(log->messages)));

  pthread_mutex_unlock(log->mutex);

  if(nth > gui_thread->nth_message ||
     !g_atomic_int_get(&(xorg_application_context->show_animation))){
    return(TRUE);
  }else{
    return(FALSE);
  }
}

gboolean
ags_gui_thread_animation_dispatch(GSource *source,
				  GSourceFunc callback,
				  gpointer user_data)
{
  AgsXorgApplicationContext *xorg_application_context;

  static GtkWindow *window = NULL;
  static GtkWidget *widget;

  AgsGuiThread *gui_thread;

  GMainContext *main_context;
  
  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  xorg_application_context = ags_application_context_get_instance();
  main_loop = AGS_APPLICATION_CONTEXT(xorg_application_context)->main_loop;
  
  gui_thread = ags_thread_find_type(main_loop,
				    AGS_TYPE_GUI_THREAD);

  main_context = g_main_context_default();

  if(window == NULL){
    window = g_object_new(GTK_TYPE_WINDOW,
			  "app-paintable", TRUE,
			  "type", GTK_WINDOW_TOPLEVEL,
			  "decorated", FALSE,
			  "window-position", GTK_WIN_POS_CENTER,
			  NULL);
  
    gtk_widget_set_size_request(window,
				800, 450);

    widget = gtk_drawing_area_new();
    gtk_container_add(window,
		      widget);

    gtk_widget_show_all((GtkWidget *) window);

    g_signal_connect(widget, "expose-event",
		     G_CALLBACK(ags_gui_thread_do_animation_callback), gui_thread);
  }

  gtk_widget_queue_draw(widget);

  g_main_context_iteration(main_context,
  			   FALSE);
  
  if(g_atomic_int_get(&(xorg_application_context->show_animation))){
    return(G_SOURCE_CONTINUE);
  }else{
    gtk_widget_destroy(window);
    window = NULL;

    gtk_widget_show_all(xorg_application_context->window);

    return(G_SOURCE_REMOVE);
  }
}

gboolean
ags_gui_thread_sync_task_prepare(GSource *source,
				 gint *timeout_)
{  
  AgsGuiThread *gui_thread;

  AgsApplicationContext *application_context;
  
  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  application_context = ags_application_context_get_instance();
  main_loop = application_context->main_loop;
  
  task_thread = ags_thread_find_type(main_loop,
				     AGS_TYPE_TASK_THREAD);

  gui_thread = ags_thread_find_type(main_loop,
				    AGS_TYPE_GUI_THREAD);
  
  if(gui_thread->queued_sync > 0){
    if(timeout_ != NULL){
      *timeout_ = 0;
    }

    return(TRUE);
  }else{
    if(timeout_ != NULL){
      *timeout_ = 0;
    }
    
    return(FALSE);
  }
}

gboolean
ags_gui_thread_sync_task_check(GSource *source)
{
  AgsGuiThread *gui_thread;

  AgsApplicationContext *application_context;
  
  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  application_context = ags_application_context_get_instance();
  main_loop = application_context->main_loop;
  
  task_thread = ags_thread_find_type(main_loop,
				     AGS_TYPE_TASK_THREAD);

  gui_thread = ags_thread_find_type(main_loop,
				    AGS_TYPE_GUI_THREAD);

  if(gui_thread->queued_sync > 0){    
    return(TRUE);
  }else{
    return(FALSE);
  }
}

gboolean
ags_gui_thread_sync_task_dispatch(GSource *source,
				  GSourceFunc callback,
				  gpointer user_data)
{
  AgsGuiThread *gui_thread;

  AgsApplicationContext *application_context;

  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  GMainContext *main_context;

  gboolean do_sync;
  
  struct timespec timeout = {
    0,
    AGS_GUI_THREAD_SYNC_AVAILABLE_TIMEOUT,
  };
  
  application_context = ags_application_context_get_instance();
  main_loop = application_context->main_loop;
  
  task_thread = ags_thread_find_type(main_loop,
				     AGS_TYPE_TASK_THREAD);

  gui_thread = ags_thread_find_type(main_loop,
				    AGS_TYPE_GUI_THREAD);

  main_context = gui_thread->main_context;

  if(g_atomic_int_get(&(AGS_XORG_APPLICATION_CONTEXT(application_context)->show_animation)) == TRUE){
    return(G_SOURCE_CONTINUE);
  }

  /* ask for sync */
  if(gui_thread->queued_sync > 0){
    while((AGS_TASK_THREAD_EXTERN_AVAILABLE & (g_atomic_int_get(&(task_thread->flags)))) == 0){
      nanosleep(&timeout,
		NULL);
    }
  }
  
  if(g_atomic_pointer_get(&(task_thread->queue)) == NULL){
    gui_thread->queued_sync -= 1;

    if(gui_thread->queued_sync == 0){
      g_atomic_int_and(&(task_thread->flags),
		       (~AGS_TASK_THREAD_EXTERN_AVAILABLE));
    }
  }
  
  /* signal */
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

  ags_gui_thread_enter();

  do_sync = FALSE;
  
  if(gui_thread->queued_sync > 0){
    do_sync = TRUE;
    
    pthread_mutex_lock(task_thread->sync_mutex);
    
    g_atomic_int_and(&(task_thread->flags),
		     (~AGS_TASK_THREAD_SYNC_WAIT));
    
    if((AGS_TASK_THREAD_SYNC_DONE & (g_atomic_int_get(&(task_thread->flags)))) == 0){
      pthread_cond_signal(task_thread->sync_cond);
    }  
    
    pthread_mutex_unlock(task_thread->sync_mutex);
  }else{
    pthread_mutex_lock(task_thread->sync_mutex);

    g_atomic_int_and(&(task_thread->flags),
		     (~(AGS_TASK_THREAD_EXTERN_SYNC |
			AGS_TASK_THREAD_EXTERN_READY)));
    
    g_atomic_int_and(&(task_thread->flags),
		     (~AGS_TASK_THREAD_SYNC_WAIT));
    
    if((AGS_TASK_THREAD_SYNC_DONE & (g_atomic_int_get(&(task_thread->flags)))) == 0){
      do_sync = TRUE;
      
      pthread_cond_signal(task_thread->sync_cond);
    }  
    
    pthread_mutex_unlock(task_thread->sync_mutex);
  }
  
  /* wait */
  if(do_sync){
    pthread_mutex_lock(task_thread->extern_sync_mutex);

    g_atomic_int_and(&(task_thread->flags),
		     (~AGS_TASK_THREAD_EXTERN_SYNC_DONE));
    
    if((AGS_TASK_THREAD_EXTERN_SYNC_WAIT & (g_atomic_int_get(&(task_thread->flags)))) != 0 &&
       (AGS_TASK_THREAD_EXTERN_SYNC_DONE & (g_atomic_int_get(&(task_thread->flags)))) == 0){
      while((AGS_TASK_THREAD_EXTERN_SYNC_WAIT & (g_atomic_int_get(&(task_thread->flags)))) != 0 &&
	    (AGS_TASK_THREAD_EXTERN_SYNC_DONE & (g_atomic_int_get(&(task_thread->flags)))) == 0){
	pthread_cond_wait(task_thread->extern_sync_cond,
			  task_thread->extern_sync_mutex);
      }
    }
    
    g_atomic_int_or(&(task_thread->flags),
		    (AGS_TASK_THREAD_EXTERN_SYNC_WAIT |
		     AGS_TASK_THREAD_EXTERN_SYNC_DONE));
  
    pthread_mutex_unlock(task_thread->extern_sync_mutex);
  }
  
  ags_gui_thread_leave();
  
  g_main_context_release(main_context);
  
  return(G_SOURCE_CONTINUE);
}

gboolean
ags_gui_thread_task_prepare(GSource *source,
			    gint *timeout_)
{
  AgsApplicationContext *application_context;

  AgsGuiThread *gui_thread;
  
  AgsThread *main_loop;

  application_context = ags_application_context_get_instance();
  main_loop = application_context->main_loop;

  gui_thread = ags_thread_find_type(main_loop,
				    AGS_TYPE_GUI_THREAD);

  if(timeout_ != NULL){
    *timeout_ = 0;
  }

  if(gui_thread->collected_task != NULL){
    return(TRUE);
  }else{
    return(FALSE);
  }
}

gboolean
ags_gui_thread_task_check(GSource *source)
{
  AgsApplicationContext *application_context;

  AgsGuiThread *gui_thread;
  
  AgsThread *main_loop;

  application_context = ags_application_context_get_instance();
  main_loop = application_context->main_loop;
  
  gui_thread = ags_thread_find_type(main_loop,
				    AGS_TYPE_GUI_THREAD);
  
  if(gui_thread->collected_task != NULL){
    return(TRUE);
  }else{
    return(FALSE);
  }
}

gboolean
ags_gui_thread_task_dispatch(GSource *source,
			     GSourceFunc callback,
			     gpointer user_data)
{
  AgsApplicationContext *application_context;

  AgsGuiThread *gui_thread;
  
  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  application_context = ags_application_context_get_instance();
  main_loop = application_context->main_loop;
  
  gui_thread = ags_thread_find_type(main_loop,
				    AGS_TYPE_GUI_THREAD);
  task_thread = ags_thread_find_type(main_loop,
				     AGS_TYPE_TASK_THREAD);

  ags_task_thread_append_tasks(task_thread,
			       g_list_reverse(gui_thread->collected_task));

  gui_thread->collected_task = NULL;

  ags_gui_thread_complete_task(gui_thread);
  
  return(G_SOURCE_CONTINUE);
}

gboolean
ags_gui_thread_do_animation_callback(GtkWidget *widget, GdkEventExpose *event,
				     AgsGuiThread *gui_thread)
{
  AgsLog *log;

  GdkRectangle rectangle;

  cairo_t *i_cr, *cr;
  static cairo_surface_t *surface;

  static gchar *filename = NULL;
  
  /* create a buffer suitable to image size */
  GList *list, *start;

  static unsigned char *image_data, *bg_data;
  
  static guint image_size;
  gdouble x0, y0;
  guint i;
  guint nth = 0;

  log = ags_log_get_instance();  
  
  if(filename == NULL){
    /* create gdk cairo graphics context */
#ifdef AGS_ANIMATION_FILENAME
    filename = g_strdup(AGS_ANIMATION_FILENAME);
#else
    if((filename = getenv("AGS_ANIMATION_FILENAME")) == NULL){
      filename = g_strdup_printf("%s%s", DESTDIR, "/gsequencer/images/ags_supermoon-800x450.png");
    }else{
      filename = g_strdup(filename);
    }
#endif

    /* create surface and get image data */
    surface = cairo_image_surface_create_from_png(filename);
    cairo_surface_reference(surface);
    
    image_data = cairo_image_surface_get_data(surface);
    image_size = 3 * 800 * 600;

    bg_data = (unsigned char *) malloc(image_size * sizeof(unsigned char));
  }
  
  cr = gdk_cairo_create(widget->window);
  
  start = 
    list = ags_log_get_messages(log);

  pthread_mutex_lock(log->mutex);
    
  i = g_list_length(start);

  pthread_mutex_unlock(log->mutex);
  
  x0 = 4.0;
  y0 = 4.0 + (i * 12.0);

  nth = gui_thread->nth_message;

  //  ags_gui_thread_enter();
  
  if(nth < i){
    /* create image cairo graphics context */
    i_cr = cairo_create(surface);
    cairo_select_font_face(i_cr, "Georgia",
			   CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(i_cr, (gdouble) 11.0);
    
    /* show ... */
    cairo_move_to(i_cr,
		  x0, 4.0 + (i + 1) * 12.0);
    cairo_show_text(i_cr, "...");

    /* show all text */
    while(i >= 0 && i > nth && list != NULL){
      cairo_set_source_rgb(i_cr,
			   1.0,
			   0.0,
			   1.0);
	
      cairo_move_to(i_cr,
		    x0, y0);

      pthread_mutex_lock(log->mutex);

      cairo_show_text(i_cr, list->data);
      cairo_stroke(i_cr);
    
      list = list->next;

      pthread_mutex_unlock(log->mutex);

      y0 -= 12.0;
      i--;
    }

    nth = g_list_length(start);
    gui_thread->nth_message = nth;
    
    if((image_data = cairo_image_surface_get_data(surface)) != NULL){
      memcpy(bg_data, image_data, image_size * sizeof(unsigned char));
    }
    
    cairo_destroy(i_cr);

    cairo_set_source_surface(cr, surface, 0, 0);

    cairo_paint(cr);
    
    cairo_destroy(cr);
  }else{
    /* create image cairo graphics context */
    i_cr = cairo_create(surface);

    if((image_data = cairo_image_surface_get_data(surface)) != NULL){
      memcpy(image_data, bg_data, image_size * sizeof(unsigned char));
    }
    
    cairo_destroy(i_cr);

    cairo_set_source_surface(cr, surface, 0, 0);

    //    cairo_surface_mark_dirty(surface);
    cairo_paint(cr);    

    cairo_destroy(cr);
  }

  //  ags_gui_thread_leave();

  return(TRUE);
}

void
ags_gui_thread_do_animation(AgsGuiThread *gui_thread)
{
  AgsXorgApplicationContext *xorg_application_context;

  GtkWindow *window;
  GtkWidget *widget;
  
  GMainContext *main_context;

  xorg_application_context = ags_application_context_get_instance();

  main_context = g_main_context_default();

  window = g_object_new(GTK_TYPE_WINDOW,
			"app-paintable", TRUE,
			"type", GTK_WINDOW_TOPLEVEL,
			"decorated", FALSE,
			"window-position", GTK_WIN_POS_CENTER,
			NULL);
  
  gtk_widget_set_size_request(window,
			      800, 450);

  widget = gtk_drawing_area_new();
  gtk_container_add(window,
		    widget);

  gtk_widget_show_all((GtkWidget *) window);

  g_signal_connect(widget, "expose-event",
		   G_CALLBACK(ags_gui_thread_do_animation_callback), gui_thread);
  
  /* iterate log messages */
  while(g_atomic_int_get(&(xorg_application_context->show_animation))){
    gtk_widget_queue_draw(widget);

    gdk_threads_enter();
    
    g_main_context_iteration(main_context,
    			     FALSE);

    gdk_threads_leave();
    //gdk_flush();
        
    usleep(12500);
  }

  gtk_widget_destroy(window);
}

void
ags_gui_thread_do_run(AgsGuiThread *gui_thread)
{
  AgsXorgApplicationContext *xorg_application_context;

  AgsThread *thread;
  AgsTaskThread *task_thread;
  
  GMainContext *main_context;

  GSourceFuncs animation_funcs;
  GSourceFuncs task_funcs;
  GSourceFuncs sync_funcs;

  struct timespec idle = {
    0,
    4000000,
  };

  xorg_application_context = ags_application_context_get_instance();

  thread = (AgsThread *) gui_thread;
  g_atomic_int_or(&(gui_thread->flags),
		  AGS_GUI_THREAD_RUNNING);

  task_thread = (AgsThread *) AGS_APPLICATION_CONTEXT(xorg_application_context)->task_thread;

  main_context = gui_thread->main_context;

  pthread_once(&ags_gui_thread_sigact_key_once, ags_gui_thread_sigact_create);

  ags_gui_thread_get_sigact()->sa_handler = ags_gui_thread_signal_handler;

  sigemptyset(&(ags_gui_thread_get_sigact()->sa_mask));
  ags_gui_thread_get_sigact()->sa_flags = 0;

  sigaction(SIGIO, ags_gui_thread_get_sigact(), (struct sigaction *) NULL);
  
  /* notify start */
  pthread_mutex_lock(thread->start_mutex);
      
  g_atomic_int_set(&(thread->start_done),
		   TRUE);    
      
  if(g_atomic_int_get(&(thread->start_wait)) == TRUE){
    pthread_cond_broadcast(thread->start_cond);
  }
      
  pthread_mutex_unlock(thread->start_mutex);

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
  
  /* animation functions */
  animation_funcs.prepare = ags_gui_thread_animation_prepare;
  animation_funcs.check = ags_gui_thread_animation_check;
  animation_funcs.dispatch = ags_gui_thread_animation_dispatch;
  animation_funcs.finalize = NULL;
  
  gui_thread->animation_source = g_source_new(&animation_funcs,
					      sizeof(GSource));
  g_source_attach(gui_thread->animation_source,
  		  main_context);

  /* task functions */
  task_funcs.prepare = ags_gui_thread_task_prepare;
  task_funcs.check = ags_gui_thread_task_check;
  task_funcs.dispatch = ags_gui_thread_task_dispatch;
  task_funcs.finalize = NULL;

  gui_thread->task_source = g_source_new(&task_funcs,
					 sizeof(GSource));
  g_source_attach(gui_thread->task_source,
  		  main_context);

  /* sync functions */
  sync_funcs.prepare = ags_gui_thread_sync_task_prepare;
  sync_funcs.check = ags_gui_thread_sync_task_check;
  sync_funcs.dispatch = ags_gui_thread_sync_task_dispatch;
  sync_funcs.finalize = NULL;

  gui_thread->sync_source = g_source_new(&sync_funcs,
					 sizeof(GSource));
  g_source_attach(gui_thread->sync_source,
  		  main_context);
  
  /* show animation */
  g_atomic_int_set(&(xorg_application_context->gui_ready),
		   TRUE);

  /* gtk+-2.0 main */    
  gtk_main();
}

void
ags_gui_thread_schedule_task(AgsGuiThread *gui_thread,
			     GObject *task)
{
  AgsApplicationContext *application_context;

  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  if(!AGS_IS_GUI_THREAD(gui_thread) ||
     !AGS_IS_TASK(task)){
    return;
  }

  application_context = ags_application_context_get_instance();
  main_loop = application_context->main_loop;

  task_thread = ags_thread_find_type(main_loop,
				     AGS_TYPE_TASK_THREAD);

  if(task_thread == NULL){
    return;
  }

#if 0  
  g_atomic_int_or(&(task_thread->flags),
		  (AGS_TASK_THREAD_EXTERN_SYNC |
		   AGS_TASK_THREAD_EXTERN_READY));

  gui_thread->queued_sync = 4;
#endif
  
  pthread_mutex_lock(gui_thread->task_schedule_mutex);

  gui_thread->collected_task = g_list_prepend(gui_thread->collected_task,
					      task);
  
  pthread_mutex_unlock(gui_thread->task_schedule_mutex);
}

void
ags_gui_thread_schedule_task_list(AgsGuiThread *gui_thread,
				  GList *task)
{
  AgsApplicationContext *application_context;

  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  if(!AGS_IS_GUI_THREAD(gui_thread) ||
     task == NULL){
    return;
  }

  application_context = ags_application_context_get_instance();
  main_loop = application_context->main_loop;

  task_thread = ags_thread_find_type(main_loop,
				     AGS_TYPE_TASK_THREAD);

  if(task_thread == NULL){
    return;
  }

#if 0  
  g_atomic_int_or(&(task_thread->flags),
		  (AGS_TASK_THREAD_EXTERN_SYNC |
		   AGS_TASK_THREAD_EXTERN_READY));

  gui_thread->queued_sync = 4;
#endif
  
  pthread_mutex_lock(gui_thread->task_schedule_mutex);

  gui_thread->collected_task = g_list_concat(g_list_reverse(task),
					     gui_thread->collected_task);
  
  pthread_mutex_unlock(gui_thread->task_schedule_mutex);
}

/**
 * ags_gui_thread_new:
 *
 * Create a new #AgsGuiThread.
 *
 * Returns: the new #AgsGuiThread
 *
 * Since: 1.0.0
 */
AgsGuiThread*
ags_gui_thread_new()
{
  AgsGuiThread *gui_thread;
  
  gui_thread = (AgsGuiThread *) g_object_new(AGS_TYPE_GUI_THREAD,
					     NULL);

  return(gui_thread);
}
