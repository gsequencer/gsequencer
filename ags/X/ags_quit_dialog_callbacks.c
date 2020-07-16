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

#include <ags/X/ags_quit_dialog_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>

#include <ags/X/machine/ags_audiorec.h>

#include <ags/i18n.h>


void
ags_quit_dialog_response_callback(GtkDialog *dialog,
				  gint response_id,
				  gpointer user_data)
{
  AgsQuitDialog *quit_dialog;

  gchar *fast_export_question = i18n("Do you want to fast export quit?");

  quit_dialog = AGS_QUIT_DIALOG(dialog);

#if AGS_DEBUG
  g_message("question %d; nth_machine %d", quit_dialog->current_question,  quit_dialog->nth_wave_export_machine);
#endif
  
  switch(response_id){
  case GTK_RESPONSE_YES:
  {
    AgsWindow *window;
    
    AgsApplicationContext *application_context;

    gboolean accept_all;

    application_context = ags_application_context_get_instance();

    window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

    accept_all = gtk_toggle_button_get_active((GtkToggleButton *) quit_dialog->accept_all);

    if(quit_dialog->current_question == AGS_QUIT_DIALOG_QUESTION_SAVE_FILE){
      AgsFile *file;

      GList *start_list, *list;
      
      file = (AgsFile *) g_object_new(AGS_TYPE_FILE,
				      "filename", window->name,
				      NULL);

      ags_file_write(file);
      g_object_unref(G_OBJECT(file));

      list = 
	start_list = gtk_container_get_children(window->machines);
      
      while(list != NULL){
	if(AGS_IS_AUDIOREC(list->data)){
	  quit_dialog->wave_export_machine = g_list_prepend(quit_dialog->wave_export_machine,
							    list->data);
	}

	list = list->next;
      }

      quit_dialog->wave_export_machine = g_list_reverse(quit_dialog->wave_export_machine);

      g_list_free(start_list);
    }

    if(accept_all ||
       quit_dialog->current_question == AGS_QUIT_DIALOG_QUESTION_EXPORT_WAVE){
      AgsMachine *machine;

      GList *wave_export_machine;

      gchar *str;
      gchar *filename;
      gchar *machine_name;
	
      wave_export_machine = g_list_nth(quit_dialog->wave_export_machine,
				       quit_dialog->nth_wave_export_machine);


      if(accept_all){
	while(wave_export_machine != NULL){
	  machine = wave_export_machine->data;

	  if(AGS_IS_AUDIOREC(machine)){
	    ags_quit_dialog_fast_export(quit_dialog,
					machine);
	  }
  
	  wave_export_machine = wave_export_machine->next;
	}
      }else{
	if(wave_export_machine != NULL){
	  machine = wave_export_machine->data;

	  if(AGS_IS_AUDIOREC(machine)){
	    ags_quit_dialog_fast_export(quit_dialog,
					machine);
	  }
	}
      }
      
      quit_dialog->nth_wave_export_machine += 1;

      if(quit_dialog->nth_wave_export_machine < g_list_length(quit_dialog->wave_export_machine)){
	gchar *str;
	gchar *filename;
	gchar *machine_name;

	machine = g_list_nth_data(quit_dialog->wave_export_machine,
				  quit_dialog->nth_wave_export_machine);
      
	filename = NULL;

	if(AGS_IS_AUDIOREC(machine)){
	  filename = gtk_entry_get_text(AGS_AUDIOREC(machine)->filename);
	}

	machine_name = NULL;

	if(machine != NULL){
	  machine_name = machine->machine_name;
	}
	
	str = g_strdup_printf("%s\n\n%s:%s\nfilename: %s",
			      fast_export_question,
			      G_OBJECT_TYPE_NAME(machine),
			      machine_name,
			      filename);

	gtk_label_set_text(quit_dialog->question,
			   str);

	g_free(str);
      }
    }

    if(quit_dialog->current_question == AGS_QUIT_DIALOG_QUESTION_SAVE_FILE){
      if(quit_dialog->wave_export_machine != NULL){
	gchar *str;
	gchar *filename;
	
	quit_dialog->nth_wave_export_machine = 0;

	quit_dialog->current_question = AGS_QUIT_DIALOG_QUESTION_EXPORT_WAVE;

	filename = NULL;

	if(AGS_IS_AUDIOREC(quit_dialog->wave_export_machine->data)){
	  filename = gtk_entry_get_text(AGS_AUDIOREC(quit_dialog->wave_export_machine->data)->filename);
	}

	str = g_strdup_printf("%s\n\nmachine: %s:%s\nfilename: %s",
			      fast_export_question,
			      G_OBJECT_TYPE_NAME(quit_dialog->wave_export_machine->data),
			      AGS_MACHINE(quit_dialog->wave_export_machine->data)->machine_name,
			      filename);
	
	gtk_label_set_text(quit_dialog->question,
			   str);
	
	g_free(str);
      }
    }
    
    if(accept_all ||
       quit_dialog->nth_wave_export_machine >= g_list_length(quit_dialog->wave_export_machine)){
      ags_application_context_quit(AGS_APPLICATION_CONTEXT(application_context));
    }
  }
  break;
  case GTK_RESPONSE_NO:
  { 
    AgsWindow *window;
    
    AgsApplicationContext *application_context;

    application_context = ags_application_context_get_instance();

    window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

    if(quit_dialog->current_question == AGS_QUIT_DIALOG_QUESTION_SAVE_FILE){
      GList *start_list, *list;

      list = 
	start_list = gtk_container_get_children(window->machines);

      while(list != NULL){
	if(AGS_IS_AUDIOREC(list->data)){
	  quit_dialog->wave_export_machine = g_list_prepend(quit_dialog->wave_export_machine,
							    list->data);
	}

	list = list->next;
      }
    }
    
    if(quit_dialog->current_question == AGS_QUIT_DIALOG_QUESTION_EXPORT_WAVE){
      quit_dialog->nth_wave_export_machine += 1;

      if(quit_dialog->nth_wave_export_machine < g_list_length(quit_dialog->wave_export_machine)){
	AgsMachine *machine;
      
	gchar *str;
	gchar *filename;
	gchar *machine_name;
      
	machine = g_list_nth_data(quit_dialog->wave_export_machine,
				  quit_dialog->nth_wave_export_machine);
      
	filename = NULL;

	if(AGS_IS_AUDIOREC(machine)){
	  filename = gtk_entry_get_text(AGS_AUDIOREC(machine)->filename);
	}

	machine_name = NULL;

	if(machine != NULL){
	  machine_name = machine->machine_name;
	}
      
	str = g_strdup_printf("%s\n\n%s:%s\nfilename: %s",
			      fast_export_question,
			      G_OBJECT_TYPE_NAME(machine),
			      machine_name,
			      filename);

	gtk_label_set_text(quit_dialog->question,
			   str);

	g_free(str);
      }
    }

    if(quit_dialog->current_question == AGS_QUIT_DIALOG_QUESTION_SAVE_FILE){
      if(quit_dialog->wave_export_machine != NULL){
	gchar *str;
	gchar *filename;

	quit_dialog->nth_wave_export_machine = 0;

	quit_dialog->current_question = AGS_QUIT_DIALOG_QUESTION_EXPORT_WAVE;

	filename = NULL;

	if(AGS_IS_AUDIOREC(quit_dialog->wave_export_machine->data)){
	  filename = gtk_entry_get_text(AGS_AUDIOREC(quit_dialog->wave_export_machine->data)->filename);
	}
	
	str = g_strdup_printf("%s\n\n%s:%s\nfilename: %s",
			      fast_export_question,
			      G_OBJECT_TYPE_NAME(quit_dialog->wave_export_machine->data),
			      AGS_MACHINE(quit_dialog->wave_export_machine->data)->machine_name,
			      filename);

	gtk_label_set_text(quit_dialog->question,
			   str);

	g_free(str);
      }
    }

    if(quit_dialog->nth_wave_export_machine >= g_list_length(quit_dialog->wave_export_machine)){
      ags_application_context_quit(AGS_APPLICATION_CONTEXT(application_context));
    }    
  }
  break;
  case GTK_RESPONSE_CANCEL:
  {
    gtk_widget_destroy(GTK_WIDGET(dialog));
  }
  }
}
