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

#include <ags/app/ags_app_action_util.h>

#include <ags/ags_api_config.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_gsequencer_application.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_composite_editor.h>
#include <ags/app/ags_export_window.h>
#include <ags/app/ags_meta_data_window.h>
#include <ags/app/ags_preferences.h>
#include <ags/app/ags_online_help_window.h>
#include <ags/app/ags_quit_dialog.h>
#include <ags/app/ags_machine_util.h>

#include <ags/app/editor/ags_composite_edit.h>
#include <ags/app/editor/ags_composite_edit_callbacks.h>
#include <ags/app/editor/ags_notation_edit.h>
#include <ags/app/editor/ags_automation_edit.h>
#include <ags/app/editor/ags_scrolled_wave_edit_box.h>
#include <ags/app/editor/ags_wave_edit_box.h>
#include <ags/app/editor/ags_wave_edit.h>
#include <ags/app/editor/ags_sheet_edit.h>
#include <ags/app/editor/ags_tempo_edit.h>

#include <ags/app/import/ags_midi_import_wizard.h>

#include <ags/app/export/ags_midi_export_wizard.h>

#include <ags/app/machine/ags_panel.h>
#include <ags/app/machine/ags_mixer.h>
#include <ags/app/machine/ags_spectrometer.h>
#include <ags/app/machine/ags_equalizer10.h>
#include <ags/app/machine/ags_drum.h>
#include <ags/app/machine/ags_matrix.h>
#include <ags/app/machine/ags_synth.h>
#include <ags/app/machine/ags_fm_synth.h>
#include <ags/app/machine/ags_syncsynth.h>
#include <ags/app/machine/ags_fm_syncsynth.h>
#include <ags/app/machine/ags_hybrid_synth.h>
#include <ags/app/machine/ags_hybrid_fm_synth.h>

#if defined(AGS_WITH_LIBINSTPATCH)
#include <ags/app/machine/ags_ffplayer.h>
#include <ags/app/machine/ags_sf2_synth.h>
#endif

#include <ags/app/machine/ags_pitch_sampler.h>
#include <ags/app/machine/ags_sfz_synth.h>

#include <ags/app/machine/ags_audiorec.h>
#include <ags/app/machine/ags_desk.h>

#include <ags/app/machine/ags_ladspa_bridge.h>
#include <ags/app/machine/ags_dssi_bridge.h>
#include <ags/app/machine/ags_lv2_bridge.h>
#include <ags/app/machine/ags_live_dssi_bridge.h>
#include <ags/app/machine/ags_live_lv2_bridge.h>

#if defined(AGS_WITH_VST3)
#include <ags/app/machine/ags_vst3_bridge.h>
#include <ags/app/machine/ags_live_vst3_bridge.h>
#endif

#include <ags/app/file/ags_simple_file.h>

#include <gdk/gdk.h>

#if defined(AGS_W32API)
#include <windows.h>
#include <tchar.h>
#endif

#if defined(AGS_OSX_DMG_ENV)
#include <Cocoa/Cocoa.h>
#include <Foundation/Foundation.h>
#endif

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>

#include <ags/i18n.h>

#define _GNU_SOURCE
#include <locale.h>

static GMutex locale_mutex;

#if defined(AGS_OSXAPI) || defined(AGS_W32API)
static char *locale_env;
#else
static locale_t c_utf8_locale;
#endif

static gboolean locale_initialized = FALSE;

void ags_app_action_util_open_response_callback(AgsFileDialog *file_dialog,
						gint response,
						gpointer data);

void ags_app_action_util_save_as_response_callback(AgsFileDialog *file_dialog,
						   gint response,
						   gpointer data);

#if defined(AGS_WITH_VST3)
void ags_app_action_util_add_vst3_bridge_add_audio_callback(AgsTask *task,
							    AgsVst3Bridge *vst3_bridge);

void ags_app_action_util_add_live_vst3_bridge_add_audio_callback(AgsTask *task,
								 AgsLiveVst3Bridge *live_vst3_bridge);
#endif

void
ags_app_action_util_open()
{
  AgsWindow *window;
  AgsFileDialog *file_dialog;
  AgsFileWidget *file_widget;
  
  AgsApplicationContext *application_context;

  gchar *recently_used_filename;
  gchar *bookmark_filename;
  gchar *home_path;
  gchar *sandbox_path;
  gchar *current_path;
  gchar *str;

  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  file_dialog = (AgsFileDialog *) ags_file_dialog_new((GtkWidget *) window,
						      i18n("open file"));

  file_widget = ags_file_dialog_get_file_widget(file_dialog);

  home_path = ags_file_widget_get_home_path(file_widget);

  sandbox_path = NULL;
  
#if defined(AGS_MACOS_SANDBOX)
  sandbox_path = g_strdup_printf("%s/Library/Containers/%s/Data",
  				 home_path,
				 AGS_DEFAULT_BUNDLE_ID);

  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_app_recently_used.xml",
					   sandbox_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_app_bookmark.xml",
				      sandbox_path,
				      AGS_DEFAULT_DIRECTORY);
#endif

#if defined(AGS_FLATPAK_SANDBOX)
  if((str = getenv("HOME")) != NULL){
    sandbox_path = g_strdup_printf("%s",
				   str);
  }

  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_app_recently_used.xml",
					   sandbox_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_app_bookmark.xml",
				      sandbox_path,
				      AGS_DEFAULT_DIRECTORY);
#endif

#if defined(AGS_SNAP_SANDBOX)
  if((str = getenv("SNAP_USER_DATA")) != NULL){
    sandbox_path = g_strdup_printf("%s",
				   str);
  }

  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_app_recently_used.xml",
					   sandbox_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_app_bookmark.xml",
				      sandbox_path,
				      AGS_DEFAULT_DIRECTORY);
#endif
  
#if !defined(AGS_MACOS_SANDBOX) && !defined(AGS_FLATPAK_SANDBOX) && !defined(AGS_SNAP_SANDBOX)
  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_app_recently_used.xml",
					   home_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_app_bookmark.xml",
				      home_path,
				      AGS_DEFAULT_DIRECTORY);
#endif

  /* recently-used */
  ags_file_widget_set_recently_used_filename(file_widget,
					     recently_used_filename);
  
  ags_file_widget_read_recently_used(file_widget);

  /* bookmark */
  ags_file_widget_set_bookmark_filename(file_widget,
					bookmark_filename);

  ags_file_widget_read_bookmark(file_widget);

  /* current path */
  current_path = NULL;
  
#if defined(AGS_MACOS_SANDBOX)
  current_path = g_strdup(home_path);
#endif

#if defined(AGS_FLATPAK_SANDBOX)
  ags_file_widget_set_flags(file_widget,
			    AGS_FILE_WIDGET_APP_SANDBOX);

  current_path = g_strdup(sandbox_path);
#endif

#if defined(AGS_SNAP_SANDBOX)
  ags_file_widget_set_flags(file_widget,
			    AGS_FILE_WIDGET_APP_SANDBOX);

  current_path = g_strdup(sandbox_path);
#endif
  
#if !defined(AGS_MACOS_SANDBOX) && !defined(AGS_FLATPAK_SANDBOX) && !defined(AGS_SNAP_SANDBOX)
  current_path = g_strdup(home_path);
