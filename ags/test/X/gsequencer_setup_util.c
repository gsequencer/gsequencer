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

#include <glib.h>
#include <glib-object.h>

#include <gdk/gdk.h>
#include <pango/pangocairo.h>

#include "gsequencer_setup_util.h"

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

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

#include <string.h>

#include <unistd.h>

#include <libintl.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/mman.h>

#include <sys/types.h>
#include <pwd.h>

#include <locale.h>

#include "config.h"

struct sigaction ags_test_sigact;

extern AgsApplicationContext *ags_application_context;

extern volatile gboolean ags_show_start_animation;

void
ags_test_init(int *argc, char ***argv,
	      gchar *conf_str)
{
  AgsConfig *config;

  gchar *filename;

  gboolean single_thread_enabled;
  gboolean builtin_theme_disabled;
  guint i;

#ifdef AGS_WITH_RT
  struct sched_param param;
  struct rlimit rl;
#endif
  struct passwd *pw;

  gchar *wdir, *config_file;
  gchar *rc_filename;
  gchar *base_dir;
  gchar *str, *data_dir;
  gchar path[PATH_MAX];

  uint32_t size = sizeof(path);
  uid_t uid;
  int result;

  const rlim_t kStackSize = 64L * 1024L * 1024L;   // min stack size = 64 Mb

  base_dir = strdup(SRCDIR);
  printf("base dir %s\n", base_dir);
  
  /* set some environment variables */
  sprintf(path, "%s/gsequencer.share/styles",
	  base_dir);
  data_dir = realpath(path,
		      NULL);
  str = malloc(PATH_MAX * sizeof(gchar));
  sprintf(str,
	  "AGS_RC_FILENAME=%s/ags.rc",
	  data_dir);
  putenv(str);

  sprintf(path, "%s/gsequencer.share/images",
	  base_dir);
  data_dir = realpath(path,
		      NULL);
  str = malloc(PATH_MAX * sizeof(gchar));
  sprintf(str,
	  "AGS_ANIMATION_FILENAME=%s/ags_supermoon-800x450.png",
	  data_dir);
  putenv(str);

  sprintf(path, "%s/gsequencer.share/images",
	  base_dir);
  data_dir = realpath(path,
		      NULL);
  str = malloc(PATH_MAX * sizeof(gchar));
  sprintf(str,
	  "AGS_LOGO_FILENAME=%s/ags.png",
	  data_dir);
  putenv(str);

  sprintf(path, "%s",
	  base_dir);
  data_dir = realpath(path,
		      NULL);
  str = malloc(PATH_MAX * sizeof(gchar));
  sprintf(str,
	  "AGS_LICENSE_FILENAME=%s/COPYING",
	  data_dir);
  putenv(str);

  /* gettext */
  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, LOCALEDIR);
  textdomain(PACKAGE);

  /* parameters */
  single_thread_enabled = FALSE;
  builtin_theme_disabled = FALSE;
  
  //  g_log_set_fatal_mask("GLib", // "Gtk" , // 
  //		       G_LOG_LEVEL_CRITICAL); // G_LOG_LEVEL_WARNING

#ifdef AGS_WITH_RT
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

  param.sched_priority = GSEQUENCER_RT_PRIORITY;
      
  if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
    perror("sched_setscheduler failed");
  }
