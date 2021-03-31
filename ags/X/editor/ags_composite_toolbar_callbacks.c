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

#include <ags/X/editor/ags_composite_toolbar_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_composite_editor.h>
#include <ags/X/ags_notation_editor.h>
#include <ags/X/ags_sheet_window.h>
#include <ags/X/ags_sheet_editor.h>
#include <ags/X/ags_automation_window.h>
#include <ags/X/ags_automation_editor.h>
#include <ags/X/ags_wave_window.h>
#include <ags/X/ags_wave_editor.h>

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
  //TODO:JK: implement me
}

void
ags_composite_toolbar_paste_match_line_callback(GtkMenuItem *button, AgsCompositeToolbar *composite_toolbar)
{
  //TODO:JK: implement me
}

void
ags_composite_toolbar_paste_no_duplicates_callback(GtkMenuItem *button, AgsCompositeToolbar *composite_toolbar)
{
  //TODO:JK: implement me
}

void
ags_composite_toolbar_menu_tool_popup_notation_move_note_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar)
{
  //TODO:JK: implement me
}

void
ags_composite_toolbar_menu_tool_popup_notation_crop_note_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar)
{
  //TODO:JK: implement me
}

void
ags_composite_toolbar_menu_tool_popup_notation_select_note_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar)
{
  //TODO:JK: implement me
}

void
ags_composite_toolbar_menu_tool_popup_notation_position_cursor_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar)
{
  //TODO:JK: implement me
}

void
ags_composite_toolbar_menu_tool_popup_sheet_position_cursor_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar)
{
  //TODO:JK: implement me
}

void
ags_composite_toolbar_menu_tool_popup_automation_select_acceleration_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar)
{
  //TODO:JK: implement me
}

void
ags_composite_toolbar_menu_tool_popup_automation_ramp_acceleration_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar)
{
  //TODO:JK: implement me
}

void
ags_composite_toolbar_menu_tool_popup_automation_position_cursor_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar)
{
  //TODO:JK: implement me
}

void
ags_composite_toolbar_menu_tool_popup_wave_select_buffer_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar)
{
  //TODO:JK: implement me
}

void
ags_composite_toolbar_menu_tool_popup_wave_position_cursor_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar)
{
  //TODO:JK: implement me
}

void
ags_composite_toolbar_menu_tool_popup_enable_all_audio_channels_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar)
{
  //TODO:JK: implement me
}

void
ags_composite_toolbar_menu_tool_popup_disable_all_audio_channels_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar)
{
  //TODO:JK: implement me
}

void
ags_composite_toolbar_menu_tool_popup_enable_all_lines_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar)
{
  //TODO:JK: implement me
}

void
ags_composite_toolbar_menu_tool_popup_disable_all_lines_callback(GtkMenuItem *item, AgsCompositeToolbar *composite_toolbar)
{
  //TODO:JK: implement me
}

void
ags_composite_toolbar_zoom_callback(GtkComboBox *combo_box, AgsCompositeToolbar *composite_toolbar)
{
  //TODO:JK: implement me
}

void
ags_composite_toolbar_opacity_callback(GtkSpinButton *spin_button, AgsCompositeToolbar *composite_toolbar)
{
  //TODO:JK: implement me
}
