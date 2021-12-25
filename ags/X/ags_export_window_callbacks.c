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

#include <ags/X/ags_export_window_callbacks.h>

#include <ags/X/ags_ui_provider.h>
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
  GtkBox *hbox;
  GtkButton *remove_button;
  
  /* create GtkHBox */
  hbox = (GtkHBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				 0);
  gtk_box_pack_start(export_window->export_soundcard,
		     (GtkWidget *) hbox,
		     FALSE, FALSE,
		     0);
    
  /* instantiate export soundcard */
  export_soundcard = (AgsExportSoundcard *) g_object_new(AGS_TYPE_EXPORT_SOUNDCARD,
							 NULL);
  gtk_box_pack_start(hbox,
		     (GtkWidget *) export_soundcard,
		     FALSE, FALSE,
		     0);
  ags_connectable_connect(AGS_CONNECTABLE(export_soundcard));
    
  /* remove button */    
  remove_button = (GtkButton *) gtk_button_new_from_icon_name("list-remove",
							      GTK_ICON_SIZE_BUTTON);
  gtk_box_pack_start(hbox,
		     (GtkWidget *) remove_button,
		     FALSE, FALSE,
		     0);
    
  g_signal_connect(G_OBJECT(remove_button), "clicked",
		   G_CALLBACK(ags_export_window_remove_export_soundcard_callback), export_window);

  /* show all */
  gtk_widget_show_all(GTK_WIDGET(hbox));

  /* refresh card */
  ags_export_soundcard_refresh_card(export_soundcard);
}

void
ags_export_window_remove_export_soundcard_callback(GtkWidget *button,
						   AgsExportWindow *export_window)
{
  GtkBox *hbox;

  hbox = (GtkBox *) gtk_widget_get_ancestor(button,
					    GTK_TYPE_BOX);
  gtk_widget_destroy(GTK_WIDGET(hbox));
}

void
ags_export_window_tact_callback(GtkWidget *spin_button,
				AgsExportWindow *export_window)
{
  AgsWindow *window;

  AgsApplicationContext *application_context;

  GObject *default_soundcard;
  
  gchar *str;
  
  gdouble delay_factor;
  gdouble delay;

  /* retrieve window */
  window = AGS_WINDOW(export_window->main_window);

  application_context = ags_application_context_get_instance();

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  /* get some properties */
  delay_factor = ags_soundcard_get_delay_factor(AGS_SOUNDCARD(default_soundcard));
  delay = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(default_soundcard));

  /* update duration */
  str = ags_time_get_uptime_from_offset(gtk_spin_button_get_value(export_window->tact) * 16.0,
					gtk_spin_button_get_value(window->navigation->bpm),
					delay,
					delay_factor);
  gtk_label_set_text(export_window->duration,
		     str);
  g_free(str);
}

