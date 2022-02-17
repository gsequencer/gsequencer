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

#include <ags/GSequencer/editor/ags_wave_toolbar_callbacks.h>

#include <ags/GSequencer/ags_wave_editor.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <math.h>
#include <gdk/gdkkeysyms.h>

void ags_wave_toolbar_zoom_callback_apply(GList *list,
					  gdouble old_zoom_factor,
					  gdouble zoom_factor, gdouble zoom);

void
ags_wave_toolbar_position_callback(GtkToggleToolButton *toggle_tool_button, AgsWaveToolbar *wave_toolbar)
{
  AgsWaveEditor *wave_editor;

  wave_editor = (AgsWaveEditor *) gtk_widget_get_ancestor((GtkWidget *) wave_toolbar,
							  AGS_TYPE_WAVE_EDITOR);
  
  if(toggle_tool_button == wave_toolbar->selected_edit_mode){
    if(!gtk_toggle_tool_button_get_active(toggle_tool_button)){
      gtk_toggle_tool_button_set_active(toggle_tool_button, TRUE);
    }
  }else if(gtk_toggle_tool_button_get_active(toggle_tool_button)){
    GtkToggleToolButton *old_selected_edit_mode;
    
    old_selected_edit_mode = wave_toolbar->selected_edit_mode;
    wave_toolbar->selected_edit_mode = toggle_tool_button;
    gtk_toggle_tool_button_set_active(old_selected_edit_mode, FALSE);
  }
}

void
ags_wave_toolbar_select_callback(GtkToggleToolButton *toggle_tool_button, AgsWaveToolbar *wave_toolbar)
{
  if(toggle_tool_button == wave_toolbar->selected_edit_mode){
    if(!gtk_toggle_tool_button_get_active(toggle_tool_button)){
      gtk_toggle_tool_button_set_active(toggle_tool_button, TRUE);
    }
  }else if(gtk_toggle_tool_button_get_active(toggle_tool_button)){
    GtkToggleToolButton *old_selected_edit_mode;
    
    old_selected_edit_mode = wave_toolbar->selected_edit_mode;
    wave_toolbar->selected_edit_mode = toggle_tool_button;
    gtk_toggle_tool_button_set_active(old_selected_edit_mode, FALSE);
  }
}

void
ags_wave_toolbar_copy_or_cut_callback(GtkWidget *widget, AgsWaveToolbar *wave_toolbar)
{
  AgsWaveEditor *wave_editor;

  wave_editor = AGS_WAVE_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(wave_toolbar),
							AGS_TYPE_WAVE_EDITOR));

  if(widget == (GtkWidget *) wave_toolbar->copy){
    ags_wave_editor_copy(wave_editor);
  }else{
    ags_wave_editor_cut(wave_editor);
  }
}

void
ags_wave_toolbar_paste_callback(GtkWidget *widget, AgsWaveToolbar *wave_toolbar)
{
  AgsWaveEditor *wave_editor;

  wave_editor = AGS_WAVE_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(wave_toolbar),
							AGS_TYPE_WAVE_EDITOR));

  ags_wave_editor_paste(wave_editor);
}

void
ags_wave_toolbar_match_line_callback(GtkWidget *widget, AgsWaveToolbar *wave_toolbar)
{
  AgsWaveEditor *wave_editor;

  wave_editor = AGS_WAVE_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(wave_toolbar),
							AGS_TYPE_WAVE_EDITOR));

  if(gtk_check_menu_item_get_active((GtkCheckMenuItem *) widget)){
    wave_editor->flags |= AGS_WAVE_EDITOR_PASTE_MATCH_LINE;
  }else{
    wave_editor->flags &= (~AGS_WAVE_EDITOR_PASTE_MATCH_LINE);
  }
}

void
ags_wave_toolbar_tool_popup_select_buffer_callback(GtkWidget *item, AgsWaveToolbar *wave_toolbar)
{
  gtk_widget_show_all((GtkWidget *) wave_toolbar->select_buffer);
}

void
ags_wave_toolbar_tool_popup_position_cursor_callback(GtkWidget *item, AgsWaveToolbar *wave_toolbar)
{
  gtk_widget_show_all((GtkWidget *) wave_toolbar->position_wave_cursor);  
}

