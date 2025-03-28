/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include "gsequencer_main.h"

#include "config.h"

#include <glib.h>
#include <glib-object.h>

#include <gdk/gdk.h>
#include <pango/pangocairo.h>

#if defined(AGS_WITH_GSTREAMER)
#include <gst/gst.h>
#endif

#include <gtk/gtk.h>

#include <ags/libags.h>

#if defined(AGS_WITH_LIBINSTPATCH)
#include <libinstpatch/libinstpatch.h>
#endif

#include <libxml/parser.h>
#include <libxml/xlink.h>
#include <libxml/xpath.h>
#include <libxml/valid.h>
#include <libxml/xmlIO.h>
#include <libxml/xmlmemory.h>
#include <libxml/xmlsave.h>

#include <ags/app/ags_gsequencer_application.h>
#include <ags/app/ags_gsequencer_application_context.h>
#include <ags/app/ags_gsequencer_resource.h>
#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>

#include <glib.h>

#if defined(AGS_OSX_DMG_ENV)
#include <AVFoundation/AVFoundation.h>
#include <Cocoa/Cocoa.h>
#include <CoreFoundation/CoreFoundation.h>
#include <Foundation/Foundation.h>


#include <uuid/uuid.h>
#endif

#include <stdbool.h>

#include <libintl.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

//#include <ao/ao.h>

#include <sys/types.h>

#if defined(AGS_W32API)
#else
#include <pwd.h>
#endif

#define _GNU_SOURCE
#include <locale.h>

#include <stdlib.h>

#ifndef AGS_W32API
#include <sys/time.h>
#include <sys/resource.h>
#endif

#include <ags/i18n.h>

void install_data();

