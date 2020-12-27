/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/X/ags_menu_action_callbacks.h>

#include "config.h"

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_quit_dialog.h>
#include <ags/X/ags_export_window.h>
#include <ags/X/ags_machine_util.h>
#include <ags/X/ags_online_help_window.h>

#include <ags/X/import/ags_midi_import_wizard.h>

#include <ags/X/export/ags_midi_export_wizard.h>

#include <ags/X/file/ags_simple_file.h>

#include <ags/X/machine/ags_panel.h>
#include <ags/X/machine/ags_mixer.h>
#include <ags/X/machine/ags_spectrometer.h>
#include <ags/X/machine/ags_equalizer10.h>
#include <ags/X/machine/ags_drum.h>
#include <ags/X/machine/ags_matrix.h>
#include <ags/X/machine/ags_synth.h>
#include <ags/X/machine/ags_fm_synth.h>
#include <ags/X/machine/ags_syncsynth.h>
#include <ags/X/machine/ags_fm_syncsynth.h>

#ifdef AGS_WITH_LIBINSTPATCH
#include <ags/X/machine/ags_ffplayer.h>
#include <ags/X/machine/ags_sf2_synth.h>
#endif

#include <ags/X/machine/ags_pitch_sampler.h>
#include <ags/X/machine/ags_sfz_synth.h>

#include <ags/X/machine/ags_audiorec.h>
#include <ags/X/machine/ags_desk.h>

#include <ags/X/machine/ags_ladspa_bridge.h>
#include <ags/X/machine/ags_dssi_bridge.h>
#include <ags/X/machine/ags_lv2_bridge.h>
#include <ags/X/machine/ags_live_dssi_bridge.h>
#include <ags/X/machine/ags_live_lv2_bridge.h>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>

#define _GNU_SOURCE
#include <locale.h>

#if defined __APPLE__ || AGS_W32API
#else
#include <X11/Xlib.h>
#endif

static GMutex locale_mutex;

#if defined(AGS_OSXAPI) || defined(AGS_W32API)
static char *locale_env;
#else
static locale_t c_locale;
#endif

static gboolean locale_initialized = FALSE;

void ags_menu_action_open_response_callback(GtkFileChooserDialog *file_chooser, gint response, gpointer data);

void
ags_menu_action_open_callback(GtkWidget *menu_item, gpointer data)
{
  AgsApplicationContext *application_context;
  AgsWindow *window;
  GtkFileChooserDialog *file_chooser;

  application_context = ags_application_context_get_instance();
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  file_chooser = (GtkFileChooserDialog *) gtk_file_chooser_dialog_new("open file",
								      (GtkWindow *) window,
								      GTK_FILE_CHOOSER_ACTION_OPEN,
								      "_Cancel", GTK_RESPONSE_CANCEL,
								      "_OK", GTK_RESPONSE_ACCEPT,
								      NULL);
  gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(file_chooser), FALSE);
  gtk_widget_show_all((GtkWidget *) file_chooser);

  g_signal_connect((GObject *) file_chooser, "response",
		   G_CALLBACK(ags_menu_action_open_response_callback), data);
}

void
ags_menu_action_open_response_callback(GtkFileChooserDialog *file_chooser, gint response, gpointer data)
{
  if(response == GTK_RESPONSE_ACCEPT){
#if defined(AGS_W32API) || defined(AGS_OSXAPI)
    AgsApplicationContext *application_context;
#endif
    
    char *filename;
    gchar *str;
#if defined(AGS_W32API) || defined(AGS_OSXAPI)
    gchar *app_dir;
#endif
    
    GError *error;

    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));

    error = NULL;

