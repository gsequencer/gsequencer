/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/X/ags_export_window_callbacks.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_connectable.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_export_thread.h>

#include <ags/audio/task/ags_export_output.h>

#include <ags/X/ags_xorg_application_context.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_navigation.h>
#include <ags/X/ags_export_soundcard.h>

#include <glib/gstdio.h>

void ags_export_window_stop_callback(AgsThread *thread,
				     AgsExportWindow *export_window);

void
ags_export_window_add_export_soundcard_callback(GtkWidget *button,
						AgsExportWindow *export_window)
{
  AgsExportSoundcard *export_soundcard;
  GtkHBox *hbox;
  GtkAlignment *alignment;
  GtkButton *remove_button;
  
  /* create GtkHBox */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE,
				  0);
  gtk_box_pack_start((GtkBox *) export_window->export_soundcard,
		     (GtkWidget *) hbox,
		     FALSE, FALSE,
		     0);
    
  /* instantiate export soundcard */
  export_soundcard = (AgsExportSoundcard *) g_object_new(AGS_TYPE_EXPORT_SOUNDCARD,
							 NULL);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) export_soundcard,
		     FALSE, FALSE,
		     0);
  ags_connectable_connect(AGS_CONNECTABLE(export_soundcard));
    
  /* remove button */
  alignment = (GtkAlignment *) gtk_alignment_new(0.5, 1.0,
						 0.0, 0.0);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) alignment,
		     FALSE, FALSE,
		     0);
    
  remove_button = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_REMOVE);
  gtk_container_add((GtkContainer *) alignment,
		    (GtkWidget *) remove_button);
    
  g_signal_connect(G_OBJECT(remove_button), "clicked\0",
		   G_CALLBACK(ags_export_window_remove_export_soundcard_callback), export_window);

  /* show all */
  gtk_widget_show_all(hbox);
}

void
ags_export_window_remove_export_soundcard_callback(GtkWidget *button,
						   AgsExportWindow *export_window)
{
  GtkHBox *hbox;

  hbox = gtk_widget_get_ancestor(button,
				 GTK_TYPE_HBOX);
  gtk_widget_destroy(hbox);
}

void
ags_export_window_tact_callback(GtkWidget *spin_button,
				AgsExportWindow *export_window)
{
  AgsWindow *window;

  AgsMutexManager *mutex_manager;

  gdouble delay_factor;
  gdouble delay;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *soundcard_mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* retrieve window */
  window = export_window->main_window;

  /* retrieve soundcard mutex */
  pthread_mutex_lock(application_mutex);

  soundcard_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) window->soundcard);
  
  pthread_mutex_unlock(application_mutex);

  /* get some properties */
  pthread_mutex_lock(soundcard_mutex);

  delay_factor = ags_soundcard_get_delay_factor(AGS_SOUNDCARD(window->soundcard));
  delay = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(window->soundcard));

  pthread_mutex_unlock(soundcard_mutex);

  /* update duration */
  gtk_label_set_text(export_window->duration,
		     ags_time_get_uptime_from_offset(gtk_spin_button_get_value(export_window->tact) * 16.0,
						     window->navigation->bpm->adjustment->value,
						     delay,
						     delay_factor));
}