void
install_data()
{
#if defined(AGS_OSX_DMG_ENV)
  struct passwd *pw;

  gchar *music_path;
  gchar *app_dir;
  gchar *default_path;
  gchar *default_filename, *template_default_filename;
  gchar *default_dirs_mkdir_cmd;
  gchar *local_share_dirs_mkdir_cmd;
  gchar *local_state_dirs_mkdir_cmd;
  gchar *config_dirs_mkdir_cmd;
  gchar *cache_dirs_mkdir_cmd;
  gchar *default_file_cp_cmd;
  gchar *default_file_sed_cmd;
  gchar *free_sounds_cp_cmd;
  gchar *ags_conf_dirs_mkdir_cmd;
  gchar *ags_conf_cp_cmd;
  gchar *str;
  
  uid_t uid;

  GError *error;
  
  uid = getuid();
  pw = getpwuid(uid);

  app_dir = [[NSBundle mainBundle] bundlePath].UTF8String;
  
  music_path = g_strdup_printf("%s/Music",
			       pw->pw_dir);

  //TODO:JK: remove
  //  music_path = [[NSSearchPathForDirectoriesInDomains(NSMusicDirectory, NSUserDomainMask, YES) objectAtIndex:0] UTF8String];

  default_path = g_strdup_printf("%s%s",
				 music_path,
				 "/GSequencer/workspace/default");

  default_filename = g_strdup_printf("%s%s",
				     default_path,
				     "/gsequencer-default.xml");
  
  template_default_filename = g_strdup_printf("%s%s",
					      app_dir,
					      "/Contents/Resources/gsequencer-default.xml");

  default_dirs_mkdir_cmd = g_strdup_printf("mkdir -p %s/GSequencer/workspace/default",
					   music_path);

  local_state_dirs_mkdir_cmd = g_strdup_printf("mkdir -p %s/Library/Containers/%s/Data/.local/state",
					       pw->pw_dir, AGS_DEFAULT_BUNDLE_ID);

  local_share_dirs_mkdir_cmd = g_strdup_printf("mkdir -p %s/Library/Containers/%s/Data/.local/share",
					       pw->pw_dir, AGS_DEFAULT_BUNDLE_ID);
  
  cache_dirs_mkdir_cmd = g_strdup_printf("mkdir -p %s/Library/Containers/%s/Data/.cache",
					 pw->pw_dir, AGS_DEFAULT_BUNDLE_ID);
  
  config_dirs_mkdir_cmd = g_strdup_printf("mkdir -p %s/Library/Containers/%s/Data/.config",
					  pw->pw_dir, AGS_DEFAULT_BUNDLE_ID);

  default_file_cp_cmd = g_strdup_printf("cp -v %s/Contents/Resources/gsequencer-default.xml.in %s",
					app_dir,
					default_filename);
  
  default_file_sed_cmd = g_strdup_printf("sed -i '' 's,@MUSIC_DIR@,%s,g' %s",
					 music_path,
					 default_filename);
  free_sounds_cp_cmd = g_strdup_printf("cp -rv %s/Contents/Resources/free-sounds %s/GSequencer",
				       app_dir,
				       music_path);

  ags_conf_dirs_mkdir_cmd = g_strdup_printf("mkdir -p %s/Library/Containers/%s/Data/.gsequencer",
					    pw->pw_dir, AGS_DEFAULT_BUNDLE_ID);
  ags_conf_cp_cmd = g_strdup_printf("cp -v %s/Contents/Resources/ags.conf %s/Library/Containers/%s/Data/.gsequencer/",
				    app_dir,
				    pw->pw_dir,
				    AGS_DEFAULT_BUNDLE_ID);
  error = NULL;
  g_spawn_command_line_sync(default_dirs_mkdir_cmd,
			    NULL,
			    NULL,
			    NULL,
			    &error);

  error = NULL;
  g_spawn_command_line_sync(local_share_dirs_mkdir_cmd,
			    NULL,
			    NULL,
			    NULL,
			    &error);

  error = NULL;
  g_spawn_command_line_sync(local_state_dirs_mkdir_cmd,
			    NULL,
			    NULL,
			    NULL,
			    &error);

  error = NULL;
  g_spawn_command_line_sync(cache_dirs_mkdir_cmd,
			    NULL,
			    NULL,
			    NULL,
			    &error);

  error = NULL;
  g_spawn_command_line_sync(config_dirs_mkdir_cmd,
			    NULL,
			    NULL,
			    NULL,
			    &error);
  
  error = NULL;
  g_spawn_command_line_sync(default_file_cp_cmd,
			    NULL,
			    NULL,
			    NULL,
			    &error);
  
  error = NULL;
  g_spawn_command_line_sync(default_file_sed_cmd,
			    NULL,
			    NULL,
			    NULL,
			    &error);

#if 1
  error = NULL;
  g_spawn_command_line_sync(free_sounds_cp_cmd,
			    NULL,
			    NULL,
			    NULL,
			    &error);
#endif
  
  error = NULL;
  g_spawn_command_line_sync(ags_conf_dirs_mkdir_cmd,
			    NULL,
			    NULL,
			    NULL,
			    &error);
  
  error = NULL;
  g_spawn_command_line_sync(ags_conf_cp_cmd,
			    NULL,
			    NULL,
			    NULL,
			    &error);
#endif
}

int
main(int argc, char **argv)
{  
  GtkSettings *settings;
  GtkCssProvider *css_provider;
  AgsGSequencerApplication *gsequencer_app;
  
  AgsApplicationContext *application_context;  
  AgsConfig *config;
  AgsPriority *priority;
  
#ifdef AGS_WITH_RT
  struct sched_param param;
  struct rlimit rl;
#endif

#ifndef AGS_W32API
  struct passwd *pw;

  uid_t uid;
#endif
  
  char **gst_argv;  
  gchar **iter;

  gchar *application_id;
  gchar *filename;
#if defined(AGS_W32API) || defined(AGS_OSXAPI)
  gchar *app_dir;
  gchar *path;
#endif

  gchar *wdir;
  gchar *config_filename;
  gchar *priority_filename;
  gchar *css_filename;
  gchar *str;
  
  int gst_argc;
  gboolean has_file;
  gboolean no_config;
  int result;

  gboolean builtin_theme_disabled;
  gboolean handles_command_line;
  gboolean non_unique;
  gboolean is_remote;
  gboolean force_menu_bar;
  
  guint i;

  GError *error;  

#ifdef AGS_WITH_RT
  const rlim_t kStackSize = 64L * 1024L * 1024L;   // min stack size = 64 Mb
#endif

#if 0
  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, LOCALEDIR);
  textdomain(PACKAGE);
#else
  putenv("LANG=C.UTF-8");
  putenv("LC_ALL=C.UTF-8");
#endif
  
  builtin_theme_disabled = FALSE;

