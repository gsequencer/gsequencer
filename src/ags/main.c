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
#include <ags/main.h>

#include <ags/object/ags_connectable.h>

#include <ags/object/ags_main_loop.h>

#include <ags/plugin/ags_ladspa_manager.h>

#include <ags/file/ags_file.h>

#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>
#include <ags/thread/ags_devout_thread.h>
#include <ags/thread/ags_export_thread.h>
#include <ags/thread/ags_audio_thread.h>
#include <ags/thread/ags_channel_thread.h>
#include <ags/thread/ags_gui_thread.h>
#include <ags/thread/ags_autosave_thread.h>
#include <ags/thread/ags_single_thread.h>

#include <ags/audio/ags_config.h>

#include <sys/mman.h>

#include <gtk/gtk.h>

#include <stdlib.h>
#include <libintl.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/resource.h>
#include <mcheck.h>
#include <signal.h>
#include <time.h>

#include <gdk/gdk.h>

#include <sys/types.h>
#include <pwd.h>

#include "config.h"

void application_context_class_init(AgsMainClass *application_context);
void application_context_connectable_interface_init(AgsConnectableInterface *connectable);
void application_context_init(AgsMain *application_context);
void application_context_connect(AgsConnectable *connectable);
void application_context_disconnect(AgsConnectable *connectable);
void application_context_finalize(GObject *gobject);

void ags_colors_alloc();

static void ags_signal_cleanup();
void ags_signal_handler(int signr);
void ags_signal_handler_timer(int sig, siginfo_t *si, void *uc);

static gpointer application_context_parent_class = NULL;
static sigset_t ags_wait_mask;
static sigset_t ags_timer_mask;

pthread_mutex_t ags_application_mutex;
AgsMain *application_context;

static const gchar *ags_config_thread = AGS_CONFIG_THREAD;
static const gchar *ags_config_devout = AGS_CONFIG_DEVOUT;

extern void ags_thread_resume_handler(int sig);
extern void ags_thread_suspend_handler(int sig);

extern AgsConfig *config;

extern GtkStyle *cell_pattern_style;
extern GtkStyle *ffplayer_style;
extern GtkStyle *meter_style;
extern GtkStyle *pattern_edit_style;
extern GtkStyle *note_edit_style;
extern GtkStyle *meter_style;
extern GtkStyle *dial_style;
extern GtkStyle *indicator_style;
extern GtkStyle *led_style;

extern AgsLadspaManager *ags_ladspa_manager;

struct sigaction ags_sigact;
struct sigaction ags_sigact_timer;

struct sigevent ags_sev_timer;
struct itimerspec its;

GType
application_context_get_type()
{
  static GType ags_type_main = 0;

  if(!ags_type_main){
    static const GTypeInfo application_context_info = {
      sizeof (AgsMainClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) application_context_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMain),
      0,    /* n_preallocs */
      (GInstanceInitFunc) application_context_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) application_context_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_main = g_type_register_static(G_TYPE_OBJECT,
					   "AgsMain\0",
					   &application_context_info,
					   0);

    g_type_add_interface_static(ags_type_main,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_main);
}

void
application_context_class_init(AgsMainClass *application_context)
{
  GObjectClass *gobject;

  application_context_parent_class = g_type_class_peek_parent(application_context);

  /* GObjectClass */
  gobject = (GObjectClass *) application_context;

  gobject->finalize = application_context_finalize;
}

void
application_context_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = application_context_connect;
  connectable->disconnect = application_context_disconnect;
}

