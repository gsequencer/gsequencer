/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/app/editor/ags_composite_toolbar_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_composite_editor.h>
#include <ags/app/ags_notation_editor.h>
#include <ags/app/ags_sheet_window.h>
#include <ags/app/ags_sheet_editor.h>
#include <ags/app/ags_automation_window.h>
#include <ags/app/ags_automation_editor.h>
#include <ags/app/ags_wave_window.h>
#include <ags/app/ags_wave_editor.h>

void
ags_composite_toolbar_position_callback(GtkToolButton *button, AgsCompositeToolbar *composite_toolbar)
{
  if(composite_toolbar->block_selected_tool){
    return;
  }

  composite_toolbar->block_selected_tool = TRUE;

  ags_composite_toolbar_set_selected_tool(composite_toolbar,
					  (GtkToggleToolButton *) button);
  
  composite_toolbar->block_selected_tool = FALSE;
}

void
ags_composite_toolbar_edit_callback(GtkToolButton *button, AgsCompositeToolbar *composite_toolbar)
{
  if(composite_toolbar->block_selected_tool){
    return;
  }

  composite_toolbar->block_selected_tool = TRUE;

  ags_composite_toolbar_set_selected_tool(composite_toolbar,
					  (GtkToggleToolButton *) button);
  
  composite_toolbar->block_selected_tool = FALSE;
}

void
ags_composite_toolbar_clear_callback(GtkToolButton *button, AgsCompositeToolbar *composite_toolbar)
{
  if(composite_toolbar->block_selected_tool){
    return;
  }

  composite_toolbar->block_selected_tool = TRUE;

  ags_composite_toolbar_set_selected_tool(composite_toolbar,
					  (GtkToggleToolButton *) button);
  
  composite_toolbar->block_selected_tool = FALSE;
}

void
ags_composite_toolbar_select_callback(GtkToolButton *button, AgsCompositeToolbar *composite_toolbar)
{
  if(composite_toolbar->block_selected_tool){
    return;
  }

  composite_toolbar->block_selected_tool = TRUE;

  ags_composite_toolbar_set_selected_tool(composite_toolbar,
					  (GtkToggleToolButton *) button);
  
  composite_toolbar->block_selected_tool = FALSE;
}

void
ags_composite_toolbar_invert_callback(GtkToolButton *button, AgsCompositeToolbar *composite_toolbar)
{
  AgsCompositeEditor *composite_editor;
  AgsNotationEditor *notation_editor;
  AgsSheetEditor *sheet_editor;
  AgsAutomationEditor *automation_editor;
  AgsWaveEditor *wave_editor;

  AgsApplicationContext *application_context;

  gboolean success;
  
  application_context = ags_application_context_get_instance();

  composite_editor = (AgsCompositeEditor *) ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));
  
  notation_editor = NULL;
  sheet_editor = NULL;
  automation_editor = NULL;
  wave_editor = NULL;

  success = FALSE;
  
  if(composite_editor != NULL){
    ags_composite_editor_invert(composite_editor);
    
    success = TRUE;
  }else{
    notation_editor = gtk_widget_get_ancestor(composite_toolbar,
					      AGS_TYPE_NOTATION_EDITOR);

    if(notation_editor != NULL){
      ags_notation_editor_invert(notation_editor);
      
      success = TRUE;
    }else{
      sheet_editor = gtk_widget_get_ancestor(composite_toolbar,
					     AGS_TYPE_SHEET_EDITOR);
    }

    if(sheet_editor != NULL){
      g_warning("invert not supported");
      
      success = TRUE;
    }else{
      if(notation_editor == NULL){
	automation_editor = gtk_widget_get_ancestor(composite_toolbar,
						    AGS_TYPE_AUTOMATION_EDITOR);
      }
    }

    if(automation_editor != NULL){
      g_warning("invert not supported");
      
      success = TRUE;
    }else{
      if(notation_editor == NULL &&
	 sheet_editor == NULL){
	wave_editor = gtk_widget_get_ancestor(composite_toolbar,
					      AGS_TYPE_WAVE_EDITOR);
      }
    }

    if(wave_editor != NULL){
      g_warning("invert not supported");
      
      success = TRUE;
    }
  }
}

