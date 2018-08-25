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

#include <ags/X/editor/ags_wave_edit_callbacks.h>

#include <ags/X/ags_wave_editor.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <math.h>
#include <gdk/gdkkeysyms.h>

gboolean
ags_wave_edit_drawing_area_expose_event(GtkWidget *widget, GdkEventExpose *event, AgsWaveEdit *wave_edit)
{
  ags_wave_edit_reset_vscrollbar(wave_edit);
  ags_wave_edit_reset_hscrollbar(wave_edit);
  
  ags_wave_edit_draw(wave_edit);
  
  return(TRUE);
}

gboolean
ags_wave_edit_drawing_area_configure_event(GtkWidget *widget, GdkEventConfigure *event, AgsWaveEdit *wave_edit)
{
  ags_wave_edit_reset_vscrollbar(wave_edit);
  ags_wave_edit_reset_hscrollbar(wave_edit);
  
  ags_wave_edit_draw(wave_edit);
  
  return(TRUE);
}

gboolean
ags_wave_edit_drawing_area_button_press_event(GtkWidget *widget, GdkEventButton *event, AgsWaveEdit *wave_edit)
{
  AgsWaveEditor *wave_editor;
  AgsWaveToolbar *wave_toolbar;
  AgsMachine *machine;

  auto void ags_wave_edit_drawing_area_button_press_position_cursor();
  auto void ags_wave_edit_drawing_area_button_press_select_buffer();

  void ags_wave_edit_drawing_area_button_press_position_cursor()
  {
    gdouble c_range;
    guint g_range;
    double zoom_factor;

    c_range = wave_edit->upper - wave_edit->lower;

    g_range = GTK_RANGE(wave_edit->vscrollbar)->adjustment->upper + GTK_WIDGET(wave_edit->drawing_area)->allocation.height;

    /* zoom */
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) wave_toolbar->zoom));

    /* cursor position */
    wave_edit->cursor_position_x = (guint) (zoom_factor * (event->x + GTK_RANGE(wave_edit->hscrollbar)->adjustment->value)) / wave_edit->control_width;
    
    wave_edit->cursor_position_y = (((GTK_WIDGET(wave_edit->drawing_area)->allocation.height - event->y) / g_range) * c_range);

    /* queue draw */
    gtk_widget_queue_draw(wave_edit);
  }

  void ags_wave_edit_drawing_area_button_press_select_buffer()
  {
    wave_edit->selection_x0 = (guint) event->x + GTK_RANGE(wave_edit->hscrollbar)->adjustment->value;
    wave_edit->selection_x1 = wave_edit->selection_x0;
    
    wave_edit->selection_y0 = (guint) event->y + GTK_RANGE(wave_edit->vscrollbar)->adjustment->value;
    wave_edit->selection_y1 = wave_edit->selection_y0;

    gtk_widget_queue_draw(wave_edit);
  }

  wave_editor = (AgsWaveEditor *) gtk_widget_get_ancestor(GTK_WIDGET(wave_edit),
								      AGS_TYPE_WAVE_EDITOR);

  wave_toolbar = wave_editor->wave_toolbar;

  gtk_widget_grab_focus((GtkWidget *) wave_edit->drawing_area);
  wave_editor->focused_wave_edit = wave_edit;

  if((machine = wave_editor->selected_machine) != NULL &&
     event->button == 1){    
    wave_edit->button_mask = AGS_WAVE_EDIT_BUTTON_1;
    
    if(wave_toolbar->selected_edit_mode == wave_toolbar->position){
      wave_edit->mode = AGS_WAVE_EDIT_POSITION_CURSOR;

      ags_wave_edit_drawing_area_button_press_position_cursor();
    }else if(wave_toolbar->selected_edit_mode == wave_toolbar->select){
      wave_edit->mode = AGS_WAVE_EDIT_SELECT_BUFFER;

      ags_wave_edit_drawing_area_button_press_select_buffer();
    }
  }
  
  return(TRUE);
}