void
application_context_init(AgsMain *application_context)
{
  GFile *file;
  FILE *log_file;
  struct sigaction sa;
  struct passwd *pw;
  uid_t uid;
  gchar *wdir, *filename;
    
  uid = getuid();
  pw = getpwuid(uid);
  
  wdir = g_strdup_printf("%s/%s\0",
			 pw->pw_dir,
			 AGS_DEFAULT_DIRECTORY);

  file = g_file_new_for_path(wdir);

  g_file_make_directory(file,
			NULL,
			NULL);

  application_context->flags = 0;

  application_context->version = AGS_VERSION;
  application_context->build_id = AGS_BUILD_ID;

  ags_colors_alloc();

  application_context->main_loop = NULL;

  /* AgsAutosaveThread */
  application_context->autosave_thread = NULL;
  application_context->autosave_thread = (AgsThread *) ags_autosave_thread_new(NULL, application_context);
  g_object_ref(G_OBJECT(application_context->autosave_thread));
  ags_connectable_connect(AGS_CONNECTABLE(application_context->autosave_thread));
  
  application_context->thread_pool = ags_thread_pool_new(NULL);
  application_context->server = NULL;
  application_context->devout = NULL;
  application_context->window = NULL;
  // ags_log_message(ags_default_log, "starting Advanced Gtk+ Sequencer\n\0");

  sigfillset(&(ags_wait_mask));
  sigdelset(&(ags_wait_mask), AGS_THREAD_SUSPEND_SIG);
  sigdelset(&(ags_wait_mask), AGS_THREAD_RESUME_SIG);

  sigfillset(&(sa.sa_mask));
  sa.sa_flags = 0;

  sa.sa_handler = ags_thread_resume_handler;
  sigaction(AGS_THREAD_RESUME_SIG, &sa, NULL);

  sa.sa_handler = ags_thread_suspend_handler;
  sigaction(AGS_THREAD_SUSPEND_SIG, &sa, NULL);
  
  filename = g_strdup_printf("%s/%s\0",
			     wdir,
			     AGS_DEFAULT_CONFIG);

  application_context->config = config;
  //TODO:JK: ugly
  config->application_context = application_context;

  g_free(filename);
  g_free(wdir);
}

void
application_context_connect(AgsConnectable *connectable)
{
  AgsMain *application_context;
  GList *list;

  application_context = AGS_MAIN(connectable);

  if((AGS_MAIN_CONNECTED & (application_context->flags)) != 0)
    return;

  application_context->flags |= AGS_MAIN_CONNECTED;

  ags_connectable_connect(AGS_CONNECTABLE(G_OBJECT(application_context->main_loop)));
  ags_connectable_connect(AGS_CONNECTABLE(application_context->thread_pool));

  g_message("connected threads\0");

  list = application_context->devout;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_message("connected audio\0");

  ags_connectable_connect(AGS_CONNECTABLE(application_context->window));
  g_message("connected gui\0");
}

void
application_context_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
application_context_finalize(GObject *gobject)
{
  AgsMain *application_context;

  application_context = AGS_MAIN(gobject);

  G_OBJECT_CLASS(application_context_parent_class)->finalize(gobject);
}

void
ags_colors_alloc()
{
  static gboolean allocated = FALSE;
}

void
application_context_load_config(AgsMain *application_context)
{
  AgsConfig *config;
  GList *list;

  auto void application_context_load_config_thread(AgsThread *thread);
  auto void application_context_load_config_devout(AgsDevout *devout);

  void application_context_load_config_thread(AgsThread *thread){
    gchar *model;
    
    model = ags_config_get(config,
			   ags_config_devout,
			   "model\0");
    
    if(model != NULL){
      if(!strncmp(model,
		  "single-threaded\0",
		  16)){
	//TODO:JK: implement me
	
      }else if(!strncmp(model,
			"multi-threaded",
			15)){
	//TODO:JK: implement me
      }else if(!strncmp(model,
			"super-threaded",
			15)){
	//TODO:JK: implement me
      }
    }
  }
  void application_context_load_config_devout(AgsDevout *devout){
    gchar *alsa_handle;
    guint samplerate;
    guint buffer_size;
    guint pcm_channels, dsp_channels;

    alsa_handle = ags_config_get(config,
				 ags_config_devout,
				 "alsa-handle\0");

    dsp_channels = strtoul(ags_config_get(config,
					  ags_config_devout,
					  "dsp-channels\0"),
			   NULL,
			   10);
    
    pcm_channels = strtoul(ags_config_get(config,
					  ags_config_devout,
					  "pcm-channels\0"),
			   NULL,
			   10);

    samplerate = strtoul(ags_config_get(config,
					ags_config_devout,
					"samplerate\0"),
			 NULL,
			 10);

    buffer_size = strtoul(ags_config_get(config,
					 ags_config_devout,
					 "buffer-size\0"),
			  NULL,
			  10);
    
    g_object_set(G_OBJECT(devout),
		 "device\0", alsa_handle,
		 "dsp-channels\0", dsp_channels,
		 "pcm-channels\0", pcm_channels,
		 "frequency\0", samplerate,
		 "buffer-size\0", buffer_size,
		 NULL);
  }
  
  if(application_context == NULL){
    return;
  }

  config = application_context->config;

  if(config == NULL){
    return;
  }

  /* thread */
  application_context_load_config_thread(application_context->main_loop);

  /* devout */
  list = application_context->devout;

  while(list != NULL){
    application_context_load_config_devout(AGS_DEVOUT(list->data));

    list = list->next;
  }
}

