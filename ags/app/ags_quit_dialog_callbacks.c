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

#include <ags/app/ags_quit_dialog_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>

#include <ags/app/machine/ags_audiorec.h>

#include <ags/i18n.h>

void
ags_quit_dialog_accept_callback(GtkButton *button, AgsQuitDialog *quit_dialog)
{
  AgsWindow *window;
  AgsMachine *machine;
  
  AgsApplicationContext *application_context;

  GList *start_list, *list;

  gchar *filename;
  
  gboolean accept_all;    

  application_context = ags_application_context_get_instance();
  
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  filename = NULL;
  
  accept_all = gtk_check_button_get_active(quit_dialog->accept_all);

  ags_quit_dialog_response(quit_dialog,
			   GTK_RESPONSE_ACCEPT);

  if(quit_dialog->current_question == AGS_QUIT_DIALOG_QUESTION_SAVE_FILE){
    /* save file */
    ags_quit_dialog_save_file(quit_dialog);

    /* prepare export */
    list = 
      start_list = ags_window_get_machine(window);
    
    while(list != NULL){
      if(AGS_IS_AUDIOREC(list->data)){
	quit_dialog->wave_export_machine = g_list_prepend(quit_dialog->wave_export_machine,
							  list->data);
      }
      
      list = list->next;
    }

    quit_dialog->nth_wave_export_machine = 0;
    
    quit_dialog->wave_export_machine = g_list_reverse(quit_dialog->wave_export_machine);

    machine = g_list_nth_data(quit_dialog->wave_export_machine,
			      0);

    if(machine != NULL){
      filename = NULL;
      
      if(AGS_IS_AUDIOREC(machine)){
	filename = gtk_editable_get_text(GTK_EDITABLE(AGS_AUDIOREC(machine)->filename));
      }

      gtk_label_set_text(quit_dialog->export_wave_filename,
			 filename);
    }else{
      ags_quit_dialog_response(quit_dialog,
			       GTK_RESPONSE_OK);
    }
    
    g_list_free(start_list);
  }

  if(accept_all ||
     quit_dialog->current_question == AGS_QUIT_DIALOG_QUESTION_EXPORT_WAVE){
    if(accept_all){
      list = quit_dialog->wave_export_machine;
      
      while(list != NULL){
	machine = list->data;

	if(AGS_IS_AUDIOREC(machine)){
	  ags_quit_dialog_fast_export(quit_dialog,
				      machine);
	}
  
	list = list->next;
      }
    }else{
      machine = g_list_nth_data(quit_dialog->wave_export_machine,
				quit_dialog->nth_wave_export_machine);

      if(AGS_IS_AUDIOREC(machine)){
	ags_quit_dialog_fast_export(quit_dialog,
				    machine);
      }
      
      quit_dialog->nth_wave_export_machine += 1;

      machine = g_list_nth_data(quit_dialog->wave_export_machine,
				quit_dialog->nth_wave_export_machine);

      if(machine != NULL){
	filename = NULL;
      
	if(AGS_IS_AUDIOREC(machine)){
	  filename = gtk_editable_get_text(GTK_EDITABLE(AGS_AUDIOREC(machine)->filename));
	}

	gtk_label_set_text(quit_dialog->export_wave_filename,
			   filename);
      }else{
	ags_quit_dialog_response(quit_dialog,
				 GTK_RESPONSE_OK);
      }
    }
  }

  if(quit_dialog->current_question == AGS_QUIT_DIALOG_QUESTION_SAVE_FILE){
    quit_dialog->current_question = AGS_QUIT_DIALOG_QUESTION_EXPORT_WAVE;

    gtk_widget_set_visible((GtkWidget *) quit_dialog->save_file_question,
			   FALSE);

    gtk_widget_set_visible((GtkWidget *) quit_dialog->export_wave_question,
			   TRUE);

    gtk_widget_set_visible((GtkWidget *) quit_dialog->export_wave,
			   TRUE);

    gtk_widget_set_visible((GtkWidget *) quit_dialog->export_wave_filename,
			   TRUE);
  }
  
  if(accept_all ||
     quit_dialog->nth_wave_export_machine >= g_list_length(quit_dialog->wave_export_machine)){
    ags_application_context_quit(AGS_APPLICATION_CONTEXT(application_context));
  }
}

void
ags_quit_dialog_reject_callback(GtkButton *button, AgsQuitDialog *quit_dialog)
{
  AgsWindow *window;
  AgsMachine *machine;
  
  AgsApplicationContext *application_context;

  GList *start_list, *list;
  
  gchar *filename;
  
  gboolean accept_all;    

  application_context = ags_application_context_get_instance();
  
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  filename = NULL;
  
  accept_all = gtk_check_button_get_active(quit_dialog->accept_all);

  ags_quit_dialog_response(quit_dialog,
			   GTK_RESPONSE_REJECT);

  if(quit_dialog->current_question == AGS_QUIT_DIALOG_QUESTION_SAVE_FILE){
    /* prepare export */
    list = 
      start_list = ags_window_get_machine(window);
    
    while(list != NULL){
      if(AGS_IS_AUDIOREC(list->data)){
	quit_dialog->wave_export_machine = g_list_prepend(quit_dialog->wave_export_machine,
							  list->data);
      }
      
      list = list->next;
    }

    quit_dialog->nth_wave_export_machine = 0;
    
    machine = g_list_nth_data(quit_dialog->wave_export_machine,
			      0);

    if(machine != NULL){
      filename = NULL;
      
      if(AGS_IS_AUDIOREC(machine)){
	filename = gtk_editable_get_text(GTK_EDITABLE(AGS_AUDIOREC(machine)->filename));
      }

      gtk_label_set_text(quit_dialog->export_wave_filename,
			 filename);
    }else{
      ags_quit_dialog_response(quit_dialog,
			       GTK_RESPONSE_CANCEL);
    }
    
    g_list_free(start_list);
  }

  if(accept_all ||
     quit_dialog->current_question == AGS_QUIT_DIALOG_QUESTION_EXPORT_WAVE){
    if(accept_all){
      ags_quit_dialog_response(quit_dialog,
			       GTK_RESPONSE_CANCEL);
    }else{      
      quit_dialog->nth_wave_export_machine += 1;
    }
  }

  if(quit_dialog->current_question == AGS_QUIT_DIALOG_QUESTION_SAVE_FILE){
    quit_dialog->current_question = AGS_QUIT_DIALOG_QUESTION_EXPORT_WAVE;

    gtk_widget_set_visible((GtkWidget *) quit_dialog->save_file_question,
			   FALSE);

    gtk_widget_set_visible((GtkWidget *) quit_dialog->export_wave_question,
			   TRUE);

    gtk_widget_set_visible((GtkWidget *) quit_dialog->export_wave,
			   TRUE);

    gtk_widget_set_visible((GtkWidget *) quit_dialog->export_wave_filename,
			   TRUE);
  }

  if(accept_all ||
     quit_dialog->nth_wave_export_machine >= g_list_length(quit_dialog->wave_export_machine)){
    ags_application_context_quit(AGS_APPLICATION_CONTEXT(application_context));
  }
}

void
ags_quit_dialog_cancel_callback(GtkButton *button, AgsQuitDialog *quit_dialog)
{
  ags_quit_dialog_response(quit_dialog,
			   GTK_RESPONSE_CANCEL);
}