#endif

  if(window->filename != NULL){
    g_free(current_path);

    current_path = g_path_get_dirname(window->filename);
  }
  
  ags_file_widget_set_current_path(file_widget,
				   current_path);

  g_free(current_path);

  ags_file_widget_refresh(file_widget);
  
  ags_file_widget_add_location(file_widget,
			       AGS_FILE_WIDGET_LOCATION_OPEN_USER_DESKTOP,
			       NULL);

  ags_file_widget_add_location(file_widget,
			       AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_DOCUMENTS,
			       NULL);  

  ags_file_widget_add_location(file_widget,
			       AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_MUSIC,
			       NULL);

  ags_file_widget_add_location(file_widget,
			       AGS_FILE_WIDGET_LOCATION_OPEN_USER_HOME,
			       NULL);

  ags_file_widget_set_file_action(file_widget,
				  AGS_FILE_WIDGET_OPEN);

  ags_file_widget_set_default_bundle(file_widget,
				     AGS_DEFAULT_BUNDLE_ID);
  
  gtk_widget_set_visible((GtkWidget *) file_dialog,
			 TRUE);

  //  gtk_widget_set_size_request(GTK_WIDGET(file_dialog),
  //			      AGS_UI_PROVIDER_DEFAULT_OPEN_DIALOG_WIDTH, AGS_UI_PROVIDER_DEFAULT_OPEN_DIALOG_HEIGHT);

  g_signal_connect((GObject *) file_dialog, "response",
		   G_CALLBACK(ags_app_action_util_open_response_callback), NULL);
}

void
ags_app_action_util_open_response_callback(AgsFileDialog *file_dialog,
					   gint response,
					   gpointer data)
{
  if(response == GTK_RESPONSE_ACCEPT){
    AgsWindow *window;
    AgsFileWidget *file_widget;
    
    AgsApplicationContext *application_context;
    
    char *filename;
    gchar *str;
#if defined(AGS_W32API)
    gchar *app_dir;
#elif defined(AGS_OSXAPI)
    gchar *app_dir;
    gchar *application_id;
#endif

    gint strv_length;
    
    GError *error;

    application_context = ags_application_context_get_instance();

    window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

    file_widget = ags_file_dialog_get_file_widget(file_dialog);

    filename = ags_file_widget_get_filename(file_widget);

    if(!g_strv_contains((const gchar * const *) file_widget->recently_used, filename)){
      strv_length = g_strv_length(file_widget->recently_used);

      file_widget->recently_used = g_realloc(file_widget->recently_used,
					     (strv_length + 2) * sizeof(gchar *));

      file_widget->recently_used[strv_length] = g_strdup(filename);
      file_widget->recently_used[strv_length + 1] = NULL; 
    
      ags_file_widget_write_recently_used(file_widget);
    }
    
    error = NULL;

#if defined(AGS_W32API)
    app_dir = g_path_get_dirname(application_context->argv[0]);

    if(g_path_is_absolute(app_dir)){
      str = g_strdup_printf("%s --filename %s",
			    application_context->argv[0],
			    filename);
    }else{
      str = g_strdup_printf("%s\\%s --filename %s",
			    g_get_current_dir(),
			    application_context->argv[0],
			    filename);
    }
    
    g_free(app_dir);

    {
      STARTUPINFO si;
      PROCESS_INFORMATION pi;

      ZeroMemory(&si, sizeof(si));
      si.cb = sizeof(si);
      ZeroMemory(&pi, sizeof(pi));

      g_message("launch %s", str);
      
      CreateProcess(NULL,
		    str,
		    NULL,
		    NULL,
		    FALSE,
		    0,
		    NULL,
		    NULL,
		    &si,
		    &pi);
    }

    g_free(str);
#elif defined(AGS_OSXAPI)
#if !defined(AGS_MACOS_SANDBOX)
  application_id = "org.nongnu.gsequencer.gsequencer";
#else
  application_id = "com.gsequencer.GSequencer";
#endif

    //FIXME:JK: macos open project files sandbox work-around
#if 1
    GFile* file[2];
    
    g_message("open %s", filename);

    file[0] = g_file_new_for_path(filename);
    file[1] = NULL;
    
    g_application_open(G_APPLICATION(ags_ui_provider_get_app(AGS_UI_PROVIDER(application_context))),
		       file,
		       1,
		       "local command line");
#else
#if defined(AGS_OSX_DMG_ENV)
    app_dir = [[NSBundle mainBundle] bundlePath].UTF8String;

    str = g_strdup_printf("%s/Contents/MacOS/%s --filename '%s'",
			  app_dir,
			  application_id,
			  filename);
#else
    str = g_strdup_printf("%s --filename '%s'",
			  application_context->argv[0],
			  filename);
#endif
    
    g_spawn_command_line_async(str,
			       &error);

    if(error != NULL){
      g_message("%s", error->message);

      g_error_free(error);
    }    

    g_free(str);
#endif
#else
    str = g_strdup_printf("%s --filename '%s'",
			  application_context->argv[0],
			  filename);

    g_spawn_command_line_async(str,
			       &error);

    if(error != NULL){
      g_message("%s", error->message);

      g_error_free(error);
    }

    g_free(str);
#endif    
    
    g_free(filename);
  }

  gtk_window_destroy((GtkWindow *) file_dialog);
}

void
ags_app_action_util_save()
{
  AgsApplicationContext *application_context;
  AgsWindow *window;

  gchar *str;
  
  GError *error;

  application_context = ags_application_context_get_instance();
  
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  str = ags_config_get_value(AGS_APPLICATION_CONTEXT(application_context)->config,
			     AGS_CONFIG_GENERIC,
			     "simple-file");
  
  if((!g_strcmp0(str,
		 "false")) == FALSE){
    AgsSimpleFile *simple_file;

#if defined(AGS_OSXAPI) || defined(AGS_W32API)
#else
    locale_t current;
#endif

    g_mutex_lock(&locale_mutex);

    if(!locale_initialized){
#if defined(AGS_OSXAPI) || defined(AGS_W32API)
      locale_env = getenv("LC_ALL");
#else
      c_utf8_locale = newlocale(LC_ALL_MASK, "C.UTF-8", (locale_t) 0);
#endif
    
      locale_initialized = TRUE;
    }

    g_mutex_unlock(&locale_mutex);

#if defined(AGS_OSXAPI) || defined(AGS_W32API)
    setlocale(LC_ALL, "C.UTF-8");
#else
    current = uselocale(c_utf8_locale);
#endif

    simple_file = (AgsSimpleFile *) g_object_new(AGS_TYPE_SIMPLE_FILE,
						 "filename", window->name,
						 NULL);
      
    error = NULL;
    ags_simple_file_rw_open(simple_file,
			    TRUE,
			    &error);

    if(error != NULL){
      g_message("%s", error->message);

      g_error_free(error);
    }
    
    ags_simple_file_write(simple_file);
    ags_simple_file_close(simple_file);

    g_object_unref(G_OBJECT(simple_file));

#if defined(AGS_OSXAPI) || defined(AGS_W32API)
    setlocale(LC_ALL, locale_env);
#else
    uselocale(current);
#endif
  }else{
    AgsFile *file;

    file = (AgsFile *) g_object_new(AGS_TYPE_FILE,
				    "filename", window->name,
				    NULL);
      
    error = NULL;
    ags_file_rw_open(file,
		     TRUE,
		     &error);

    if(error != NULL){
      g_message("%s", error->message);

      g_error_free(error);
    }

    ags_file_write(file);
    ags_file_close(file);

    g_object_unref(G_OBJECT(file));
  }

  g_free(str);
}

