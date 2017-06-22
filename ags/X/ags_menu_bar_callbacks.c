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

#include <ags/X/ags_menu_bar_callbacks.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_sequencer.h>

#include <ags/file/ags_file.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/plugin/ags_lv2_manager.h>
#include <ags/plugin/ags_lv2_plugin.h>

#include <ags/audio/ags_midiin.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_output.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <ags/audio/task/ags_save_file.h>
#include <ags/audio/task/ags_add_audio.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_export_window.h>

#include <ags/X/import/ags_midi_import_wizard.h>

#include <ags/X/export/ags_midi_export_wizard.h>

#include <ags/X/file/ags_simple_file.h>

#include <ags/X/machine/ags_panel.h>
#include <ags/X/machine/ags_mixer.h>
#include <ags/X/machine/ags_drum.h>
#include <ags/X/machine/ags_matrix.h>
#include <ags/X/machine/ags_synth.h>
#include <ags/X/machine/ags_ffplayer.h>
#include <ags/X/machine/ags_ladspa_bridge.h>
#include <ags/X/machine/ags_dssi_bridge.h>
#include <ags/X/machine/ags_lv2_bridge.h>
#include <ags/X/machine/ags_live_dssi_bridge.h>
#include <ags/X/machine/ags_live_lv2_bridge.h>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>

#include <X11/Xlib.h>

void ags_menu_bar_open_response_callback(GtkFileChooserDialog *file_chooser, gint response, AgsMenuBar *menu_bar);

void
ags_menu_bar_open_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;
  GtkFileChooserDialog *file_chooser;
  gint response;

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) menu_bar);

  file_chooser = (GtkFileChooserDialog *) gtk_file_chooser_dialog_new("open file",
								      (GtkWindow *) window,
								      GTK_FILE_CHOOSER_ACTION_OPEN,
								      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
								      GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
								      NULL);
  gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(file_chooser), FALSE);
  gtk_widget_show_all((GtkWidget *) file_chooser);

  g_signal_connect((GObject *) file_chooser, "response",
		   G_CALLBACK(ags_menu_bar_open_response_callback), menu_bar);
}

void
ags_menu_bar_open_response_callback(GtkFileChooserDialog *file_chooser, gint response, AgsMenuBar *menu_bar)
{
  if(response == GTK_RESPONSE_ACCEPT){
    AgsFile *file;
    AgsSaveFile *save_file;

    char *filename;
    gchar *str;
    
    GError *error;

    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));

    error = NULL;

    str = g_strdup_printf("gsequencer --filename %s",
			  filename);
    g_spawn_command_line_async(str,
			       &error);

    g_free(filename);
    g_free(str);
  }

  gtk_widget_destroy((GtkWidget *) file_chooser);
}

void
ags_menu_bar_save_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;
  AgsApplicationContext *application_context;
  
  GError *error;

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) menu_bar);
  application_context = (AgsApplicationContext *) window->application_context;

  if(g_strcmp0(ags_config_get_value(application_context->config,
				    AGS_CONFIG_GENERIC,
				    "simple-file"),
	       "false")){
    AgsSimpleFile *simple_file;

    simple_file = (AgsSimpleFile *) g_object_new(AGS_TYPE_SIMPLE_FILE,
						 "application-context", window->application_context,
						 "filename", window->name,
						 NULL);
      
    error = NULL;
    ags_simple_file_rw_open(simple_file,
			    TRUE,
			    &error);
    ags_simple_file_write(simple_file);
    ags_simple_file_close(simple_file);
    g_object_unref(G_OBJECT(simple_file));
  }else{
    AgsFile *file;

    file = (AgsFile *) g_object_new(AGS_TYPE_FILE,
				    "application-context", window->application_context,
				    "filename", window->name,
				    NULL);
      
    error = NULL;
    ags_file_rw_open(file,
		     TRUE,
		     &error);
    ags_file_write(file);
    ags_file_close(file);
    g_object_unref(G_OBJECT(file));
  }
}

