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
ags_toolbar_edit_callback(GtkWidget *widget, AgsToolbar *toolbar)
{
}

void
ags_toolbar_clear_callback(GtkWidget *widget, AgsToolbar *toolbar)
{
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
ags_toolbar_select_callback(GtkWidget *widget, AgsToolbar *toolbar)
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
  widget = (GtkWidget *) editor->drawing_area;
  adjustment = GTK_RANGE(editor->hscrollbar)->adjustment;

  history = gtk_option_menu_get_history(option);

  zoom = exp2(4.0 - (double) history);
  zoom_old = exp2(4.0 - (double) toolbar->zoom_history);

  toolbar->zoom_history = history;

  /* reset viewport */
  editor->flags |= AGS_EDITOR_RESET_HSCROLLBAR;
  editor->map_width = (guint) ((double) editor->map_width / zoom_old * zoom);

  if(editor->map_width > widget->allocation.width){
    editor->width = widget->allocation.width;

    //    gtk_adjustment_set_upper(adjustment, (double) (editor->map_width - editor->width));
    adjustment->upper = (double) (editor->map_width - editor->width);

    if(adjustment->value > adjustment->upper)
      gtk_adjustment_set_value(adjustment, adjustment->upper);
  }else{
    editor->width = editor->map_width;

    //    gtk_adjustment_set_upper(adjustment, 0.0);
    adjustment->upper = 0.0;
    gtk_adjustment_set_value(adjustment, 0.0);
  }

  editor->flags &= (~AGS_EDITOR_RESET_HSCROLLBAR);

  /* reset AgsEditorControlCurrent */
  editor->control_current.control_width = (guint) ((double) editor->control_current.control_width / zoom_old * zoom);

  if(editor->map_width > editor->width){
    editor->control_current.x0 = ((guint) round(adjustment->value) % editor->control_current.control_width);

    if(editor->control_current.x0 != 0)
      editor->control_current.x0 = editor->control_current.control_width - editor->control_current.x0;

    editor->control_current.x1 = (editor->width - editor->control_current.x0) % editor->control_current.control_width;

    editor->control_current.nth_x = (guint) ceil(adjustment->value / (double) editor->control_current.control_width);
  }else{
    editor->control_current.x0 = 0;
    editor->control_current.x1 = 0;

    editor->control_current.nth_x = 0;
  }

  /* reset AgsEditorControlUnit */
  editor->control_unit.control_width = editor->control_unit.control_width / zoom_old * zoom;

  if(editor->map_width > editor->width){
    editor->control_unit.x0 = ((guint) round(adjustment->value) % editor->control_unit.control_width);

    if(editor->control_unit.x0 != 0)
      editor->control_unit.x0 = editor->control_unit.control_width - editor->control_unit.x0;

    editor->control_unit.x1 = (editor->width - editor->control_unit.x0) % editor->control_unit.control_width;

    editor->control_unit.nth_x = (guint) ceil(adjustment->value / (double) editor->control_unit.control_width);
    editor->control_unit.stop_x = (editor->width - editor->control_unit.x0 - editor->control_unit.x1) / editor->control_unit.control_width;
  }else{
    editor->control_unit.x0 = 0;
    editor->control_unit.x1 = 0;

    editor->control_unit.nth_x = 0;
    editor->control_unit.stop_x = editor->control_unit.control_width * editor->control_unit.control_count;
  }

  /* refresh display */
  ags_editor_draw_segment(editor);
  ags_editor_draw_notation(editor);
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

  /* reset viewport */
  editor->flags |= AGS_EDITOR_RESET_HSCROLLBAR;
  editor->map_width = (guint) ((double) editor->map_width / tact_old * tact);
  adjustment = GTK_RANGE(editor->hscrollbar)->adjustment;

  if(editor->map_width > widget->allocation.width){
    editor->width = widget->allocation.width;

    //    gtk_adjustment_set_upper(adjustment, (double) (editor->map_width - editor->width));
    adjustment->upper = (double) (editor->map_width - editor->width);

    if(adjustment->value > adjustment->upper)
      gtk_adjustment_set_value(adjustment, adjustment->upper);
  }else{
    editor->width = editor->map_width;

    //    gtk_adjustment_set_upper(adjustment, 0.0);
    adjustment->upper = 0.0;
    gtk_adjustment_set_value(adjustment, 0.0);
  }

  editor->flags &= (~AGS_EDITOR_RESET_HSCROLLBAR);

  /* reset AgsEditorControlCurrent */
  editor->control_current.control_count = (guint) ((double) editor->control_current.control_count / tact_old * tact);

  if(editor->map_width > editor->width){
    // x0 is unchanged
    editor->control_current.x1 = (editor->width - editor->control_current.x0) % editor->control_current.control_width;

    editor->control_current.nth_x = (guint) ceil(adjustment->value / (double) editor->control_current.control_width);
  }else{
    // x0 is unchanged
    editor->control_current.x1 = 0;

    editor->control_current.nth_x = 0;
  }

  /* reset AgsEditorControlUnit */
  if(editor->map_width > editor->width){
    // x0 is unchanged
    editor->control_unit.x1 = (editor->width - editor->control_current.x0) % editor->control_unit.control_width;

    editor->control_unit.nth_x = (guint) ceil(adjustment->value / (double) editor->control_unit.control_width);
    editor->control_unit.stop_x = (editor->width - editor->control_current.x0 - editor->control_current.x1) / editor->control_unit.control_width;
  }else{
    // x0 is unchanged
    editor->control_current.x1 = 0;

    editor->control_unit.nth_x = 0;
    editor->control_unit.stop_x = editor->control_unit.control_width * editor->control_unit.control_count;
  }

  /* refresh display */
  ags_editor_draw_segment(editor);
  ags_editor_draw_notation(editor);
}

void
ags_toolbar_mode_default_callback(GtkWidget *widget, AgsToolbar *toolbar)
{
}

void
ags_toolbar_mode_group_channels_callback(GtkWidget *widget, AgsToolbar *toolbar)
{
}