void
ags_export_window_export_callback(GtkWidget *toggle_button,
				  AgsExportWindow *export_window)
{
  AgsWindow *window;
  AgsMachine *machine;
  
  AgsThread *main_loop;
  
  AgsApplicationContext *application_context;
  
  GObject *default_soundcard;

  GList *machines_start;

  gboolean success;

  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
    
  /* collect */  
  machines_start = NULL;

  success = FALSE;

  if(gtk_toggle_button_get_active((GtkToggleButton *) toggle_button)){
    AgsExportOutput *export_output;

    AgsExportThread *export_thread, *current_export_thread;

    GList *export_soundcard, *export_soundcard_start;
    GList *child, *child_start;
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

    export_soundcard =
      export_soundcard_start = gtk_container_get_children(GTK_CONTAINER(export_window->export_soundcard));
    
    all_filename = NULL;
    remove_filename = NULL;
    
    file_exists = FALSE;

    while(export_soundcard != NULL){
      child = 
	child_start = gtk_container_get_children(GTK_CONTAINER(export_soundcard->data));

      while(child != NULL){
	if(AGS_IS_EXPORT_SOUNDCARD(child->data)){
	  break;
	}

	child = child->next;
      }
	
      if(child == NULL ||
	 !AGS_IS_EXPORT_SOUNDCARD(child->data)){
	export_soundcard = export_soundcard->next;
	g_list_free(child_start);
	  
	continue;
      }
      
      filename = gtk_entry_get_text(AGS_EXPORT_SOUNDCARD(child->data)->filename);
      all_filename = g_list_prepend(all_filename,
				    filename);
      
      /* test filename */
      if(filename == NULL ||
	 strlen(filename) == 0){
	export_soundcard = export_soundcard->next;
	g_list_free(child_start);
	  
	continue;
      }
      
      if(g_file_test(filename,
		     G_FILE_TEST_EXISTS)){      
	if(g_file_test(filename,
		       (G_FILE_TEST_IS_DIR | G_FILE_TEST_IS_SYMLINK))){
	  export_soundcard = export_soundcard->next;
	  g_list_free(child_start);
	  
	  continue;
	}

	remove_filename = g_list_prepend(remove_filename,
					 filename);
	file_exists = TRUE;
      }

      g_list_free(child_start);
      
      export_soundcard = export_soundcard->next;
    }

    if(file_exists){
      GtkDialog *dialog;
      gint response;

      dialog = (GtkDialog *) gtk_message_dialog_new((GtkWindow *) export_window,
						    GTK_DIALOG_MODAL,
						    GTK_MESSAGE_QUESTION,
						    GTK_BUTTONS_OK_CANCEL,
						    "Replace existing file(s)?");
      response = gtk_dialog_run(dialog);
      gtk_widget_destroy((GtkWidget *) dialog);

      if(response == GTK_RESPONSE_REJECT ||
	 response == GTK_RESPONSE_CANCEL){
	goto ags_export_window_export_callback_END;
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
    while(machines != NULL){
      machine = AGS_MACHINE(machines->data);

      if((AGS_MACHINE_IS_SEQUENCER & (machine->flags)) != 0 ||
	 (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0){
	g_message("found machine to play!");

	ags_machine_set_run_extended(machine,
				     TRUE,
				     !gtk_toggle_button_get_active((GtkToggleButton *) export_window->exclude_sequencer), TRUE, FALSE, FALSE);
	success = TRUE;
      }else if((AGS_MACHINE_IS_WAVE_PLAYER & (machine->flags)) != 0){
	g_message("found machine to play!");

	ags_machine_set_run_extended(machine,
				     TRUE,
				     FALSE, TRUE, FALSE, FALSE);
	success = TRUE;
      }

      machines = machines->next;
    }

    /* start export thread */
    if(success){
      gchar *str;
      
      guint tic;
      guint format;
      
      gdouble delay;
      
      /* create task */
      delay = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(default_soundcard));

      /*  */
      tic = (gtk_spin_button_get_value(export_window->tact) + 1) * (16.0 * delay);
      
      export_soundcard = export_soundcard_start;
      task = NULL;
      
      while(export_soundcard != NULL){
	child =
	  child_start = gtk_container_get_children(GTK_CONTAINER(export_soundcard->data));

	while(child != NULL){
	  if(AGS_IS_EXPORT_SOUNDCARD(child->data)){
	    break;
	  }

	  child = child->next;
	}
	
	if(child == NULL ||
	   !AGS_IS_EXPORT_SOUNDCARD(child->data)){
	  export_soundcard = export_soundcard->next;
	  g_list_free(child_start);
	  
	  continue;
	}

	current_export_thread = ags_export_thread_find_soundcard(export_thread,
								 AGS_EXPORT_SOUNDCARD(child->data)->soundcard);
	
	
	filename = gtk_entry_get_text(AGS_EXPORT_SOUNDCARD(child->data)->filename);

	export_output = ags_export_output_new(current_export_thread,
					      AGS_EXPORT_SOUNDCARD(child->data)->soundcard,
					      filename,
					      tic,
					      live_performance);

	str = gtk_combo_box_text_get_active_text(AGS_EXPORT_SOUNDCARD(child->data)->output_format);
	format = 0;

	if(!g_ascii_strncasecmp(str,
				"wav",
				4)){
	  format = AGS_EXPORT_OUTPUT_FORMAT_WAV;
	}else if(!g_ascii_strncasecmp(str,
				      "flac",
				      5)){
	  format = AGS_EXPORT_OUTPUT_FORMAT_FLAC;
	}else if(!g_ascii_strncasecmp(str,
				      "ogg",
				      4)){
	  format = AGS_EXPORT_OUTPUT_FORMAT_OGG;
	}

	g_object_set(G_OBJECT(export_output),
		     "format", format,
		     NULL);
	
	task = g_list_prepend(task,
			      export_output);
	
	if(AGS_EXPORT_SOUNDCARD(child->data)->soundcard == default_soundcard){
	  g_signal_connect(current_export_thread, "stop",
			   G_CALLBACK(ags_export_window_stop_callback), export_window);
	}

	g_list_free(child_start);
	
      	export_soundcard = export_soundcard->next;
      }
      
      /* append AgsStartSoundcard */
      task = g_list_reverse(task);
      
      ags_ui_provider_schedule_task_all(AGS_UI_PROVIDER(application_context),
					task);
      
      ags_navigation_set_seeking_sensitive(window->navigation,
					   FALSE);
    }

    g_list_free(export_soundcard_start);
  }else{
    GList *machines;

    machines_start = 
      machines = gtk_container_get_children(GTK_CONTAINER(window->machines));

    /* stop machines */
    while(machines != NULL){
      machine = AGS_MACHINE(machines->data);

      if((AGS_MACHINE_IS_SEQUENCER & (machine->flags)) !=0 ||
	 (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0){
	g_message("found machine to stop!");
    
	ags_machine_set_run_extended(machine,
				     FALSE,
				     TRUE, TRUE, FALSE, FALSE);
	
	success = TRUE;
      }else if((AGS_MACHINE_IS_WAVE_PLAYER & (machine->flags)) != 0){
	g_message("found machine to stop!");
	
	ags_machine_set_run_extended(machine,
				     FALSE,
				     FALSE, FALSE, TRUE, FALSE);
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

 ags_export_window_export_callback_END:

  /* free machine list */
  g_list_free(machines_start);

  g_object_unref(main_loop);
}

void
ags_export_window_stop_callback(AgsThread *thread,
				AgsExportWindow *export_window)
{
  gtk_toggle_button_set_active(export_window->export,
			       FALSE);
}
