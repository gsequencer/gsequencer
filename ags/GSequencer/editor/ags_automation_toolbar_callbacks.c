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

#include <ags/GSequencer/editor/ags_automation_toolbar_callbacks.h>

#include <ags/GSequencer/ags_automation_editor.h>

#include <math.h>

void ags_automation_toolbar_zoom_callback_apply(GList *list,
						gdouble old_zoom_factor,
						gdouble zoom_factor, gdouble zoom);

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
ags_automation_toolbar_position_callback(GtkToggleToolButton *toggle_button, AgsAutomationToolbar *automation_toolbar)
{
  AgsAutomationEditor *automation_editor;

  automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor((GtkWidget *) automation_toolbar,
								      AGS_TYPE_AUTOMATION_EDITOR);
  
  if(toggle_button == automation_toolbar->selected_edit_mode){
    if(!gtk_toggle_tool_button_get_active(toggle_button)){
      gtk_toggle_tool_button_set_active(toggle_button, TRUE);
    }
  }else if(gtk_toggle_tool_button_get_active(toggle_button)){
    GtkToggleToolButton *old_selected_edit_mode;
    
    old_selected_edit_mode = automation_toolbar->selected_edit_mode;
    automation_toolbar->selected_edit_mode = toggle_button;
    gtk_toggle_tool_button_set_active(old_selected_edit_mode, FALSE);
  }
}

void
ags_automation_toolbar_edit_callback(GtkToggleToolButton *toggle_button, AgsAutomationToolbar *automation_toolbar)
{
  if(toggle_button == automation_toolbar->selected_edit_mode){
    if(!gtk_toggle_tool_button_get_active(toggle_button)){
      gtk_toggle_tool_button_set_active(toggle_button, TRUE);
    }
  }else if(gtk_toggle_tool_button_get_active(toggle_button)){
    GtkToggleToolButton *old_selected_edit_mode;
    
    old_selected_edit_mode = automation_toolbar->selected_edit_mode;
    automation_toolbar->selected_edit_mode = toggle_button;
    gtk_toggle_tool_button_set_active(old_selected_edit_mode, FALSE);
  }
}

void
ags_automation_toolbar_clear_callback(GtkToggleToolButton *toggle_button, AgsAutomationToolbar *automation_toolbar)
{
  if(toggle_button == automation_toolbar->selected_edit_mode){
    if(!gtk_toggle_tool_button_get_active(toggle_button)){
      gtk_toggle_tool_button_set_active(toggle_button, TRUE);
    }
  }else if(gtk_toggle_tool_button_get_active(toggle_button)){
    GtkToggleToolButton *old_selected_edit_mode;
    
    old_selected_edit_mode = automation_toolbar->selected_edit_mode;
    automation_toolbar->selected_edit_mode = toggle_button;
    gtk_toggle_tool_button_set_active(old_selected_edit_mode, FALSE);
  }
}

void
ags_automation_toolbar_select_callback(GtkToggleToolButton *toggle_button, AgsAutomationToolbar *automation_toolbar)
{
  if(toggle_button == automation_toolbar->selected_edit_mode){
    if(!gtk_toggle_tool_button_get_active(toggle_button)){
      gtk_toggle_tool_button_set_active(toggle_button, TRUE);
    }
  }else if(gtk_toggle_tool_button_get_active(toggle_button)){
    GtkToggleToolButton *old_selected_edit_mode;
    
    old_selected_edit_mode = automation_toolbar->selected_edit_mode;
    automation_toolbar->selected_edit_mode = toggle_button;
    gtk_toggle_tool_button_set_active(old_selected_edit_mode, FALSE);
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
ags_automation_toolbar_match_line_callback(GtkWidget *widget, AgsAutomationToolbar *automation_toolbar)
{
  AgsAutomationEditor *automation_editor;

  automation_editor = AGS_AUTOMATION_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(automation_toolbar),
								    AGS_TYPE_AUTOMATION_EDITOR));

  if(gtk_check_menu_item_get_active((GtkCheckMenuItem *) widget)){
    automation_editor->flags |= AGS_AUTOMATION_EDITOR_PASTE_MATCH_LINE;
  }else{
    automation_editor->flags &= (~AGS_AUTOMATION_EDITOR_PASTE_MATCH_LINE);
  }
}

void
ags_automation_toolbar_no_duplicates_callback(GtkWidget *widget, AgsAutomationToolbar *automation_toolbar)
{
  AgsAutomationEditor *automation_editor;

  automation_editor = AGS_AUTOMATION_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(automation_toolbar),
								    AGS_TYPE_AUTOMATION_EDITOR));

  if(gtk_check_menu_item_get_active((GtkCheckMenuItem *) widget)){
    automation_editor->flags |= AGS_AUTOMATION_EDITOR_PASTE_NO_DUPLICATES;
  }else{
    automation_editor->flags &= (~AGS_AUTOMATION_EDITOR_PASTE_NO_DUPLICATES);
  }
}

