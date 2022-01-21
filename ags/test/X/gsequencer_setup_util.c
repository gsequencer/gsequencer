/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

//FIXME:JK: conflict of KeyCode
#define __AGS_VST_KEYCODES_H__

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_xorg_application_context.h>
#include <ags/X/ags_window.h>

#include <ags/X/file/ags_simple_file.h>

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

#include <X11/Xlib.h>

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

static void ags_test_driver_mutex_create();

static GRecMutex ags_test_driver_mutex;

struct sigaction ags_test_sigact;

extern AgsApplicationContext *ags_application_context;

void
ags_test_enter()
{
  g_rec_mutex_lock(ags_test_get_driver_mutex());
  gdk_threads_enter();
}

void
ags_test_leave()
{
  gdk_threads_leave();
  g_rec_mutex_unlock(ags_test_get_driver_mutex());
}

GRecMutex*
ags_test_get_driver_mutex()
{
  return(&ags_test_driver_mutex);
}

void
ags_test_init(int *argc, char ***argv,
	      gchar *conf_str)
{
  AgsConfig *config;
  AgsPriority *priority;
  GtkSettings *settings;
  
  gchar *filename;

  gboolean no_config;
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

  base_dir = strdup(AGS_SRC_DIR);
  printf("base dir %s\n", base_dir);
  
  /* set some environment variables */
  sprintf(path, "%s/gsequencer.share/styles",
	  base_dir);
  data_dir = realpath(path,
		      NULL);
  str = malloc(PATH_MAX * sizeof(gchar));
  sprintf(str,
	  "AGS_CSS_FILENAME=%s/ags.css",
	  data_dir);
  putenv(str);

  sprintf(path, "%s/gsequencer.share/images",
	  base_dir);
  data_dir = realpath(path,
		      NULL);
  str = malloc(PATH_MAX * sizeof(gchar));
  sprintf(str,
	  "AGS_ANIMATION_FILENAME=%s/gsequencer-800x450.png",
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
  priority = ags_priority_get_instance();  
  ags_priority_load_defaults(priority);
  
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

  priority = ags_priority_get_instance();  

  param.sched_priority = 1;

  str = ags_priority_get_value(priority,
			       AGS_PRIORITY_RT_THREAD,
			       AGS_PRIORITY_KEY_GUI_MAIN_LOOP);

  if(str != NULL){
    param.sched_priority = (int) g_ascii_strtoull(str,
						  NULL,
						  10);

    g_free(str);
  }
      
  if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
    perror("sched_setscheduler failed");
  }
#endif

  //#ifdef AGS_WITH_X11
  XInitThreads();
  //#endif
  
  /* parse command line parameter */
  filename = NULL;
  no_config = FALSE;
  
  for(i = 0; i < argc[0]; i++){
    if(!strncmp(argv[0][i], "--help", 7)){
      printf("GSequencer is an audio sequencer and notation editor\n\n");

      printf("Usage:\n\t%s\n\t%s\n\t%s\n\t%s\n\n",
	     "Report bugs to <jkraehemann@gmail.com>\n",
	     "--filename file     open file",
	     "--help              display this help and exit",
	     "--version           output version information and exit");
      
      exit(0);
    }else if(!strncmp(argv[0][i], "--version", 10)){
      printf("GSequencer %s\n\n", AGS_VERSION);
      
      printf("%s\n%s\n%s\n\n",
	     "Copyright (C) 2005-2020 Joël Krähemann",
	     "This is free software; see the source for copying conditions.  There is NO",
	     "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.");
      
      printf("Written by Joël Krähemann\n");

      exit(0);
    }else if(!strncmp(argv[0][i], "--filename", 11)){
      filename = g_strdup(argv[0][i + 1]);
      i++;
    }else if(!strncmp(argv[0][i], "--no-config", 12)){
      no_config = TRUE;
      i++;
    }
  }

  uid = getuid();
  pw = getpwuid(uid);
  
  /**/
  LIBXML_TEST_VERSION;

#if defined(AGS_WITH_GSTREAMER)
  gst_init(argc, argv);
#endif

  gtk_init(argc, argv);
  
#ifdef AGS_WITH_LIBINSTPATCH
  ipatch_init();
#endif
  
  settings = gtk_settings_get_default();

  g_object_set(settings,
	       "gtk-primary-button-warps-slider", FALSE,
	       NULL);
  g_signal_handlers_block_matched(settings,
				  G_SIGNAL_MATCH_DETAIL,
				  g_signal_lookup("set-property",
						  GTK_TYPE_SETTINGS),
				  g_quark_from_string("gtk-primary-button-warps-slider"),
				  NULL,
				  NULL,
				  NULL);
  

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
ags_test_quit()
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;

  xorg_application_context = ags_application_context_get_instance();

  ags_test_enter();
  
  window = xorg_application_context->window;
  
  window->flags |= AGS_WINDOW_TERMINATING;

  ags_test_leave();

  //FIXME:JK: avoids exit to crash :(
  sleep(5);
}

