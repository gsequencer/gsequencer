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

#include <ags/libags.h>
#include <ags/libags-audio.h>

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
  AgsChannel *channel;
  
  AgsPluginPort *plugin_port;

  GList *start_port, *port;

  gchar *specifier;

  window = AGS_WINDOW(ramp_acceleration_dialog->main_window);
  automation_editor = window->automation_window->automation_editor;

  machine = automation_editor->selected_machine;

  if(machine == NULL){
    return;
  }
  
  audio = machine->audio;

  /* specifier */
  specifier = gtk_combo_box_text_get_active_text(ramp_acceleration_dialog->port);

  switch(gtk_notebook_get_current_page(automation_editor->notebook)){
  case 0:
    {
      port = ags_audio_collect_all_audio_ports_by_specifier_and_context(machine->audio,
									specifier,
									TRUE);
      start_port = port;
      
      port = ags_audio_collect_all_audio_ports_by_specifier_and_context(machine->audio,
									specifier,
									FALSE);

      if(start_port != NULL){
	start_port = g_list_concat(start_port,
				   port);
      }else{
	start_port = port;
      }

      port = start_port;
    }
    break;
  case 1:
    {
      g_object_get(machine->audio,
		   "output", &channel,
		   NULL);

      start_port = NULL;
      
      while(channel != NULL){
	port = ags_channel_collect_all_channel_ports_by_specifier_and_context(channel,
									      specifier,
									      TRUE);
	if(start_port != NULL){
	  start_port = g_list_concat(start_port,
				     port);
	}else{
	  start_port = port;
	}
      
	port = ags_channel_collect_all_channel_ports_by_specifier_and_context(channel,
									      specifier,
									      FALSE);

	if(start_port != NULL){
	  start_port = g_list_concat(start_port,
				     port);
	}else{
	  start_port = port;
	}

	/* iterate */
	g_object_get(channel,
		     "next", &channel,
		     NULL);
      }
    }
    break;
  case 2:
    {
      g_object_get(machine->audio,
		   "input", &channel,
		   NULL);

      start_port = NULL;
      
      while(channel != NULL){
	port = ags_channel_collect_all_channel_ports_by_specifier_and_context(channel,
									      specifier,
									      TRUE);
	if(start_port != NULL){
	  start_port = g_list_concat(start_port,
				     port);
	}else{
	  start_port = port;
	}
      
	port = ags_channel_collect_all_channel_ports_by_specifier_and_context(channel,
									      specifier,
									      FALSE);

	if(start_port != NULL){
	  start_port = g_list_concat(start_port,
				     port);
	}else{
	  start_port = port;
	}

	/* iterate */
	g_object_get(channel,
		     "next", &channel,
		     NULL);
      }
    }
    break;
  }
  
  /* reset range */
  plugin_port = NULL;
    
  if(start_port != NULL){
    g_object_get(start_port->data,
		 "plugin-port", &plugin_port,
		 NULL);      

    g_list_free(start_port);
  }

  if(plugin_port != NULL){
    gint scale_steps;
    gdouble lower, upper;

    GValue *upper_value, *lower_value;
    
    /* get some fields */
    g_object_get(plugin_port,
		 "upper-value", &upper_value,
		 "lower-value", &lower_value,
		 "scale-steps", &scale_steps,
		 NULL);
    
    upper = g_value_get_float(upper_value);
    lower = g_value_get_float(lower_value);

    if(scale_steps == -1){
      scale_steps = AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT;
    }
    
    gtk_spin_button_set_range(ramp_acceleration_dialog->ramp_y0,
			      lower,
			      upper);
    gtk_spin_button_set_increments(ramp_acceleration_dialog->ramp_y0,
				   (upper - lower) / scale_steps,
				   (upper - lower) / scale_steps);
    
    gtk_spin_button_set_range(ramp_acceleration_dialog->ramp_y1,
			      lower,
			      upper);
    gtk_spin_button_set_increments(ramp_acceleration_dialog->ramp_y1,
				   (upper - lower) / scale_steps,
				   (upper - lower) / scale_steps);
    
    gtk_spin_button_set_range(ramp_acceleration_dialog->ramp_step_count,
			      0.0,
			      scale_steps);
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