void
ags_menu_bar_save_as_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;
  GtkFileChooserDialog *file_chooser;
  
  gint response;
        
  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) menu_bar);

  file_chooser = (GtkFileChooserDialog *) gtk_file_chooser_dialog_new("save file as",
								      (GtkWindow *) window,
								      GTK_FILE_CHOOSER_ACTION_SAVE,
								      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
								      GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
								      NULL);
  gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(file_chooser), FALSE);
  gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(file_chooser), TRUE);
  gtk_widget_show_all((GtkWidget *) file_chooser);

  response = gtk_dialog_run(GTK_DIALOG(file_chooser));

  if(response == GTK_RESPONSE_ACCEPT){
    AgsApplicationContext *application_context;

    gchar *filename;
    gchar *str;
    
    GError *error;
    
    application_context = (AgsApplicationContext *) window->application_context;
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));
    
    if(g_strcmp0(ags_config_get_value(application_context->config,
				      AGS_CONFIG_GENERIC,
				      "simple-file"),
		 "false")){
      AgsSimpleFile *simple_file;

      simple_file = (AgsSimpleFile *) g_object_new(AGS_TYPE_SIMPLE_FILE,
						   "application-context", window->application_context,
						   "filename", filename,
						   NULL);
      
      error = NULL;
      ags_simple_file_rw_open(simple_file,
			      TRUE,
			      &error);
      ags_simple_file_write(simple_file);
      ags_simple_file_close(simple_file);
      g_object_unref(G_OBJECT(simple_file));
    }else{
      AgsFile *file;

      file = (AgsFile *) g_object_new(AGS_TYPE_FILE,
				      "application-context", window->application_context,
				      "filename", filename,
				      NULL);
      
      error = NULL;
      ags_file_rw_open(file,
		       TRUE,
		       &error);
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
ags_menu_bar_export_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) menu_bar);

  gtk_widget_show_all((GtkWidget *) window->export_window);
}

void
ags_menu_bar_quit_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;
  GtkDialog *dialog;
  GtkWidget *cancel_button;
  gint response;

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) menu_bar);

  /* ask the user if he wants save to a file */
  dialog = (GtkDialog *) gtk_message_dialog_new(GTK_WINDOW(window),
						GTK_DIALOG_DESTROY_WITH_PARENT,
						GTK_MESSAGE_QUESTION,
						GTK_BUTTONS_YES_NO,
						"Do you want to save '%s'?", window->name);
  cancel_button = gtk_dialog_add_button(dialog,
					GTK_STOCK_CANCEL,
					GTK_RESPONSE_CANCEL);
  gtk_widget_grab_focus(cancel_button);

  response = gtk_dialog_run(dialog);

  if(response == GTK_RESPONSE_YES){
    AgsFile *file;

    //TODO:JK: revise me
    file = (AgsFile *) g_object_new(AGS_TYPE_FILE,
				    "application-context", window->application_context,
				    "filename", window->name,
				    NULL);

    ags_file_write(file);
    g_object_unref(G_OBJECT(file));
  }

  if(response != GTK_RESPONSE_CANCEL){
    ags_application_context_quit(AGS_APPLICATION_CONTEXT(window->application_context));
  }else{
    gtk_widget_destroy(GTK_WIDGET(dialog));
  }
}


void
ags_menu_bar_add_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
}


void
ags_menu_bar_add_panel_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;
  AgsPanel *panel;
  
  AgsAddAudio *add_audio;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;
    
  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) menu_bar, AGS_TYPE_WINDOW);
  
  application_context = (AgsApplicationContext *) window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get task thread */
  task_thread = (AgsTaskThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_TASK_THREAD);

  panel = ags_panel_new(G_OBJECT(window->soundcard));

  add_audio = ags_add_audio_new(window->soundcard,
				AGS_MACHINE(panel)->audio);
  ags_task_thread_append_task(task_thread,
			      AGS_TASK(add_audio));

  gtk_box_pack_start((GtkBox *) window->machines,
		     GTK_WIDGET(panel),
		     FALSE, FALSE, 0);

  AGS_MACHINE(panel)->audio->audio_channels = 2;
  
  ags_audio_set_pads(AGS_MACHINE(panel)->audio,
		     AGS_TYPE_INPUT, 1);
  ags_audio_set_pads(AGS_MACHINE(panel)->audio,
		     AGS_TYPE_OUTPUT, 1);

  ags_connectable_connect(AGS_CONNECTABLE(panel));

  gtk_widget_show_all(GTK_WIDGET(panel));
}