void
ags_composite_toolbar_copy_callback(GtkToolButton *button, AgsCompositeToolbar *composite_toolbar)
{
  AgsCompositeEditor *composite_editor;
  AgsNotationEditor *notation_editor;
  AgsSheetEditor *sheet_editor;
  AgsAutomationEditor *automation_editor;
  AgsWaveEditor *wave_editor;

  AgsApplicationContext *application_context;

  gboolean success;
  
  application_context = ags_application_context_get_instance();

  composite_editor = (AgsCompositeEditor *) ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));
  
  notation_editor = NULL;
  sheet_editor = NULL;
  automation_editor = NULL;
  wave_editor = NULL;

  success = FALSE;
  
  if(composite_editor != NULL){
    ags_composite_editor_copy(composite_editor);
    
    success = TRUE;
  }else{
    notation_editor = gtk_widget_get_ancestor(composite_toolbar,
					      AGS_TYPE_NOTATION_EDITOR);

    if(notation_editor != NULL){
      ags_notation_editor_copy(notation_editor);
      
      success = TRUE;
    }else{
      sheet_editor = gtk_widget_get_ancestor(composite_toolbar,
					     AGS_TYPE_SHEET_EDITOR);
    }

    if(sheet_editor != NULL){
      ags_sheet_editor_copy(sheet_editor);
      
      success = TRUE;
    }else{
      if(notation_editor == NULL){
	automation_editor = gtk_widget_get_ancestor(composite_toolbar,
						    AGS_TYPE_AUTOMATION_EDITOR);
      }
    }

    if(automation_editor != NULL){
      ags_automation_editor_copy(automation_editor);
      
      success = TRUE;
    }else{
      if(notation_editor == NULL &&
	 sheet_editor == NULL){
	wave_editor = gtk_widget_get_ancestor(composite_toolbar,
					      AGS_TYPE_WAVE_EDITOR);
      }
    }

    if(wave_editor != NULL){
      ags_wave_editor_copy(wave_editor);
      
      success = TRUE;
    }
  }
}

void
ags_composite_toolbar_cut_callback(GtkToolButton *button, AgsCompositeToolbar *composite_toolbar)
{
  AgsCompositeEditor *composite_editor;
  AgsNotationEditor *notation_editor;
  AgsSheetEditor *sheet_editor;
  AgsAutomationEditor *automation_editor;
  AgsWaveEditor *wave_editor;

  AgsApplicationContext *application_context;

  gboolean success;
  
  application_context = ags_application_context_get_instance();

  composite_editor = (AgsCompositeEditor *) ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));
  
  notation_editor = NULL;
  sheet_editor = NULL;
  automation_editor = NULL;
  wave_editor = NULL;

  success = FALSE;
  
  if(composite_editor != NULL){
    ags_composite_editor_cut(composite_editor);
    
    success = TRUE;
  }else{
    notation_editor = gtk_widget_get_ancestor(composite_toolbar,
					      AGS_TYPE_NOTATION_EDITOR);

    if(notation_editor != NULL){
      ags_notation_editor_cut(notation_editor);
      
      success = TRUE;
    }else{
      sheet_editor = gtk_widget_get_ancestor(composite_toolbar,
					     AGS_TYPE_SHEET_EDITOR);
    }

    if(sheet_editor != NULL){
      ags_sheet_editor_cut(sheet_editor);
      
      success = TRUE;
    }else{
      if(notation_editor == NULL){
	automation_editor = gtk_widget_get_ancestor(composite_toolbar,
						    AGS_TYPE_AUTOMATION_EDITOR);
      }
    }

    if(automation_editor != NULL){
      ags_automation_editor_cut(automation_editor);
      
      success = TRUE;
    }else{
      if(notation_editor == NULL &&
	 sheet_editor == NULL){
	wave_editor = gtk_widget_get_ancestor(composite_toolbar,
					      AGS_TYPE_WAVE_EDITOR);
      }
    }

    if(wave_editor != NULL){
      ags_wave_editor_cut(wave_editor);
      
      success = TRUE;
    }
  }
}

