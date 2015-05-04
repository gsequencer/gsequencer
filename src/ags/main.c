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

#include <ags/main.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>

#include <ags/thread/ags_concurrency_provider.h>
#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif
#include <ags/thread/ags_single_thread.h>
#include <ags/thread/ags_autosave_thread.h>
#include <ags/thread/ags_task_thread.h>
#include <ags/thread/ags_thread_pool.h>

#include <ags/thread/task/ags_start_thread.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_soundcard_thread.h>
#include <ags/audio/thread/ags_export_thread.h>

#include <ags/server/ags_server.h>

#include <ags/plugin/ags_ladspa_manager.h>

#include <ags/audio/ags_sound_provider.h>

#include <ags/X/ags_xorg_init.h>
#include <ags/X/ags_xorg_application_context.h>
#include <ags/X/ags_window.h>

#include <ags/X/thread/ags_gui_thread.h>

#include <libintl.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/resource.h>
#include <mcheck.h>

#include <ao/ao.h>

#include <X11/Xlib.h>

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>

#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include <sys/types.h>
#include <pwd.h>

#include <sys/mman.h>

static void ags_signal_cleanup();
void ags_signal_handler(int signr);

extern void ags_thread_resume_handler(int sig);
extern void ags_thread_suspend_handler(int sig);

static sigset_t ags_wait_mask;

extern AgsLadspaManager *ags_ladspa_manager;

struct sigaction ags_sigact;

void
ags_signal_handler(int signr)
{
  if(signr == SIGINT){
    //TODO:JK: do backup
    
    exit(-1);
  }else{
    sigemptyset(&(ags_sigact.sa_mask));

    //    if(signr == AGS_ASYNC_QUEUE_SIGNAL_HIGH){
      // pthread_yield();
    //    }
  }
}

static void
ags_signal_cleanup()
{
  sigemptyset(&(ags_sigact.sa_mask));
}

void
ags_main_quit(AgsApplicationContext *application_context)
{
  AgsThread *gui_thread;
  AgsThread *children;

  /* find gui thread */
  children = AGS_THREAD(application_context->main_loop)->children;

  while(children != NULL){
    if(AGS_IS_GUI_THREAD(children)){
      gui_thread = children;

      break;
    }

    children = children->next;
  }

  ags_thread_stop(gui_thread);
}