void
ags_menu_bar_add_mixer_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;
  AgsMixer *mixer;

  AgsAddAudio *add_audio;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;
    
  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) menu_bar, AGS_TYPE_WINDOW);

  application_context = (AgsApplicationContext *) window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get task thread */
  task_thread = (AgsTaskThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_TASK_THREAD);

  mixer = ags_mixer_new(G_OBJECT(window->soundcard));

  add_audio = ags_add_audio_new(window->soundcard,
				AGS_MACHINE(mixer)->audio);
  ags_task_thread_append_task(task_thread,
			      AGS_TASK(add_audio));

  gtk_box_pack_start((GtkBox *) window->machines,
		     GTK_WIDGET(mixer),
		     FALSE, FALSE, 0);

  mixer->machine.audio->audio_channels = 2;
  ags_audio_set_pads(mixer->machine.audio,
		     AGS_TYPE_INPUT, 8);
  ags_audio_set_pads(mixer->machine.audio,
		     AGS_TYPE_OUTPUT, 1);

  ags_connectable_connect(AGS_CONNECTABLE(mixer));

  gtk_widget_show_all(GTK_WIDGET(mixer));
}

void
ags_menu_bar_add_drum_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;
  AgsDrum *drum;

  AgsAddAudio *add_audio;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;
    
  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) menu_bar, AGS_TYPE_WINDOW);
  application_context = (AgsApplicationContext *) window->application_context;

  drum = ags_drum_new(G_OBJECT(window->soundcard));

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get task thread */
  task_thread = (AgsTaskThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_TASK_THREAD);
  
  add_audio = ags_add_audio_new(window->soundcard,
				AGS_MACHINE(drum)->audio);
  ags_task_thread_append_task(task_thread,
			      AGS_TASK(add_audio));

  gtk_box_pack_start((GtkBox *) window->machines,
		     GTK_WIDGET(drum),
		     FALSE, FALSE, 0);

  /* */
  drum->machine.audio->audio_channels = 2;

  /* AgsDrumInputPad */
  ags_audio_set_pads(drum->machine.audio, AGS_TYPE_INPUT, 8);
  ags_audio_set_pads(drum->machine.audio, AGS_TYPE_OUTPUT, 1);

  /* connect everything */
  ags_connectable_connect(AGS_CONNECTABLE(drum));

  /* */
  gtk_widget_show_all(GTK_WIDGET(drum));
}

void
ags_menu_bar_add_matrix_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;
  AgsMatrix *matrix;

  AgsAddAudio *add_audio;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;
    
  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) menu_bar, AGS_TYPE_WINDOW);

  matrix = ags_matrix_new(G_OBJECT(window->soundcard));

  application_context = (AgsApplicationContext *) window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get task thread */
  task_thread = (AgsTaskThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_TASK_THREAD);

  add_audio = ags_add_audio_new(window->soundcard,
				AGS_MACHINE(matrix)->audio);
  ags_task_thread_append_task(task_thread,
			      AGS_TASK(add_audio));
  
  gtk_box_pack_start((GtkBox *) window->machines,
		     GTK_WIDGET(matrix),
		     FALSE, FALSE, 0);
  
  /* */
  matrix->machine.audio->audio_channels = 1;

  /* AgsMatrixInputPad */
  ags_audio_set_pads(matrix->machine.audio, AGS_TYPE_INPUT, 78);
  ags_audio_set_pads(matrix->machine.audio, AGS_TYPE_OUTPUT, 1);

  /* connect everything */
  ags_connectable_connect(AGS_CONNECTABLE(matrix));

  /* */
  gtk_widget_show_all(GTK_WIDGET(matrix));
}