void
ags_export_window_export_callback(GtkWidget *toggle_button,
				  AgsExportWindow *export_window)
{
  AgsWindow *window;
  AgsMachine *machine;
  
  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;
  
  GList *machines_start;
  gboolean success;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *soundcard_mutex;

  window = AGS_XORG_APPLICATION_CONTEXT(export_window->application_context)->window;

  application_context = (AgsApplicationContext *) window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
      
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get task and soundcard thread */
  task_thread = (AgsTaskThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_TASK_THREAD);
  
  machines_start = NULL;

  if(gtk_toggle_button_get_active((GtkToggleButton *) toggle_button)){
    AgsExportOutput *export_output;

    AgsExportThread *export_thread, *current_export_thread;

    GList *export_soundcard, *export_soundcard_start;
    GList *machines;
    GList *all_filename;
    GList *remove_filename;
    GList *task;
    GList *list;
    
    gchar *filename;

    gboolean file_exists;
    gboolean live_performance;

    export_thread = (AgsExportThread *) ags_thread_find_type(main_loop,
							     AGS_TYPE_EXPORT_THREAD);

    export_soundcard_start = 
      export_soundcard = gtk_container_get_children(GTK_CONTAINER(export_window->export_soundcard));
    
    all_filename = NULL;
    remove_filename = NULL;
    
    file_exists = FALSE;

    while(export_soundcard != NULL){
      filename = gtk_entry_get_text(AGS_EXPORT_SOUNDCARD(export_soundcard->data)->filename);
      all_filename = g_list_prepend(all_filename,
				    filename);
      
      /* test filename */
      if(filename == NULL ||
	 strlen(filename) == 0){
	return;
      }
      
      if(g_file_test(filename,
		     G_FILE_TEST_EXISTS)){      
	if(g_file_test(filename,
		       (G_FILE_TEST_IS_DIR | G_FILE_TEST_IS_SYMLINK))){
	  //TODO:JK: improve me
	  
	  return;
	}

	remove_filename = g_list_prepend(remove_filename,
					 filename);
	file_exists = TRUE;
      }
    }

    if(file_exists){
      GtkDialog *dialog;
      gint response;

      dialog = (GtkDialog *) gtk_message_dialog_new((GtkWindow *) export_window,
						    GTK_DIALOG_MODAL,
						    GTK_MESSAGE_QUESTION,
						    GTK_BUTTONS_OK_CANCEL,
						    "Replace existing file(s)?\0");
      response = gtk_dialog_run(dialog);
      gtk_widget_destroy((GtkWidget *) dialog);

      if(response == GTK_RESPONSE_REJECT){
	return;
      }

      /* remove files */
      list = remove_filename;
      
      while(list != NULL){
	g_remove(list->data);

	list = list->next;
      }

      g_list_free(remove_filename);
    }

    /* get some preferences */
    if(export_window->live_export != NULL){
      live_performance = gtk_toggle_button_get_active((GtkToggleButton *) export_window->live_export);
    }else{
      live_performance = TRUE;
    }
    
    machines_start = 
      machines = gtk_container_get_children(GTK_CONTAINER(window->machines));

    /* start machines */
    success = FALSE;

    while(machines != NULL){
      machine = AGS_MACHINE(machines->data);

      if((AGS_MACHINE_IS_SEQUENCER & (machine->flags)) != 0 ||
	 (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0){
	g_message("found machine to play!\0");

	ags_machine_set_run_extended(machine,
				     TRUE,
				     !gtk_toggle_button_get_active((GtkToggleButton *) export_window->exclude_sequencer), TRUE);
	success = TRUE;
      }

      machines = machines->next;
    }

    /* start export thread */
    if(success){
      guint tic;
      gdouble delay;
      gdouble delay_factor;
      
      pthread_mutex_lock(application_mutex);

      soundcard_mutex = ags_mutex_manager_lookup(mutex_manager,
						 (GObject *) window->soundcard);
  
      pthread_mutex_unlock(application_mutex);

      /* create task */
      pthread_mutex_lock(soundcard_mutex);

      delay = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(window->soundcard));
      delay_factor = ags_soundcard_get_delay_factor(AGS_SOUNDCARD(window->soundcard));
	
      pthread_mutex_unlock(soundcard_mutex);

      /*  */
      tic = (gtk_spin_button_get_value(export_window->tact) + 1) * (16.0 * delay);

      export_soundcard = export_soundcard_start;
      
      while(export_soundcard != NULL){
	filename = gtk_entry_get_text(AGS_EXPORT_SOUNDCARD(export_soundcard->data)->filename);
	
	export_output = ags_export_output_new(ags_export_thread_find_soundcard(export_thread,
									       AGS_EXPORT_SOUNDCARD(export_soundcard->data)->soundcard),
					      AGS_EXPORT_SOUNDCARD(export_soundcard->data)->soundcard,
					      filename,
					      tic,
					      live_performance);
	task = g_list_prepend(task,
			      export_output);
	
	if(AGS_EXPORT_SOUNDCARD(export_soundcard->data)->soundcard == window->soundcard){
	  g_signal_connect(export_thread, "stop\0",
			   G_CALLBACK(ags_export_window_stop_callback), export_window);
	}

	export_soundcard = export_soundcard->next;
      }
      
      /* append AgsStartSoundcard */
      task = g_list_reverse(task);
      
      ags_task_thread_append_tasks(task_thread,
				   task);
      
      ags_navigation_set_seeking_sensitive(window->navigation,
					   FALSE);
    }
  }else{
    GList *machines;

    machines_start = 
      machines = gtk_container_get_children(GTK_CONTAINER(window->machines));

    /* stop machines */
    while(machines != NULL){
      machine = AGS_MACHINE(machines->data);

      if((AGS_MACHINE_IS_SEQUENCER & (machine->flags)) !=0 ||
	 (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0){
	printf("found machine to stop!\n\0");
    
	ags_machine_set_run(machine,
			    FALSE);
	
	success = TRUE;
      }

      machines = machines->next;
    }

    /* disable auto-seeking */
    if(success){
      ags_navigation_set_seeking_sensitive(window->navigation,
					   TRUE);
    }
  }

  /* free machine list */
  g_list_free(machines_start);
}

void
ags_export_window_stop_callback(AgsThread *thread,
				AgsExportWindow *export_window)
{
  gtk_toggle_button_set_active(export_window->export,
			       FALSE);
}