#if defined(AGS_W32API)
    application_context = ags_application_context_get_instance();

    if(strlen(application_context->argv[0]) > strlen("gsequencer.exe")){
      app_dir = g_strndup(application_context->argv[0],
			  strlen(application_context->argv[0]) - strlen("gsequencer.exe"));
    }else{
      app_dir = NULL;
    }
  
    if(g_path_is_absolute(app_dir)){
      str = g_strdup_printf("%s\\gsequencer.exe --filename %s",
			    app_dir,
			    filename);
    }else{
      str = g_strdup_printf("%s\\%s\\gsequencer.exe --filename %s",
			    g_get_current_dir(),
			    app_dir,
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
#else
#if defined(AGS_OSXAPI)
    application_context = ags_application_context_get_instance();

    if(strlen(application_context->argv[0]) > strlen("GSequencer-bin")){
      app_dir = g_strndup(application_context->argv[0],
			  strlen(application_context->argv[0]) - strlen("GSequencer-bin"));
    }else{
      app_dir = NULL;
    }
  
    if(g_path_is_absolute(app_dir)){
      str = g_strdup_printf("%s%s %s",
			    app_dir,
			    "GSequencer --filename",
			    filename);
    }else{
      str = g_strdup_printf("%s/%s%s %s",
			    g_get_current_dir(),
			    app_dir,
			    "GSequencer --filename",
			    filename);
    }
    
    g_free(app_dir);

    g_spawn_command_line_async(str,
			       &error);

    if(error != NULL){
      g_message("%s", error->message);

      g_error_free(error);
    }    
#else
    str = g_strdup_printf("gsequencer --filename %s",
			  filename);

    g_spawn_command_line_async(str,
			       &error);

    if(error != NULL){
      g_message("%s", error->message);

      g_error_free(error);
    }
#endif    
#endif    
    
    g_free(filename);
    g_free(str);
  }

  gtk_widget_destroy((GtkWidget *) file_chooser);
}

void
ags_menu_action_save_callback(GtkWidget *menu_item, gpointer data)
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
      c_locale = newlocale(LC_ALL_MASK, "C", (locale_t) 0);
#endif
    
      locale_initialized = TRUE;
    }

    g_mutex_unlock(&locale_mutex);

#if defined(AGS_OSXAPI) || defined(AGS_W32API)
    setlocale(LC_ALL, "C");
#else
    current = uselocale(c_locale);
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
ags_menu_action_save_as_callback(GtkWidget *menu_item, gpointer data)
{
  AgsApplicationContext *application_context;
  AgsWindow *window;
  GtkFileChooserDialog *file_chooser;
  
  gint response;
        
  application_context = ags_application_context_get_instance();
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  file_chooser = (GtkFileChooserDialog *) gtk_file_chooser_dialog_new("save file as",
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
    gchar *filename;
    gchar *str;
    
    GError *error;
    
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));
    
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
	c_locale = newlocale(LC_ALL_MASK, "C", (locale_t) 0);
#endif
    
	locale_initialized = TRUE;
      }

      g_mutex_unlock(&locale_mutex);

#if defined(AGS_OSXAPI) || defined(AGS_W32API)
      setlocale(LC_ALL, "C");
#else
      current = uselocale(c_locale);
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
    }else{
      AgsFile *file;

      file = (AgsFile *) g_object_new(AGS_TYPE_FILE,
				      "filename", filename,
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

    if(window->name != NULL){
      g_free(window->name);
    }
    
    window->name = g_strdup(filename);

    str = g_strconcat("GSequencer - ", window->name, NULL);
    gtk_window_set_title((GtkWindow *) window, str);

    g_free(str);
  }

  gtk_widget_destroy((GtkWidget *) file_chooser);
}

void
ags_menu_action_export_callback(GtkWidget *menu_item, gpointer data)
{
  AgsApplicationContext *application_context;
  AgsWindow *window;

  application_context = ags_application_context_get_instance();
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  gtk_widget_show_all((GtkWidget *) window->export_window);
}

void
ags_menu_action_quit_callback(GtkWidget *menu_item, gpointer data)
{
  AgsQuitDialog *quit_dialog;

  quit_dialog = ags_quit_dialog_new();

  gtk_widget_show_all((GtkWidget *) quit_dialog);
  
  gtk_widget_grab_focus((GtkWidget *) quit_dialog->cancel);
  
  ags_connectable_connect(AGS_CONNECTABLE(quit_dialog));

  gtk_dialog_run((GtkDialog *) quit_dialog);
}

void
ags_menu_action_add_callback(GtkWidget *menu_item, gpointer data)
{
}

void
ags_menu_action_add_panel_callback(GtkWidget *menu_item, gpointer data)
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
ags_menu_action_add_mixer_callback(GtkWidget *menu_item, gpointer data)
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
ags_menu_action_add_spectrometer_callback(GtkWidget *menu_item, gpointer data)
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
ags_menu_action_add_equalizer_callback(GtkWidget *menu_item, gpointer data)
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
ags_menu_action_add_drum_callback(GtkWidget *menu_item, gpointer data)
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
ags_menu_action_add_matrix_callback(GtkWidget *menu_item, gpointer data)
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
ags_menu_action_add_synth_callback(GtkWidget *menu_item, gpointer data)
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
ags_menu_action_add_fm_synth_callback(GtkWidget *menu_item, gpointer data)
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
ags_menu_action_add_syncsynth_callback(GtkWidget *menu_item, gpointer data)
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
ags_menu_action_add_fm_syncsynth_callback(GtkWidget *menu_item, gpointer data)
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