void
application_context_add_devout(AgsMain *application_context,
		    AgsDevout *devout)
{
  if(application_context == NULL ||
     devout == NULL){
    return;
  }

  g_object_ref(G_OBJECT(devout));
  application_context->devout = g_list_prepend(application_context->devout,
				    devout);
}

void
application_context_register_recall_type()
{
  ags_play_audio_get_type();
  ags_play_channel_get_type();
  ags_play_channel_run_get_type();
  ags_play_channel_run_master_get_type();

  ags_stream_channel_get_type();
  ags_stream_channel_run_get_type();

  ags_loop_channel_get_type();
  ags_loop_channel_run_get_type();

  ags_copy_channel_get_type();
  ags_copy_channel_run_get_type();

  ags_mute_audio_get_type();
  ags_mute_audio_run_get_type();
  ags_mute_channel_get_type();
  ags_mute_channel_run_get_type();

  ags_volume_channel_get_type();
  ags_volume_channel_run_get_type();

  ags_peak_channel_get_type();
  ags_peak_channel_run_get_type();

  ags_recall_ladspa_get_type();
  ags_recall_channel_run_dummy_get_type();
  ags_recall_ladspa_run_get_type();

  ags_delay_audio_get_type();
  ags_delay_audio_run_get_type();

  ags_count_beats_audio_get_type();
  ags_count_beats_audio_run_get_type();

  ags_copy_pattern_audio_get_type();
  ags_copy_pattern_audio_run_get_type();
  ags_copy_pattern_channel_get_type();
  ags_copy_pattern_channel_run_get_type();

  ags_buffer_channel_get_type();
  ags_buffer_channel_run_get_type();

  ags_play_notation_audio_get_type();
  ags_play_notation_audio_run_get_type();
}

void
application_context_register_task_type()
{
  //TODO:JK: implement me
}

void
application_context_register_widget_type()
{
  ags_dial_get_type();
}

void
application_context_register_machine_type()
{
  ags_panel_get_type();
  ags_panel_input_pad_get_type();
  ags_panel_input_line_get_type();

  ags_mixer_get_type();
  ags_mixer_input_pad_get_type();
  ags_mixer_input_line_get_type();

  ags_drum_get_type();
  ags_drum_output_pad_get_type();
  ags_drum_output_line_get_type();
  ags_drum_input_pad_get_type();
  ags_drum_input_line_get_type();

  ags_matrix_get_type();

  ags_synth_get_type();
  ags_synth_input_pad_get_type();
  ags_synth_input_line_get_type();

  ags_ffplayer_get_type();
}

void
application_context_register_thread_type()
{
  ags_thread_get_type();

  ags_audio_loop_get_type();
  ags_task_thread_get_type();
  ags_devout_thread_get_type();
  ags_export_thread_get_type();
  ags_audio_thread_get_type();
  ags_channel_thread_get_type();
  ags_iterator_thread_get_type();
  ags_recycling_thread_get_type();
  ags_timestamp_thread_get_type();
  ags_gui_thread_get_type();

  ags_thread_pool_get_type();
  ags_returnable_thread_get_type();
}

void
application_context_quit(AgsMain *application_context)
{
  AgsThread *gui_thread;

  gui_thread = ags_thread_find_type(application_context->main_loop,
				    AGS_TYPE_GUI_THREAD);

  ags_thread_stop(gui_thread);
}

AgsMain*
application_context_new()
{
  AgsMain *application_context;

  application_context = (AgsMain *) g_object_new(AGS_TYPE_MAIN,
				      NULL);

  return(application_context);
}

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