#endif

  //#ifdef AGS_WITH_X11
  XInitThreads();
  //#endif
  
  /* parse command line parameter */
  filename = NULL;

  for(i = 0; i < argc[0]; i++){
    if(!strncmp(argv[0][i], "--help", 7)){
      printf("GSequencer is an audio sequencer and notation editor\n\n");

      printf("Usage:\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n\n",
	     "Report bugs to <jkraehemann@gmail.com>\n",
	     "--filename file     open file",
	     "--single-thread     run in single thread mode",
	     "--no-builtin-theme  disable built-in theme",
	     "--help              display this help and exit",
	     "--version           output version information and exit");
      
      exit(0);
    }else if(!strncmp(argv[0][i], "--version", 10)){
      printf("GSequencer %s\n\n", AGS_VERSION);
      
      printf("%s\n%s\n%s\n\n",
	     "Copyright (C) 2005-2017 Joël Krähemann",
	     "This is free software; see the source for copying conditions.  There is NO",
	     "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.");
      
      printf("Written by Joël Krähemann\n");

      exit(0);
    }else if(!strncmp(argv[0][i], "--single-thread", 16)){
      single_thread_enabled = TRUE;
    }else if(!strncmp(argv[0][i], "--no-builtin-theme", 19)){
      builtin_theme_disabled = TRUE;
    }else if(!strncmp(argv[0][i], "--filename", 11)){
      filename = *argv[i + 1];
      i++;
    }
  }

  uid = getuid();
  pw = getpwuid(uid);
    
  /* parse rc file */
  if(!builtin_theme_disabled){
    rc_filename = g_strdup_printf("%s/%s/ags.rc",
				  pw->pw_dir,
				  AGS_DEFAULT_DIRECTORY);

    if(!g_file_test(rc_filename,
		    G_FILE_TEST_IS_REGULAR)){
      g_free(rc_filename);

#ifdef AGS_RC_FILENAME
      rc_filename = g_strdup(AGS_RC_FILENAME);
#else
      if((rc_filename = getenv("AGS_RC_FILENAME")) == NULL){
	rc_filename = g_strdup_printf("%s%s",
				      DESTDIR,
				      "/gsequencer/styles/ags.rc");
      }else{
	rc_filename = g_strdup(rc_filename);
      }
#endif
    }
  
    gtk_rc_parse(rc_filename);
    g_free(rc_filename);
  }
  
  /**/
  LIBXML_TEST_VERSION;

  ags_gui_init(argc, argv);
  gtk_init(argc, argv);

  if(!builtin_theme_disabled){
    g_object_set(gtk_settings_get_default(),
		 "gtk-theme-name", "Raleigh",
		 NULL);
    g_signal_handlers_block_matched(gtk_settings_get_default(),
				    G_SIGNAL_MATCH_DETAIL,
				    g_signal_lookup("set-property",
						    GTK_TYPE_SETTINGS),
				    g_quark_from_string("gtk-theme-name"),
				    NULL,
				    NULL,
				    NULL);
  }
  
#ifdef AGS_WITH_LIBINSTPATCH
  ipatch_init();
#endif
  
  /* setup */
  wdir = g_strdup_printf("%s/%s",
			 pw->pw_dir,
			 AGS_DEFAULT_DIRECTORY);

  config_file = g_strdup_printf("%s/%s",
				wdir,
				AGS_DEFAULT_CONFIG);

  config = ags_config_get_instance();

  if(conf_str != NULL){
    ags_config_load_from_data(config,
			      conf_str,
			      strlen(conf_str));
  }else{
    ags_config_load_from_file(config,
			      config_file);
  }
  
  g_free(wdir);
  g_free(config_file);  
}

void
ags_test_show_file_error(gchar *filename,
			 GError *error)
{
  GtkDialog *dialog;
      
  g_warning("could not parse file %s\0", filename);
      
  dialog = gtk_message_dialog_new(NULL,
				  0,
				  GTK_MESSAGE_WARNING,
				  GTK_BUTTONS_OK,
				  "Failed to open '%s'\0",
				  filename);
  gtk_widget_show_all((GtkWidget *) dialog);
  g_signal_connect(dialog, "response\0",
		   G_CALLBACK(gtk_main_quit), NULL);
  gtk_main();
}

void
ags_test_signal_handler(int signr)
{
  if(signr == SIGINT){
    //TODO:JK: do backup
    
    exit(-1);
  }else{
    sigemptyset(&(ags_test_sigact.sa_mask));

    //    if(signr == AGS_ASYNC_QUEUE_SIGNAL_HIGH){
      // pthread_yield();
    //    }
  }
}

void
ags_test_signal_cleanup()
{
  sigemptyset(&(ags_test_sigact.sa_mask));
}