//  mtrace();

  /* env */
#if defined(AGS_W32_EXE_ENV)
  app_dir = NULL;

  if(strlen(argv[0]) > strlen("\\gsequencer.exe")){
    app_dir = g_strndup(argv[0],
			strlen(argv[0]) - strlen("\\gsequencer.exe"));
  }

  putenv(g_strdup_printf("GIO_MODULE_DIR=%s\\lib\\gio\\modules", app_dir));

  putenv(g_strdup_printf("GSETTINGS_SCHEMA_DIR=%s\\share\\glib-2.0\\schemas", app_dir));  

  putenv(g_strdup_printf("XDG_DATA_DIRS=%s\\share", app_dir));
  putenv(g_strdup_printf("XDG_CONFIG_HOME=%s\\etc", app_dir));

  putenv(g_strdup_printf("GDK_PIXBUF_MODULE_FILE=%s\\lib\\gdk-pixbuf-2.0\\2.10.0\\loaders.cache", app_dir));

  putenv(g_strdup_printf("GTK_EXE_PREFIX=%s", app_dir));
  putenv(g_strdup_printf("GTK_DATA_PREFIX=%s\\share", app_dir));
  putenv(g_strdup_printf("GTK_PATH=%s", app_dir));
  putenv(g_strdup_printf("GTK_IM_MODULE_FILE=%s\\lib\\gtk-3.0\\3.0.0\\immodules.cache", app_dir));

  putenv(g_strdup_printf("GST_PLUGIN_SYSTEM_PATH=%s\\lib\\gstreamer-1.0", app_dir));
  putenv(g_strdup_printf("GST_PLUGIN_SCANNER=%s\\libexec\\gstreamer-1.0\\gst-plugin-scanner.exe", app_dir));

  putenv(g_strdup_printf("GST_PLUGIN_SYSTEM_PATH=%s\\lib\\gstreamer-1.0", app_dir));

  putenv(g_strdup_printf("AGS_ANIMATION_FILENAME=%s\\share\\gsequencer\\images\\gsequencer-800x450.png", app_dir));
  
  putenv(g_strdup_printf("AGS_LOGO_FILENAME=%s\\share\\gsequencer\\images\\ags.png", app_dir));
  
  putenv(g_strdup_printf("AGS_LICENSE_FILENAME=%s\\COPYING", app_dir));
  
  putenv(g_strdup_printf("AGS_ONLINE_HELP_PDF_FILENAME=%s\\share\\doc\\gsequencer\\pdf\\ags-user-manual.pdf", app_dir));
#endif

#if defined(AGS_OSX_DMG_ENV)
  uid = getuid();
  pw = getpwuid(uid);

  app_dir = [[NSBundle mainBundle] bundlePath].UTF8String;
  
  putenv(g_strdup_printf("GIO_MODULE_DIR=%s/Contents/Resources/lib/gio/modules", app_dir));

  putenv(g_strdup_printf("GSETTINGS_SCHEMA_DIR=%s/Contents/Resources/share/glib-2.0/schemas", app_dir));  
  
#if !defined(AGS_MACOS_SANDBOX)
  putenv(g_strdup_printf("XDG_RUNTIME_DIR=%s/Contents/Resources/var/run", app_dir));

  putenv(g_strdup_printf("XDG_CACHE_HOME=%s/Contents/Resources/var/cache", app_dir));

  putenv(g_strdup_printf("XDG_DATA_HOME=%s/Contents/Resources/share", app_dir));
  putenv(g_strdup_printf("XDG_CONFIG_HOME=%s/Contents/Resources/etc", app_dir));
  putenv(g_strdup_printf("XDG_STATE_HOME=%s/Contents/Resources/var", app_dir));
  
  putenv(g_strdup_printf("XDG_DATA_DIRS=%s/Contents/Resources/share", app_dir));
  putenv(g_strdup_printf("XDG_CONFIG_DIRS=%s/Contents/Resources/etc", app_dir));