#ifdef AGS_WITH_LIBINSTPATCH
void
ags_menu_action_add_ffplayer_callback(GtkWidget *menu_item, gpointer data)
{
  AgsFFPlayer *ffplayer;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();
  
  /* create ffplayer */
  ffplayer = (AgsFFPlayer *) ags_machine_util_new_ffplayer();

  add_audio = ags_add_audio_new(AGS_MACHINE(ffplayer)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_menu_action_add_sf2_synth_callback(GtkWidget *menu_item, gpointer data)
{
  AgsSF2Synth *sf2_synth;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();
  
  /* create SF2 synth */
  sf2_synth = (AgsSF2Synth *) ags_machine_util_new_sf2_synth();
  
  add_audio = ags_add_audio_new(AGS_MACHINE(sf2_synth)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}
#endif

void
ags_menu_action_add_pitch_sampler_callback(GtkWidget *menu_item, gpointer data)
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
ags_menu_action_add_sfz_synth_callback(GtkWidget *menu_item, gpointer data)
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
ags_menu_action_add_audiorec_callback(GtkWidget *menu_item, gpointer data)
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
ags_menu_action_add_desk_callback(GtkWidget *menu_item, gpointer data)
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
ags_menu_action_add_ladspa_bridge_callback(GtkWidget *menu_item, gpointer data)
{
  AgsLadspaBridge *ladspa_bridge;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;

  gchar *filename, *effect;
  
  filename = g_object_get_data((GObject *) menu_item,
			       AGS_MENU_ITEM_FILENAME_KEY);
  effect = g_object_get_data((GObject *) menu_item,
			     AGS_MENU_ITEM_EFFECT_KEY);
  
  application_context = ags_application_context_get_instance();
  
  /* create ladspa bridge */
  ladspa_bridge = (AgsLadspaBridge *) ags_machine_util_new_ladspa_bridge(filename, effect);
  
  add_audio = ags_add_audio_new(AGS_MACHINE(ladspa_bridge)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_menu_action_add_dssi_bridge_callback(GtkWidget *menu_item, gpointer data)
{
  AgsDssiBridge *dssi_bridge;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;

  gchar *filename, *effect;

  filename = g_object_get_data((GObject *) menu_item,
			       AGS_MENU_ITEM_FILENAME_KEY);
  effect = g_object_get_data((GObject *) menu_item,
			     AGS_MENU_ITEM_EFFECT_KEY);
  
  application_context = ags_application_context_get_instance();
  
  /* create dssi bridge */
  dssi_bridge = (AgsDssiBridge *) ags_machine_util_new_dssi_bridge(filename, effect);
  
  add_audio = ags_add_audio_new(AGS_MACHINE(dssi_bridge)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_menu_action_add_lv2_bridge_callback(GtkWidget *menu_item, gpointer data)
{
  AgsLv2Bridge *lv2_bridge;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  gchar *filename, *effect;
    
  filename = g_object_get_data((GObject *) menu_item,
			       AGS_MENU_ITEM_FILENAME_KEY);
  effect = g_object_get_data((GObject *) menu_item,
			     AGS_MENU_ITEM_EFFECT_KEY);
  
  application_context = ags_application_context_get_instance();

  /* create lv2 bridge */
  lv2_bridge = (AgsLv2Bridge *) ags_machine_util_new_lv2_bridge(filename, effect);
  
  add_audio = ags_add_audio_new(AGS_MACHINE(lv2_bridge)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_menu_action_add_live_dssi_bridge_callback(GtkWidget *menu_item, gpointer data)
{
  AgsLiveDssiBridge *live_dssi_bridge;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  gchar *filename, *effect;

  filename = g_object_get_data((GObject *) menu_item,
			       AGS_MENU_ITEM_FILENAME_KEY);
  effect = g_object_get_data((GObject *) menu_item,
			     AGS_MENU_ITEM_EFFECT_KEY);
  
  application_context = ags_application_context_get_instance();
  
  /* create live dssi bridge */
  live_dssi_bridge = (AgsLiveDssiBridge *) ags_machine_util_new_live_dssi_bridge(filename, effect);
  
  add_audio = ags_add_audio_new(AGS_MACHINE(live_dssi_bridge)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_menu_action_add_live_lv2_bridge_callback(GtkWidget *menu_item, gpointer data)
{
  AgsLiveLv2Bridge *live_lv2_bridge;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;

  gchar *filename, *effect;
    
  filename = g_object_get_data((GObject *) menu_item,
			       AGS_MENU_ITEM_FILENAME_KEY);
  effect = g_object_get_data((GObject *) menu_item,
			     AGS_MENU_ITEM_EFFECT_KEY);
  
  application_context = ags_application_context_get_instance();
  
  /* create live lv2 bridge */
  live_lv2_bridge = (AgsLiveLv2Bridge *) ags_machine_util_new_live_lv2_bridge(filename, effect);
    
  add_audio = ags_add_audio_new(AGS_MACHINE(live_lv2_bridge)->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
}

void
ags_menu_action_automation_callback(GtkWidget *menu_item, gpointer data)
{
  AgsApplicationContext *application_context;
  AgsWindow *window;

  application_context = ags_application_context_get_instance();
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  gtk_widget_show_all((GtkWidget *) window->automation_window);
}

void
ags_menu_action_wave_callback(GtkWidget *menu_item, gpointer data)
{
  AgsApplicationContext *application_context;
  AgsWindow *window;

  application_context = ags_application_context_get_instance();
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  gtk_widget_show_all((GtkWidget *) window->wave_window);
}

void
ags_menu_action_preferences_callback(GtkWidget *menu_item, gpointer data)
{
  AgsApplicationContext *application_context;
  AgsWindow *window;

  application_context = ags_application_context_get_instance();
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  if(window->preferences != NULL){
    return;
  }

  window->preferences = ags_preferences_new();
  window->preferences->main_window = GTK_WINDOW(window);

  ags_connectable_connect(AGS_CONNECTABLE(window->preferences));
  ags_applicable_reset(AGS_APPLICABLE(window->preferences));

  gtk_widget_show_all(GTK_WIDGET(window->preferences));
}

void
ags_menu_action_midi_import_callback(GtkWidget *menu_item, gpointer data)
{
  AgsApplicationContext *application_context;
  AgsWindow *window;

  application_context = ags_application_context_get_instance();
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  if(window->midi_import_wizard != NULL){
    return;
  }

  window->midi_import_wizard = (GtkWidget *) ags_midi_import_wizard_new();
  g_object_set(window->midi_import_wizard,
	       "main-window", window,
	       NULL);

  ags_connectable_connect(AGS_CONNECTABLE(window->midi_import_wizard));
  ags_applicable_reset(AGS_APPLICABLE(window->midi_import_wizard));

  gtk_widget_show_all(GTK_WIDGET(window->midi_import_wizard));
}

void
ags_menu_action_midi_export_track_callback(GtkWidget *menu_item, gpointer data)
{
  AgsApplicationContext *application_context;
  AgsWindow *window;

  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  if(window->midi_export_wizard != NULL){
    return;
  }

  window->midi_export_wizard = (GtkWidget *) ags_midi_export_wizard_new((GtkWidget *) window);

  ags_connectable_connect(AGS_CONNECTABLE(window->midi_export_wizard));
  ags_applicable_reset(AGS_APPLICABLE(window->midi_export_wizard));

  gtk_widget_show_all(GTK_WIDGET(window->midi_export_wizard));
}

void
ags_menu_action_midi_playback_callback(GtkWidget *menu_item, gpointer data)
{
  //TODO:JK: implement me
}

void
ags_menu_action_online_help_callback(GtkWidget *menu_item, gpointer data)
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
ags_menu_action_about_callback(GtkWidget *menu_item, gpointer data)
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
					   "\\share\\gsequencer\\license\\GPL-3");
      }else{
	license_filename = g_strdup_printf("%s\\%s\\%s",
					   g_get_current_dir(),
					   app_dir,
					   "\\share\\gsequencer\\license\\GPL-3");
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
					    "\\share\\gsequencer\\images\\ags.png");
	  }else{
	    logo_filename = g_strdup_printf("%s\\%s\\%s",
					    g_get_current_dir(),
					    app_dir,
					    "\\share\\gsequencer\\images\\ags.png");
	  }
	}
#else
	logo_filename = g_strdup_printf("%s/%s",
					DESTDIR,
					"/gsequencer/images/ags.png");
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