void
ags_signal_handler_timer(int sig, siginfo_t *si, void *uc)
{
  pthread_mutex_lock(application_context->main_loop->timer_mutex);

  g_atomic_int_set(&(application_context->main_loop->timer_expired),
		   TRUE);
  
  if(application_context->main_loop->timer_wait){
    pthread_cond_signal(application_context->main_loop->timer_cond);
  }
  
  pthread_mutex_unlock(application_context->main_loop->timer_mutex);

  //  g_message("sig\0");
  //  signal(sig, SIG_IGN);
}

static void
ags_signal_cleanup()
{
  sigemptyset(&(ags_sigact.sa_mask));
}

int
main(int argc, char **argv)
{
  AgsDevout *devout;
  AgsWindow *window;
  AgsThread *gui_thread;
  AgsThread *async_queue;
  AgsThread *devout_thread;
  AgsThread *export_thread;
  
  GFile *autosave_file;
  struct sched_param param;
  struct rlimit rl;
  timer_t timerid;
  gchar *filename, *autosave_filename;

  struct passwd *pw;
  uid_t uid;
  gchar *wdir, *config_file;
  int result;
  gboolean single_thread = FALSE;
  guint i;

  const char *error;
  const rlim_t kStackSize = 256L * 1024L * 1024L;   // min stack size = 128 Mb

  pthread_mutexattr_t attr;
  
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

  /**/
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr,
			    PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(&ags_application_mutex,
		     &attr);

#ifdef AGS_USE_TIMER
  /* create timer */
  ags_sigact_timer.sa_flags = SA_SIGINFO;
  ags_sigact_timer.sa_sigaction = ags_signal_handler_timer;
  sigemptyset(&ags_sigact_timer.sa_mask);
  
  if(sigaction(SIGRTMIN, &ags_sigact_timer, NULL) == -1){
    perror("sigaction\0");
    exit(EXIT_FAILURE);
  }
  
  /* Block timer signal temporarily */
  sigemptyset(&ags_timer_mask);
  sigaddset(&ags_timer_mask, SIGRTMIN);
  
  if(sigprocmask(SIG_SETMASK, &ags_timer_mask, NULL) == -1){
    perror("sigprocmask\0");
    exit(EXIT_FAILURE);
  }

  /* Create the timer */
  ags_sev_timer.sigev_notify = SIGEV_SIGNAL;
  ags_sev_timer.sigev_signo = SIGRTMIN;
  ags_sev_timer.sigev_value.sival_ptr = &timerid;
  
  if(timer_create(CLOCK_MONOTONIC, &ags_sev_timer, &timerid) == -1){
    perror("timer_create\0");
    exit(EXIT_FAILURE);
  }
#endif

  /* parse gtkrc */
  uid = getuid();
  pw = getpwuid(uid);
  
  gtk_rc_parse(g_strdup_printf("%s/%s/ags.rc",
			       pw->pw_dir,
			       AGS_DEFAULT_DIRECTORY));
  
  /**/
  LIBXML_TEST_VERSION;

  g_thread_init(NULL);
  gdk_threads_init();

  gtk_init(&argc, &argv);
  ipatch_init();

  ao_initialize();

  filename = NULL;

  for(i = 0; i < argc; i++){
    if(!strncmp(argv[i], "--help\0", 7)){
      printf("GSequencer is an audio sequencer and notation editor\n\n\0");

      printf("Usage:\n\t%s\n\t%s\n\t%s\n\t%s\n\n",
	     "Report bugs to <jkraehemann@gmail.com>\n\0",
	     "--filename file     open file\0",
	     "--single-thread     run in single thread mode\0",     
	     "--help              display this help and exit\0",
	     "--version           output version information and exit\0");
      
      exit(0);
    }else if(!strncmp(argv[i], "--version\0", 10)){
      printf("GSequencer 0.4.2\n\n\0");
      
      printf("%s\n%s\n%s\n\n\0",
	     "Copyright (C) 2005-2015 Joël Krähemann\0",
	     "This is free software; see the source for copying conditions.  There is NO\0",
	     "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\0");
      
      printf("Written by Joël Krähemann\n\0");
      exit(0);
    }else if(!strncmp(argv[i], "--single-thread\0", 16)){
      single_thread = TRUE;
    }else if(!strncmp(argv[i], "--filename\0", 11)){
      filename = argv[i + 1];
      i++;
    }
  }

  config = ags_config_new();
  
  wdir = g_strdup_printf("%s/%s\0",
			 pw->pw_dir,
			 AGS_DEFAULT_DIRECTORY);

  config_file = g_strdup_printf("%s/%s\0",
				wdir,
				AGS_DEFAULT_CONFIG);

  ags_config_load_from_file(config,
			    config_file);

  g_free(wdir);
  g_free(config_file);

  if(filename != NULL){
    AgsFile *file;

    file = g_object_new(AGS_TYPE_FILE,
			"filename\0", filename,
			NULL);
    ags_file_open(file);
    ags_file_read(file);

    application_context = AGS_MAIN(file->application_context);
    ags_file_close(file);

    ags_thread_start(application_context->main_loop);

    /* complete thread pool */
    application_context->thread_pool->parent = AGS_THREAD(application_context->main_loop);
    ags_thread_pool_start(application_context->thread_pool);

#ifdef AGS_USE_TIMER
    /* Start the timer */
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = NSEC_PER_SEC / AGS_THREAD_MAX_PRECISION;
    its.it_interval.tv_sec = its.it_value.tv_sec;
    its.it_interval.tv_nsec = its.it_value.tv_nsec;

    if(timer_settime(timerid, 0, &its, NULL) == -1){
      perror("timer_settime\0");
      exit(EXIT_FAILURE);
    
    }
    
    if(sigprocmask(SIG_UNBLOCK, &ags_timer_mask, NULL) == -1){
      perror("sigprocmask\0");
      exit(EXIT_FAILURE);
    }
#endif

    gui_thread = ags_thread_find_type(application_context->main_loop,
				      AGS_TYPE_GUI_THREAD);
    
#ifdef _USE_PTH
    pth_join(gui_thread->thread,
	     NULL);
#else
    pthread_join(*(gui_thread->thread),
		 NULL);
#endif
  }else{
    application_context = application_context_new();

    if(single_thread){
      application_context->flags = AGS_MAIN_SINGLE_THREAD;
    }

    /* Declare ourself as a real time task */
    param.sched_priority = AGS_PRIORITY;

    if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
      perror("sched_setscheduler failed\0");
    }

    mlockall(MCL_CURRENT | MCL_FUTURE);

    if((AGS_MAIN_SINGLE_THREAD & (application_context->flags)) == 0){
      //      GdkFrameClock *frame_clock;

#ifdef AGS_WITH_XMLRPC_C
      AbyssInit(&error);

      xmlrpc_env_init(&(application_context->env));
#endif /* AGS_WITH_XMLRPC_C */

      /* AgsDevout */
      devout = ags_devout_new((GObject *) application_context);
      application_context_add_devout(application_context,
			  devout);

      /*  */
      g_object_set(G_OBJECT(application_context->autosave_thread),
		   "devout\0", devout,
		   NULL);

      /* AgsWindow */
      application_context->window =
	window = ags_window_new((GObject *) application_context);
      g_object_set(G_OBJECT(window),
		   "devout\0", devout,
		   NULL);
      g_object_ref(G_OBJECT(window));

      gtk_window_set_default_size((GtkWindow *) window, 500, 500);
      gtk_paned_set_position((GtkPaned *) window->paned, 300);

      ags_connectable_connect(window);
      gtk_widget_show_all((GtkWidget *) window);

      /* AgsServer */
      application_context->server = ags_server_new((GObject *) application_context);

      /* AgsMainLoop */
      application_context->main_loop = (AgsThread *) ags_audio_loop_new((GObject *) devout, (GObject *) application_context);
      application_context->thread_pool->parent = application_context->main_loop;
      g_object_ref(G_OBJECT(application_context->main_loop));
      ags_connectable_connect(AGS_CONNECTABLE(G_OBJECT(application_context->main_loop)));

      /* AgsTaskThread */
      async_queue = (AgsThread *) ags_task_thread_new(devout);
      AGS_TASK_THREAD(async_queue)->thread_pool = application_context->thread_pool;
      ags_main_loop_set_async_queue(AGS_MAIN_LOOP(application_context->main_loop),
				    async_queue);
      ags_thread_add_child_extended(application_context->main_loop,
				    async_queue,
				    TRUE, TRUE);

      /* AgsGuiThread */
      gui_thread = (AgsThread *) ags_gui_thread_new();
      ags_thread_add_child_extended(application_context->main_loop,
				    gui_thread,
				    TRUE, TRUE);

      /* AgsDevoutThread */
      devout_thread = (AgsThread *) ags_devout_thread_new(devout);
      ags_thread_add_child_extended(application_context->main_loop,
				    devout_thread,
				    TRUE, TRUE);

      /* AgsExportThread */
      export_thread = (AgsThread *) ags_export_thread_new(devout, NULL);
      ags_thread_add_child_extended(application_context->main_loop,
				    export_thread,
				    TRUE, TRUE);

      /* start thread tree */
      ags_thread_start(application_context->main_loop);

      /* wait thread */
      pthread_mutex_lock(AGS_THREAD(application_context->main_loop)->start_mutex);

      g_atomic_int_set(&(AGS_THREAD(application_context->main_loop)->start_wait),
		       TRUE);
	
      if(g_atomic_int_get(&(AGS_THREAD(application_context->main_loop)->start_wait)) == TRUE &&
	 g_atomic_int_get(&(AGS_THREAD(application_context->main_loop)->start_done)) == FALSE){
	while(g_atomic_int_get(&(AGS_THREAD(application_context->main_loop)->start_wait)) == TRUE &&
	      g_atomic_int_get(&(AGS_THREAD(application_context->main_loop)->start_done)) == FALSE){
	  pthread_cond_wait(AGS_THREAD(application_context->main_loop)->start_cond,
			    AGS_THREAD(application_context->main_loop)->start_mutex);
	}
      }
	
      pthread_mutex_unlock(AGS_THREAD(application_context->main_loop)->start_mutex);

      /* complete thread pool */
      ags_thread_pool_start(application_context->thread_pool);
    }else{
      AgsSingleThread *single_thread;

      devout = ags_devout_new((GObject *) application_context);
      application_context_add_devout(application_context,
			  devout);

      g_object_set(G_OBJECT(application_context->autosave_thread),
		   "devout\0", devout,
		   NULL);

      /* threads */
      single_thread = ags_single_thread_new((GObject *) devout);

      /* AgsWindow */
      application_context->window = 
	window = ags_window_new((GObject *) application_context);
      g_object_set(G_OBJECT(window),
		   "devout\0", devout,
		   NULL);
      gtk_window_set_default_size((GtkWindow *) window, 500, 500);
      gtk_paned_set_position((GtkPaned *) window->paned, 300);

      ags_connectable_connect(window);
      gtk_widget_show_all((GtkWidget *) window);

      /* AgsMainLoop */
      application_context->main_loop = AGS_MAIN_LOOP(ags_audio_loop_new((GObject *) devout, (GObject *) application_context));
      g_object_ref(G_OBJECT(application_context->main_loop));

      /* complete thread pool */
      application_context->thread_pool->parent = AGS_THREAD(application_context->main_loop);
      ags_thread_pool_start(application_context->thread_pool);

      /* start thread tree */
      ags_thread_start((AgsThread *) single_thread);
    }

#ifdef AGS_USE_TIMER
    /* Start the timer */
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = NSEC_PER_SEC / AGS_THREAD_MAX_PRECISION; // / AGS_AUDIO_LOOP_DEFAULT_JIFFIE;
    its.it_interval.tv_sec = its.it_value.tv_sec;
    its.it_interval.tv_nsec = its.it_value.tv_nsec;

    if(timer_settime(timerid, 0, &its, NULL) == -1){
      perror("timer_settime\0");
      exit(EXIT_FAILURE);
    
    }
    
    if(sigprocmask(SIG_UNBLOCK, &ags_timer_mask, NULL) == -1){
      perror("sigprocmask\0");
      exit(EXIT_FAILURE);
    }
#endif

    gui_thread = ags_thread_find_type(application_context->main_loop,
				      AGS_TYPE_GUI_THREAD);

    if(!single_thread){
      /* join gui thread */
#ifdef _USE_PTH
      pth_join(gui_thread->thread,
	       NULL);
#else
      pthread_join(*(gui_thread->thread),
		   NULL);
#endif
    }
  }

  /* free managers */
  if(ags_ladspa_manager != NULL){
    g_object_unref(ags_ladspa_manager_get_instance());
  }

  uid = getuid();
  pw = getpwuid(uid);
  
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