void
ags_app_action_util_save_as_response_callback(AgsFileDialog *file_dialog,
					      gint response,
					      gpointer data)
{
  if(response == GTK_RESPONSE_ACCEPT){
    AgsWindow *window;
    GtkLabel *label;    
    AgsFileWidget *file_widget;

    AgsSimpleFile *simple_file;

    AgsApplicationContext *application_context;
    
#if defined(AGS_OSXAPI) || defined(AGS_W32API)
#else
    locale_t current;
#endif
    
    gchar *filename;
    gchar *window_title;

    gint strv_length;
    
    GError *error;
        
    application_context = ags_application_context_get_instance();

    window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
    
    file_widget = ags_file_dialog_get_file_widget(file_dialog);

    filename = ags_file_widget_get_current_path(file_widget);

    if(!g_strv_contains((const gchar * const *) file_widget->recently_used, filename)){
      strv_length = g_strv_length(file_widget->recently_used);

      file_widget->recently_used = g_realloc(file_widget->recently_used,
					     (strv_length + 2) * sizeof(gchar *));

      file_widget->recently_used[strv_length] = g_strdup(filename);
      file_widget->recently_used[strv_length + 1] = NULL; 
    
      ags_file_widget_write_recently_used(file_widget);
    }
    
    g_mutex_lock(&locale_mutex);

    if(!locale_initialized){
#if defined(AGS_OSXAPI) || defined(AGS_W32API)
      locale_env = getenv("LC_ALL");
#else
      c_utf8_locale = newlocale(LC_ALL_MASK, "C.UTF-8", (locale_t) 0);
#endif
    
      locale_initialized = TRUE;
    }

    g_mutex_unlock(&locale_mutex);

#if defined(AGS_OSXAPI) || defined(AGS_W32API)
    setlocale(LC_ALL, "C.UTF-8");
#else
    current = uselocale(c_utf8_locale);
#endif

    simple_file = (AgsSimpleFile *) g_object_new(AGS_TYPE_SIMPLE_FILE,
						 "filename", filename,
						 NULL);
      
    error = NULL;
    ags_simple_file_rw_open(simple_file,
			    TRUE,
			    &error);

    if(error != NULL){
      g_message("%s", error->message);

      g_error_free(error);
    }
      
    ags_simple_file_write(simple_file);
    ags_simple_file_close(simple_file);

    g_object_unref(G_OBJECT(simple_file));

#if defined(AGS_OSXAPI) || defined(AGS_W32API)
    setlocale(LC_ALL, locale_env);
#else
    uselocale(current);
#endif

    window->name = g_strdup(filename);

    window_title = g_strdup_printf("GSequencer - %s", window->name);
    gtk_window_set_title((GtkWindow *) window,
			 window_title);
    
    g_free(window_title);    

    label = GTK_LABEL(gtk_header_bar_get_title_widget(window->header_bar));

    window_title = g_strdup_printf("GSequencer\n<small>%s</small>", window->name);

    gtk_label_set_label(label,
			window_title);
    
    g_free(window_title);

    g_free(filename);
  }
  
  gtk_window_destroy((GtkWindow *) file_dialog);
}

void
ags_app_action_util_save_as()
{
  AgsWindow *window;
  AgsFileDialog *file_dialog;
  AgsFileWidget *file_widget;

  gchar *recently_used_filename;
  gchar *home_path;
  gchar *sandbox_path;
  gchar *current_path;
  gchar *str;
  
  AgsApplicationContext *application_context;
        
  application_context = ags_application_context_get_instance();
  
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  file_dialog = (AgsFileDialog *) ags_file_dialog_new((GtkWidget *) window,
						      i18n("save file as"));  

  file_widget = ags_file_dialog_get_file_widget(file_dialog);

  home_path = ags_file_widget_get_home_path(file_widget);

  sandbox_path = NULL;
  
#if defined(AGS_MACOS_SANDBOX)
  sandbox_path = g_strdup_printf("%s/Library/Containers/%s/Data",
				 home_path,
				 AGS_DEFAULT_BUNDLE_ID);

  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_app_recently_used.xml",
					   sandbox_path,
					   AGS_DEFAULT_DIRECTORY);
#endif

#if defined(AGS_FLATPAK_SANDBOX)
  if((str = getenv("HOME")) != NULL){
    sandbox_path = g_strdup_printf("%s",
				   str);
  }

  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_app_recently_used.xml",
					   sandbox_path,
					   AGS_DEFAULT_DIRECTORY);
#endif

#if defined(AGS_SNAP_SANDBOX)
  if((str = getenv("SNAP_USER_DATA")) != NULL){
    sandbox_path = g_strdup_printf("%s",
				   str);
  }

  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_app_recently_used.xml",
					   sandbox_path,
					   AGS_DEFAULT_DIRECTORY);
#endif
  
#if !defined(AGS_MACOS_SANDBOX) && !defined(AGS_FLATPAK_SANDBOX) && !defined(AGS_SNAP_SANDBOX)
  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_app_recently_used.xml",
					   home_path,
					   AGS_DEFAULT_DIRECTORY);
#endif

  /* recently-used */
  ags_file_widget_set_recently_used_filename(file_widget,
					     recently_used_filename);
  
  ags_file_widget_read_recently_used(file_widget);

  /* current path */
  current_path = NULL;
  
#if defined(AGS_MACOS_SANDBOX)
  current_path = g_strdup(home_path);
#endif

#if defined(AGS_FLATPAK_SANDBOX)
  ags_file_widget_set_flags(file_widget,
			    AGS_FILE_WIDGET_APP_SANDBOX);

  current_path = g_strdup(sandbox_path);
#endif

#if defined(AGS_SNAP_SANDBOX)
  ags_file_widget_set_flags(file_widget,
			    AGS_FILE_WIDGET_APP_SANDBOX);

  current_path = g_strdup(sandbox_path);
#endif
  
#if !defined(AGS_MACOS_SANDBOX) && !defined(AGS_FLATPAK_SANDBOX) && !defined(AGS_SNAP_SANDBOX)
  current_path = g_strdup(home_path);
#endif

  if(window->filename != NULL){
    g_free(current_path);

    current_path = g_path_get_dirname(window->filename);
  }

  ags_file_widget_set_current_path(file_widget,
				   current_path);

  g_free(current_path);

  ags_file_widget_refresh(file_widget);

  ags_file_widget_add_location(file_widget,
			       AGS_FILE_WIDGET_LOCATION_OPEN_USER_DESKTOP,
			       NULL);

  ags_file_widget_add_location(file_widget,
			       AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_DOCUMENTS,
			       NULL);  

  ags_file_widget_add_location(file_widget,
			       AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_MUSIC,
			       NULL);

  ags_file_widget_add_location(file_widget,
			       AGS_FILE_WIDGET_LOCATION_OPEN_USER_HOME,
			       NULL);

  ags_file_widget_set_file_action(file_widget,
				  AGS_FILE_WIDGET_SAVE_AS);

  ags_file_widget_set_default_bundle(file_widget,
				     AGS_DEFAULT_BUNDLE_ID);
  
  gtk_widget_set_visible((GtkWidget *) file_dialog,
			 TRUE);
  
  //  gtk_widget_set_size_request(GTK_WIDGET(file_dialog),
  //			      AGS_UI_PROVIDER_DEFAULT_OPEN_DIALOG_WIDTH, AGS_UI_PROVIDER_DEFAULT_OPEN_DIALOG_HEIGHT);

  g_signal_connect(file_dialog, "response",
		   G_CALLBACK(ags_app_action_util_save_as_response_callback), NULL);

}

void
ags_app_action_util_meta_data()
{
  AgsMetaDataWindow *meta_data_window;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  meta_data_window = (AgsMetaDataWindow *) ags_ui_provider_get_meta_data_window(AGS_UI_PROVIDER(application_context));
  gtk_widget_set_visible((GtkWidget *) meta_data_window,
			 TRUE);

  gtk_window_present((GtkWindow *) meta_data_window);

  ags_gsequencer_application_refresh_window_menu((AgsGSequencerApplication *) ags_ui_provider_get_app(AGS_UI_PROVIDER(application_context)));
}