gboolean
ags_wave_edit_drawing_area_button_release_event(GtkWidget *widget, GdkEventButton *event, AgsWaveEdit *wave_edit)
{
  AgsWaveEditor *wave_editor;
  AgsWaveToolbar *wave_toolbar;
  AgsMachine *machine;

  auto void ags_wave_edit_drawing_area_button_release_position_cursor();
  auto void ags_wave_edit_drawing_area_button_release_select_buffer();

  void ags_wave_edit_drawing_area_button_release_position_cursor()
  {
    gdouble c_range;
    guint g_range;
    double zoom_factor;

    c_range = wave_edit->upper - wave_edit->lower;

    g_range = GTK_RANGE(wave_edit->vscrollbar)->adjustment->upper + GTK_WIDGET(wave_edit->drawing_area)->allocation.height;

    /* zoom */
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) wave_toolbar->zoom));

    /* cursor position */
    wave_edit->cursor_position_x = (guint) (zoom_factor * (event->x + GTK_RANGE(wave_edit->hscrollbar)->adjustment->value)) / wave_edit->control_width;
    
    wave_edit->cursor_position_y = (((GTK_WIDGET(wave_edit->drawing_area)->allocation.height - event->y) / g_range) * c_range);
    
    /* queue draw */
    gtk_widget_queue_draw(wave_edit);
  }

  void ags_wave_edit_drawing_area_button_release_select_buffer()
  {
    gdouble c_range;
    guint g_range;
    double zoom_factor;
    guint x0, x1;
    gdouble y0, y1;
    
    c_range = wave_edit->upper - wave_edit->lower;

    g_range = GTK_RANGE(wave_edit->vscrollbar)->adjustment->upper + GTK_WIDGET(wave_edit->drawing_area)->allocation.height;

    /* zoom */
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) wave_toolbar->zoom));

    /* region */
    x0 = (guint) zoom_factor * wave_edit->selection_x0;

    y0 = ((gdouble) (GTK_WIDGET(wave_edit->drawing_area)->allocation.height - wave_edit->selection_y0) / g_range) * c_range;
  
    x1 = (guint) zoom_factor * (event->x + GTK_RANGE(wave_edit->hscrollbar)->adjustment->value);
    
    y1 = (((GTK_WIDGET(wave_edit->drawing_area)->allocation.height - event->y) + GTK_RANGE(wave_edit->vscrollbar)->adjustment->value) / g_range) * c_range;
    
    /* select region */
    ags_wave_editor_select_region(wave_editor,
					x0, y0,
					x1, y1);
  }

  wave_editor = (AgsWaveEditor *) gtk_widget_get_ancestor(GTK_WIDGET(wave_edit),
								      AGS_TYPE_WAVE_EDITOR);
  
  wave_toolbar = wave_editor->wave_toolbar;

  if((machine = wave_editor->selected_machine) != NULL &&
     event->button == 1){
    wave_edit->button_mask &= (~AGS_WAVE_EDIT_BUTTON_1);
    
    if(wave_edit->mode == AGS_WAVE_EDIT_POSITION_CURSOR){
      ags_wave_edit_drawing_area_button_release_position_cursor();

      wave_edit->mode = AGS_WAVE_EDIT_NO_EDIT_MODE;
    }else if(wave_edit->mode == AGS_WAVE_EDIT_SELECT_BUFFER){
      ags_wave_edit_drawing_area_button_release_select_buffer();

      wave_edit->mode = AGS_WAVE_EDIT_NO_EDIT_MODE;
    }
  }

  return(FALSE);
}

