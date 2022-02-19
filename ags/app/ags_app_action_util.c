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
#include <ags/app/ags_composite_editor.h>
#include <ags/app/ags_export_window.h>
#include <ags/app/ags_preferences.h>
#include <ags/app/ags_online_help_window.h>
#include <ags/app/ags_quit_dialog.h>
#include <ags/app/ags_machine_util.h>

#include <ags/app/editor/ags_composite_edit.h>
#include <ags/app/editor/ags_composite_edit_callbacks.h>

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

void ags_app_action_util_open_response_callback(GtkFileChooserDialog *file_chooser,
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
ags_app_action_util_smf_import()
{
  AgsMidiImportWizard *midi_import_wizard;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();
  
  midi_import_wizard = (AgsMidiImportWizard *) ags_ui_provider_get_midi_import_wizard(AGS_UI_PROVIDER(application_context));

  if(midi_import_wizard != NULL){
    return;
  }

  midi_import_wizard = ags_midi_import_wizard_new();
  ags_ui_provider_set_midi_import_wizard(AGS_UI_PROVIDER(application_context),
					 midi_import_wizard);

  ags_connectable_connect(AGS_CONNECTABLE(midi_import_wizard));
  ags_applicable_reset(AGS_APPLICABLE(midi_import_wizard));

  gtk_widget_show_all(GTK_WIDGET(midi_import_wizard));
}

void
ags_app_action_util_smf_export()
{
  AgsMidiExportWizard *midi_export_wizard;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();
  
  midi_export_wizard = (AgsMidiExportWizard *) ags_ui_provider_get_midi_export_wizard(AGS_UI_PROVIDER(application_context));

  if(midi_export_wizard != NULL){
    return;
  }

  midi_export_wizard = ags_midi_export_wizard_new();
  ags_ui_provider_set_midi_export_wizard(AGS_UI_PROVIDER(application_context),
					 midi_export_wizard);

  ags_connectable_connect(AGS_CONNECTABLE(midi_export_wizard));
  ags_applicable_reset(AGS_APPLICABLE(midi_export_wizard));

  gtk_widget_show_all(GTK_WIDGET(midi_export_wizard));
}

void
ags_app_action_util_preferences()
{
  AgsPreferences *preferences;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  preferences = (AgsPreferences *) ags_ui_provider_get_preferences(AGS_UI_PROVIDER(application_context));

  if(preferences != NULL){
    return;
  }

  preferences = ags_preferences_new();
  ags_ui_provider_set_preferences(AGS_UI_PROVIDER(application_context),
				  preferences);

  ags_connectable_connect(AGS_CONNECTABLE(preferences));
  
  ags_applicable_reset(AGS_APPLICABLE(preferences));

  gtk_widget_show_all(GTK_WIDGET(preferences));
}

void
ags_app_action_util_about()
{
  AgsApplicationContext *application_context;
  AgsWindow *window;

  static FILE *file = NULL;
  struct stat sb;
  static gchar *license = NULL;
  static GdkPixbuf *logo = NULL;

  gchar *license_filename;
  gchar *logo_filename;
#if defined(AGS_W32API)
  gchar *app_dir;
#endif
  
  int n_read;
  
  GError *error;

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

      error = NULL;
      logo = gdk_pixbuf_new_from_file(logo_filename,
				      &error);
  
      //g_free(logo_filename);

      if(error != NULL){
	g_message("%s", error->message);

	g_error_free(error);
      }
  
    }
  }

  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  gtk_show_about_dialog((GtkWindow *) window,
			"program-name", "gsequencer",
			"authors", authors,
			"license", license,
			"version", AGS_VERSION,
			"website", "http://nongnu.org/gsequencer",
			"title", "Advanced Gtk+ Sequencer",
			"logo", logo,
			NULL);

  g_free(license_filename);

#if defined AGS_W32API
  g_free(app_dir);
#endif
}

void
ags_app_action_util_help()
{
  AgsOnlineHelpWindow *online_help_window;
  
  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  online_help_window = ags_ui_provider_get_online_help_window(AGS_UI_PROVIDER(application_context));

  if(online_help_window == NULL){
    online_help_window = ags_online_help_window_new();
    
    ags_connectable_connect(AGS_CONNECTABLE(online_help_window));
    
    ags_ui_provider_set_online_help_window(AGS_UI_PROVIDER(application_context),
					   (GtkWidget *) online_help_window);
  }
  
  gtk_widget_show_all((GtkWidget *) online_help_window);
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
  //TODO:JK: implement me
}

void
ags_app_action_util_edit_automation()
{
  //TODO:JK: implement me
}

void
ags_app_action_util_edit_wave()
{
  //TODO:JK: implement me
}

void
ags_app_action_util_edit_sheet()
{
  //TODO:JK: implement me
}
