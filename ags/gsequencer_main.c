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

#include <gtk/gtk.h>

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

#define _GNU_SOURCE
#include <locale.h>

#include <stdlib.h>

#ifndef AGS_W32API
#include <sys/time.h>
#include <sys/resource.h>
#endif

#include <ags/X/ags_xorg_application_context.h>

#include "config.h"
#include "gsequencer_main.h"

#include <ags/i18n.h>

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

  ags_application_context_setup(AGS_APPLICATION_CONTEXT(xorg_application_context));
  
  g_thread_exit(NULL);

  return(NULL);
}

void
ags_setup(int argc, char **argv)
{
  AgsApplicationContext *application_context;
  AgsLog *log;

  GThread *thread;

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
  thread = g_thread_new("Advanced Gtk+ Sequencer - setup",
			ags_setup_thread,
			application_context);
  
  ags_application_context_prepare(application_context);
}

int
main(int argc, char **argv)
{  
  AgsConfig *config;

  gchar *filename;
#if defined AGS_W32API
  gchar *app_dir;
  gchar *path;
#endif

  gboolean single_thread_enabled;
  gboolean builtin_theme_disabled;
  guint i;

#ifdef AGS_WITH_RT
  struct sched_param param;
  struct rlimit rl;
#endif

#ifndef AGS_W32API
  struct passwd *pw;

  uid_t uid;
#endif
  
  gchar *wdir, *config_file;
  gchar *rc_filename;

  gboolean has_file;
  int result;

#ifdef AGS_WITH_RT
  const rlim_t kStackSize = 64L * 1024L * 1024L;   // min stack size = 64 Mb
#endif
  
  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, LOCALEDIR);
  textdomain(PACKAGE);
  
  single_thread_enabled = FALSE;
  builtin_theme_disabled = FALSE;

  config = NULL;
  
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
  has_file = FALSE;
  
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
	     "Copyright (C) 2005-2018 Joël Krähemann",
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

      if(g_file_test(filename,
		     G_FILE_TEST_EXISTS) &&
	 g_file_test(filename,
		     G_FILE_TEST_IS_REGULAR)){
	has_file = TRUE;
      }
    }
  }

#ifdef AGS_W32API
  if(!builtin_theme_disabled){
    app_dir = NULL;
    
    if(strlen(argv[0]) > strlen("\\gsequencer.exe")){
      app_dir = g_strndup(argv[0],
			  strlen(argv[0]) - strlen("\\gsequencer.exe"));
    }
    
    if((rc_filename = getenv("AGS_RC_FILENAME")) == NULL){
      rc_filename = g_strdup_printf("%s\\share\\gsequencer\\styles\\ags.rc",
				    g_get_current_dir());
    
      if(!g_file_test(rc_filename,
		      G_FILE_TEST_IS_REGULAR)){
	g_free(rc_filename);

	if(g_path_is_absolute(app_dir)){
	  rc_filename = g_strdup_printf("%s\\%s",
					app_dir,
					"\\share\\gsequencer\\styles\\ags.rc");
	}else{
	  rc_filename = g_strdup_printf("%s\\%s\\%s",
					g_get_current_dir(),
					app_dir,
					"\\share\\gsequencer\\styles\\ags.rc");
	}
      }
    }else{
      rc_filename = g_strdup(rc_filename);
    }
  
    gtk_rc_parse(rc_filename);
    
    g_free(rc_filename);
    g_free(app_dir);
  }
#else
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
#endif
  
  /**/
  LIBXML_TEST_VERSION;
  xmlInitParser();
  
  //ao_initialize();

  //  gdk_threads_enter();
  //  g_thread_init(NULL);
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
  
#if 0
  g_log_set_fatal_mask("GLib",
  		       G_LOG_LEVEL_CRITICAL);

  g_log_set_fatal_mask("libInstPatch",
  		       G_LOG_LEVEL_CRITICAL);

  g_log_set_fatal_mask("GLib-GObject",
  		       G_LOG_LEVEL_WARNING | G_LOG_LEVEL_CRITICAL);

  g_log_set_fatal_mask(NULL,
  		       G_LOG_LEVEL_WARNING | G_LOG_LEVEL_CRITICAL);  

  g_log_set_fatal_mask("Gtk",
  		       G_LOG_LEVEL_CRITICAL);
#endif

  g_set_application_name(i18n("Advanced Gtk+ Sequencer"));
  gtk_window_set_default_icon_name("gsequencer");
  g_setenv("PULSE_PROP_media.role", "production", TRUE);
  
  /* setup */
  if(!has_file){
#ifdef AGS_W32API
  app_dir = NULL;

  if(strlen(argv[0]) > strlen("\\gsequencer.exe")){
    app_dir = g_strndup(argv[0],
			strlen(argv[0]) - strlen("\\gsequencer.exe"));
  }
  
  path = g_strdup_printf("%s\\etc\\gsequencer",
			 g_get_current_dir());
    
  if(!g_file_test(path,
		  G_FILE_TEST_IS_DIR)){
    g_free(path);

    if(g_path_is_absolute(app_dir)){
      path = g_strdup_printf("%s\\%s",
			     app_dir,
			     "\\etc\\gsequencer");
    }else{
      path = g_strdup_printf("%s\\%s\\%s",
			     g_get_current_dir(),
			     app_dir,
			     "\\etc\\gsequencer");
    }
  }
    
  config_file = g_strdup_printf("%s\\%s",
				path,
				AGS_DEFAULT_CONFIG);

  g_free(path);
#else
  wdir = g_strdup_printf("%s/%s",
			 pw->pw_dir,
			 AGS_DEFAULT_DIRECTORY);
    
  config_file = g_strdup_printf("%s/%s",
				wdir,
				AGS_DEFAULT_CONFIG);

  g_free(wdir);
#endif

    config = ags_config_get_instance();

    ags_config_load_from_file(config,
			      config_file);

    g_free(config_file);
  }

  /* some GUI scaling */
  if(!builtin_theme_disabled &&
     !has_file){
//    ags_xorg_application_context_load_gui_scale(ags_application_context_get_instance());
  }

  ags_setup(argc, argv);
    
  //  muntrace();

  return(0);
}
