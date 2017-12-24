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

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_xorg_application_context.h>

#include <ags/X/thread/ags_gui_thread.h>

#include "gsequencer_main.h"

#include "config.h"

#ifdef AGS_MAC_BUNDLE
void premain() __attribute__ ((constructor));
#endif

void* ags_setup_thread(void *ptr);
void ags_setup(int argc, char **argv);

extern AgsApplicationContext *ags_application_context;

gchar *base_dir;

#ifdef AGS_MAC_BUNDLE
void
premain()
{
  gchar path[PATH_MAX];
  uint32_t size = sizeof(path);  

  if(_NSGetExecutablePath(path, &size) == 0){
    gchar *ld_library_path;
    gchar *gdk_pixbuf_module_file;
    gchar *frameworks_dir;
    gchar *data_dir;
    gchar *plugin_dir;
    gchar *str;
    
    base_dir = strndup(path,
		       rindex(path, '/') - path);
    printf("base dir %s\n", base_dir);

    sprintf(path, "%s/../Frameworks",
	    base_dir);
    gdk_pixbuf_module_file = realpath(path,
				      NULL);
    str = malloc(PATH_MAX * sizeof(gchar));
    sprintf(str,
	    "GDK_PIXBUF_MODULE_FILE=%s/gdk-pixbuf-2.0/2.10.0/loaders.cache",
	    gdk_pixbuf_module_file);
    putenv(str);

    ld_library_path = realpath(path,
			       NULL);
    str = malloc(PATH_MAX * sizeof(gchar));
    sprintf(str,
	    "DT_RUNPATH=%s/gdk-pixbuf-2.0/2.10.0/loaders",
	    ld_library_path);
    putenv(str);

    frameworks_dir = realpath(path,
			      NULL);
    str = malloc(PATH_MAX * sizeof(gchar));
    sprintf(str,
	    "DYLD_FALLBACK_LIBRARY_PATH=%s",
	    frameworks_dir);
    putenv(str);

    str = malloc(PATH_MAX * sizeof(gchar));
    sprintf(str,
	    "GDK_PIXBUF_MODULEDIR=%s",
	    frameworks_dir);
    putenv(str);

    printf(".. %s", str);
    
    sprintf(path, "%s/../Resources",
	    base_dir);
    data_dir = realpath(path,
			NULL);
    str = malloc(PATH_MAX * sizeof(gchar));
    sprintf(str,
	    "GSEQUENCER_DATA_DIR=%s",
	    data_dir);
    putenv(str);

    sprintf(path, "%s/../Resources",
	    base_dir);
    data_dir = realpath(path,
			NULL);
    str = malloc(PATH_MAX * sizeof(gchar));
    sprintf(str,
	    "AGS_RC_FILENAME=%s/ags.rc",
	    data_dir);
    putenv(str);

    sprintf(path, "%s/../Resources",
	    base_dir);
    data_dir = realpath(path,
			NULL);
    str = malloc(PATH_MAX * sizeof(gchar));
    sprintf(str,
	    "AGS_ANIMATION_FILENAME=%s/ags_supermoon-800x450.png",
	    data_dir);
    putenv(str);

    sprintf(path, "%s/../Resources",
	    base_dir);
    data_dir = realpath(path,
			NULL);
    str = malloc(PATH_MAX * sizeof(gchar));
    sprintf(str,
	    "AGS_LOGO_FILENAME=%s/ags.png",
	    data_dir);
    putenv(str);

    sprintf(path, "%s/../Resources",
	    base_dir);
    data_dir = realpath(path,
			NULL);
    str = malloc(PATH_MAX * sizeof(gchar));
    sprintf(str,
	    "AGS_LICENSE_FILENAME=%s/GPL-3",
	    data_dir);
    putenv(str);

    sprintf(path, "%s/../Plugins",
	    base_dir);
    plugin_dir = realpath(path,
			  NULL);
    str = malloc(PATH_MAX * sizeof(gchar));
    sprintf(str,
	    "GSEQUENCER_PLUGIN_DIR=%s",
	    plugin_dir);
    putenv(str);
  }else{
    base_dir = NULL;
  }
}
#endif