void
ags_composite_toolbar_paste_callback(GtkToolButton *button, AgsCompositeToolbar *composite_toolbar)
{
  AgsCompositeEditor *composite_editor;
  AgsNotationEditor *notation_editor;
  AgsSheetEditor *sheet_editor;
  AgsAutomationEditor *automation_editor;
  AgsWaveEditor *wave_editor;

  AgsApplicationContext *application_context;

  gboolean success;
  
  application_context = ags_application_context_get_instance();

  composite_editor = (AgsCompositeEditor *) ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));
  
  notation_editor = NULL;
  sheet_editor = NULL;
  automation_editor = NULL;
  wave_editor = NULL;

  success = FALSE;
  
  if(composite_editor != NULL){
    ags_composite_editor_paste(composite_editor);
    
    success = TRUE;
  }else{
    notation_editor = gtk_widget_get_ancestor(composite_toolbar,
					      AGS_TYPE_NOTATION_EDITOR);

    if(notation_editor != NULL){
      ags_notation_editor_paste(notation_editor);
      
      success = TRUE;
    }else{
      sheet_editor = gtk_widget_get_ancestor(composite_toolbar,
					     AGS_TYPE_SHEET_EDITOR);
    }

    if(sheet_editor != NULL){
      ags_sheet_editor_paste(sheet_editor);
      
      success = TRUE;
    }else{
      if(notation_editor == NULL){
	automation_editor = gtk_widget_get_ancestor(composite_toolbar,
						    AGS_TYPE_AUTOMATION_EDITOR);
      }
    }

    if(automation_editor != NULL){
      ags_automation_editor_paste(automation_editor);
      
      success = TRUE;
    }else{
      if(notation_editor == NULL &&
	 sheet_editor == NULL){
	wave_editor = gtk_widget_get_ancestor(composite_toolbar,
					      AGS_TYPE_WAVE_EDITOR);
      }
    }

    if(wave_editor != NULL){
      ags_wave_editor_paste(wave_editor);
      
      success = TRUE;
    }
  }
}

void
ags_composite_toolbar_paste_match_audio_channel_callback(GtkMenuItem *button, AgsCompositeToolbar *composite_toolbar)
{
  AgsCompositeEditor *composite_editor;

  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) composite_toolbar,
								    AGS_TYPE_COMPOSITE_EDITOR);

  if(composite_editor->selected_edit == NULL){
    return;
  }

  if(!gtk_check_menu_item_get_active(button)){
    composite_editor->selected_edit->paste_flags &= (~AGS_COMPOSITE_EDIT_PASTE_MATCH_AUDIO_CHANNEL);
  }else{
    composite_editor->selected_edit->paste_flags |= AGS_COMPOSITE_EDIT_PASTE_MATCH_AUDIO_CHANNEL;
  }
}

void
ags_composite_toolbar_paste_match_line_callback(GtkMenuItem *button, AgsCompositeToolbar *composite_toolbar)
{
  AgsCompositeEditor *composite_editor;

  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) composite_toolbar,
								    AGS_TYPE_COMPOSITE_EDITOR);

  if(composite_editor->selected_edit == NULL){
    return;
  }

  if(!gtk_check_menu_item_get_active(button)){
    composite_editor->selected_edit->paste_flags &= (~AGS_COMPOSITE_EDIT_PASTE_MATCH_LINE);
  }else{
    composite_editor->selected_edit->paste_flags |= AGS_COMPOSITE_EDIT_PASTE_MATCH_LINE;
  }
}

void
ags_composite_toolbar_paste_no_duplicates_callback(GtkMenuItem *button, AgsCompositeToolbar *composite_toolbar)
{
  AgsCompositeEditor *composite_editor;

  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) composite_toolbar,
								    AGS_TYPE_COMPOSITE_EDITOR);

  if(composite_editor->selected_edit == NULL){
    return;
  }

  if(!gtk_check_menu_item_get_active(button)){
    composite_editor->selected_edit->paste_flags &= (~AGS_COMPOSITE_EDIT_PASTE_NO_DUPLICATES);
  }else{
    composite_editor->selected_edit->paste_flags |= AGS_COMPOSITE_EDIT_PASTE_NO_DUPLICATES;
  }
}

