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

#include <ags/X/editor/ags_automation_toolbar_callbacks.h>

#include <ags/X/ags_automation_editor.h>

#include <math.h>

void
ags_automation_toolbar_machine_changed_callback(AgsAutomationEditor *automation_editor,
						AgsMachine *machine,
						AgsAutomationToolbar *toolbar)
{
  AgsAutomation *automation;

  /* load ports */
  ags_automation_toolbar_load_port(automation_editor->automation_toolbar);
}

void
ags_automation_toolbar_position_callback(GtkToggleButton *toggle_button, AgsAutomationToolbar *automation_toolbar)
{
  AgsAutomationEditor *automation_editor;

  automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor((GtkWidget *) automation_toolbar,
								      AGS_TYPE_AUTOMATION_EDITOR);
  
  if(toggle_button == automation_toolbar->selected_edit_mode){
    if(!gtk_toggle_button_get_active(toggle_button)){
      gtk_toggle_button_set_active(toggle_button, TRUE);
    }
  }else if(gtk_toggle_button_get_active(toggle_button)){
    GtkToggleButton *old_selected_edit_mode;
    
    old_selected_edit_mode = automation_toolbar->selected_edit_mode;
    automation_toolbar->selected_edit_mode = toggle_button;
    gtk_toggle_button_set_active(old_selected_edit_mode, FALSE);
  }

  gtk_widget_queue_draw((GtkWidget *) automation_editor->current_audio_automation_edit);
  gtk_widget_queue_draw((GtkWidget *) automation_editor->current_output_automation_edit);
  gtk_widget_queue_draw((GtkWidget *) automation_editor->current_input_automation_edit);
}

void
ags_automation_toolbar_edit_callback(GtkToggleButton *toggle_button, AgsAutomationToolbar *automation_toolbar)
{
  if(toggle_button == automation_toolbar->selected_edit_mode){
    if(!gtk_toggle_button_get_active(toggle_button)){
      gtk_toggle_button_set_active(toggle_button, TRUE);
    }
  }else if(gtk_toggle_button_get_active(toggle_button)){
    GtkToggleButton *old_selected_edit_mode;
    
    old_selected_edit_mode = automation_toolbar->selected_edit_mode;
    automation_toolbar->selected_edit_mode = toggle_button;
    gtk_toggle_button_set_active(old_selected_edit_mode, FALSE);
  }
}

void
ags_automation_toolbar_clear_callback(GtkToggleButton *toggle_button, AgsAutomationToolbar *automation_toolbar)
{
  if(toggle_button == automation_toolbar->selected_edit_mode){
    if(!gtk_toggle_button_get_active(toggle_button)){
      gtk_toggle_button_set_active(toggle_button, TRUE);
    }
  }else if(gtk_toggle_button_get_active(toggle_button)){
    GtkToggleButton *old_selected_edit_mode;
    
    old_selected_edit_mode = automation_toolbar->selected_edit_mode;
    automation_toolbar->selected_edit_mode = toggle_button;
    gtk_toggle_button_set_active(old_selected_edit_mode, FALSE);
  }
}

void
ags_automation_toolbar_select_callback(GtkToggleButton *toggle_button, AgsAutomationToolbar *automation_toolbar)
{
  if(toggle_button == automation_toolbar->selected_edit_mode){
    if(!gtk_toggle_button_get_active(toggle_button)){
      gtk_toggle_button_set_active(toggle_button, TRUE);
    }
  }else if(gtk_toggle_button_get_active(toggle_button)){
    GtkToggleButton *old_selected_edit_mode;
    
    old_selected_edit_mode = automation_toolbar->selected_edit_mode;
    automation_toolbar->selected_edit_mode = toggle_button;
    gtk_toggle_button_set_active(old_selected_edit_mode, FALSE);
  }
}

void
ags_automation_toolbar_copy_or_cut_callback(GtkWidget *widget, AgsAutomationToolbar *automation_toolbar)
{
  AgsAutomationEditor *automation_editor;

  automation_editor = AGS_AUTOMATION_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(automation_toolbar),
								    AGS_TYPE_AUTOMATION_EDITOR));

  if(widget == (GtkWidget *) automation_toolbar->copy){
    ags_automation_editor_copy(automation_editor);
  }else{
    ags_automation_editor_cut(automation_editor);
  }
}

void
ags_automation_toolbar_paste_callback(GtkWidget *widget, AgsAutomationToolbar *automation_toolbar)
{
  AgsAutomationEditor *automation_editor;

  automation_editor = AGS_AUTOMATION_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(automation_toolbar),
								    AGS_TYPE_AUTOMATION_EDITOR));

  ags_automation_editor_paste(automation_editor);
}