void
ags_test_setup(int argc, char **argv)
{
  AgsLadspaManager *ladspa_manager;
  AgsDssiManager *dssi_manager;
  AgsLv2Manager *lv2_manager;
  AgsLv2uiManager *lv2ui_manager;
  AgsLv2WorkerManager *lv2_worker_manager;

  AgsLog *log;

  gchar *blacklist_filename;
  gchar *filename;
  
  uid_t uid;

  guint i;
  
  /* check filename */
  log = ags_log_get_instance();
  filename = NULL;

  ags_log_add_message(log,
		      "Welcome to Advanced Gtk+ Sequencer\0");
  
  for(i = 0; i < argc; i++){
    if(!strncmp(argv[i], "--filename\0", 11)){
      AgsSimpleFile *simple_file;

      xmlXPathContext *xpath_context; 
      xmlXPathObject *xpath_object;
      xmlNode **node;

      xmlChar *xpath;
      
      gchar *buffer;
      guint buffer_length;
      
      filename = argv[i + 1];
      simple_file = ags_simple_file_new();
      g_object_set(simple_file,
		   "filename\0", filename,
		   NULL);
      ags_simple_file_open(simple_file,
			   NULL);

      xpath = "/ags-simple-file/ags-sf-config";

      /* Create xpath evaluation context */
      xpath_context = xmlXPathNewContext(simple_file->doc);

      if(xpath_context == NULL) {
	g_warning("Error: unable to create new XPath context\0");

	break;
      }

      /* Evaluate xpath expression */
      xpath_object = xmlXPathEval(xpath, xpath_context);

      if(xpath_object == NULL) {
	g_warning("Error: unable to evaluate xpath expression \"%s\"\0", xpath);
	xmlXPathFreeContext(xpath_context); 

	break;
      }

      node = xpath_object->nodesetval->nodeTab;
  
      for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
	if(node[i]->type == XML_ELEMENT_NODE){
	  buffer = xmlNodeGetContent(node[i]);
	  buffer_length = strlen(buffer);
	  
	  break;
	}
      }
      
      if(buffer != NULL){
	//	ags_config_clear(ags_config_get_instance());
	ags_config_load_from_data(ags_config_get_instance(),
				  buffer, buffer_length);
      }
      
      break;
    }
  }

  /* load ladspa manager */
  ladspa_manager = ags_ladspa_manager_get_instance();

  blacklist_filename = "ladspa.blacklist\0";
  ags_ladspa_manager_load_blacklist(ladspa_manager,
				    blacklist_filename);

  ags_log_add_message(ags_log_get_instance(),
		      "* Loading LADSPA plugins\0");
  
  ags_ladspa_manager_load_default_directory(ladspa_manager);

  /* load dssi manager */
  dssi_manager = ags_dssi_manager_get_instance();

  blacklist_filename = "dssi_plugin.blacklist\0";
  ags_dssi_manager_load_blacklist(dssi_manager,
				  blacklist_filename);

  ags_log_add_message(ags_log_get_instance(),
		      "* Loading DSSI plugins\0");

  ags_dssi_manager_load_default_directory(dssi_manager);

  /* load lv2 manager */
  lv2_manager = ags_lv2_manager_get_instance();
  lv2_worker_manager = ags_lv2_worker_manager_get_instance();    

  blacklist_filename = "lv2_plugin.blacklist\0";
  ags_lv2_manager_load_blacklist(lv2_manager,
				 blacklist_filename);

  ags_log_add_message(ags_log_get_instance(),
		      "* Loading Lv2 plugins\0");

  ags_lv2_manager_load_default_directory(lv2_manager);

  /* load lv2ui manager */
  lv2ui_manager = ags_lv2ui_manager_get_instance();  

  blacklist_filename = "lv2ui_plugin.blacklist\0";
  ags_lv2ui_manager_load_blacklist(lv2ui_manager,
				   blacklist_filename);
  
  ags_log_add_message(ags_log_get_instance(),
		      "* Loading Lv2ui plugins\0");

  ags_lv2ui_manager_load_default_directory(lv2ui_manager);
  
  /* application contex */
  ags_application_context = (AgsApplicationContext *) ags_xorg_application_context_new();
  ags_application_context->argc = argc;
  ags_application_context->argv = argv;

  ags_application_context_register_types(ags_application_context);

  /* fix cross-references in managers */
  lv2_worker_manager->thread_pool = ((AgsXorgApplicationContext *) ags_application_context)->thread_pool;

  g_atomic_int_set(&(ags_show_start_animation),
		   FALSE);
}