void
ags_wave_toolbar_tool_popup_enable_all_lines_callback(GtkWidget *item, AgsWaveToolbar *wave_toolbar)
{
  AgsWaveEditor *wave_editor;
  AgsNotebook *notebook;
  
  wave_editor = (AgsWaveEditor *) gtk_widget_get_ancestor((GtkWidget *) wave_toolbar,
							  AGS_TYPE_WAVE_EDITOR);

  /* enable */
  notebook = wave_editor->notebook;

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
ags_wave_toolbar_tool_popup_disable_all_lines_callback(GtkWidget *item, AgsWaveToolbar *wave_toolbar)
{
  AgsWaveEditor *wave_editor;
  AgsNotebook *notebook;
  
  wave_editor = (AgsWaveEditor *) gtk_widget_get_ancestor((GtkWidget *) wave_toolbar,
							  AGS_TYPE_WAVE_EDITOR);

  /* disable */
  notebook = wave_editor->notebook;

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
ags_wave_toolbar_zoom_callback_apply(GList *list,
				     gdouble old_zoom_factor,
				     gdouble zoom_factor, gdouble zoom)
{
  AgsWaveEdit *wave_edit;
    
  while(list != NULL){
    wave_edit = list->data;
    
    gtk_widget_queue_draw((GtkWidget *) wave_edit);
      
    /* reset ruler */
    wave_edit->ruler->factor = zoom_factor;
    wave_edit->ruler->precision = zoom;
    wave_edit->ruler->scale_precision = 1.0 / zoom;
  
    gtk_widget_queue_draw((GtkWidget *) wave_edit->ruler);

    list = list->next;
  }
}

void
ags_wave_toolbar_zoom_callback(GtkComboBox *combo_box, AgsWaveToolbar *wave_toolbar)
{
  AgsWaveEditor *wave_editor;
  GtkWidget *widget;
  
  GtkAdjustment *adjustment;

  GList *list_start;

  gdouble old_value;
  gdouble old_zoom_factor;
  gdouble zoom_factor, zoom;
    
  wave_editor = (AgsWaveEditor *) gtk_widget_get_ancestor((GtkWidget *) wave_toolbar,
							  AGS_TYPE_WAVE_EDITOR);
  
  /* zoom */
  old_zoom_factor = exp2(6.0 - (double) wave_toolbar->zoom_history);
  
  wave_toolbar->zoom_history = gtk_combo_box_get_active((GtkComboBox *) wave_toolbar->zoom);

  zoom_factor = exp2(6.0 - (double) wave_toolbar->zoom_history);
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) wave_toolbar->zoom) - 2.0);

  old_value = gtk_range_get_value(wave_editor->hscrollbar);
    
  /* edit */
  wave_editor->ruler->factor = zoom_factor;
  wave_editor->ruler->precision = zoom;
  wave_editor->ruler->scale_precision = 1.0 / zoom;
  
  gtk_widget_queue_draw((GtkWidget *) wave_editor->ruler);

  list_start = gtk_container_get_children((GtkContainer *) wave_editor->scrolled_wave_edit_box->wave_edit_box);
  ags_wave_toolbar_zoom_callback_apply(list_start,
				       old_zoom_factor,
				       zoom_factor, zoom);

  g_list_free(list_start);

  ags_wave_editor_reset_scrollbar(wave_editor);

  gtk_range_set_value(wave_editor->hscrollbar,
		      old_value * old_zoom_factor / zoom_factor);
}

void
ags_wave_toolbar_opacity_callback(GtkSpinButton *spin_button, AgsWaveToolbar *wave_toolbar)
{
  AgsWaveEditor *wave_editor;

  GList *start_list, *list;
  
  wave_editor = (AgsWaveEditor *) gtk_widget_get_ancestor((GtkWidget *) wave_toolbar,
							  AGS_TYPE_WAVE_EDITOR);


  /* queue draw */
  list =
    start_list = gtk_container_get_children((GtkContainer *) wave_editor->scrolled_wave_edit_box->wave_edit_box);

  while(list != NULL){
    gtk_widget_queue_draw((GtkWidget *) list->data);

    list = list->next;
  }

  g_list_free(start_list);
}