void
ags_automation_toolbar_tool_popup_select_acceleration_callback(GtkWidget *item, AgsAutomationToolbar *automation_toolbar)
{
  gtk_widget_show_all((GtkWidget *) automation_toolbar->select_acceleration);
}

void
ags_automation_toolbar_tool_popup_ramp_acceleration_callback(GtkWidget *item, AgsAutomationToolbar *automation_toolbar)
{
  gtk_widget_show_all((GtkWidget *) automation_toolbar->ramp_acceleration);  
}

void
ags_automation_toolbar_tool_popup_position_cursor_callback(GtkWidget *item, AgsAutomationToolbar *automation_toolbar)
{
  gtk_widget_show_all((GtkWidget *) automation_toolbar->position_automation_cursor);  
}

void
ags_automation_toolbar_tool_popup_enable_all_lines_callback(GtkWidget *item, AgsAutomationToolbar *automation_toolbar)
{
  AgsAutomationEditor *automation_editor;
  AgsNotebook *notebook;
  
  automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor((GtkWidget *) automation_toolbar,
								      AGS_TYPE_AUTOMATION_EDITOR);

  notebook = NULL;
  
  /* queue draw */
  switch(gtk_notebook_get_current_page(automation_editor->notebook)){
  case 1:
    {
      notebook = automation_editor->output_notebook;
    }
    break;
  case 2:
    {
      notebook = automation_editor->input_notebook;
    }
    break;
  }

  if(notebook != NULL){
    GList *start_list, *list;

    list =
      start_list = g_list_copy(notebook->tab);

    while(list != NULL){
      gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(list->data)->toggle,
				   TRUE);

      list = list->next;
    }
    
    g_list_free(start_list);
  }
}

void
ags_automation_toolbar_tool_popup_disable_all_lines_callback(GtkWidget *item, AgsAutomationToolbar *automation_toolbar)
{
  AgsAutomationEditor *automation_editor;
  AgsNotebook *notebook;
  
  automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor((GtkWidget *) automation_toolbar,
								      AGS_TYPE_AUTOMATION_EDITOR);

  notebook = NULL;
  
  /* queue draw */
  switch(gtk_notebook_get_current_page(automation_editor->notebook)){
  case 1:
    {
      notebook = automation_editor->output_notebook;
    }
    break;
  case 2:
    {
      notebook = automation_editor->input_notebook;
    }
    break;
  }

  if(notebook != NULL){
    GList *start_list, *list;

    list =
      start_list = g_list_copy(notebook->tab);

    while(list != NULL){
      gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(list->data)->toggle,
				   FALSE);

      list = list->next;
    }

    g_list_free(start_list);
  }
}

void
ags_automation_toolbar_zoom_callback_apply(GList *list,
					   gdouble old_zoom_factor,
					   gdouble zoom_factor, gdouble zoom)
{
  AgsAutomationEdit *automation_edit;
    
  while(list != NULL){
    automation_edit = list->data;
      
    gtk_widget_queue_draw((GtkWidget *) automation_edit);
      
    /* reset ruler */
    automation_edit->ruler->factor = zoom_factor;
    automation_edit->ruler->precision = zoom;
    automation_edit->ruler->scale_precision = 1.0 / zoom;
  
    gtk_widget_queue_draw((GtkWidget *) automation_edit->ruler);

    list = list->next;
  }
}