void
ags_test_start_animation(pthread_t *thread)
{
  GtkWindow *window;

  GdkWindowAttr *attr;

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  g_object_set(window,
	       "decorated\0", FALSE,
	       0);
  gtk_widget_set_size_request(window,
			      800, 450);
  gtk_widget_show_all(window);

  pthread_create(thread, NULL,
		 ags_test_start_animation_thread, window);
}
void*
ags_test_start_animation_thread(void *ptr)
{
  GtkWidget *window;
  GdkRectangle rectangle;

  cairo_t *gdk_cr, *cr;
  cairo_surface_t *surface;

  AgsLog *log;

  gchar *filename;
  unsigned char *bg_data, *image_data;
  
  /* create a buffer suitable to image size */
  GList *list, *start;

  guint image_size;
  gdouble x0, y0;
  guint i, nth;
  
  gdk_threads_enter();
  
  window = (GtkWidget *) ptr;

  rectangle.x = 0;
  rectangle.y = 0;
  rectangle.width = 800;
  rectangle.height = 450;

  image_size = 4 * 800 * 450;
  
  gdk_cr = gdk_cairo_create(window->window);
  
  filename = g_strdup_printf("%s/%s\0",
			     SRCDIR,
			     "gsequencer.share/images/ags_supermoon-800x450.png\0");

  surface = cairo_image_surface_create_from_png(filename);
  image_data = cairo_image_surface_get_data(surface);
  
  bg_data = (unsigned char *) malloc(image_size * sizeof(unsigned char));

  if(image_data != NULL){
    memcpy(bg_data, image_data, image_size * sizeof(unsigned char));
  }
  
  cr = cairo_create(surface);
  
  cairo_select_font_face(cr, "Georgia\0",
			 CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(cr, (gdouble) 11.0);
  
  gdk_window_show(window->window);
  
  gdk_threads_leave();

  log = ags_log_get_instance();  
  nth = 0;
  
  while(g_atomic_int_get(&(ags_show_start_animation))){
    start = 
      list = ags_log_get_messages(log);

    i = g_list_length(start);

    if(i > nth){
      if(image_data != NULL){
	memcpy(image_data, bg_data, image_size * sizeof(unsigned char));
      }
      
      cairo_set_source_surface(cr, surface, 0, 0);
      cairo_paint(cr);
      cairo_surface_flush(surface);
    
      x0 = 4.0;
      y0 = 4.0 + (i * 12.0);

      while(y0 > 4.0 && list != NULL){
	cairo_set_source_rgb(cr,
			     1.0,
			     0.0,
			     1.0);
	
	cairo_move_to(cr,
		      x0, y0);

	cairo_show_text(cr, list->data);

	list = list->next;
	y0 -= 12.0;
      }

      cairo_move_to(cr,
		    x0, 4.0 + (i + 1) * 12.0);
      cairo_show_text(cr, "...\0");
      
      nth = g_list_length(start);
    }

    cairo_set_source_surface(gdk_cr, surface, 0, 0);
    cairo_paint(gdk_cr);
    cairo_surface_flush(surface);
    gdk_flush();
  }

  free(bg_data);
  
  gdk_threads_enter();

  gtk_widget_destroy(window);
  
  gdk_threads_leave();
}

void
ags_test_launch(gboolean single_thread)
{
  AgsMutexManager *mutex_manager;
  AgsThread *audio_loop, *polling_thread, *gui_thread, *task_thread;
  AgsThreadPool *thread_pool;

  AgsConfig *config;

  GList *start_queue;  

  pthread_mutex_t *application_mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
    
  /* get threads, thread pool and config */
  pthread_mutex_lock(application_mutex);
  
  audio_loop = (AgsThread *) ags_application_context->main_loop;
  task_thread = (AgsThread *) ags_application_context->task_thread;
  thread_pool = AGS_TASK_THREAD(task_thread)->thread_pool;

  config = ags_application_context->config;
  
  pthread_mutex_unlock(application_mutex);
  
  polling_thread = ags_thread_find_type(audio_loop,
					AGS_TYPE_POLLING_THREAD);
  gui_thread = ags_thread_find_type(audio_loop,
				    AGS_TYPE_GUI_THREAD);

  /* start engine */
  pthread_mutex_lock(audio_loop->start_mutex);
  
  start_queue = NULL;
  start_queue = g_list_prepend(start_queue,
			       polling_thread);
  start_queue = g_list_prepend(start_queue,
			       task_thread);
  //  start_queue = g_list_prepend(start_queue,
  //			       gui_thread);
  g_atomic_pointer_set(&(audio_loop->start_queue),
		       start_queue);
  
  pthread_mutex_unlock(audio_loop->start_mutex);

  /* start audio loop and thread pool*/
  ags_thread_start(audio_loop);
  
  ags_thread_pool_start(thread_pool);

  if(!single_thread){
    /* wait for audio loop */
    pthread_mutex_lock(audio_loop->start_mutex);

    if(g_atomic_int_get(&(audio_loop->start_wait)) == TRUE){	
      g_atomic_int_set(&(audio_loop->start_done),
		       FALSE);
      
      while(g_atomic_int_get(&(audio_loop->start_wait)) == TRUE &&
	    g_atomic_int_get(&(audio_loop->start_done)) == FALSE){
	pthread_cond_wait(audio_loop->start_cond,
			  audio_loop->start_mutex);
      }
    }
    
    pthread_mutex_unlock(audio_loop->start_mutex);

    /* start gui thread */
    ags_thread_start(gui_thread);

    /* wait for gui thread */
    pthread_mutex_lock(gui_thread->start_mutex);

    if(g_atomic_int_get(&(gui_thread->start_done)) == FALSE){
      
      g_atomic_int_set(&(gui_thread->start_wait),
    		       TRUE);

      while(g_atomic_int_get(&(gui_thread->start_done)) == FALSE){
    	g_atomic_int_set(&(gui_thread->start_wait),
    			 TRUE);
	
    	pthread_cond_wait(gui_thread->start_cond,
			  gui_thread->start_mutex);
      }
    }
    
    pthread_mutex_unlock(gui_thread->start_mutex);

    g_atomic_int_set(&(AGS_XORG_APPLICATION_CONTEXT(ags_application_context)->gui_ready),
		     1);
    
    /* autosave thread */
    if(!g_strcmp0(ags_config_get_value(config,
				       AGS_CONFIG_GENERIC,
				       "autosave-thread\0"),
		  "true\0")){
      pthread_mutex_lock(audio_loop->start_mutex);

      start_queue = g_atomic_pointer_get(&(audio_loop->start_queue));
      start_queue = g_list_prepend(start_queue,
				   task_thread);

      g_atomic_pointer_set(&(audio_loop->start_queue),
			   start_queue);
	
      pthread_mutex_unlock(audio_loop->start_mutex);
    }
  }else{
    AgsSingleThread *single_thread;

    /* single thread */
    single_thread = ags_single_thread_new((GObject *) ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(ags_application_context))->data);

    /* add known threads to single_thread */
    ags_thread_add_child(AGS_THREAD(single_thread),
			 audio_loop);
    
    /* autosave thread */
    if(!g_strcmp0(ags_config_get_value(config,
				       AGS_CONFIG_GENERIC,
				       "autosave-thread\0"),
		  "true\0")){
      pthread_mutex_lock(audio_loop->start_mutex);

      start_queue = g_atomic_pointer_get(&(audio_loop->start_queue));
      start_queue = g_list_prepend(start_queue,
				   task_thread);

      g_atomic_pointer_set(&(audio_loop->start_queue),
			   start_queue);
	
      pthread_mutex_unlock(audio_loop->start_mutex);
    }

    /* start thread tree */
    ags_thread_start((AgsThread *) single_thread);
  }
}