void
ags_composite_toolbar_menu_tool_popup_notation_move_note_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar)
{
  gtk_widget_show_all((GtkWidget *) composite_toolbar->notation_move_note);
}

void
ags_composite_toolbar_menu_tool_popup_notation_crop_note_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar)
{
  gtk_widget_show_all((GtkWidget *) composite_toolbar->notation_crop_note);
}

void
ags_composite_toolbar_menu_tool_popup_notation_select_note_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar)
{
  gtk_widget_show_all((GtkWidget *) composite_toolbar->notation_select_note);
}

void
ags_composite_toolbar_menu_tool_popup_notation_position_cursor_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar)
{
  gtk_widget_show_all((GtkWidget *) composite_toolbar->notation_position_cursor);
}

void
ags_composite_toolbar_menu_tool_popup_sheet_position_cursor_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar)
{
  gtk_widget_show_all((GtkWidget *) composite_toolbar->sheet_position_cursor);
}

void
ags_composite_toolbar_menu_tool_popup_sheet_add_page_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar)
{
  gtk_widget_show_all((GtkWidget *) composite_toolbar->sheet_add_page);
}

void
ags_composite_toolbar_menu_tool_popup_sheet_remove_page_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar)
{
  gtk_widget_show_all((GtkWidget *) composite_toolbar->sheet_remove_page);
}

void
ags_composite_toolbar_menu_tool_popup_automation_select_acceleration_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar)
{
  gtk_widget_show_all((GtkWidget *) composite_toolbar->automation_select_acceleration);
}

void
ags_composite_toolbar_menu_tool_popup_automation_ramp_acceleration_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar)
{
  ags_applicable_reset(AGS_APPLICABLE(composite_toolbar->automation_ramp_acceleration));
  
  gtk_widget_show_all((GtkWidget *) composite_toolbar->automation_ramp_acceleration);
}

void
ags_composite_toolbar_menu_tool_popup_automation_position_cursor_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar)
{  
  gtk_widget_show_all((GtkWidget *) composite_toolbar->automation_position_cursor);
}

void
ags_composite_toolbar_menu_tool_popup_wave_select_buffer_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar)
{
  gtk_widget_show_all((GtkWidget *) composite_toolbar->wave_select_buffer);
}

void
ags_composite_toolbar_menu_tool_popup_wave_position_cursor_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar)
{
  gtk_widget_show_all((GtkWidget *) composite_toolbar->wave_position_cursor);
}

void
ags_composite_toolbar_menu_tool_popup_wave_time_stretch_buffer_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar)
{
  gtk_widget_show_all((GtkWidget *) composite_toolbar->wave_time_stretch_buffer);
}

void
ags_composite_toolbar_menu_tool_popup_enable_all_audio_channels_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar)
{
  AgsCompositeEditor *composite_editor;

  GList *list;
  
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) composite_toolbar,
								    AGS_TYPE_COMPOSITE_EDITOR);

  if(composite_editor->selected_edit == NULL){
    return;
  }

  list = composite_editor->selected_edit->channel_selector->tab;

  while(list != NULL){
    gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(list->data)->toggle,
				 TRUE);

    list = list->next;
  }
}

void
ags_composite_toolbar_menu_tool_popup_disable_all_audio_channels_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar)
{
  AgsCompositeEditor *composite_editor;

  GList *list;
  
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) composite_toolbar,
								    AGS_TYPE_COMPOSITE_EDITOR);

  if(composite_editor->selected_edit == NULL){
    return;
  }

  list = composite_editor->selected_edit->channel_selector->tab;

  while(list != NULL){
    gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(list->data)->toggle,
				 FALSE);

    list = list->next;
  }
}

void
ags_composite_toolbar_menu_tool_popup_enable_all_lines_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar)
{
  AgsCompositeEditor *composite_editor;

  GList *list;
  
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) composite_toolbar,
								    AGS_TYPE_COMPOSITE_EDITOR);

  if(composite_editor->selected_edit == NULL){
    return;
  }

  list = composite_editor->selected_edit->channel_selector->tab;

  while(list != NULL){
    gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(list->data)->toggle,
				 TRUE);

    list = list->next;
  }
}