void
ags_menu_bar_add_synth_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;
  AgsSynth *synth;

  AgsAddAudio *add_audio;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;
    
  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) menu_bar, AGS_TYPE_WINDOW);

  synth = ags_synth_new(G_OBJECT(window->soundcard));

  application_context = (AgsApplicationContext *) window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get task thread */
  task_thread = (AgsTaskThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_TASK_THREAD);

  add_audio = ags_add_audio_new(window->soundcard,
				AGS_MACHINE(synth)->audio);
  ags_task_thread_append_task(task_thread,
			      AGS_TASK(add_audio));

  gtk_box_pack_start((GtkBox *) window->machines,
		     (GtkWidget *) synth,
		     FALSE, FALSE, 0);

  synth->machine.audio->audio_channels = 1;
  ags_audio_set_pads((AgsAudio*) synth->machine.audio, AGS_TYPE_INPUT, 2);
  ags_audio_set_pads((AgsAudio*) synth->machine.audio, AGS_TYPE_OUTPUT, 78);

  ags_connectable_connect(AGS_CONNECTABLE(synth));

  gtk_widget_show_all((GtkWidget *) synth);
}

void
ags_menu_bar_add_ffplayer_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;
  AgsFFPlayer *ffplayer;

  AgsAddAudio *add_audio;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;
  
  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) menu_bar, AGS_TYPE_WINDOW);
  application_context = (AgsApplicationContext *) window->application_context;
  
  ffplayer = ags_ffplayer_new(G_OBJECT(window->soundcard));

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);    

  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get task thread */
  task_thread = (AgsTaskThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_TASK_THREAD);

  add_audio = ags_add_audio_new(window->soundcard,
				AGS_MACHINE(ffplayer)->audio);
  ags_task_thread_append_task(task_thread,
			      AGS_TASK(add_audio));

  gtk_box_pack_start((GtkBox *) window->machines,
		     (GtkWidget *) ffplayer,
		     FALSE, FALSE, 0);

    //  ffplayer->machine.audio->frequence = ;
  ffplayer->machine.audio->audio_channels = 2;
  ags_audio_set_pads(AGS_MACHINE(ffplayer)->audio, AGS_TYPE_INPUT, 78);
  ags_audio_set_pads(AGS_MACHINE(ffplayer)->audio, AGS_TYPE_OUTPUT, 1);
  
  ags_connectable_connect(AGS_CONNECTABLE(ffplayer));

  gtk_widget_show_all((GtkWidget *) ffplayer);
}

void
ags_menu_bar_add_ladspa_bridge_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;
  AgsLadspaBridge *ladspa_bridge;

  AgsAddAudio *add_audio;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;

  gchar *filename, *effect;
  
  pthread_mutex_t *application_mutex;

  filename = g_object_get_data((GObject *) menu_item,
			       AGS_MENU_ITEM_FILENAME_KEY);
  effect = g_object_get_data((GObject *) menu_item,
			     AGS_MENU_ITEM_EFFECT_KEY);
  
  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) menu_bar, AGS_TYPE_WINDOW);
  application_context = (AgsApplicationContext *) window->application_context;

  ladspa_bridge = ags_ladspa_bridge_new(G_OBJECT(window->soundcard),
					filename,
					effect);
  
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get task thread */
  task_thread = (AgsTaskThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_TASK_THREAD);
  
  add_audio = ags_add_audio_new(window->soundcard,
				AGS_MACHINE(ladspa_bridge)->audio);
  ags_task_thread_append_task(task_thread,
			      AGS_TASK(add_audio));

  gtk_box_pack_start((GtkBox *) window->machines,
		     GTK_WIDGET(ladspa_bridge),
		     FALSE, FALSE, 0);

  /* */
  ladspa_bridge->machine.audio->audio_channels = 2;

  /*  */
  ags_audio_set_pads(ladspa_bridge->machine.audio, AGS_TYPE_INPUT, 1);
  ags_audio_set_pads(ladspa_bridge->machine.audio, AGS_TYPE_OUTPUT, 1);

  /* connect everything */
  ags_connectable_connect(AGS_CONNECTABLE(ladspa_bridge));

  /*  */
  ags_ladspa_bridge_load(ladspa_bridge);

  /* */
  gtk_widget_show_all(GTK_WIDGET(ladspa_bridge));
}