int
main(int argc, char **argv)
{
  AgsThread *audio_loop, *gui_thread, *task_thread;
  AgsThreadPool *thread_pool;
  AgsApplicationContext *application_context;
  GFile *autosave_file;
    
  gchar *filename, *autosave_filename;
  gboolean single_thread = FALSE;
  guint i;

  struct passwd *pw;
  uid_t uid;

  struct sched_param param;
  struct rlimit rl;
  struct sigaction sa;

  int result;

  const rlim_t kStackSize = 64L * 1024L * 1024L;   // min stack size = 64 Mb
  
  //  mtrace();
  atexit(ags_signal_cleanup);

  result = getrlimit(RLIMIT_STACK, &rl);

  /* set stack size 64M */
  if(result == 0){
    if(rl.rlim_cur < kStackSize){
      rl.rlim_cur = kStackSize;
      result = setrlimit(RLIMIT_STACK, &rl);

      if(result != 0){
	//TODO:JK
      }
    }
  }

  /* Ignore interactive and job-control signals.  */
  signal(SIGINT, SIG_IGN);
  signal(SIGQUIT, SIG_IGN);
  signal(SIGTSTP, SIG_IGN);
  signal(SIGTTIN, SIG_IGN);
  signal(SIGTTOU, SIG_IGN);
  signal(SIGCHLD, SIG_IGN);

  ags_sigact.sa_handler = ags_signal_handler;
  sigemptyset(&ags_sigact.sa_mask);
  ags_sigact.sa_flags = 0;
  sigaction(SIGINT, &ags_sigact, (struct sigaction *) NULL);
  sigaction(SA_RESTART, &ags_sigact, (struct sigaction *) NULL);

  /* register signal handler */
  sigfillset(&(ags_wait_mask));
  sigdelset(&(ags_wait_mask), AGS_THREAD_SUSPEND_SIG);
  sigdelset(&(ags_wait_mask), AGS_THREAD_RESUME_SIG);

  sigfillset(&(sa.sa_mask));
  sa.sa_flags = 0;

  sa.sa_handler = ags_thread_resume_handler;
  sigaction(AGS_THREAD_RESUME_SIG, &sa, NULL);

  sa.sa_handler = ags_thread_suspend_handler;
  sigaction(AGS_THREAD_SUSPEND_SIG, &sa, NULL);

  /* get user info */
  uid = getuid();
  pw = getpwuid(uid);

  /* init gsequencer */
  gtk_init(&argc, &argv);

  application_context = ags_xorg_application_context_new();
  application_context->argc = argc;
  application_context->argv = argv;
  
  ags_xorg_init_context(application_context);

  audio_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));
  task_thread = ags_concurrency_provider_get_task_thread(AGS_CONCURRENCY_PROVIDER(application_context));
  thread_pool = ags_concurrency_provider_get_thread_pool(AGS_CONCURRENCY_PROVIDER(application_context));
  
  /* Declare ourself as a real time task */
  param.sched_priority = AGS_RT_PRIORITY;

  if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
    //    perror("sched_setscheduler failed\0");
  }

  mlockall(MCL_CURRENT | MCL_FUTURE);

  /* parse command line parameter */
  filename = NULL;

  for(i = 0; i < argc; i++){
    if(!strncmp(argv[i], "--single-thread\0", 16)){
      single_thread = TRUE;
    }else if(!strncmp(argv[i], "--filename\0", 11)){
      filename = argv[i + 1];
      i++;
    }
  }

  if(filename != NULL){
    AgsFile *file;

    file = g_object_new(AGS_TYPE_FILE,
			"application-context\0", application_context,
			"filename\0", filename,
			NULL);
    ags_file_open(file);
    ags_file_read(file);
    ags_file_close(file);
    
    gui_thread = ags_thread_find_type(application_context->main_loop,
				      AGS_TYPE_GUI_THREAD);
  }else{
    guint val;
    
    /* wait for audio loop */
    thread_pool->parent = audio_loop;
    ags_thread_pool_start(thread_pool);

    task_thread = ags_thread_find_type(audio_loop,
				       AGS_TYPE_TASK_THREAD);
    
    gui_thread = ags_thread_find_type(audio_loop,
				      AGS_TYPE_GUI_THREAD);

    ags_thread_start(task_thread);
    ags_thread_start(audio_loop);
    ags_thread_start(gui_thread);
    
    /* wait for task thread */
    pthread_mutex_lock(&(task_thread->start_mutex));
  
    while((AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(AGS_THREAD(task_thread)->flags)))) != 0){
      pthread_cond_wait(&(task_thread->start_cond),
			&(task_thread->start_mutex));
    }
  
    pthread_mutex_unlock(&(task_thread->start_mutex));

    /* wait for audio loop */
    pthread_mutex_lock(&(audio_loop->start_mutex));
  
    while((AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(AGS_THREAD(audio_loop)->flags)))) != 0){
      pthread_cond_wait(&(audio_loop->start_cond),
			&(audio_loop->start_mutex));
      val = g_atomic_int_get(&(AGS_THREAD(audio_loop)->flags));
    }
  
    pthread_mutex_unlock(&(audio_loop->start_mutex));

    /* wait for audio loop */
    pthread_mutex_lock(&(gui_thread->start_mutex));
  
    while((AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(AGS_THREAD(gui_thread)->flags)))) != 0){
      pthread_cond_wait(&(gui_thread->start_cond),
			&(gui_thread->start_mutex));
      val = g_atomic_int_get(&(AGS_THREAD(gui_thread)->flags));
    }
  
    pthread_mutex_unlock(&(gui_thread->start_mutex));
  }

  if(!single_thread){
    /* join gui thread */
#ifdef _USE_PTH
    pth_join(gui_thread->thread,
	     NULL);
#else
    pthread_join(gui_thread->thread,
		 NULL);
#endif
  }else{
    AgsSingleThread *single_thread;

    /* single thread */
    single_thread = ags_single_thread_new((GObject *) ags_sound_provider_get_soundcard(application_context)->data);

    /* add known threads to single_thread */
    ags_thread_add_child(AGS_THREAD(single_thread),
			 audio_loop);
    
    /* start thread tree */
    ags_thread_start((AgsThread *) single_thread);
  }
    
  /* free managers */
  if(ags_ladspa_manager != NULL){
    g_object_unref(ags_ladspa_manager_get_instance());
  }
  
  /* delete autosave file */  
  autosave_filename = g_strdup_printf("%s/%s/%d-%s\0",
				      pw->pw_dir,
				      AGS_DEFAULT_DIRECTORY,
				      getpid(),
				      AGS_AUTOSAVE_THREAD_DEFAULT_FILENAME);
  
  autosave_file = g_file_new_for_path(autosave_filename);

  if(g_file_query_exists(autosave_file,
			 NULL)){
    g_file_delete(autosave_file,
		  NULL,
		  NULL);
  }

  g_free(autosave_filename);
  g_object_unref(autosave_file);

  //  muntrace();

  return(0);
}