void
ags_composite_toolbar_menu_tool_popup_disable_all_lines_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar)
{
  AgsCompositeEditor *composite_editor;

  GList *list;
  
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) composite_toolbar,
								    AGS_TYPE_COMPOSITE_EDITOR);

  if(composite_editor->selected_edit == NULL){
    return;
  }

  list = composite_editor->selected_edit->channel_selector->tab;

  while(list != NULL){
    gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(list->data)->toggle,
				 FALSE);

    list = list->next;
  }
}

void
ags_composite_toolbar_port_callback(GtkComboBox *combo_box, AgsCompositeToolbar *composite_toolbar)
{
  AgsCompositeEditor *composite_editor;
  AgsMachine *machine;
  
  GtkTreeModel *model;
  GtkTreeIter iter;

  gchar *scope;
  gchar *control_name;

  GType channel_type;
  
  GValue value = G_VALUE_INIT;

  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) composite_toolbar,
								    AGS_TYPE_COMPOSITE_EDITOR);

  machine = composite_editor->selected_machine;
  
  model = gtk_combo_box_get_model(combo_box);

  if(gtk_combo_box_get_active_iter(combo_box, &iter)){
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
      gtk_tree_model_get_value(model,
			       &iter,
			       0, &value);
      
      if(g_value_get_boolean(&value)){
	AgsMachineAutomationPort *current_automation_port;
	
	GList *automation_port;
	
	g_value_set_boolean(&value, FALSE);
	
	gtk_list_store_set_value(GTK_LIST_STORE(model),
			       &iter,
			       0,
			       &value);

	automation_port = ags_machine_automation_port_find_channel_type_with_control_name(machine->enabled_automation_port,
											  channel_type, control_name);

	if(automation_port != NULL){
	  current_automation_port = automation_port->data;
	
	  machine->enabled_automation_port = g_list_remove(machine->enabled_automation_port,
							   current_automation_port);

	  ags_machine_automation_port_free(current_automation_port);
	}
	
	ags_composite_editor_remove_automation_port(composite_editor,
						    channel_type,
						    control_name);
      }else{
	AgsMachineAutomationPort *current_automation_port;
	
	GList *automation_port;
	
	g_value_set_boolean(&value, TRUE);
	
	gtk_list_store_set_value(GTK_LIST_STORE(model),
			       &iter,
			       0,
			       &value);

	automation_port = ags_machine_automation_port_find_channel_type_with_control_name(machine->enabled_automation_port,
											  channel_type, control_name);

	if(automation_port == NULL){
	  current_automation_port = ags_machine_automation_port_alloc(channel_type, control_name);
	  machine->enabled_automation_port = g_list_prepend(machine->enabled_automation_port,
							    current_automation_port);
	}
	
	ags_composite_editor_add_automation_port(composite_editor,
						 channel_type,
						 control_name);
      }
    }
  }
}