void*
ags_setup_thread(void *ptr)
{
  AgsXorgApplicationContext *xorg_application_context;
  
  xorg_application_context = (AgsXorgApplicationContext *) ptr;

  while(g_atomic_int_get(&(xorg_application_context->gui_ready)) == 0){
    usleep(500000);
  }

  //  pthread_mutex_lock(ags_gui_thread_get_dispatch_mutex());
  
  ags_application_context_setup(xorg_application_context);

  //  pthread_mutex_unlock(ags_gui_thread_get_dispatch_mutex());
  
  pthread_exit(NULL);
}

void
ags_setup(int argc, char **argv)
{
  AgsApplicationContext *application_context;
  AgsLog *log;

  pthread_t thread;

  /* application context */
  application_context = 
    ags_application_context = (AgsApplicationContext *) ags_xorg_application_context_new();
  g_object_ref(application_context);
  
  application_context->argc = argc;
  application_context->argv = argv;

  log = ags_log_get_instance();

  ags_log_add_message(log,
		      "Welcome to Advanced Gtk+ Sequencer");
  
  /* application context */
  pthread_create(&thread, NULL,
		 ags_setup_thread, application_context);
  
  ags_application_context_prepare(application_context);
}

int
main(int argc, char **argv)
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
  
  uid_t uid;
  int result;

  const rlim_t kStackSize = 64L * 1024L * 1024L;   // min stack size = 64 Mb

  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, LOCALEDIR);
  textdomain(PACKAGE);
  
  single_thread_enabled = FALSE;
  builtin_theme_disabled = FALSE;
  
  //  mtrace();

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

  /* parse command line parameter */
  filename = NULL;

  for(i = 0; i < argc; i++){
    if(!strncmp(argv[i], "--help", 7)){
      printf("GSequencer is an audio sequencer and notation editor\n\n");

      printf("Usage:\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n\n",
	     "Report bugs to <jkraehemann@gmail.com>\n",
	     "--filename file     open file",
	     "--single-thread     run in single thread mode",
	     "--no-builtin-theme  disable built-in theme",
	     "--help              display this help and exit",
	     "--version           output version information and exit");
      
      exit(0);
    }else if(!strncmp(argv[i], "--version", 10)){
      printf("GSequencer %s\n\n", AGS_VERSION);
      
      printf("%s\n%s\n%s\n\n",
	     "Copyright (C) 2005-2017 Joël Krähemann",
	     "This is free software; see the source for copying conditions.  There is NO",
	     "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.");
      
      printf("Written by Joël Krähemann\n");

      exit(0);
    }else if(!strncmp(argv[i], "--single-thread", 16)){
      single_thread_enabled = TRUE;
    }else if(!strncmp(argv[i], "--no-builtin-theme", 19)){
      builtin_theme_disabled = TRUE;
    }else if(!strncmp(argv[i], "--filename", 11)){
      filename = argv[i + 1];
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

  //ao_initialize();

  //  gdk_threads_enter();
  //  g_thread_init(NULL);
  ags_gui_init(&argc, &argv);  
  gtk_init(&argc, &argv);
  
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

  g_log_set_fatal_mask("GLib-GObject", // "Gtk" , // 
  		       G_LOG_LEVEL_WARNING | G_LOG_LEVEL_CRITICAL); // G_LOG_LEVEL_WARNING

#if 0

  g_log_set_fatal_mask("Gtk", // "Gtk" , // 
  		       G_LOG_LEVEL_CRITICAL); // G_LOG_LEVEL_WARNING
#endif
  
  /* setup */
  wdir = g_strdup_printf("%s/%s",
			 pw->pw_dir,
			 AGS_DEFAULT_DIRECTORY);

  config_file = g_strdup_printf("%s/%s",
				wdir,
				AGS_DEFAULT_CONFIG);

  config = ags_config_get_instance();

  ags_config_load_from_file(config,
			    config_file);

  g_free(wdir);
  g_free(config_file);
  
  ags_setup(argc, argv);
    
  //  muntrace();

  return(0);
}