void
ags_menu_bar_add_dssi_bridge_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;
  AgsDssiBridge *dssi_bridge;

  AgsAddAudio *add_audio;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;

  gchar *filename, *effect;
  
  pthread_mutex_t *application_mutex;

  filename = g_object_get_data((GObject *) menu_item,
			       AGS_MENU_ITEM_FILENAME_KEY);
  effect = g_object_get_data((GObject *) menu_item,
			     AGS_MENU_ITEM_EFFECT_KEY);
  
  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) menu_bar, AGS_TYPE_WINDOW);
  application_context = (AgsApplicationContext *) window->application_context;

  dssi_bridge = ags_dssi_bridge_new(G_OBJECT(window->soundcard),
				    filename,
				    effect);
  
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;
  
  pthread_mutex_unlock(application_mutex);

  /* get task thread */
  task_thread = (AgsTaskThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_TASK_THREAD);
  
  add_audio = ags_add_audio_new(window->soundcard,
				AGS_MACHINE(dssi_bridge)->audio);
  ags_task_thread_append_task(task_thread,
			      AGS_TASK(add_audio));

  gtk_box_pack_start((GtkBox *) window->machines,
		     GTK_WIDGET(dssi_bridge),
		     FALSE, FALSE, 0);
  
  /* */
  dssi_bridge->machine.audio->audio_channels = 2;

  /*  */
  ags_audio_set_pads(dssi_bridge->machine.audio, AGS_TYPE_INPUT, 128);
  ags_audio_set_pads(dssi_bridge->machine.audio, AGS_TYPE_OUTPUT, 1);

  /* connect everything */
  ags_connectable_connect(AGS_CONNECTABLE(dssi_bridge));

  /*  */
  ags_dssi_bridge_load(dssi_bridge);

  /* */
  gtk_widget_show_all(GTK_WIDGET(dssi_bridge));
}

void
ags_menu_bar_add_lv2_bridge_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;
  AgsLv2Bridge *lv2_bridge;

  AgsAddAudio *add_audio;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;

  AgsLv2Plugin *lv2_plugin;

  gchar *filename, *effect;
  
  pthread_mutex_t *application_mutex;
    
  filename = g_object_get_data((GObject *) menu_item,
			       AGS_MENU_ITEM_FILENAME_KEY);
  effect = g_object_get_data((GObject *) menu_item,
			     AGS_MENU_ITEM_EFFECT_KEY);
  
  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) menu_bar, AGS_TYPE_WINDOW);
  application_context = (AgsApplicationContext *) window->application_context;

  lv2_bridge = ags_lv2_bridge_new(G_OBJECT(window->soundcard),
				  filename,
				  effect);
    
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
					       filename, effect);
  
  if(lv2_plugin != NULL &&
     (AGS_LV2_PLUGIN_IS_SYNTHESIZER & (lv2_plugin->flags)) != 0){
    AGS_MACHINE(lv2_bridge)->audio->flags |= (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
					      AGS_AUDIO_INPUT_HAS_RECYCLING |
					      AGS_AUDIO_SYNC |
					      AGS_AUDIO_ASYNC |
					      AGS_AUDIO_HAS_NOTATION | 
					      AGS_AUDIO_NOTATION_DEFAULT |
					      AGS_AUDIO_REVERSE_MAPPING);
    g_object_set(AGS_MACHINE(lv2_bridge)->audio,
		 "audio-start-mapping", 0,
		 "audio-end-mapping", 128,
		 "midi-start-mapping", 0,
		 "midi-end-mapping", 128,
		 NULL);
    
    AGS_MACHINE(lv2_bridge)->flags |= (AGS_MACHINE_IS_SYNTHESIZER |
				       AGS_MACHINE_REVERSE_NOTATION);

    ags_machine_popup_add_connection_options((AgsMachine *) lv2_bridge,
					     (AGS_MACHINE_POPUP_MIDI_DIALOG));

    ags_machine_popup_add_edit_options((AgsMachine *) lv2_bridge,
				       (AGS_MACHINE_POPUP_ENVELOPE));
  }
  
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get task thread */
  task_thread = (AgsTaskThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_TASK_THREAD);
  
  add_audio = ags_add_audio_new(window->soundcard,
				AGS_MACHINE(lv2_bridge)->audio);
  ags_task_thread_append_task(task_thread,
			      AGS_TASK(add_audio));

  gtk_box_pack_start((GtkBox *) window->machines,
		     GTK_WIDGET(lv2_bridge),
		     FALSE, FALSE, 0);
  
  /*  */
  lv2_bridge->machine.audio->audio_channels = 2;

  /*  */
  if(lv2_plugin != NULL){
    if((AGS_LV2_PLUGIN_IS_SYNTHESIZER & (lv2_plugin->flags)) == 0){
      ags_audio_set_pads(lv2_bridge->machine.audio, AGS_TYPE_INPUT, 1);
    }else{
      ags_audio_set_pads(lv2_bridge->machine.audio, AGS_TYPE_INPUT, 128);
    }
  }
  
  ags_audio_set_pads(lv2_bridge->machine.audio, AGS_TYPE_OUTPUT, 1);

  /* connect everything */
  ags_connectable_connect(AGS_CONNECTABLE(lv2_bridge));

  /*  */
  ags_lv2_bridge_load(lv2_bridge);

  /* */
  gtk_widget_show_all(GTK_WIDGET(lv2_bridge));
}