void
ags_composite_toolbar_zoom_callback(GtkComboBox *combo_box, AgsCompositeToolbar *composite_toolbar)
{

  AgsCompositeEditor *composite_editor;
  AgsNotationEditor *notation_editor;
  AgsSheetEditor *sheet_editor;
  AgsAutomationEditor *automation_editor;
  AgsWaveEditor *wave_editor;

  AgsApplicationContext *application_context;

  gdouble old_zoom_factor;
  gdouble zoom_factor, zoom;
  gboolean success;
  
  application_context = ags_application_context_get_instance();

  composite_editor = (AgsCompositeEditor *) ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));
  
  notation_editor = NULL;
  sheet_editor = NULL;
  automation_editor = NULL;
  wave_editor = NULL;

  success = FALSE;

  old_zoom_factor = exp2(6.0 - (double) composite_toolbar->selected_zoom);
  
  composite_toolbar->selected_zoom = gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom);

  zoom_factor = exp2(6.0 - (double) composite_toolbar->selected_zoom);
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom) - 2.0);
  
  if(composite_editor != NULL){
    if(composite_editor->notation_edit != NULL){
      gtk_range_set_value(GTK_RANGE(composite_editor->notation_edit->hscrollbar),
			  gtk_range_get_value(GTK_RANGE(composite_editor->notation_edit->hscrollbar)) * old_zoom_factor / zoom_factor);
  
      gtk_widget_queue_draw((GtkWidget *) composite_editor->notation_edit);
      
      /* reset ruler */
      composite_editor->notation_edit->ruler->factor = zoom_factor;
      composite_editor->notation_edit->ruler->precision = zoom;
      composite_editor->notation_edit->ruler->scale_precision = 1.0 / zoom;
  
      gtk_widget_queue_draw((GtkWidget *) composite_editor->notation_edit->ruler);
    }

    if(composite_editor->automation_edit != NULL){
      gtk_range_set_value(GTK_RANGE(composite_editor->automation_edit->hscrollbar),
			  gtk_range_get_value(GTK_RANGE(composite_editor->automation_edit->hscrollbar)) * old_zoom_factor / zoom_factor);
  
      gtk_widget_queue_draw((GtkWidget *) composite_editor->automation_edit);
      
      /* reset ruler */
      composite_editor->automation_edit->ruler->factor = zoom_factor;
      composite_editor->automation_edit->ruler->precision = zoom;
      composite_editor->automation_edit->ruler->scale_precision = 1.0 / zoom;
  
      gtk_widget_queue_draw((GtkWidget *) composite_editor->automation_edit->ruler);
    }
    
    if(composite_editor->wave_edit != NULL){
      gtk_range_set_value(GTK_RANGE(composite_editor->wave_edit->hscrollbar),
			  gtk_range_get_value(GTK_RANGE(composite_editor->wave_edit->hscrollbar)) * old_zoom_factor / zoom_factor);
  
      gtk_widget_queue_draw((GtkWidget *) composite_editor->wave_edit);
      
      /* reset ruler */
      composite_editor->wave_edit->ruler->factor = zoom_factor;
      composite_editor->wave_edit->ruler->precision = zoom;
      composite_editor->wave_edit->ruler->scale_precision = 1.0 / zoom;
  
      gtk_widget_queue_draw((GtkWidget *) composite_editor->wave_edit->ruler);
    }
    
    success = TRUE;
  }else{
    notation_editor = gtk_widget_get_ancestor(composite_toolbar,
					      AGS_TYPE_NOTATION_EDITOR);

    if(notation_editor != NULL){
      AgsNotationEdit *notation_edit;
      
      notation_edit = notation_editor->notation_edit;
      
      ags_notation_edit_reset_hscrollbar(notation_edit);

      gtk_range_set_value(GTK_RANGE(notation_edit->hscrollbar),
			  gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar)) * old_zoom_factor / zoom_factor);
  
      gtk_widget_queue_draw((GtkWidget *) notation_edit);  

      /* reset ruler */
      notation_edit->ruler->factor = zoom_factor;
      notation_edit->ruler->precision = zoom;
      notation_edit->ruler->scale_precision = 1.0 / zoom;
  
      gtk_widget_queue_draw((GtkWidget *) notation_edit->ruler);
      
      success = TRUE;
    }else{
      sheet_editor = gtk_widget_get_ancestor(composite_toolbar,
					     AGS_TYPE_SHEET_EDITOR);
    }

    if(sheet_editor != NULL){
      //TODO:JK: implement me
      
      success = TRUE;
    }else{
      if(notation_editor == NULL){
	automation_editor = gtk_widget_get_ancestor(composite_toolbar,
						    AGS_TYPE_AUTOMATION_EDITOR);
      }
    }

    if(automation_editor != NULL){
      //TODO:JK: implement me
      
      success = TRUE;
    }else{
      if(notation_editor == NULL &&
	 sheet_editor == NULL){
	wave_editor = gtk_widget_get_ancestor(composite_toolbar,
					      AGS_TYPE_WAVE_EDITOR);
      }
    }

    if(wave_editor != NULL){
      //TODO:JK: implement me
      
      success = TRUE;
    }
  }
}

void
ags_composite_toolbar_opacity_callback(GtkSpinButton *spin_button, AgsCompositeToolbar *composite_toolbar)
{
  //TODO:JK: implement me
}