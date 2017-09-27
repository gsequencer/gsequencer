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

#include <ags/X/editor/ags_ramp_acceleration_dialog_callbacks.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_applicable.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_automation_window.h>
#include <ags/X/ags_automation_editor.h>
#include <ags/X/ags_machine.h>

void
ags_ramp_acceleration_dialog_response_callback(GtkWidget *dialog, gint response,
					       AgsRampAccelerationDialog *ramp_acceleration_dialog)
{
  gboolean hide_dialog;

  hide_dialog = TRUE;
  
  switch(response){
  case GTK_RESPONSE_APPLY:
    {
      hide_dialog = FALSE;
    }
  case GTK_RESPONSE_OK:
    {
      ags_applicable_apply(AGS_APPLICABLE(ramp_acceleration_dialog));
    }
  case GTK_RESPONSE_CANCEL:
    {
      if(hide_dialog){
	gtk_widget_hide(ramp_acceleration_dialog);
      }
    }
  }
}

void
ags_ramp_acceleration_dialog_port_callback(GtkComboBox *combo_box,
					   AgsRampAccelerationDialog *ramp_acceleration_dialog)
{
  AgsWindow *window;
  AgsAutomationEditor *automation_editor;
  AgsMachine *machine;

  AgsAudio *audio;
  
  AgsMutexManager *mutex_manager;

  GList *list_automation;
  
  gchar *specifier;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;

  window = ramp_acceleration_dialog->main_window;
  automation_editor = window->automation_window->automation_editor;

  machine = automation_editor->selected_machine;

  audio = machine->audio;

  /* get mutex manager and application mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* specifier */
  list_automation = NULL;
  
  specifier = gtk_combo_box_text_get_active_text(ramp_acceleration_dialog->port);

  if(specifier != NULL &&
     strlen(specifier) > 0){
    pthread_mutex_lock(audio_mutex);
    
    list_automation = ags_automation_find_specifier(audio->automation,
						    specifier);
    
    pthread_mutex_unlock(audio_mutex);
  }
    
  /* reset range */
  if(list_automation != NULL){
    AgsAutomation *automation;

    automation = AGS_AUTOMATION(list_automation->data);
    
    gtk_spin_button_set_range(ramp_acceleration_dialog->ramp_y0,
			      automation->lower,
			      automation->upper);
    gtk_spin_button_set_increments(ramp_acceleration_dialog->ramp_y0,
				   (automation->upper - automation->lower) / automation->steps,
				   (automation->upper - automation->lower) / automation->steps);
    
    gtk_spin_button_set_range(ramp_acceleration_dialog->ramp_y1,
			      automation->lower,
			      automation->upper);
    gtk_spin_button_set_increments(ramp_acceleration_dialog->ramp_y1,
				   (automation->upper - automation->lower) / automation->steps,
				   (automation->upper - automation->lower) / automation->steps);
    
    gtk_spin_button_set_range(ramp_acceleration_dialog->ramp_step_count,
			      0.0,
			      automation->steps);
  }else{
    gtk_spin_button_set_range(ramp_acceleration_dialog->ramp_y0,
			      0.0,
			      0.0);
    gtk_spin_button_set_increments(ramp_acceleration_dialog->ramp_y0,
				   1.0,
				   1.0);
    
    gtk_spin_button_set_range(ramp_acceleration_dialog->ramp_y1,
			      0.0,
			      0.0);
    gtk_spin_button_set_increments(ramp_acceleration_dialog->ramp_y1,
				   1.0,
				   1.0);
    
    gtk_spin_button_set_range(ramp_acceleration_dialog->ramp_step_count,
			      0.0,
			      0.0);
  }
}

void
ags_ramp_acceleration_dialog_machine_changed_callback(AgsAutomationEditor *automation_editor,
						      AgsMachine *machine,
						      AgsRampAccelerationDialog *ramp_acceleration_dialog)
{
  ags_applicable_reset(AGS_APPLICABLE(ramp_acceleration_dialog));
}