void
ags_automation_toolbar_zoom_callback(GtkComboBox *combo_box, AgsAutomationToolbar *automation_toolbar)
{
  AgsAutomationEditor *automation_editor;
  GtkWidget *widget;
  
  GtkAdjustment *adjustment;

  GList *list_start;

  gdouble old_value;
  gdouble old_zoom_factor;
  gdouble zoom_factor, zoom;
    
  automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor((GtkWidget *) automation_toolbar,
								      AGS_TYPE_AUTOMATION_EDITOR);
  
  /* zoom */
  old_zoom_factor = exp2(6.0 - (double) automation_toolbar->zoom_history);
  
  automation_toolbar->zoom_history = gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom);

  zoom_factor = exp2(6.0 - (double) automation_toolbar->zoom_history);
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom) - 2.0);

  /* audio */
  old_value = gtk_range_get_value(automation_editor->audio_hscrollbar);

  automation_editor->audio_ruler->factor = zoom_factor;
  automation_editor->audio_ruler->precision = zoom;
  automation_editor->audio_ruler->scale_precision = 1.0 / zoom;
  
  gtk_widget_queue_draw((GtkWidget *) automation_editor->audio_ruler);

  list_start = gtk_container_get_children((GtkContainer *) automation_editor->audio_scrolled_automation_edit_box->automation_edit_box);
  ags_automation_toolbar_zoom_callback_apply(list_start,
					     old_zoom_factor,
					     zoom_factor, zoom);

  g_list_free(list_start);

  ags_automation_editor_reset_audio_scrollbar(automation_editor);

  gtk_range_set_value(automation_editor->audio_hscrollbar,
		      old_value * old_zoom_factor / zoom_factor);

  /* output */  
  old_value = gtk_range_get_value(automation_editor->output_hscrollbar);

  automation_editor->output_ruler->factor = zoom_factor;
  automation_editor->output_ruler->precision = zoom;
  automation_editor->output_ruler->scale_precision = 1.0 / zoom;
  
  gtk_widget_queue_draw((GtkWidget *) automation_editor->output_ruler);

  list_start = gtk_container_get_children((GtkContainer *) automation_editor->output_scrolled_automation_edit_box->automation_edit_box);
  ags_automation_toolbar_zoom_callback_apply(list_start,
					     old_zoom_factor,
					     zoom_factor, zoom);

  g_list_free(list_start);

  ags_automation_editor_reset_output_scrollbar(automation_editor);

  gtk_range_set_value(automation_editor->output_hscrollbar,
		      old_value * old_zoom_factor / zoom_factor);

  /* input */
  old_value = gtk_range_get_value(automation_editor->input_hscrollbar);

  automation_editor->input_ruler->factor = zoom_factor;
  automation_editor->input_ruler->precision = zoom;
  automation_editor->input_ruler->scale_precision = 1.0 / zoom;
  
  gtk_widget_queue_draw((GtkWidget *) automation_editor->input_ruler);

  list_start = gtk_container_get_children((GtkContainer *) automation_editor->input_scrolled_automation_edit_box->automation_edit_box);
  ags_automation_toolbar_zoom_callback_apply(list_start,
					     old_zoom_factor,
					     zoom_factor, zoom);

  g_list_free(list_start);

  ags_automation_editor_reset_input_scrollbar(automation_editor);

  gtk_range_set_value(automation_editor->input_hscrollbar,
		      old_value * old_zoom_factor / zoom_factor);
}

void
ags_automation_toolbar_port_callback(GtkComboBox *combo_box,
				     AgsAutomationToolbar *automation_toolbar)
{
  GtkTreeModel *model;
  GtkTreeIter iter;

  gchar *scope;
  gchar *control_name;

  GType channel_type;
  
  GValue value = {0,};

  if((AGS_AUTOMATION_TOOLBAR_RESET_PORT & (automation_toolbar->flags)) != 0){
    return;
  }

  automation_toolbar->flags |= AGS_AUTOMATION_TOOLBAR_RESET_PORT;
    
  model = gtk_combo_box_get_model(combo_box);
  gtk_combo_box_get_active_iter(combo_box, &iter);

  gtk_tree_model_get(model,
		     &iter,
		     1, &scope,
		     2, &control_name,
		     -1);

  channel_type = G_TYPE_NONE;
  
  if(!g_strcmp0("audio",
		scope)){
    channel_type = G_TYPE_NONE;
  }else if(!g_strcmp0("output",
		      scope)){
    channel_type = AGS_TYPE_OUTPUT;
  }else if(!g_strcmp0("input",
		      scope)){
    channel_type = AGS_TYPE_INPUT;
  }

  if(control_name != NULL &&
     strlen(control_name) > 0){
    ags_automation_toolbar_apply_port(automation_toolbar,
				      channel_type, control_name);
  }
  
  automation_toolbar->flags &= (~AGS_AUTOMATION_TOOLBAR_RESET_PORT);
}

void
ags_automation_toolbar_opacity_callback(GtkSpinButton *spin_button, AgsAutomationToolbar *automation_toolbar)
{
  AgsAutomationEditor *automation_editor;

  GList *start_list, *list;
  
  automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor((GtkWidget *) automation_toolbar,
								      AGS_TYPE_AUTOMATION_EDITOR);


  /* queue draw */
  switch(gtk_notebook_get_current_page(automation_editor->notebook)){
  case 0:
    {
      list =
	start_list = gtk_container_get_children((GtkContainer *) automation_editor->audio_scrolled_automation_edit_box->automation_edit_box);
    }
    break;
  case 1:
    {
      list =
	start_list = gtk_container_get_children((GtkContainer *) automation_editor->output_scrolled_automation_edit_box->automation_edit_box);
    }
    break;
  case 2:
    {
      list =
	start_list = gtk_container_get_children((GtkContainer *) automation_editor->input_scrolled_automation_edit_box->automation_edit_box);
    }
    break;
  default:
    {
      list =
	start_list = NULL;
    }
  }
    
  
  while(list != NULL){
    gtk_widget_queue_draw((GtkWidget *) list->data);

    list = list->next;
  }

  g_list_free(start_list);
}