void
ags_app_action_util_export()
{  
  AgsExportWindow *export_window;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  export_window = (AgsExportWindow *) ags_ui_provider_get_export_window(AGS_UI_PROVIDER(application_context));
  gtk_widget_set_visible((GtkWidget *) export_window,
			 TRUE);

  gtk_window_present((GtkWindow *) export_window);

  ags_gsequencer_application_refresh_window_menu((AgsGSequencerApplication *) ags_ui_provider_get_app(AGS_UI_PROVIDER(application_context)));
}

void
ags_app_action_util_smf_import()
{
  AgsMidiImportWizard *midi_import_wizard;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();
  
  midi_import_wizard = (AgsMidiImportWizard *) ags_ui_provider_get_midi_import_wizard(AGS_UI_PROVIDER(application_context));

  if(midi_import_wizard == NULL){
    midi_import_wizard = ags_midi_import_wizard_new();
    ags_ui_provider_set_midi_import_wizard(AGS_UI_PROVIDER(application_context),
					   (GtkWidget *) midi_import_wizard);

    ags_connectable_connect(AGS_CONNECTABLE(midi_import_wizard));
    ags_applicable_reset(AGS_APPLICABLE(midi_import_wizard));
  }

  gtk_widget_set_visible((GtkWidget *) midi_import_wizard,
			 TRUE);

  gtk_window_present((GtkWindow *) midi_import_wizard);

  ags_gsequencer_application_refresh_window_menu((AgsGSequencerApplication *) ags_ui_provider_get_app(AGS_UI_PROVIDER(application_context)));
}

void
ags_app_action_util_smf_export()
{
  AgsMidiExportWizard *midi_export_wizard;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();
  
  midi_export_wizard = (AgsMidiExportWizard *) ags_ui_provider_get_midi_export_wizard(AGS_UI_PROVIDER(application_context));

  if(midi_export_wizard == NULL){
    midi_export_wizard = ags_midi_export_wizard_new();
    ags_ui_provider_set_midi_export_wizard(AGS_UI_PROVIDER(application_context),
					   (GtkWidget *) midi_export_wizard);
    
    ags_connectable_connect(AGS_CONNECTABLE(midi_export_wizard));
    ags_applicable_reset(AGS_APPLICABLE(midi_export_wizard));
  }
  
  gtk_widget_set_visible((GtkWidget *) midi_export_wizard,
			 TRUE);
  
  gtk_window_present((GtkWindow *) midi_export_wizard);

  ags_gsequencer_application_refresh_window_menu((AgsGSequencerApplication *) ags_ui_provider_get_app(AGS_UI_PROVIDER(application_context)));
}

void
ags_app_action_util_preferences()
{
  AgsPreferences *preferences;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  preferences = (AgsPreferences *) ags_ui_provider_get_preferences(AGS_UI_PROVIDER(application_context));

  if(preferences == NULL){
    preferences = ags_preferences_new();
    ags_ui_provider_set_preferences(AGS_UI_PROVIDER(application_context),
				    (GtkWidget *) preferences);

    ags_connectable_connect(AGS_CONNECTABLE(preferences));
  
    ags_applicable_reset(AGS_APPLICABLE(preferences));
  }
  
  gtk_widget_set_visible((GtkWidget *) preferences,
			 TRUE);

  gtk_window_present((GtkWindow *) preferences);

  ags_gsequencer_application_refresh_window_menu((AgsGSequencerApplication *) ags_ui_provider_get_app(AGS_UI_PROVIDER(application_context)));
}

void
ags_app_action_util_about()
{
  AgsWindow *window;

  AgsApplicationContext *application_context;

  GdkTexture *texture;
  
  static FILE *file = NULL;
  struct stat sb;
  static gchar *license = NULL;

  gchar *license_filename;
  gchar *logo_filename;
#if defined(AGS_W32API)
  gchar *app_dir;
#endif
  
  int n_read;
  
  gchar *authors[] = { "Joël Krähemann", "Daniel Maksymow", NULL }; 

  license_filename = NULL;
  logo_filename = NULL;
  
#if defined AGS_W32API
  app_dir = NULL;
#endif
  
#ifdef AGS_LICENSE_FILENAME
  license_filename = g_strdup(AGS_LICENSE_FILENAME);
#else
  if((license_filename = getenv("AGS_LICENSE_FILENAME")) == NULL){
#if defined (AGS_W32API)
    application_context = ags_application_context_get_instance();

    if(strlen(application_context->argv[0]) > strlen("\\gsequencer.exe")){
      app_dir = g_strndup(application_context->argv[0],
			  strlen(application_context->argv[0]) - strlen("\\gsequencer.exe"));
    }
  
    license_filename = g_strdup_printf("%s\\share\\gsequencer\\license\\GPL-3",
				       g_get_current_dir());
    
    if(!g_file_test(license_filename,
		    G_FILE_TEST_IS_REGULAR)){
      g_free(license_filename);

      if(g_path_is_absolute(app_dir)){
	license_filename = g_strdup_printf("%s\\%s",
					   app_dir,
					   "share\\gsequencer\\license\\GPL-3");
      }else{
	license_filename = g_strdup_printf("%s\\%s\\%s",
					   g_get_current_dir(),
					   app_dir,
					   "share\\gsequencer\\license\\GPL-3");
      }
    }
#else
    license_filename = g_strdup("/usr/share/common-licenses/GPL-3");
#endif
  }else{
    license_filename = g_strdup(license_filename);
  }
#endif
  
  if(g_file_test(license_filename,
		 G_FILE_TEST_EXISTS)){
    if(file == NULL){
      file = fopen(license_filename, "r");

      if(file != NULL){
	stat(license_filename, &sb);
	license = (gchar *) malloc((sb.st_size + 1) * sizeof(gchar));

	n_read = fread(license, sizeof(char), sb.st_size, file);

	if(n_read != sb.st_size){
	  g_critical("fread() number of bytes returned doesn't match buffer size");
	}
      
	license[sb.st_size] = '\0';
	fclose(file);
      }
      
#ifdef AGS_LOGO_FILENAME
      logo_filename = g_strdup(AGS_LOGO_FILENAME);
#else
      if((logo_filename = getenv("AGS_LOGO_FILENAME")) == NULL){
#if defined AGS_W32API
	logo_filename = g_strdup_printf("%s\\share\\gsequencer\\images\\ags.png",
				      g_get_current_dir());
    
	if(!g_file_test(logo_filename,
			G_FILE_TEST_IS_REGULAR)){
	  g_free(logo_filename);

	  if(g_path_is_absolute(app_dir)){
	    logo_filename = g_strdup_printf("%s\\%s",
					    app_dir,
					    "share\\gsequencer\\images\\ags.png");
	  }else{
	    logo_filename = g_strdup_printf("%s\\%s\\%s",
					    g_get_current_dir(),
					    app_dir,
					    "share\\gsequencer\\images\\ags.png");
	  }
	}
#else
	logo_filename = g_strdup_printf("%s/gsequencer/images/ags.png",
					AGS_DATA_DIR);
#endif
      }else{
	logo_filename = g_strdup(logo_filename);
      }
#endif
    }
  }

  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  texture = gdk_texture_new_from_filename(logo_filename,
					  NULL);
  
  gtk_show_about_dialog((GtkWindow *) window,
			"program-name", "gsequencer",
			"authors", authors,
			"license", license,
			"version", AGS_VERSION,
			"website", "http://nongnu.org/gsequencer",
			"title", "Advanced Gtk+ Sequencer",
			"logo", texture,
			NULL);

  g_free(license_filename);

#if defined AGS_W32API
  g_free(app_dir);
#endif
}