#else
  putenv(g_strdup_printf("XDG_RUNTIME_DIR=%s/Library/Containers/%s/Data/var/run", pw->pw_dir, AGS_DEFAULT_BUNDLE_ID));

  putenv(g_strdup_printf("XDG_CACHE_HOME=%s/Library/Containers/%s/Data/.cache", pw->pw_dir, AGS_DEFAULT_BUNDLE_ID));

  putenv(g_strdup_printf("XDG_DATA_HOME=%s/Library/Containers/%s/Data/.local/share", pw->pw_dir, AGS_DEFAULT_BUNDLE_ID));
  putenv(g_strdup_printf("XDG_CONFIG_HOME=%s/Library/Containers/%s/Data/.config", pw->pw_dir, AGS_DEFAULT_BUNDLE_ID));
  putenv(g_strdup_printf("XDG_STATE_HOME=%s/Library/Containers/%s/Data/.local/state", pw->pw_dir, AGS_DEFAULT_BUNDLE_ID));
  
  putenv(g_strdup_printf("XDG_DATA_DIRS=%s/Contents/Resources/share", app_dir));
  putenv(g_strdup_printf("XDG_CONFIG_DIRS=%s/Contents/Resources/etc", app_dir));
#endif
  
  putenv(g_strdup_printf("GDK_PIXBUF_MODULE_FILE=%s/Contents/Resources/lib/gdk-pixbuf-2.0/2.10.0/loaders.cache", app_dir));

  putenv(g_strdup_printf("GTK_EXE_PREFIX=%s/Contents/Resources", app_dir));
  putenv(g_strdup_printf("GTK_DATA_PREFIX=%s/Contents/Resources/share", app_dir));
  putenv(g_strdup_printf("GTK_PATH=%s/Contents/Resources", app_dir));
  putenv(g_strdup_printf("GTK_IM_MODULE_FILE=%s/Contents/Resourcess/lib/gtk-3.0/3.0.0/immodules.cache", app_dir));

  putenv(g_strdup_printf("GST_PLUGIN_SYSTEM_PATH=%s/lib/gstreamer-1.0", app_dir));
  putenv(g_strdup_printf("GST_PLUGIN_SCANNER=%s/libexec/gstreamer-1.0/gst-plugin-scanner", app_dir));

  putenv(g_strdup_printf("AGS_ANIMATION_FILENAME=%s/Contents/Resources/gsequencer-800x450.png", app_dir));
  
  putenv(g_strdup_printf("AGS_LOGO_FILENAME=%s/Contents/Resources/ags.png", app_dir));
  
  putenv(g_strdup_printf("AGS_LICENSE_FILENAME=%s/Contents/Resources/GPL-3", app_dir));
  
  putenv(g_strdup_printf("AGS_ONLINE_HELP_PDF_FILENAME=%s/Contents/Resources/share/doc/gsequencer/pdf/ags-user-manual.pdf", app_dir));
  
  putenv(g_strdup_printf("AGS_CSS_FILENAME=%s/Contents/Resources/ags.css", app_dir));
  
  if((getenv("VST3_PATH")) == NULL){
    putenv(g_strdup_printf("VST3_PATH=/Library/Audio/Plug-Ins/VST3:%s/Library/Audio/Plug-Ins/VST3:%s/Contents/Plugins/vst3", pw->pw_dir, app_dir));
  }
  
  if((getenv("LADSPA_PATH")) == NULL){  
    putenv(g_strdup_printf("LADSPA_PATH=%s/Contents/Plugins/ladspa", app_dir));
  }

  if((getenv("DSSI_PATH")) == NULL){
    putenv(g_strdup_printf("DSSI_PATH=%s/Contents/Plugins/dssi", app_dir));
  }
  
  if((getenv("LV2_PATH")) == NULL){
    putenv(g_strdup_printf("LV2_PATH=%s/Contents/Plugins/lv2", app_dir));
  }
#endif

#if !defined(AGS_W32_EXE_ENV) && !defined(AGS_OSX_DMG_ENV)
  uid = getuid();
  pw = getpwuid(uid);

  wdir = g_strdup_printf("%s/%s",
			 pw->pw_dir,
			 AGS_DEFAULT_DIRECTORY);
  
  priority_filename = g_strdup_printf("%s/priority.conf",
				      wdir);

  priority = ags_priority_get_instance();

  ags_priority_load_from_file(priority,
			      priority_filename);

  g_free(priority_filename);
  g_free(wdir);
#endif

  /* init */
  LIBXML_TEST_VERSION;
  xmlInitParser();
  
  //ao_initialize();