void
ags_test_launch_filename(gchar *filename,
			 gboolean single_thread)
{
  AgsMutexManager *mutex_manager;
  AgsThread *audio_loop, *polling_thread, *gui_thread, *task_thread;
  AgsThreadPool *thread_pool;

  AgsConfig *config;

  GList *start_queue;  

  pthread_mutex_t *application_mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
    
  /* get threads, thread pool and config */
  pthread_mutex_lock(application_mutex);

  audio_loop = (AgsThread *) ags_application_context->main_loop;
  task_thread =(AgsThread *)  ags_application_context->task_thread;
  thread_pool = AGS_TASK_THREAD(task_thread)->thread_pool;

  config = ags_application_context->config;
  
  pthread_mutex_unlock(application_mutex);

  polling_thread = ags_thread_find_type(audio_loop,
					AGS_TYPE_POLLING_THREAD);
  gui_thread = ags_thread_find_type(audio_loop,
				    AGS_TYPE_GUI_THREAD);

  /* open file */
  if(g_strcmp0(ags_config_get_value(config,
				    AGS_CONFIG_GENERIC,
				    "simple-file\0"),
		 "false\0")){
    AgsSimpleFile *simple_file;

    AgsSimpleFileRead *simple_file_read;
      
    GError *error;

    simple_file = (AgsSimpleFile *) g_object_new(AGS_TYPE_SIMPLE_FILE,
						 "application-context\0", ags_application_context,
						 "filename\0", filename,
						 NULL);
    error = NULL;
    ags_simple_file_open(simple_file,
			 &error);

    if(error != NULL){
      ags_test_show_file_error(filename,
			       error);
      ags_application_context_quit(ags_application_context);
    }
    
    /* start engine */  
    pthread_mutex_lock(audio_loop->start_mutex);
    
    start_queue = NULL;
    start_queue = g_list_prepend(start_queue,
				 polling_thread);
    start_queue = g_list_prepend(start_queue,
				 task_thread);
    //    start_queue = g_list_prepend(start_queue,
    //				 gui_thread);
    g_atomic_pointer_set(&(audio_loop->start_queue),
			 start_queue);
  
    pthread_mutex_unlock(audio_loop->start_mutex);

    /* start audio loop and thread pool */
    ags_thread_start(audio_loop);

    ags_thread_pool_start(thread_pool);

    if(!single_thread){
      /* wait for audio loop */
      pthread_mutex_lock(audio_loop->start_mutex);

      if(g_atomic_int_get(&(audio_loop->start_wait)) == TRUE){	
	g_atomic_int_set(&(audio_loop->start_done),
			 FALSE);
      
	while(g_atomic_int_get(&(audio_loop->start_wait)) == TRUE &&
	      g_atomic_int_get(&(audio_loop->start_done)) == FALSE){
	  pthread_cond_wait(audio_loop->start_cond,
			    audio_loop->start_mutex);
	}
      }
    
      pthread_mutex_unlock(audio_loop->start_mutex);

      /* start gui thread */
      ags_thread_start(gui_thread);

      /* wait for gui thread */
      pthread_mutex_lock(gui_thread->start_mutex);

      if(g_atomic_int_get(&(gui_thread->start_done)) == FALSE){
      
      	g_atomic_int_set(&(gui_thread->start_wait),
      			 TRUE);

    	while(g_atomic_int_get(&(gui_thread->start_done)) == FALSE){
    	  g_atomic_int_set(&(gui_thread->start_wait),
    			   TRUE);
	
    	  pthread_cond_wait(gui_thread->start_cond,
    			    gui_thread->start_mutex);
    	}
      }
    
      pthread_mutex_unlock(gui_thread->start_mutex);     
      
      /* autosave thread */
      if(!g_strcmp0(ags_config_get_value(config,
					 AGS_CONFIG_GENERIC,
					 "autosave-thread\0"),
		    "true\0")){
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
    }else{
      AgsFile *file;

      GError *error;
    
      file = g_object_new(AGS_TYPE_FILE,
			  "application-context\0", ags_application_context,
			  "filename\0", filename,
			  NULL);
      error = NULL;
      ags_file_open(file,
		    &error);

      if(error != NULL){
	ags_test_show_file_error(filename,
				 error);
	
	ags_application_context_quit(ags_application_context);
      }
    
      ags_file_read(file);
      ags_file_close(file);
    }
  }
}