gboolean
ags_wave_edit_drawing_area_motion_notify_event(GtkWidget *widget, GdkEventMotion *event, AgsWaveEdit *wave_edit)
{
  AgsWaveEditor *wave_editor;
  AgsWaveToolbar *wave_toolbar;
  AgsMachine *machine;

  auto void ags_wave_edit_drawing_area_motion_notify_position_cursor();
  auto void ags_wave_edit_drawing_area_motion_notify_select_buffer();

  void ags_wave_edit_drawing_area_motion_notify_position_cursor()
  {
    gdouble c_range;
    guint g_range;
    double zoom_factor;

    c_range = wave_edit->upper - wave_edit->lower;

    g_range = GTK_RANGE(wave_edit->vscrollbar)->adjustment->upper + GTK_WIDGET(wave_edit->drawing_area)->allocation.height;

    /* zoom */
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) wave_toolbar->zoom));

    /* cursor position */
    wave_edit->cursor_position_x = (guint) zoom_factor * ((event->x + GTK_RANGE(wave_edit->hscrollbar)->adjustment->value));

    wave_edit->cursor_position_y = (((GTK_WIDGET(wave_edit->drawing_area)->allocation.height - event->y) / g_range) * c_range);

#ifdef AGS_DEBUG
    g_message("%lu %f", wave_edit->cursor_position_x, wave_edit->cursor_position_y);
#endif
    
    /* queue draw */
    gtk_widget_queue_draw(wave_edit);
  }

  void ags_wave_edit_drawing_area_motion_notify_select_buffer()
  {
    if(event->x + GTK_RANGE(wave_edit->hscrollbar)->adjustment->value >= 0.0){
      wave_edit->selection_x1 = (guint) event->x + GTK_RANGE(wave_edit->hscrollbar)->adjustment->value;
    }else{
      wave_edit->selection_x1 = 0.0;
    }
    
    if(event->y + GTK_RANGE(wave_edit->vscrollbar)->adjustment->value >= 0.0){
      wave_edit->selection_y1 = (guint) event->y + GTK_RANGE(wave_edit->vscrollbar)->adjustment->value;
    }else{
      wave_edit->selection_y1 = 0.0;
    }
    
    gtk_widget_queue_draw(wave_edit);
  }

  wave_editor = (AgsWaveEditor *) gtk_widget_get_ancestor(GTK_WIDGET(wave_edit),
								      AGS_TYPE_WAVE_EDITOR);

  wave_toolbar = wave_editor->wave_toolbar;

  gtk_widget_grab_focus((GtkWidget *) wave_edit->drawing_area);

  if((machine = wave_editor->selected_machine) != NULL &&
     (AGS_WAVE_EDIT_BUTTON_1 & (wave_edit->button_mask)) != 0){
    if(wave_edit->mode == AGS_WAVE_EDIT_POSITION_CURSOR){
      ags_wave_edit_drawing_area_motion_notify_position_cursor();
    }else if(wave_edit->mode == AGS_WAVE_EDIT_SELECT_BUFFER){
      ags_wave_edit_drawing_area_motion_notify_select_buffer();
    }
  }

  return(FALSE);
}

gboolean
ags_wave_edit_drawing_area_key_press_event(GtkWidget *widget, GdkEventKey *event, AgsWaveEdit *wave_edit)
{
  //TODO:JK: implement me
}

gboolean
ags_wave_edit_drawing_area_key_release_event(GtkWidget *widget, GdkEventKey *event, AgsWaveEdit *wave_edit)
{
  //TODO:JK: implement me
}

void
ags_wave_edit_vscrollbar_value_changed(GtkRange *range, AgsWaveEdit *wave_edit)
{
  AgsWaveEditor *wave_editor;

  wave_editor = gtk_widget_get_ancestor(wave_edit,
					AGS_TYPE_WAVE_EDITOR);

  /* queue draw */
  gtk_widget_queue_draw(wave_edit->drawing_area);
}

void
ags_wave_edit_hscrollbar_value_changed(GtkRange *range, AgsWaveEdit *wave_edit)
{
  gdouble value;

  value = GTK_RANGE(wave_edit->hscrollbar)->adjustment->value / 64.0;
  gtk_adjustment_set_value(wave_edit->ruler->adjustment,
			   value);
  gtk_widget_queue_draw(wave_edit->ruler);
  
  /* queue draw */
  gtk_widget_queue_draw(wave_edit->drawing_area);
}