#if defined(AGS_WITH_GSTREAMER)
  gst_argc = 1;
  gst_argv = (char **) g_malloc(2 * sizeof(char *));

  gst_argv[0] = g_strdup(argv[0]);
  gst_argv[1] = NULL;
    
  gst_init(&gst_argc, &gst_argv);
#endif
  
  //  gdk_threads_enter();
  //  g_thread_init(NULL);
  gtk_init();

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

#if defined(AGS_OSX_DMG_ENV)
  if(getenv("GTK_THEME") == NULL){
    NSAppearance *appearance = NSApp.mainWindow.effectiveAppearance;
    NSString *interface_style = appearance.name;
    
    gchar *theme;

    gboolean dark_mode;

    theme = "Adwaita";

    dark_mode = FALSE;

    g_object_get(settings,
		 "gtk-application-prefer-dark-theme", &dark_mode,
		 NULL);
    
    if([interface_style isEqualToString:NSAppearanceNameDarkAqua]){
      theme = "Adwaita:dark";

      dark_mode = TRUE;
    }else if([interface_style isEqualToString:NSAppearanceNameAccessibilityHighContrastAqua]){
      theme = "HighContrast";
    }else if([interface_style isEqualToString:NSAppearanceNameAccessibilityHighContrastDarkAqua]){
      theme = "HighContrast:dark";

      dark_mode = TRUE;
    }

    g_object_set(settings,
		 "gtk-theme-name", theme,
		 "gtk-application-prefer-dark-theme", dark_mode,
		 NULL);
 }
#endif
  
  config = NULL;
  
  priority = ags_priority_get_instance();  
  ags_priority_load_defaults(priority);
  
  /* real-time setup */
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

  param.sched_priority = 15;

  str = ags_priority_get_value(priority,
			       AGS_PRIORITY_RT_THREAD,
			       AGS_PRIORITY_KEY_GUI_MAIN_LOOP);

  if(str != NULL){
    param.sched_priority = (int) g_ascii_strtoull(str,
						  NULL,
						  10);
  }

  if(str == NULL ||
     ((!g_ascii_strncasecmp(str,
			    "0",
			    2)) != TRUE)){
    if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
      perror("sched_setscheduler failed");
    }
  }

  g_free(str);
#endif

  /* parse command line parameter */
  filename = NULL;

  has_file = FALSE;
  handles_command_line = FALSE;
  force_menu_bar = FALSE;
  
  non_unique = FALSE;
  no_config = FALSE;
  
  for(i = 0; i < argc; i++){
    if(!strncmp(argv[i], "--help", 7)){
      printf("GSequencer is an audio sequencer and notation editor\n\n");

      printf("Usage:\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n\n",
	     "Report bugs to <jkraehemann@gmail.com>\n",
	     "--filename file     open file",
	     "--no-config         disable config from file",
	     "--no-builtin-theme  disable built-in theme",
	     "--non-unique        don't attach to any existing application ID",
	     "--menu-bar          force traditional menu bar",
	     "--help              display this help and exit",
	     "--version           output version information and exit");
      
      exit(0);
    }else if(!strncmp(argv[i], "--version", 10)){
      printf("GSequencer %s\n\n", AGS_VERSION);
      
      printf("%s\n%s\n%s\n\n",
	     "Copyright (C) 2005-2023 Joël Krähemann",
	     "This is free software; see the source for copying conditions.  There is NO",
	     "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.");
      
      printf("Written by Joël Krähemann\n");
      
      exit(0);
    }else if(!strncmp(argv[i], "--no-builtin-theme", 19)){
      builtin_theme_disabled = TRUE;
    }else if(!strncmp(argv[i], "--non-unique", 13)){
      non_unique = TRUE;
    }else if(!strncmp(argv[i], "--menu-bar", 11)){
      force_menu_bar = TRUE;
    }else if(!strncmp(argv[i], "--filename", 11)){
      if(i + 1 < argc){
	if(strlen(argv[i + 1]) > 2 && argv[i + 1][0] == '\''){
	  filename = g_strndup(argv[i + 1] + 1,
			       strlen(argv[i + 1]) - 2);
	}else if(strlen(argv[i + 1]) > 2 && argv[i + 1][0] == '"'){
	  filename = g_strndup(argv[i + 1] + 1,
			       strlen(argv[i + 1]) - 2);
	}else{
	  filename = g_strdup(argv[i + 1]);
	}
      
	i++;
      }
      
      if(g_file_test(filename,
		     G_FILE_TEST_EXISTS) &&
	 g_file_test(filename,
		     G_FILE_TEST_IS_REGULAR)){
	handles_command_line = TRUE;
	
	has_file = TRUE;
      }
    }else if(!strncmp(argv[i], "--no-config", 12)){
      no_config = TRUE;
      i++;
    }
  }

  css_filename = NULL;
  