void
ags_test_show_file_error(gchar *filename,
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
ags_test_signal_handler(int signr)
{
  if(signr == SIGINT){
    //TODO:JK: do backup
    
    exit(-1);
  }else{
    sigemptyset(&(ags_test_sigact.sa_mask));
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

  gboolean no_config;
  guint i;
  
  /* check filename */
  log = ags_log_get_instance();
  filename = NULL;

  ags_log_add_message(log,
		      "Welcome to Advanced Gtk+ Sequencer");

  no_config = FALSE;

  for(i = 0; i < argc; i++){
    if(!strncmp(argv[i], "--help", 7)){
      i++;
    }else if(!strncmp(argv[i], "--version", 10)){
      i++;
    }else if(!strncmp(argv[i], "--filename", 11)){
      filename = g_strdup(*argv[i + 1]);
      i++;
    }else if(!strncmp(argv[i], "--no-config", 12)){
      no_config = TRUE;
      i++;
    }
  }

  
  for(i = 0; i < argc; i++){
    if(!strncmp(argv[i], "--filename", 11)){
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
		   "filename", filename,
		   "no-config", no_config,
		   NULL);
      ags_simple_file_open(simple_file,
			   NULL);

      xpath = "/ags-simple-file/ags-sf-config";

      /* Create xpath evaluation context */
      xpath_context = xmlXPathNewContext(simple_file->doc);

      if(xpath_context == NULL) {
	g_warning("Error: unable to create new XPath context");

	break;
      }

      /* Evaluate xpath expression */
      xpath_object = xmlXPathEval(xpath, xpath_context);

      if(xpath_object == NULL) {
	g_warning("Error: unable to evaluate xpath expression \"%s\"", xpath);
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

  blacklist_filename = "ladspa.blacklist";
  ags_ladspa_manager_load_blacklist(ladspa_manager,
				    blacklist_filename);

  ags_log_add_message(ags_log_get_instance(),
		      "* Loading LADSPA plugins");
  
  ags_ladspa_manager_load_default_directory(ladspa_manager);

  /* load dssi manager */
  dssi_manager = ags_dssi_manager_get_instance();

  blacklist_filename = "dssi_plugin.blacklist";
  ags_dssi_manager_load_blacklist(dssi_manager,
				  blacklist_filename);

  ags_log_add_message(ags_log_get_instance(),
		      "* Loading DSSI plugins");

  ags_dssi_manager_load_default_directory(dssi_manager);

  /* load lv2 manager */
  lv2_manager = ags_lv2_manager_get_instance();
  lv2_worker_manager = ags_lv2_worker_manager_get_instance();    

  blacklist_filename = "lv2_plugin.blacklist";
  ags_lv2_manager_load_blacklist(lv2_manager,
				 blacklist_filename);

  ags_log_add_message(ags_log_get_instance(),
		      "* Loading Lv2 plugins");

  ags_lv2_manager_load_default_directory(lv2_manager);

  /* load lv2ui manager */
  lv2ui_manager = ags_lv2ui_manager_get_instance();  

  blacklist_filename = "lv2ui_plugin.blacklist";
  ags_lv2ui_manager_load_blacklist(lv2ui_manager,
				   blacklist_filename);
  
  ags_log_add_message(ags_log_get_instance(),
		      "* Loading Lv2ui plugins");

  ags_lv2ui_manager_load_default_directory(lv2ui_manager);
  
  /* application contex */
  ags_application_context = (AgsApplicationContext *) ags_xorg_application_context_new();
  ags_application_context->argc = argc;
  ags_application_context->argv = argv;

  ags_application_context_register_types(ags_application_context);

  /* fix cross-references in managers */
  lv2_worker_manager->thread_pool = ((AgsXorgApplicationContext *) ags_application_context)->thread_pool;

  ags_ui_provider_set_show_animation(AGS_UI_PROVIDER(ags_application_context), FALSE);
}

void
ags_test_launch()
{
  AgsThread *audio_loop;
  AgsThreadPool *thread_pool;

  AgsConfig *config;

  audio_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(ags_application_context));

  /* start audio loop and thread pool*/
  ags_thread_start(audio_loop);

  /* wait for audio loop */
  g_mutex_lock(AGS_THREAD_GET_START_MUTEX(audio_loop));

  if(ags_thread_test_status_flags(audio_loop, AGS_THREAD_STATUS_START_WAIT)){
    ags_thread_unset_status_flags(audio_loop, AGS_THREAD_STATUS_START_DONE);
      
    while(ags_thread_test_status_flags(audio_loop, AGS_THREAD_STATUS_START_WAIT) &&
	  !ags_thread_test_status_flags(audio_loop, AGS_THREAD_STATUS_START_DONE)){
      g_cond_wait(AGS_THREAD_GET_START_COND(audio_loop),
		  AGS_THREAD_GET_START_MUTEX(audio_loop));
    }
  }

  g_mutex_unlock(AGS_THREAD_GET_START_MUTEX(audio_loop));

  ags_ui_provider_set_gui_ready(AGS_UI_PROVIDER(ags_application_context),
				TRUE);
}

void
ags_test_launch_filename(gchar *filename)
{
  AgsSimpleFile *simple_file;

  AgsSimpleFileRead *simple_file_read;
      
  AgsThread *audio_loop;
  AgsThreadPool *thread_pool;
  AgsTaskLauncher *task_launcher;
  
  AgsConfig *config;

  GList *start_queue;  

  GError *error;
    
  /* get main loop and thread pool */
  audio_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(ags_application_context));

  thread_pool = ags_concurrency_provider_get_thread_pool(AGS_CONCURRENCY_PROVIDER(ags_application_context));

  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(ags_application_context));
  
  /* open file */
  simple_file = (AgsSimpleFile *) g_object_new(AGS_TYPE_SIMPLE_FILE,
					       "filename", filename,
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
  g_mutex_lock(AGS_THREAD_GET_START_MUTEX(audio_loop));
    
  start_queue = NULL;
  
  g_mutex_unlock(AGS_THREAD_GET_START_MUTEX(audio_loop));

  /* start audio loop and thread pool */
  ags_thread_start(audio_loop);

  ags_thread_pool_start(thread_pool);

  /* wait for audio loop */
  g_mutex_lock(AGS_THREAD_GET_START_MUTEX(audio_loop));

  if(ags_thread_test_status_flags(audio_loop, AGS_THREAD_STATUS_START_WAIT)){
    ags_thread_unset_status_flags(audio_loop, AGS_THREAD_STATUS_START_DONE);
      
    while(ags_thread_test_status_flags(audio_loop, AGS_THREAD_STATUS_START_WAIT) &&
	  !ags_thread_test_status_flags(audio_loop, AGS_THREAD_STATUS_START_DONE)){
      g_cond_wait(AGS_THREAD_GET_START_COND(audio_loop),
		  AGS_THREAD_GET_START_MUTEX(audio_loop));
    }
  }
    
  g_mutex_unlock(AGS_THREAD_GET_START_MUTEX(audio_loop));
    
  /* now start read task */
  simple_file_read = ags_simple_file_read_new(simple_file);

  ags_task_launcher_add_task(task_launcher,
			     (AgsTask *) simple_file_read);      
}