void
ags_menu_bar_add_live_dssi_bridge_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;
  AgsLiveDssiBridge *live_dssi_bridge;

  AgsAddAudio *add_audio;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;

  gchar *filename, *effect;
  
  pthread_mutex_t *application_mutex;

  filename = g_object_get_data((GObject *) menu_item,
			       AGS_MENU_ITEM_FILENAME_KEY);
  effect = g_object_get_data((GObject *) menu_item,
			     AGS_MENU_ITEM_EFFECT_KEY);
  
  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) menu_bar, AGS_TYPE_WINDOW);
  application_context = (AgsApplicationContext *) window->application_context;

  live_dssi_bridge = ags_live_dssi_bridge_new(G_OBJECT(window->soundcard),
					      filename,
					      effect);
  
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;
  
  pthread_mutex_unlock(application_mutex);

  /* get task thread */
  task_thread = (AgsTaskThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_TASK_THREAD);
  
  add_audio = ags_add_audio_new(window->soundcard,
				AGS_MACHINE(live_dssi_bridge)->audio);
  ags_task_thread_append_task(task_thread,
			      AGS_TASK(add_audio));

  gtk_box_pack_start((GtkBox *) window->machines,
		     GTK_WIDGET(live_dssi_bridge),
		     FALSE, FALSE, 0);
  
  /* */
  live_dssi_bridge->machine.audio->audio_channels = 2;

  /*  */
  ags_audio_set_pads(live_dssi_bridge->machine.audio, AGS_TYPE_INPUT, 128);
  ags_audio_set_pads(live_dssi_bridge->machine.audio, AGS_TYPE_OUTPUT, 1);

  /* connect everything */
  ags_connectable_connect(AGS_CONNECTABLE(live_dssi_bridge));

  /*  */
  ags_live_dssi_bridge_load(live_dssi_bridge);

  /* */
  gtk_widget_show_all(GTK_WIDGET(live_dssi_bridge));
}

void
ags_menu_bar_add_live_lv2_bridge_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;
  AgsLiveLv2Bridge *live_lv2_bridge;

  AgsAddAudio *add_audio;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;

  AgsLv2Plugin *lv2_plugin;

  gchar *filename, *effect;
  
  pthread_mutex_t *application_mutex;
    
  filename = g_object_get_data((GObject *) menu_item,
			       AGS_MENU_ITEM_FILENAME_KEY);
  effect = g_object_get_data((GObject *) menu_item,
			     AGS_MENU_ITEM_EFFECT_KEY);
  
  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) menu_bar, AGS_TYPE_WINDOW);
  application_context = (AgsApplicationContext *) window->application_context;

  live_lv2_bridge = ags_live_lv2_bridge_new(G_OBJECT(window->soundcard),
					    filename,
					    effect);
    
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
					       filename, effect);
  
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get task thread */
  task_thread = (AgsTaskThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_TASK_THREAD);
  
  add_audio = ags_add_audio_new(window->soundcard,
				AGS_MACHINE(live_lv2_bridge)->audio);
  ags_task_thread_append_task(task_thread,
			      AGS_TASK(add_audio));

  gtk_box_pack_start((GtkBox *) window->machines,
		     GTK_WIDGET(live_lv2_bridge),
		     FALSE, FALSE, 0);
  
  /*  */
  live_lv2_bridge->machine.audio->audio_channels = 2;

  /*  */
  if(lv2_plugin != NULL){
    if((AGS_LV2_PLUGIN_IS_SYNTHESIZER & (lv2_plugin->flags)) == 0){
      ags_audio_set_pads(live_lv2_bridge->machine.audio, AGS_TYPE_INPUT, 1);
    }else{
      ags_audio_set_pads(live_lv2_bridge->machine.audio, AGS_TYPE_INPUT, 128);
    }
  }
  
  ags_audio_set_pads(live_lv2_bridge->machine.audio, AGS_TYPE_OUTPUT, 1);

  /* connect everything */
  ags_connectable_connect(AGS_CONNECTABLE(live_lv2_bridge));

  /*  */
  ags_live_lv2_bridge_load(live_lv2_bridge);

  /* */
  gtk_widget_show_all(GTK_WIDGET(live_lv2_bridge));
}