#ifdef AGS_W32API
  if(!builtin_theme_disabled){
    app_dir = NULL;
    
    if(strlen(argv[0]) > strlen("\\gsequencer.exe")){
      app_dir = g_strndup(argv[0],
			  strlen(argv[0]) - strlen("\\gsequencer.exe"));
    }
    
    if((css_filename = getenv("AGS_CSS_FILENAME")) == NULL){
      css_filename = g_strdup_printf("%s\\share\\gsequencer\\styles\\ags.css",
				    g_get_current_dir());
    
      if(!g_file_test(css_filename,
		      G_FILE_TEST_IS_REGULAR)){
	g_free(css_filename);

	if(g_path_is_absolute(app_dir)){
	  css_filename = g_strdup_printf("%s\\%s",
					app_dir,
					"\\share\\gsequencer\\styles\\ags.css");
	}else{
	  css_filename = g_strdup_printf("%s\\%s\\%s",
					g_get_current_dir(),
					app_dir,
					"\\share\\gsequencer\\styles\\ags.css");
	}
      }
    }else{
      css_filename = g_strdup(css_filename);
    }
  
    g_free(app_dir);
  }
#else
  uid = getuid();
  pw = getpwuid(uid);
  
  /* parse rc file */
  if(!builtin_theme_disabled){
    css_filename = g_strdup_printf("%s/%s/ags.css",
				   pw->pw_dir,
				   AGS_DEFAULT_DIRECTORY);

    if(!g_file_test(css_filename,
		    G_FILE_TEST_IS_REGULAR)){
      g_free(css_filename);

#ifdef AGS_CSS_FILENAME
      css_filename = g_strdup(AGS_CSS_FILENAME);
#else
      if((css_filename = getenv("AGS_CSS_FILENAME")) == NULL){
	css_filename = g_strdup_printf("%s%s",
				       AGS_DATA_DIR,
				       "/gsequencer/styles/ags.css");
      }else{
	css_filename = g_strdup(css_filename);
      }
#endif
    }
  }
#endif
  
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
  
  css_provider = gtk_css_provider_new();
  gtk_css_provider_load_from_path(css_provider,
				  css_filename);
  gtk_style_context_add_provider_for_display(gdk_display_get_default(),
					     GTK_STYLE_PROVIDER(css_provider),
					     GTK_STYLE_PROVIDER_PRIORITY_USER);    
    
  g_free(css_filename);

  config = ags_config_get_instance();

  if((str = getenv("AGS_CONFIG")) != NULL){
    ags_config_load_from_data(config,
			      str, strlen(str));      
  }
  
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
    
  config_filename = g_strdup_printf("%s\\%s",
				    path,
				    AGS_DEFAULT_CONFIG);

  g_free(path);
#else
#if !defined(AGS_MACOS_SANDBOX)
  wdir = g_strdup_printf("%s/%s",
			 pw->pw_dir,
			 AGS_DEFAULT_DIRECTORY);
#else
  wdir = g_strdup_printf("%s/Library/Containers/%s/Data/%s",
			 pw->pw_dir,
			 AGS_DEFAULT_BUNDLE_ID,
			 AGS_DEFAULT_DIRECTORY);
#endif
  
  config_filename = g_strdup_printf("%s/%s",
				    wdir,
				    AGS_DEFAULT_CONFIG);

  g_free(wdir);
#endif

    if((str = getenv("AGS_CONFIG")) != NULL){
    }else{
      ags_config_load_from_file(config,
				config_filename);
    }
    
    g_free(config_filename);
  }

  /* some GUI scaling */
  if(!builtin_theme_disabled &&
     !has_file){
//    ags_gsequencer_application_context_load_gui_scale(ags_application_context_get_instance());
  }
    
