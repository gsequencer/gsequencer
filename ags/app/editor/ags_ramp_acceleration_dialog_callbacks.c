/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/app/editor/ags_ramp_acceleration_dialog_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_automation_window.h>
#include <ags/app/ags_automation_editor.h>
#include <ags/app/ags_machine.h>

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
	gtk_widget_hide((GtkWidget *) ramp_acceleration_dialog);
      }
    }
  }
}

void
ags_ramp_acceleration_dialog_port_callback(GtkComboBox *combo_box,
					   AgsRampAccelerationDialog *ramp_acceleration_dialog)
{
  AgsWindow *window;
  AgsMachine *machine;

  AgsChannel *start_channel;
  AgsChannel *channel, *next_channel;
  
  AgsPluginPort *plugin_port;

  AgsApplicationContext *application_context;
  
  GList *start_port, *port;

  gchar *specifier;

  GType channel_type;
  
  gboolean use_composite_editor;
  
  /* application context */
  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));
  
  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  machine = NULL;

  if(use_composite_editor){
    AgsCompositeEditor *composite_editor;
    
    composite_editor = window->composite_editor;

    machine = composite_editor->selected_machine;

    if(composite_editor->automation_edit->focused_edit == NULL){
      return;
    }
    
    channel_type = AGS_AUTOMATION_EDIT(composite_editor->automation_edit->focused_edit)->channel_type;
  }else{
    AgsAutomationEditor *automation_editor;
    
    automation_editor = window->automation_window->automation_editor;

    machine = automation_editor->selected_machine;

    if(automation_editor->focused_automation_edit == NULL){
      return;
    }
    
    channel_type = automation_editor->focused_automation_edit->channel_type;
  }
  
  if(machine == NULL){
    return;
  }

  /* specifier */
  specifier = gtk_combo_box_text_get_active_text(ramp_acceleration_dialog->port);

  start_port = NULL;
  
  if(channel_type == G_TYPE_NONE){
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
  }else if(channel_type == AGS_TYPE_OUTPUT){
    g_object_get(machine->audio,
		 "output", &start_channel,
		 NULL);

    channel = start_channel;

    if(channel != NULL){
      g_object_ref(channel);
    }
      
    next_channel = NULL;
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
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(start_channel != NULL){
      g_object_unref(start_channel);
    }

    if(next_channel != NULL){
      g_object_unref(next_channel);
    }
  }else if(channel_type == AGS_TYPE_INPUT){
    g_object_get(machine->audio,
		 "input", &start_channel,
		 NULL);

    channel = start_channel;

    if(channel != NULL){
      g_object_ref(channel);
    }
      
    next_channel = NULL;
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
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(start_channel != NULL){
      g_object_unref(start_channel);
    }

    if(next_channel != NULL){
      g_object_unref(next_channel);
    }
  }
  
  /* reset range */
  plugin_port = NULL;
    
  if(start_port != NULL){
    g_object_get(start_port->data,
		 "plugin-port", &plugin_port,
		 NULL);      

    g_list_free_full(start_port,
		     g_object_unref);
  }

  if(plugin_port != NULL){
    gint steps;
    gdouble lower, upper;

    GValue *upper_value, *lower_value;
    
    /* get some fields */
    g_object_get(plugin_port,
		 "upper-value", &upper_value,
		 "lower-value", &lower_value,
		 NULL);
    
    upper = g_value_get_float(upper_value);
    lower = g_value_get_float(lower_value);

    steps = -1;

    if(ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_TOGGLED)){
      steps = 1;
    }
    
    if(ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_INTEGER)){
      steps = upper - lower;
    }

    if(steps == -1){
      steps = AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT;
    }
    
    gtk_spin_button_set_range(ramp_acceleration_dialog->ramp_y0,
			      lower,
			      upper);
    gtk_spin_button_set_increments(ramp_acceleration_dialog->ramp_y0,
				   (upper - lower) / steps,
				   (upper - lower) / steps);
    
    gtk_spin_button_set_range(ramp_acceleration_dialog->ramp_y1,
			      lower,
			      upper);
    gtk_spin_button_set_increments(ramp_acceleration_dialog->ramp_y1,
				   (upper - lower) / steps,
				   (upper - lower) / steps);
    
    gtk_spin_button_set_range(ramp_acceleration_dialog->ramp_step_count,
			      0.0,
			      steps);

    g_object_unref(plugin_port);
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