void
ags_menu_bar_automation_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) menu_bar,
						 AGS_TYPE_WINDOW);

  gtk_widget_show_all((GtkWidget *) window->automation_window);
}

void
ags_menu_bar_preferences_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) menu_bar, AGS_TYPE_WINDOW);

  if(window->preferences != NULL){
    return;
  }

  window->preferences = ags_preferences_new();
  window->preferences->window = GTK_WINDOW(window);

  ags_connectable_connect(AGS_CONNECTABLE(window->preferences));
  ags_applicable_reset(AGS_APPLICABLE(window->preferences));

  gtk_widget_show_all(GTK_WIDGET(window->preferences));
}

void
ags_menu_bar_midi_import_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) menu_bar, AGS_TYPE_WINDOW);

  if(window->midi_import_wizard != NULL){
    return;
  }

  window->midi_import_wizard = ags_midi_import_wizard_new();
  g_object_set(window->midi_import_wizard,
	       "application-context", window->application_context,
	       "main-window", window,
	       NULL);

  ags_connectable_connect(AGS_CONNECTABLE(window->midi_import_wizard));
  ags_applicable_reset(AGS_APPLICABLE(window->midi_import_wizard));

  gtk_widget_show_all(GTK_WIDGET(window->midi_import_wizard));
}

void
ags_menu_bar_midi_export_track_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) menu_bar, AGS_TYPE_WINDOW);

  if(window->midi_export_wizard != NULL){
    return;
  }

  window->midi_export_wizard = ags_midi_export_wizard_new();
  g_object_set(window->midi_export_wizard,
	       "application-context", window->application_context,
	       "main-window", window,
	       NULL);

  ags_connectable_connect(AGS_CONNECTABLE(window->midi_export_wizard));
  ags_applicable_reset(AGS_APPLICABLE(window->midi_export_wizard));

  gtk_widget_show_all(GTK_WIDGET(window->midi_export_wizard));
}

void
ags_menu_bar_midi_playback_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  //TODO:JK: implement me
}

void
ags_menu_bar_about_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  static FILE *file = NULL;
  struct stat sb;
  static gchar *license;
  static GdkPixbuf *logo;

  gchar *str;
  
  int n_read;
  
  GError *error;

  gchar *authors[] = { "Joël Krähemann", NULL }; 

  if(g_file_test("/usr/share/common-licenses/GPL-3",
		 G_FILE_TEST_EXISTS)){
    if(file == NULL){
      file = fopen("/usr/share/common-licenses/GPL-3", "r");
      stat("/usr/share/common-licenses/GPL-3", &sb);
      license = (gchar *) malloc((sb.st_size + 1) * sizeof(gchar));

      n_read = fread(license, sizeof(char), sb.st_size, file);

      if(n_read != sb.st_size){
	g_critical("fread() number of bytes returned doesn't match buffer size");
      }
      
      license[sb.st_size] = '\0';
      fclose(file);

      error = NULL;

      str = g_strdup_printf("%s%s", DESTDIR, "/gsequencer/images/ags.png");
      logo = gdk_pixbuf_new_from_file(str, &error);

      g_free(str);
    }
  }
  
  gtk_show_about_dialog((GtkWindow *) gtk_widget_get_ancestor((GtkWidget *) menu_bar, GTK_TYPE_WINDOW),
			"program-name", "gsequencer",
			"authors", authors,
			"license", license,
			"version", AGS_VERSION,
			"website", "http://nongnu.org/gsequencer",
			"title", "Advanced Gtk+ Sequencer",
			"logo", logo,
			NULL);
}
