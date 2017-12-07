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

#include <ags/X/editor/ags_notation_toolbar_callbacks.h>

#include <ags/X/ags_notation_editor.h>

#include <math.h>

void
ags_notation_toolbar_position_callback(GtkToggleButton *toggle_button, AgsNotationToolbar *notation_toolbar)
{
  if(toggle_button == notation_toolbar->selected_edit_mode){
    if(!gtk_toggle_button_get_active(toggle_button)){
      gtk_toggle_button_set_active(toggle_button, TRUE);
    }
  }else if(gtk_toggle_button_get_active(toggle_button)){
    GtkToggleButton *old_selected_edit_mode;
    
    old_selected_edit_mode = notation_toolbar->selected_edit_mode;
    notation_toolbar->selected_edit_mode = toggle_button;
    gtk_toggle_button_set_active(old_selected_edit_mode, FALSE);
  }
}

void
ags_notation_toolbar_edit_callback(GtkToggleButton *toggle_button, AgsNotationToolbar *notation_toolbar)
{
  if(toggle_button == notation_toolbar->selected_edit_mode){
    if(!gtk_toggle_button_get_active(toggle_button)){
      gtk_toggle_button_set_active(toggle_button, TRUE);
    }
  }else if(gtk_toggle_button_get_active(toggle_button)){
    GtkToggleButton *old_selected_edit_mode;
    
    old_selected_edit_mode = notation_toolbar->selected_edit_mode;
    notation_toolbar->selected_edit_mode = toggle_button;
    gtk_toggle_button_set_active(old_selected_edit_mode, FALSE);
  }
}

void
ags_notation_toolbar_clear_callback(GtkToggleButton *toggle_button, AgsNotationToolbar *notation_toolbar)
{
  if(toggle_button == notation_toolbar->selected_edit_mode){
    if(!gtk_toggle_button_get_active(toggle_button)){
      gtk_toggle_button_set_active(toggle_button, TRUE);
    }
  }else if(gtk_toggle_button_get_active(toggle_button)){
    GtkToggleButton *old_selected_edit_mode;
    
    old_selected_edit_mode = notation_toolbar->selected_edit_mode;
    notation_toolbar->selected_edit_mode = toggle_button;
    gtk_toggle_button_set_active(old_selected_edit_mode, FALSE);
  }
}

void
ags_notation_toolbar_select_callback(GtkToggleButton *toggle_button, AgsNotationToolbar *notation_toolbar)
{
  if(toggle_button == notation_toolbar->selected_edit_mode){
    if(!gtk_toggle_button_get_active(toggle_button)){
      gtk_toggle_button_set_active(toggle_button, TRUE);
    }
  }else if(gtk_toggle_button_get_active(toggle_button)){
    GtkToggleButton *old_selected_edit_mode;
    
    old_selected_edit_mode = notation_toolbar->selected_edit_mode;
    notation_toolbar->selected_edit_mode = toggle_button;
    gtk_toggle_button_set_active(old_selected_edit_mode, FALSE);
  }
}

void
ags_notation_toolbar_copy_or_cut_callback(GtkWidget *widget, AgsNotationToolbar *notation_toolbar)
{
  AgsNotationEditor *notation_editor;

  /* add notation to root node */
  notation_editor = (AgsNotationEditor *) gtk_widget_get_ancestor(GTK_WIDGET(notation_toolbar),
								  AGS_TYPE_NOTATION_EDITOR);

  if(widget == (GtkWidget *) notation_toolbar->copy){
    ags_notation_editor_copy(notation_editor);
  }else{
    ags_notation_editor_cut(notation_editor);
  }
}

void
ags_notation_toolbar_paste_callback(GtkWidget *widget, AgsNotationToolbar *notation_toolbar)
{
  AgsNotationEditor *notation_editor;

  notation_editor = (AgsNotationEditor *) gtk_widget_get_ancestor(GTK_WIDGET(notation_toolbar), AGS_TYPE_NOTATION_EDITOR);

  ags_notation_editor_paste(notation_editor);
}

void
ags_notation_toolbar_invert_callback(GtkWidget *widget, AgsNotationToolbar *notation_toolbar)
{
  AgsNotationEditor *notation_editor;

  notation_editor = (AgsNotationEditor *) gtk_widget_get_ancestor(GTK_WIDGET(notation_toolbar), AGS_TYPE_NOTATION_EDITOR);

  ags_notation_editor_invert(notation_editor);
}

void
ags_notation_toolbar_tool_popup_move_note_callback(GtkWidget *item, AgsNotationToolbar *notation_toolbar)
{
  gtk_widget_show_all(notation_toolbar->move_note);
}

void
ags_notation_toolbar_tool_popup_crop_note_callback(GtkWidget *item, AgsNotationToolbar *notation_toolbar)
{
  gtk_widget_show_all(notation_toolbar->crop_note);
}

void
ags_notation_toolbar_tool_popup_select_note_callback(GtkWidget *item, AgsNotationToolbar *notation_toolbar)
{
  gtk_widget_show_all(notation_toolbar->select_note);
}

void
ags_notation_toolbar_tool_popup_position_cursor_callback(GtkWidget *item, AgsNotationToolbar *notation_toolbar)
{
  gtk_widget_show_all(notation_toolbar->position_notation_cursor);
}

void
ags_notation_toolbar_zoom_callback(GtkComboBox *combo_box, AgsNotationToolbar *notation_toolbar)
{
  AgsNotationEditor *notation_editor;
  AgsNotationEdit *notation_edit;

  double zoom_factor, zoom;

  notation_editor = gtk_widget_get_ancestor(notation_toolbar,
					    AGS_TYPE_NOTATION_EDITOR);

  notation_edit = notation_editor->notation_edit;

  gtk_widget_queue_draw((GtkWidget *) notation_edit);  
  
  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) notation_toolbar->zoom));
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) notation_toolbar->zoom) - 2.0);

  /* reset ruler */
  notation_edit->ruler->factor = zoom_factor;
  notation_edit->ruler->precision = zoom;
  notation_edit->ruler->scale_precision = 1.0 / zoom;
  
  gtk_widget_queue_draw((GtkWidget *) notation_edit->ruler);
}