void
ags_app_action_util_help()
{
  AgsWindow *window;

  AgsOnlineHelpWindow *online_help_window;
  
  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  online_help_window = (AgsOnlineHelpWindow *) ags_ui_provider_get_online_help_window(AGS_UI_PROVIDER(application_context));

  if(online_help_window == NULL){
    online_help_window = ags_online_help_window_new((GtkWindow *) window);
    
    ags_connectable_connect(AGS_CONNECTABLE(online_help_window));
    
    ags_ui_provider_set_online_help_window(AGS_UI_PROVIDER(application_context),
					   (GtkWidget *) online_help_window);
  }
  
  gtk_widget_set_visible((GtkWidget *) online_help_window,
			 TRUE);
  
  gtk_window_present((GtkWindow *) online_help_window);

  ags_gsequencer_application_refresh_window_menu((AgsGSequencerApplication *) ags_ui_provider_get_app(AGS_UI_PROVIDER(application_context)));
}

void
ags_app_action_util_quit()
{  
  AgsWindow *window;
  AgsQuitDialog *quit_dialog;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  quit_dialog = ags_quit_dialog_new((GtkWindow *) window);

  gtk_widget_set_visible((GtkWidget *) quit_dialog,
			 TRUE);
  
  gtk_widget_grab_focus((GtkWidget *) quit_dialog->cancel);
  
  ags_connectable_connect(AGS_CONNECTABLE(quit_dialog));
}

