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

#include <ags/app/ags_app_action_util.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_export_window.h>
#include <ags/app/ags_quit_dialog.h>

#include <ags/app/file/ags_simple_file.h>

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

void ags_app_action_util_open_response_callback(GtkFileChooserDialog *file_chooser,
						gint response,
						gpointer data);

void
ags_app_action_util_open()
{
  AgsWindow *window;
  GtkFileChooserDialog *file_chooser;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  file_chooser = (GtkFileChooserDialog *) gtk_file_chooser_dialog_new(i18n("Open file"),
								      (GtkWindow *) window,
								      GTK_FILE_CHOOSER_ACTION_OPEN,
								      "_Cancel", GTK_RESPONSE_CANCEL,
								      "_OK", GTK_RESPONSE_ACCEPT,
								      NULL);
  gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(file_chooser), FALSE);
  gtk_widget_show_all((GtkWidget *) file_chooser);

  g_signal_connect((GObject *) file_chooser, "response",
		   G_CALLBACK(ags_app_action_util_open_response_callback), NULL);
}

void
ags_app_action_util_open_response_callback(GtkFileChooserDialog *file_chooser,
					   gint response,
					   gpointer data)
{
  if(response == GTK_RESPONSE_ACCEPT){
    AgsApplicationContext *application_context;
    
    char *filename;
    gchar *str;
#if defined(AGS_W32API) || defined(AGS_OSXAPI)
    gchar *app_dir;
#endif
    
    GError *error;

    application_context = ags_application_context_get_instance();

    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));

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
    app_dir = g_path_get_dirname(application_context->argv[0]);
  
    if(g_path_is_absolute(app_dir)){
      str = g_strdup_printf("%s %s %s",
			    application_context->argv[0],
			    "--filename",
			    filename);
    }else{
      str = g_strdup_printf("%s/%s %s %s",
			    g_get_current_dir(),
			    application_context->argv[0],
			    "--filename",
			    filename);
    }
    
    g_free(app_dir);

    g_spawn_command_line_async(str,
			       &error);

    if(error != NULL){
      g_message("%s", error->message);

      g_error_free(error);
    }    

    g_free(str);
#else
    str = g_strdup_printf("%s --filename %s",
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

  gtk_widget_destroy((GtkWidget *) file_chooser);
}

void
ags_app_action_util_save()
{
  AgsApplicationContext *application_context;
  AgsWindow *window;
  
  GError *error;

  application_context = ags_application_context_get_instance();
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  if(g_strcmp0(ags_config_get_value(AGS_APPLICATION_CONTEXT(application_context)->config,
				    AGS_CONFIG_GENERIC,
				    "simple-file"),
	       "false")){
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
}

void
ags_app_action_util_save_as()
{
  AgsWindow *window;
  GtkFileChooserDialog *file_chooser;
  
  AgsApplicationContext *application_context;
  AgsConfig *config;
  
  gint response;
        
  application_context = ags_application_context_get_instance();

  config = ags_config_get_instance();
  
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  file_chooser = (GtkFileChooserDialog *) gtk_file_chooser_dialog_new(i18n("save file as"),
								      (GtkWindow *) window,
								      GTK_FILE_CHOOSER_ACTION_SAVE,
								      "_Cancel", GTK_RESPONSE_CANCEL,
								      "_OK", GTK_RESPONSE_ACCEPT,
								      NULL);
  
  gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(file_chooser), FALSE);
  gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(file_chooser), TRUE);
  
  gtk_widget_show_all((GtkWidget *) file_chooser);

  response = gtk_dialog_run(GTK_DIALOG(file_chooser));

  if(response == GTK_RESPONSE_ACCEPT){
    AgsSimpleFile *simple_file;

#if defined(AGS_OSXAPI) || defined(AGS_W32API)
#else
    locale_t current;
#endif
    
    gchar *filename;
    gchar *str;
    
    GError *error;
    
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));
    
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
  }
  
  gtk_widget_destroy((GtkWidget *) file_chooser);
}

void
ags_app_action_util_export()
{  
  AgsExportWindow *export_window;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  export_window = (AgsExportWindow *) ags_ui_provider_get_export_window(AGS_UI_PROVIDER(application_context));
  gtk_widget_show_all((GtkWidget *) export_window);
}

void
ags_app_action_util_quit()
{  
  AgsQuitDialog *quit_dialog;

  quit_dialog = ags_quit_dialog_new();

  gtk_widget_show_all((GtkWidget *) quit_dialog);
  
  gtk_widget_grab_focus((GtkWidget *) quit_dialog->cancel);
  
  ags_connectable_connect(AGS_CONNECTABLE(quit_dialog));

  gtk_dialog_run((GtkDialog *) quit_dialog);
}

