/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#include <ags/X/editor/ags_toolbar_callbacks.h>

#include <ags/X/ags_editor.h>

#include <math.h>

gboolean
ags_toolbar_destroy_callback(GtkObject *object, AgsToolbar *toolbar)
{
  ags_toolbar_destroy(object);

  return(FALSE);
}

void
ags_toolbar_show_callback(GtkWidget *widget, AgsToolbar *toolbar)
{
  ags_toolbar_show(widget);
}

void
ags_toolbar_edit_callback(GtkToggleButton *toggle_button, AgsToolbar *toolbar)
{
  if(toggle_button == toolbar->selected_edit_mode){
    if(!gtk_toggle_button_get_active(toggle_button)){
      gtk_toggle_button_set_active(toggle_button, TRUE);
    }
  }else{
    GtkToggleButton *old_selected_edit_mode;
    
    old_selected_edit_mode = toolbar->selected_edit_mode;
    toolbar->selected_edit_mode = toggle_button;
    gtk_toggle_button_set_active(old_selected_edit_mode, FALSE);
  }
}

void
ags_toolbar_clear_callback(GtkToggleButton *toggle_button, AgsToolbar *toolbar)
{
  if(toggle_button == toolbar->selected_edit_mode){
    if(!gtk_toggle_button_get_active(toggle_button)){
      gtk_toggle_button_set_active(toggle_button, TRUE);
    }
  }else{
    GtkToggleButton *old_selected_edit_mode;
    
    old_selected_edit_mode = toolbar->selected_edit_mode;
    toolbar->selected_edit_mode = toggle_button;
    gtk_toggle_button_set_active(old_selected_edit_mode, FALSE);
  }
}

void
ags_toolbar_select_callback(GtkToggleButton *toggle_button, AgsToolbar *toolbar)
{
  if(toggle_button == toolbar->selected_edit_mode){
    if(!gtk_toggle_button_get_active(toggle_button)){
      gtk_toggle_button_set_active(toggle_button, TRUE);
    }
  }else{
    GtkToggleButton *old_selected_edit_mode;
    
    old_selected_edit_mode = toolbar->selected_edit_mode;
    toolbar->selected_edit_mode = toggle_button;
    gtk_toggle_button_set_active(old_selected_edit_mode, FALSE);
  }
}

void
ags_toolbar_copy_callback(GtkWidget *widget, AgsToolbar *toolbar)
{
}

void
ags_toolbar_cut_callback(GtkWidget *widget, AgsToolbar *toolbar)
{
}

void
ags_toolbar_paste_callback(GtkWidget *widget, AgsToolbar *toolbar)
{
}

void
ags_toolbar_zoom_callback(GtkOptionMenu *option, AgsToolbar *toolbar)
{
  AgsEditor *editor;
  GtkWidget *widget;
  GtkAdjustment *adjustment;
  double zoom, zoom_old;
  guint history;

  editor = (AgsEditor *) gtk_widget_get_ancestor((GtkWidget *) toolbar, AGS_TYPE_EDITOR);

  editor->flags |= AGS_EDITOR_RESETING_HORIZONTALLY;
  ags_editor_reset_horizontally(editor, AGS_EDITOR_RESET_HSCROLLBAR |
				AGS_EDITOR_RESET_WIDTH);
  editor->flags &= (~AGS_EDITOR_RESETING_HORIZONTALLY);
}

void
ags_toolbar_tact_callback(GtkOptionMenu *option, AgsToolbar *toolbar)
{
  AgsEditor *editor;
  GtkWidget *widget;
  GtkAdjustment *adjustment;
  double tact, tact_old;
  guint history;

  editor = (AgsEditor *) gtk_widget_get_ancestor((GtkWidget *) toolbar, AGS_TYPE_EDITOR);
  widget = (GtkWidget *) editor->drawing_area;

  history = gtk_option_menu_get_history(option);

  tact = exp2((double) history - 4.0);
  tact_old = exp2((double) toolbar->tact_history - 4.0);

  toolbar->tact_history = history;

  editor->flags |= AGS_EDITOR_RESETING_HORIZONTALLY;
  ags_editor_reset_horizontally(editor, AGS_EDITOR_RESET_HSCROLLBAR |
				AGS_EDITOR_RESET_WIDTH);
  editor->flags &= (~AGS_EDITOR_RESETING_HORIZONTALLY);
}

void
ags_toolbar_mode_default_callback(GtkWidget *widget, AgsToolbar *toolbar)
{
}

void
ags_toolbar_mode_group_channels_callback(GtkWidget *widget, AgsToolbar *toolbar)
{
}

