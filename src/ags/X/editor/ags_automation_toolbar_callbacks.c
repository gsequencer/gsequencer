/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/editor/ags_automation_toolbar_callbacks.h>

#include <ags/X/ags_automation_editor.h>

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
ags_automation_toolbar_zoom_callback(GtkComboBox *combo_box, AgsAutomationToolbar *automation_toolbar)
{
  AgsAutomationEditor *automation_editor;
  GtkWidget *widget;
  
  GtkAdjustment *adjustment;
  
  double zoom, zoom_old;
  guint history;

  automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor((GtkWidget *) automation_toolbar,
								      AGS_TYPE_AUTOMATION_EDITOR);

  history = gtk_combo_box_get_active(combo_box);

  zoom = exp2((double) history - 4.0);
  zoom_old = exp2((double) automation_toolbar->zoom_history - 4.0);

  automation_toolbar->zoom_history = history;

  /* refresh automation edit */
  if(automation_editor->audio_automation_edit != NULL){
    automation_editor->audio_automation_edit->flags |= AGS_AUTOMATION_EDIT_RESETING_HORIZONTALLY;
    ags_automation_edit_reset_horizontally(automation_editor->audio_automation_edit,
					   AGS_AUTOMATION_EDIT_RESET_HSCROLLBAR |
					   AGS_AUTOMATION_EDIT_RESET_WIDTH);
    automation_editor->audio_automation_edit->flags &= (~AGS_AUTOMATION_EDIT_RESETING_HORIZONTALLY);
  }
  
  if(automation_editor->output_automation_edit != NULL){
    automation_editor->output_automation_edit->flags |= AGS_AUTOMATION_EDIT_RESETING_HORIZONTALLY;
    ags_automation_edit_reset_horizontally(automation_editor->output_automation_edit,
					   AGS_AUTOMATION_EDIT_RESET_HSCROLLBAR |
					   AGS_AUTOMATION_EDIT_RESET_WIDTH);
    automation_editor->output_automation_edit->flags &= (~AGS_AUTOMATION_EDIT_RESETING_HORIZONTALLY);
  }

  if(automation_editor->input_automation_edit != NULL){
    automation_editor->input_automation_edit->flags |= AGS_AUTOMATION_EDIT_RESETING_HORIZONTALLY;
    ags_automation_edit_reset_horizontally(automation_editor->input_automation_edit,
					   AGS_AUTOMATION_EDIT_RESET_HSCROLLBAR |
					   AGS_AUTOMATION_EDIT_RESET_WIDTH);
    automation_editor->input_automation_edit->flags &= (~AGS_AUTOMATION_EDIT_RESETING_HORIZONTALLY);
  }
}

void
ags_automation_toolbar_port_changed_callback(GtkComboBox *combo_box,
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
  
  gtk_tree_model_get_value(model,
			   &iter,
			   0,
			   &value);

  if(g_value_get_boolean(&value)){
    g_value_set_boolean(&value, FALSE);
  }else{
    g_value_set_boolean(&value, TRUE);
  }
  
  gtk_list_store_set_value(GTK_LIST_STORE(model),
			   &iter,
			   0,
			   &value);

  ags_automation_toolbar_apply_port(automation_toolbar,
				    control_name);

  automation_toolbar->flags &= (~AGS_AUTOMATION_TOOLBAR_RESET_PORT);
}