#if !defined(AGS_MACOS_SANDBOX)
  application_id = "org.nongnu.gsequencer.gsequencer";
#else
  application_id = "com.gsequencer.GSequencer";
#endif
  
  if(non_unique){
    gsequencer_app = ags_gsequencer_application_new(application_id,
						    (GApplicationFlags) (G_APPLICATION_HANDLES_OPEN |
									 G_APPLICATION_NON_UNIQUE));
  }else{
    gsequencer_app = ags_gsequencer_application_new(application_id,
						    (GApplicationFlags) G_APPLICATION_HANDLES_OPEN);
  }
  
  error = NULL;
  g_application_register(G_APPLICATION(gsequencer_app),
			 NULL,
			 &error);
    
  if(error != NULL){
    g_warning("%s", error->message);
  }
  
  /* application context */
  is_remote = FALSE;

  application_context = ags_application_context_get_instance();
  
  if(g_application_get_is_remote(G_APPLICATION(gsequencer_app))){    
    is_remote = TRUE;
  }else{
    GtkWidget *window;

    application_context->argc = argc;
    application_context->argv = argv;

    /* application context prepare and setup */  
    ags_application_context_prepare(application_context);
    ags_application_context_setup(application_context);
  
    window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

    if(!AGS_WINDOW(window)->shows_menu_bar &&
       !force_menu_bar){
      gtk_window_set_titlebar((GtkWindow *) window,
			      (GtkWidget *) AGS_WINDOW(window)->header_bar);
    }

    gtk_application_add_window(GTK_APPLICATION(gsequencer_app),
			       GTK_WINDOW(window));

    g_object_set(G_OBJECT(window),
		 "application", gsequencer_app,
		 NULL);

    ags_connectable_connect(AGS_CONNECTABLE(application_context));

#if defined(AGS_OSXAPI)
    if(window != NULL &&
       filename == NULL){
      GFile *file;
      
      GFileOutputStream *output_stream;
      GInputStream *input_stream;
      
      GString *str;

      gchar *default_path;
      gchar *default_filename;

      gchar *home_env;
      gchar *music_path;

      GError *error;

      uid = getuid();
      pw = getpwuid(uid);

      music_path = g_strdup_printf("%s/Music",
				   pw->pw_dir);

      //TODO:JK: remove
      //      music_path = [[NSSearchPathForDirectoriesInDomains(NSMusicDirectory, NSUserDomainMask, YES) objectAtIndex:0] UTF8String];

      default_path = g_strdup_printf("%s%s",
				     music_path,
				     "/GSequencer/workspace/default");

      g_message("check directory ~/Music/GSequencer/workspace/default [%s]", default_path);
    
      if(!g_file_test(default_path, G_FILE_TEST_IS_DIR)){
	g_mkdir_with_parents(default_path,
			     0755);
      }
    
      default_filename = g_strdup_printf("%s%s",
					 default_path,
					 "/gsequencer-default.xml");

      g_message("check default file ~/Music/GSequencer/workspace/default/gsequencer-default.xml [%s]", default_filename);

      if(!g_file_test(default_filename, G_FILE_TEST_EXISTS)){
	gchar *macos_install_cmd;
	
	app_dir = [[NSBundle mainBundle] bundlePath].UTF8String;

#if 0
	macos_install_cmd = g_strdup_printf("%s/Contents/MacOS/gsequencer_macos_install",
					    app_dir);

	error = NULL;
	g_spawn_command_line_sync(macos_install_cmd,
				  NULL,
				  NULL,
				  NULL,
				  &error);
#else
	install_data();
#endif
      }
      
      AGS_WINDOW(window)->queued_filename = default_filename;

      g_free(default_path);
    }
#endif
  }
  
  if(handles_command_line && filename != NULL){      
    if(is_remote){
      GFile* file[2];

      g_message("open %s", filename);

      file[0] = g_file_new_for_path(filename);
      file[1] = NULL;
	
      g_application_open(G_APPLICATION(gsequencer_app),
			 file,
			 1,
			 "local command line");
    }
  }
    
  g_application_run(G_APPLICATION(gsequencer_app),
		    0, NULL);


  g_free(filename);
  
  //  muntrace();

  return(0);
}