void
ags_app_action_util_add_panel()
{
  AgsPanel *panel;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();

  /* create panel */
  panel = (AgsPanel *) ags_machine_util_new_panel();
  
  add_audio = ags_add_audio_new(AGS_MACHINE(panel)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_app_action_util_add_spectrometer()
{
  AgsSpectrometer *spectrometer;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
    
  application_context = ags_application_context_get_instance();

  /* create spectrometer */
  spectrometer = (AgsSpectrometer *) ags_machine_util_new_spectrometer();

  add_audio = ags_add_audio_new(AGS_MACHINE(spectrometer)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_app_action_util_add_equalizer()
{
  AgsEqualizer10 *equalizer10;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
    
  application_context = ags_application_context_get_instance();

  /* create equalizer10 */
  equalizer10 = (AgsEqualizer10 *) ags_machine_util_new_equalizer();

  add_audio = ags_add_audio_new(AGS_MACHINE(equalizer10)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_app_action_util_add_mixer()
{
  AgsMixer *mixer;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();
  
  /* create mixer */
  mixer = (AgsMixer *) ags_machine_util_new_mixer();

  add_audio = ags_add_audio_new(AGS_MACHINE(mixer)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_app_action_util_add_drum()
{
  AgsDrum *drum;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
    
  application_context = ags_application_context_get_instance();

  /* create drum */
  drum = (AgsDrum *) ags_machine_util_new_drum();
  
  add_audio = ags_add_audio_new(AGS_MACHINE(drum)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_app_action_util_add_matrix()
{
  AgsMatrix *matrix;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  /* create matrix */
  matrix = (AgsMatrix *) ags_machine_util_new_matrix();

  add_audio = ags_add_audio_new(AGS_MACHINE(matrix)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_app_action_util_add_synth()
{
  AgsSynth *synth;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
    
  application_context = ags_application_context_get_instance();

  /* create synth */
  synth = (AgsSynth *) ags_machine_util_new_synth();

  add_audio = ags_add_audio_new(AGS_MACHINE(synth)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_app_action_util_add_fm_synth()
{
  AgsFMSynth *fm_synth;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
    
  application_context = ags_application_context_get_instance();

  /* create fm synth */
  fm_synth = (AgsFMSynth *) ags_machine_util_new_fm_synth();

  add_audio = ags_add_audio_new(AGS_MACHINE(fm_synth)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_app_action_util_add_syncsynth()
{
  AgsSyncsynth *syncsynth;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();

  /* create syncsynth */
  syncsynth = (AgsSyncsynth *) ags_machine_util_new_syncsynth();

  add_audio = ags_add_audio_new(AGS_MACHINE(syncsynth)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_app_action_util_add_fm_syncsynth()
{
  AgsFMSyncsynth *fm_syncsynth;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();

  /* create fm syncsynth */
  fm_syncsynth = (AgsFMSyncsynth *) ags_machine_util_new_fm_syncsynth();

  add_audio = ags_add_audio_new(AGS_MACHINE(fm_syncsynth)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_app_action_util_add_hybrid_synth()
{
  AgsHybridSynth *hybrid_synth;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();

  /* create hybrid synth */
  hybrid_synth = (AgsHybridSynth *) ags_machine_util_new_hybrid_synth();

  add_audio = ags_add_audio_new(AGS_MACHINE(hybrid_synth)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_app_action_util_add_hybrid_fm_synth()
{
  AgsHybridFMSynth *hybrid_fm_synth;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();

  /* create FM hybrid synth */
  hybrid_fm_synth = (AgsHybridFMSynth *) ags_machine_util_new_hybrid_fm_synth();

  add_audio = ags_add_audio_new(AGS_MACHINE(hybrid_fm_synth)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_app_action_util_add_ffplayer()
{
#if defined(AGS_WITH_LIBINSTPATCH)
  AgsFFPlayer *ffplayer;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();
  
  /* create ffplayer */
  ffplayer = (AgsFFPlayer *) ags_machine_util_new_ffplayer();

  add_audio = ags_add_audio_new(AGS_MACHINE(ffplayer)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
#endif
}

void
ags_app_action_util_add_sf2_synth()
{
#if defined(AGS_WITH_LIBINSTPATCH)
  AgsSF2Synth *sf2_synth;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();
  
  /* create SF2 synth */
  sf2_synth = (AgsSF2Synth *) ags_machine_util_new_sf2_synth();
  
  add_audio = ags_add_audio_new(AGS_MACHINE(sf2_synth)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
#endif
}

void
ags_app_action_util_add_pitch_sampler()
{
  AgsPitchSampler *pitch_sampler;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();
  
  /* create pitch sampler */
  pitch_sampler = (AgsPitchSampler *) ags_machine_util_new_pitch_sampler();
  
  add_audio = ags_add_audio_new(AGS_MACHINE(pitch_sampler)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_app_action_util_add_sfz_synth()
{
  AgsSFZSynth *sfz_synth;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();
  
  /* create SFZ synth */
  sfz_synth = (AgsSFZSynth *) ags_machine_util_new_sfz_synth();
  
  add_audio = ags_add_audio_new(AGS_MACHINE(sfz_synth)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_app_action_util_add_audiorec()
{
  AgsAudiorec *audiorec;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();
  
  /* create audiorec */
  audiorec = (AgsAudiorec *) ags_machine_util_new_audiorec();

  add_audio = ags_add_audio_new(AGS_MACHINE(audiorec)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_app_action_util_add_desk()
{
  AgsDesk *desk;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();
  
  /* create desk */
  desk = (AgsDesk *) ags_machine_util_new_desk();

  add_audio = ags_add_audio_new(AGS_MACHINE(desk)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_app_action_util_add_ladspa_bridge(gchar *filename, gchar *effect)
{
  AgsLadspaBridge *ladspa_bridge;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();
  
  /* create ladspa bridge */
  ladspa_bridge = (AgsLadspaBridge *) ags_machine_util_new_ladspa_bridge(filename, effect);
  
  add_audio = ags_add_audio_new(AGS_MACHINE(ladspa_bridge)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_app_action_util_add_dssi_bridge(gchar *filename, gchar *effect)
{
  AgsDssiBridge *dssi_bridge;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();
  
  /* create dssi bridge */
  dssi_bridge = (AgsDssiBridge *) ags_machine_util_new_dssi_bridge(filename, effect);
  
  add_audio = ags_add_audio_new(AGS_MACHINE(dssi_bridge)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_app_action_util_add_lv2_bridge(gchar *filename, gchar *effect)
{
  AgsLv2Bridge *lv2_bridge;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
    
  application_context = ags_application_context_get_instance();

  /* create lv2 bridge */
  lv2_bridge = (AgsLv2Bridge *) ags_machine_util_new_lv2_bridge(filename, effect);
  
  add_audio = ags_add_audio_new(AGS_MACHINE(lv2_bridge)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

#if defined(AGS_WITH_VST3)
void
ags_app_action_util_add_vst3_bridge_add_audio_callback(AgsTask *task,
						       AgsVst3Bridge *vst3_bridge)
{
  AgsAudio *audio;
  AgsChannel *start_output, *output;
  AgsPlaybackDomain *playback_domain;

  AgsInstantiateVst3Plugin *instantiate_vst3_plugin;

  AgsVst3Plugin *vst3_plugin;

  AgsAudioThread *audio_thread;
  AgsChannelThread *channel_thread;
  
  GList *start_playback, *playback;	
  GList *start_play, *start_recall;
  GList *start_list, *list;

  guint audio_channels;

  audio = AGS_MACHINE(vst3_bridge)->audio;
  
  vst3_plugin = vst3_bridge->vst3_plugin;

  start_output = NULL;
  
  start_play = NULL;
  start_recall = NULL;

  playback_domain = NULL;

  audio_channels = 0;
  
  g_object_get(audio,
	       "audio-channels", &audio_channels,
	       "output", &start_output,
	       "play", &start_play,
	       "recall", &start_recall,
	       "playback-domain", &playback_domain,
	       NULL);

  list = 
    start_list = g_list_concat(start_play,
			       start_recall);

  start_playback = NULL;
	
  g_object_get(playback_domain,
	       "output-playback", &start_playback,
	       NULL);

  while(list != NULL){
    if(AGS_IS_FX_VST3_AUDIO(list->data)){
      AgsTaskLauncher *task_launcher;      

      if(ags_base_plugin_test_flags((AgsBasePlugin *) vst3_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
	/*  */
	if(ags_audio_test_ability_flags(audio, AGS_SOUND_ABILITY_PLAYBACK)){
	  audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							      AGS_SOUND_SCOPE_PLAYBACK);

	  task_launcher = ags_audio_thread_get_task_launcher(audio_thread);
	
	  instantiate_vst3_plugin =  ags_instantiate_vst3_plugin_new(list->data,
								     AGS_SOUND_SCOPE_PLAYBACK,
								     -1,
								     FALSE);
	  ags_task_launcher_add_task(task_launcher,
				     instantiate_vst3_plugin);

	  g_object_unref(audio_thread);

	  g_object_unref(task_launcher);
	}
	
	/*  */
	if(ags_audio_test_ability_flags(audio, AGS_SOUND_ABILITY_SEQUENCER)){
	  audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							      AGS_SOUND_SCOPE_SEQUENCER);

	  task_launcher = ags_audio_thread_get_task_launcher(audio_thread);
	
	  instantiate_vst3_plugin =  ags_instantiate_vst3_plugin_new(list->data,
								     AGS_SOUND_SCOPE_SEQUENCER,
								     -1,
								     FALSE);
	  ags_task_launcher_add_task(task_launcher,
				     instantiate_vst3_plugin);

	  g_object_unref(audio_thread);

	  g_object_unref(task_launcher);
	}
	
	/*  */
	if(ags_audio_test_ability_flags(audio, AGS_SOUND_ABILITY_NOTATION)){
	  audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							      AGS_SOUND_SCOPE_NOTATION);

	  task_launcher = ags_audio_thread_get_task_launcher(audio_thread);
	
	  instantiate_vst3_plugin =  ags_instantiate_vst3_plugin_new(list->data,
								     AGS_SOUND_SCOPE_NOTATION,
								     -1,
								     FALSE);
	  ags_task_launcher_add_task(task_launcher,
				     instantiate_vst3_plugin);

	  g_object_unref(audio_thread);

	  g_object_unref(task_launcher);
	}
      }else{
	guint i;

	for(i = 0; i < audio_channels; i++){
	  playback = start_playback;

	  output = ags_channel_nth(start_output,
				   i);
	  
	  while(playback != NULL){
	    AgsChannel *channel;
	    
	    gboolean success;

	    channel = NULL;
	    
	    g_object_get(playback->data,
			 "channel", &channel,
			 NULL);

	    success = FALSE;

	    if(channel == output){
	      success = TRUE;
	    }

	    g_object_unref(channel);
	    
	    if(success){
	      break;
	    }

	    playback = playback->next;
	  }
	  
	  /*  */
	  if(ags_audio_test_ability_flags(audio, AGS_SOUND_ABILITY_PLAYBACK)){
	    channel_thread = ags_playback_get_channel_thread(playback->data,
							     AGS_SOUND_SCOPE_PLAYBACK);
	
	    task_launcher = ags_channel_thread_get_task_launcher(channel_thread);
	
	    instantiate_vst3_plugin =  ags_instantiate_vst3_plugin_new(list->data,
								       AGS_SOUND_SCOPE_PLAYBACK,
								       -1,
								       FALSE);
	    ags_task_launcher_add_task(task_launcher,
				       instantiate_vst3_plugin);

	    g_object_unref(channel_thread);

	    g_object_unref(task_launcher);
	  }
	  
	  /*  */
	  if(ags_audio_test_ability_flags(audio, AGS_SOUND_ABILITY_SEQUENCER)){
	    channel_thread = ags_playback_get_channel_thread(playback->data,
							     AGS_SOUND_SCOPE_SEQUENCER);

	    task_launcher = ags_channel_thread_get_task_launcher(channel_thread);

	    instantiate_vst3_plugin =  ags_instantiate_vst3_plugin_new(list->data,
								       AGS_SOUND_SCOPE_SEQUENCER,
								       -1,
								       FALSE);
	    ags_task_launcher_add_task(task_launcher,
				       instantiate_vst3_plugin);

	    g_object_unref(channel_thread);

	    g_object_unref(task_launcher);
	  }
	  
	  /*  */
	  if(ags_audio_test_ability_flags(audio, AGS_SOUND_ABILITY_NOTATION)){
	    channel_thread = ags_playback_get_channel_thread(playback->data,
							     AGS_SOUND_SCOPE_NOTATION);

	    task_launcher = ags_channel_thread_get_task_launcher(channel_thread);

	    instantiate_vst3_plugin =  ags_instantiate_vst3_plugin_new(list->data,
								       AGS_SOUND_SCOPE_NOTATION,
								       -1,
								       FALSE);
	    ags_task_launcher_add_task(task_launcher,
				       instantiate_vst3_plugin);

	    g_object_unref(channel_thread);

	    g_object_unref(task_launcher);
	  }
	  
	  g_object_unref(output);
	}	
      }
    }
    
    list = list->next;
  }
    
  if(playback_domain != NULL){
    g_object_unref(playback_domain);
  }

  g_list_free_full(start_playback,
		   (GDestroyNotify) g_object_unref);

  g_list_free_full(start_list,
		   (GDestroyNotify) g_object_unref);
}
#endif

void
ags_app_action_util_add_vst3_bridge(gchar *filename, gchar *effect)
{
#if defined(AGS_WITH_VST3)
  AgsVst3Bridge *vst3_bridge;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();
  
  /* create vst3 bridge */
  vst3_bridge = (AgsVst3Bridge *) ags_machine_util_new_vst3_bridge(filename, effect);
  
  add_audio = ags_add_audio_new(AGS_MACHINE(vst3_bridge)->audio);

  g_signal_connect_after(add_audio, "launch",
			 G_CALLBACK(ags_app_action_util_add_vst3_bridge_add_audio_callback), vst3_bridge);

  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
#endif
}

void
ags_app_action_util_add_live_dssi_bridge(gchar *filename, gchar *effect)
{
  AgsLiveDssiBridge *live_dssi_bridge;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();
  
  /* create live dssi bridge */
  live_dssi_bridge = (AgsLiveDssiBridge *) ags_machine_util_new_live_dssi_bridge(filename, effect);
  
  add_audio = ags_add_audio_new(AGS_MACHINE(live_dssi_bridge)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_app_action_util_add_live_lv2_bridge(gchar *filename, gchar *effect)
{
  AgsLiveLv2Bridge *live_lv2_bridge;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();
  
  /* create live lv2 bridge */
  live_lv2_bridge = (AgsLiveLv2Bridge *) ags_machine_util_new_live_lv2_bridge(filename, effect);
    
  add_audio = ags_add_audio_new(AGS_MACHINE(live_lv2_bridge)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

#if defined(AGS_WITH_VST3)
void
ags_app_action_util_add_live_vst3_bridge_add_audio_callback(AgsTask *task,
							    AgsLiveVst3Bridge *live_vst3_bridge)
{
  AgsAudio *audio;
  AgsChannel *start_output, *output;
  AgsPlaybackDomain *playback_domain;

  AgsInstantiateVst3Plugin *instantiate_vst3_plugin;

  AgsVst3Plugin *vst3_plugin;

  AgsAudioThread *audio_thread;
  AgsChannelThread *channel_thread;
  
  GList *start_playback, *playback;	
  GList *start_play, *start_recall;
  GList *start_list, *list;

  guint audio_channels;

  audio = AGS_MACHINE(live_vst3_bridge)->audio;
  
  vst3_plugin = live_vst3_bridge->vst3_plugin;

  start_output = NULL;
  
  start_play = NULL;
  start_recall = NULL;

  playback_domain = NULL;

  audio_channels = 0;
  
  g_object_get(audio,
	       "audio-channels", &audio_channels,
	       "output", &start_output,
	       "play", &start_play,
	       "recall", &start_recall,
	       "playback-domain", &playback_domain,
	       NULL);

  list = 
    start_list = g_list_concat(start_play,
			       start_recall);

  start_playback = NULL;
	
  g_object_get(playback_domain,
	       "output-playback", &start_playback,
	       NULL);

  while(list != NULL){
    if(AGS_IS_FX_VST3_AUDIO(list->data)){
      AgsTaskLauncher *task_launcher;      

      if(ags_base_plugin_test_flags((AgsBasePlugin *) vst3_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
	/*  */
	if(ags_audio_test_ability_flags(audio, AGS_SOUND_ABILITY_PLAYBACK)){
	  audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							      AGS_SOUND_SCOPE_PLAYBACK);

	  task_launcher = ags_audio_thread_get_task_launcher(audio_thread);
	
	  instantiate_vst3_plugin =  ags_instantiate_vst3_plugin_new(list->data,
								     AGS_SOUND_SCOPE_PLAYBACK,
								     -1,
								     FALSE);
	  ags_task_launcher_add_task(task_launcher,
				     instantiate_vst3_plugin);

	  g_object_unref(audio_thread);

	  g_object_unref(task_launcher);
	}
	
	/*  */
	if(ags_audio_test_ability_flags(audio, AGS_SOUND_ABILITY_SEQUENCER)){
	  audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							      AGS_SOUND_SCOPE_SEQUENCER);

	  task_launcher = ags_audio_thread_get_task_launcher(audio_thread);
	
	  instantiate_vst3_plugin =  ags_instantiate_vst3_plugin_new(list->data,
								     AGS_SOUND_SCOPE_SEQUENCER,
								     -1,
								     FALSE);
	  ags_task_launcher_add_task(task_launcher,
				     instantiate_vst3_plugin);

	  g_object_unref(audio_thread);

	  g_object_unref(task_launcher);
	}
	
	/*  */
	if(ags_audio_test_ability_flags(audio, AGS_SOUND_ABILITY_NOTATION)){
	  audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							      AGS_SOUND_SCOPE_NOTATION);

	  task_launcher = ags_audio_thread_get_task_launcher(audio_thread);
	
	  instantiate_vst3_plugin =  ags_instantiate_vst3_plugin_new(list->data,
								     AGS_SOUND_SCOPE_NOTATION,
								     -1,
								     FALSE);
	  ags_task_launcher_add_task(task_launcher,
				     instantiate_vst3_plugin);

	  g_object_unref(audio_thread);

	  g_object_unref(task_launcher);
	}
      }
    }
    
    list = list->next;
  }
    
  if(playback_domain != NULL){
    g_object_unref(playback_domain);
  }

  g_list_free_full(start_playback,
		   (GDestroyNotify) g_object_unref);

  g_list_free_full(start_list,
		   (GDestroyNotify) g_object_unref);
}
#endif

void
ags_app_action_util_add_live_vst3_bridge(gchar *filename, gchar *effect)
{
#if defined(AGS_WITH_VST3)
  AgsLiveVst3Bridge *live_vst3_bridge;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
    
  application_context = ags_application_context_get_instance();
  
  /* create live vst3 bridge */
  live_vst3_bridge = (AgsLiveVst3Bridge *) ags_machine_util_new_live_vst3_bridge(filename, effect);
  
  add_audio = ags_add_audio_new(AGS_MACHINE(live_vst3_bridge)->audio);

  g_signal_connect_after(add_audio, "launch",
			 G_CALLBACK(ags_app_action_util_add_live_vst3_bridge_add_audio_callback), live_vst3_bridge);
  
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
#endif
}

void
ags_app_action_util_edit_notation()
{
  AgsApplicationContext *application_context;
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  AgsMachine *machine;    

  application_context = ags_application_context_get_instance();
  
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  composite_editor = window->composite_editor;

  machine = composite_editor->selected_machine;
    
  if(AGS_IS_DRUM(machine) ||
     AGS_IS_MATRIX(machine) ||
     AGS_IS_SYNCSYNTH(machine) ||
     AGS_IS_FM_SYNCSYNTH(machine) ||
     AGS_IS_HYBRID_SYNTH(machine) ||
     AGS_IS_HYBRID_FM_SYNTH(machine) ||
#ifdef AGS_WITH_LIBINSTPATCH
     AGS_IS_FFPLAYER(machine) ||
     AGS_IS_SF2_SYNTH(machine) ||
#endif
     AGS_IS_PITCH_SAMPLER(machine) ||
     AGS_IS_SFZ_SYNTH(machine) ||
     AGS_IS_DSSI_BRIDGE(machine) ||
     AGS_IS_LIVE_DSSI_BRIDGE(machine) ||
     (AGS_IS_LV2_BRIDGE(machine) && (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0) ||
     AGS_IS_LIVE_LV2_BRIDGE(machine)
#if defined(AGS_WITH_VST3)
     || (AGS_IS_VST3_BRIDGE(machine) && (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0) ||
     AGS_IS_LIVE_VST3_BRIDGE(machine)
#endif
    ){
    ags_composite_toolbar_scope_create_and_connect(composite_editor->toolbar,
						   AGS_COMPOSITE_TOOLBAR_SCOPE_NOTATION);

    composite_editor->selected_edit = composite_editor->notation_edit;
      
    gtk_widget_show((GtkWidget *) composite_editor->notation_edit);
    gtk_widget_hide((GtkWidget *) composite_editor->sheet_edit);
    gtk_widget_hide((GtkWidget *) composite_editor->automation_edit);
    gtk_widget_hide((GtkWidget *) composite_editor->wave_edit);
      
    /* shift piano */
    ags_machine_selector_set_flags(composite_editor->machine_selector,
				   AGS_MACHINE_SELECTOR_SHOW_SHIFT_PIANO);
  }
}

void
ags_app_action_util_edit_automation()
{
  AgsApplicationContext *application_context;
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;

  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  composite_editor = window->composite_editor;
    
  ags_composite_toolbar_scope_create_and_connect(composite_editor->toolbar,
						 AGS_COMPOSITE_TOOLBAR_SCOPE_AUTOMATION);

  composite_editor->selected_edit = composite_editor->automation_edit;
    
  gtk_widget_hide((GtkWidget *) composite_editor->notation_edit);
  gtk_widget_hide((GtkWidget *) composite_editor->sheet_edit);
  gtk_widget_show((GtkWidget *) composite_editor->automation_edit);
  gtk_widget_hide((GtkWidget *) composite_editor->wave_edit);
    
  /* shift piano */
  ags_machine_selector_unset_flags(composite_editor->machine_selector,
				   AGS_MACHINE_SELECTOR_SHOW_SHIFT_PIANO);
}

void
ags_app_action_util_edit_wave()
{
  AgsApplicationContext *application_context;
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  AgsMachine *machine;
  
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));  

  composite_editor = window->composite_editor;

  machine = composite_editor->selected_machine;

  if(AGS_IS_AUDIOREC(machine)){
    GtkAdjustment *adjustment;

    GList *start_wave_edit;

    gdouble lower, upper;
    gdouble page_increment, step_increment;
    gdouble page_size;
    gdouble value;
      
    ags_composite_toolbar_scope_create_and_connect(composite_editor->toolbar,
						   AGS_COMPOSITE_TOOLBAR_SCOPE_WAVE);

    composite_editor->selected_edit = composite_editor->wave_edit;
      
    gtk_widget_hide((GtkWidget *) composite_editor->notation_edit);
    gtk_widget_hide((GtkWidget *) composite_editor->sheet_edit);
    gtk_widget_hide((GtkWidget *) composite_editor->automation_edit);
    gtk_widget_show((GtkWidget *) composite_editor->wave_edit);

    /* shift piano */
    ags_machine_selector_unset_flags(composite_editor->machine_selector,
				     AGS_MACHINE_SELECTOR_SHOW_SHIFT_PIANO);

    start_wave_edit = ags_wave_edit_box_get_wave_edit(AGS_WAVE_EDIT_BOX(AGS_SCROLLED_WAVE_EDIT_BOX(composite_editor->wave_edit->edit)->wave_edit_box));
      
    if(start_wave_edit != NULL){
      adjustment = gtk_scrollbar_get_adjustment(AGS_WAVE_EDIT(start_wave_edit->data)->hscrollbar);
	
      g_object_get(adjustment,
		   "lower", &lower,
		   "upper", &upper,
		   "page-increment", &page_increment,
		   "step-increment", &step_increment,
		   "page-size", &page_size,
		   "value", &value,
		   NULL);

      g_object_set(gtk_scrollbar_get_adjustment(composite_editor->wave_edit->hscrollbar),
		   "lower", lower,
		   "upper", upper,
		   "page-increment", page_increment,
		   "step-increment", step_increment,
		   "page-size", page_size,
		   "value", value,
		   NULL);

      g_list_free(start_wave_edit);
    }
  }
}

void
ags_app_action_util_edit_sheet()
{
  AgsApplicationContext *application_context;
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  AgsMachine *machine;
  
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
      
  composite_editor = window->composite_editor;

  machine = composite_editor->selected_machine;
    
  if(AGS_IS_DRUM(machine) ||
     AGS_IS_MATRIX(machine) ||
     AGS_IS_SYNCSYNTH(machine) ||
     AGS_IS_FM_SYNCSYNTH(machine) ||
     AGS_IS_HYBRID_SYNTH(machine) ||
     AGS_IS_HYBRID_FM_SYNTH(machine) ||
#ifdef AGS_WITH_LIBINSTPATCH
     AGS_IS_FFPLAYER(machine) ||
     AGS_IS_SF2_SYNTH(machine) ||
#endif
     AGS_IS_PITCH_SAMPLER(machine) ||
     AGS_IS_SFZ_SYNTH(machine) ||
     AGS_IS_DSSI_BRIDGE(machine) ||
     AGS_IS_LIVE_DSSI_BRIDGE(machine) ||
     (AGS_IS_LV2_BRIDGE(machine) && (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0) ||
     AGS_IS_LIVE_LV2_BRIDGE(machine)
#if defined(AGS_WITH_VST3)
     || (AGS_IS_VST3_BRIDGE(machine) && (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0) ||
     AGS_IS_LIVE_VST3_BRIDGE(machine)
#endif
    ){
    ags_composite_toolbar_scope_create_and_connect(composite_editor->toolbar,
						   AGS_COMPOSITE_TOOLBAR_SCOPE_SHEET);

    composite_editor->selected_edit = composite_editor->sheet_edit;
      
    gtk_widget_hide((GtkWidget *) composite_editor->notation_edit);
    gtk_widget_show((GtkWidget *) composite_editor->sheet_edit);
    gtk_widget_hide((GtkWidget *) composite_editor->automation_edit);
    gtk_widget_hide((GtkWidget *) composite_editor->wave_edit);

    gtk_widget_queue_draw(GTK_WIDGET(AGS_SHEET_EDIT(composite_editor->sheet_edit->edit)->drawing_area));
      
    /* shift piano */
    ags_machine_selector_set_flags(composite_editor->machine_selector,
				   AGS_MACHINE_SELECTOR_SHOW_SHIFT_PIANO);
  }
}

void
ags_app_action_util_edit_meta()
{
  AgsApplicationContext *application_context;
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  AgsMachine *machine;
  GtkWidget *scrolled_edit_meta;
  
  gboolean toggle_visible;

  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
      
  composite_editor = window->composite_editor;

  machine = composite_editor->selected_machine;

  scrolled_edit_meta = NULL;
  
  if(AGS_IS_NOTATION_EDIT(composite_editor->selected_edit)){
    scrolled_edit_meta = (GtkWidget *) composite_editor->notation_edit->scrolled_edit_meta;
  }else if(AGS_IS_AUTOMATION_EDIT(composite_editor->selected_edit)){
    scrolled_edit_meta = (GtkWidget *) composite_editor->automation_edit->scrolled_edit_meta;
  }else if(AGS_IS_WAVE_EDIT(composite_editor->selected_edit)){
    scrolled_edit_meta = (GtkWidget *) composite_editor->wave_edit->scrolled_edit_meta;
  }else if(AGS_IS_SHEET_EDIT(composite_editor->selected_edit)){
    scrolled_edit_meta = (GtkWidget *) composite_editor->sheet_edit->scrolled_edit_meta;
  }

  if(scrolled_edit_meta != NULL){
    toggle_visible = (gtk_widget_get_visible(scrolled_edit_meta)) ? FALSE: TRUE;
  
    gtk_widget_set_visible(scrolled_edit_meta,
			   toggle_visible);
  }
}

void
ags_app_action_util_edit_tempo()
{
  AgsApplicationContext *application_context;
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;

  gboolean toggle_visible;
  
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
      
  composite_editor = window->composite_editor;

  toggle_visible = (gtk_widget_get_visible((GtkWidget *) composite_editor->tempo_box)) ? FALSE: TRUE;
  
  gtk_widget_set_visible((GtkWidget *) composite_editor->tempo_box,
			 toggle_visible);

  gtk_widget_queue_draw(GTK_WIDGET(composite_editor->tempo_edit->drawing_area));
}