void
ags_automation_toolbar_tool_popup_select_acceleration_callback(GtkWidget *item, AgsAutomationToolbar *automation_toolbar)
{
  gtk_widget_show_all(automation_toolbar->select_acceleration);
}

void
ags_automation_toolbar_tool_popup_ramp_acceleration_callback(GtkWidget *item, AgsAutomationToolbar *automation_toolbar)
{
  gtk_widget_show_all(automation_toolbar->ramp_acceleration);  
}

void
ags_automation_toolbar_tool_popup_position_cursor_callback(GtkWidget *item, AgsAutomationToolbar *automation_toolbar)
{
  gtk_widget_show_all(automation_toolbar->position_automation_cursor);  
}

void
ags_automation_toolbar_zoom_callback(GtkComboBox *combo_box, AgsAutomationToolbar *automation_toolbar)
{
  AgsAutomationEditor *automation_editor;
  GtkWidget *widget;
  
  GtkAdjustment *adjustment;
  
  guint history;

  automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor((GtkWidget *) automation_toolbar,
								      AGS_TYPE_AUTOMATION_EDITOR);

  history = gtk_combo_box_get_active(combo_box);

  automation_toolbar->zoom_history = history;

  /* refresh automation edit */
  if(automation_editor->current_audio_automation_edit != NULL){
    AGS_AUTOMATION_EDIT(automation_editor->current_audio_automation_edit)->flags |= AGS_AUTOMATION_EDIT_RESETING_HORIZONTALLY;
    ags_automation_edit_reset_horizontally((AgsAutomationEdit *) automation_editor->current_audio_automation_edit,
					   AGS_AUTOMATION_EDIT_RESET_HSCROLLBAR |
					   AGS_AUTOMATION_EDIT_RESET_WIDTH);
    AGS_AUTOMATION_EDIT(automation_editor->current_audio_automation_edit)->flags &= (~AGS_AUTOMATION_EDIT_RESETING_HORIZONTALLY);
  }
  
  if(automation_editor->current_output_automation_edit != NULL){
    AGS_AUTOMATION_EDIT(automation_editor->current_output_automation_edit)->flags |= AGS_AUTOMATION_EDIT_RESETING_HORIZONTALLY;
    ags_automation_edit_reset_horizontally((AgsAutomationEdit *) automation_editor->current_output_automation_edit,
					   AGS_AUTOMATION_EDIT_RESET_HSCROLLBAR |
					   AGS_AUTOMATION_EDIT_RESET_WIDTH);
    AGS_AUTOMATION_EDIT(automation_editor->current_output_automation_edit)->flags &= (~AGS_AUTOMATION_EDIT_RESETING_HORIZONTALLY);
  }

  if(automation_editor->current_input_automation_edit != NULL){
    AGS_AUTOMATION_EDIT(automation_editor->current_input_automation_edit)->flags |= AGS_AUTOMATION_EDIT_RESETING_HORIZONTALLY;
    ags_automation_edit_reset_horizontally((AgsAutomationEdit *) automation_editor->current_input_automation_edit,
					   AGS_AUTOMATION_EDIT_RESET_HSCROLLBAR |
					   AGS_AUTOMATION_EDIT_RESET_WIDTH);
    AGS_AUTOMATION_EDIT(automation_editor->current_input_automation_edit)->flags &= (~AGS_AUTOMATION_EDIT_RESETING_HORIZONTALLY);
  }
}

void
ags_automation_toolbar_port_callback(GtkComboBox *combo_box,
				     AgsAutomationToolbar *automation_toolbar)
{
  GtkTreeModel *model;
  GtkTreeIter iter;

  gchar *control_name;

  GValue value = {0,};

  if((AGS_AUTOMATION_TOOLBAR_RESET_PORT & (automation_toolbar->flags)) != 0){
    return;
  }

  automation_toolbar->flags |= AGS_AUTOMATION_TOOLBAR_RESET_PORT;
    
  model = gtk_combo_box_get_model(combo_box);
  gtk_combo_box_get_active_iter(combo_box, &iter);

  gtk_tree_model_get(model,
		     &iter,
		     1, &control_name,
		     -1);

  if(control_name != NULL &&
     g_ascii_strncasecmp(control_name,
			 "",
			 1)){
    ags_automation_toolbar_apply_port(automation_toolbar,
				      control_name);
  }
  
  automation_toolbar->flags &= (~AGS_AUTOMATION_TOOLBAR_RESET_PORT);
}
