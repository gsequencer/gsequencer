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
    wave_edit->cursor_position_x = (guint) ((zoom_factor * event->x + GTK_RANGE(wave_edit->hscrollbar)->adjustment->value));
    
    wave_edit->cursor_position_y = (((GTK_WIDGET(wave_edit->drawing_area)->allocation.height - event->y) / g_range) * c_range);

    /* queue draw */
    gtk_widget_queue_draw((GtkWidget *) wave_edit);
  }

  void ags_wave_edit_drawing_area_button_press_select_buffer()
  {
    double zoom_factor;

    /* zoom */
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) wave_toolbar->zoom));

    wave_edit->selection_x0 = (guint) zoom_factor * event->x + GTK_RANGE(wave_edit->hscrollbar)->adjustment->value;
    wave_edit->selection_x1 = wave_edit->selection_x0;
    
    wave_edit->selection_y0 = (guint) event->y + GTK_RANGE(wave_edit->vscrollbar)->adjustment->value;
    wave_edit->selection_y1 = wave_edit->selection_y0;

    gtk_widget_queue_draw((GtkWidget *) wave_edit);
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
    wave_edit->cursor_position_x = (guint) ((zoom_factor * event->x + GTK_RANGE(wave_edit->hscrollbar)->adjustment->value));
    
    wave_edit->cursor_position_y = (((GTK_WIDGET(wave_edit->drawing_area)->allocation.height - event->y) / g_range) * c_range);
    
    /* queue draw */
    gtk_widget_queue_draw((GtkWidget *) wave_edit);
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
    x0 = (guint) wave_edit->selection_x0;

    y0 = ((gdouble) (GTK_WIDGET(wave_edit->drawing_area)->allocation.height - wave_edit->selection_y0) / g_range) * c_range;
  
    x1 = (guint) (zoom_factor * event->x + GTK_RANGE(wave_edit->hscrollbar)->adjustment->value);
    
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
    wave_edit->cursor_position_x = ((zoom_factor * event->x + GTK_RANGE(wave_edit->hscrollbar)->adjustment->value));

    wave_edit->cursor_position_y = (((GTK_WIDGET(wave_edit->drawing_area)->allocation.height - event->y) / g_range) * c_range);

#ifdef AGS_DEBUG
    g_message("%lu %f", wave_edit->cursor_position_x, wave_edit->cursor_position_y);
#endif
    
    /* queue draw */
    gtk_widget_queue_draw((GtkWidget *) wave_edit);
  }

  void ags_wave_edit_drawing_area_motion_notify_select_buffer()
  {
    double zoom_factor;

    /* zoom */
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) wave_toolbar->zoom));

    if(zoom_factor * event->x + GTK_RANGE(wave_edit->hscrollbar)->adjustment->value >= 0.0){
      wave_edit->selection_x1 = (guint) zoom_factor * event->x + GTK_RANGE(wave_edit->hscrollbar)->adjustment->value;
    }else{
      wave_edit->selection_x1 = 0.0;
    }
    
    if(event->y + GTK_RANGE(wave_edit->vscrollbar)->adjustment->value >= 0.0){
      wave_edit->selection_y1 = (guint) event->y + GTK_RANGE(wave_edit->vscrollbar)->adjustment->value;
    }else{
      wave_edit->selection_y1 = 0.0;
    }
    
    gtk_widget_queue_draw((GtkWidget *) wave_edit);
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
  AgsWaveEditor *wave_editor;
  AgsMachine *machine;

  gboolean retval;
  
  if(event->keyval == GDK_KEY_Tab ||
     event->keyval == GDK_ISO_Left_Tab ||
     event->keyval == GDK_KEY_Shift_L ||
     event->keyval == GDK_KEY_Shift_R ||
     event->keyval == GDK_KEY_Alt_L ||
     event->keyval == GDK_KEY_Alt_R ||
     event->keyval == GDK_KEY_Control_L ||
     event->keyval == GDK_KEY_Control_R ){
    retval = FALSE;
  }else{
    retval = TRUE;
  }

  wave_editor = (AgsWaveEditor *) gtk_widget_get_ancestor(GTK_WIDGET(wave_edit),
							  AGS_TYPE_WAVE_EDITOR);

  machine = wave_editor->selected_machine;
  
  if(machine != NULL){
    switch(event->keyval){
    case GDK_KEY_Control_L:
      {
	wave_edit->key_mask |= AGS_WAVE_EDIT_KEY_L_CONTROL;
      }
      break;
    case GDK_KEY_Control_R:
      {
	wave_edit->key_mask |= AGS_WAVE_EDIT_KEY_R_CONTROL;
      }
      break;
    case GDK_KEY_Shift_L:
      {
	wave_edit->key_mask |= AGS_WAVE_EDIT_KEY_L_SHIFT;
      }
      break;
    case GDK_KEY_Shift_R:
      {
	wave_edit->key_mask |= AGS_WAVE_EDIT_KEY_R_SHIFT;
      }
      break;
    case GDK_KEY_a:
      {
	/* select all accelerations */
	if((AGS_WAVE_EDIT_KEY_L_CONTROL & (wave_edit->key_mask)) != 0 || (AGS_WAVE_EDIT_KEY_R_CONTROL & (wave_edit->key_mask)) != 0){
	  ags_wave_editor_select_all(wave_editor);
	}
      }
      break;
    case GDK_KEY_c:
      {
	/* copy accelerations */
	if((AGS_WAVE_EDIT_KEY_L_CONTROL & (wave_edit->key_mask)) != 0 || (AGS_WAVE_EDIT_KEY_R_CONTROL & (wave_edit->key_mask)) != 0){
	  ags_wave_editor_copy(wave_editor);
	}
      }
      break;
    case GDK_KEY_v:
      {
	/* paste accelerations */
	if((AGS_WAVE_EDIT_KEY_L_CONTROL & (wave_edit->key_mask)) != 0 || (AGS_WAVE_EDIT_KEY_R_CONTROL & (wave_edit->key_mask)) != 0){
	  ags_wave_editor_paste(wave_editor);
	}
      }
      break;
    case GDK_KEY_x:
      {
	/* cut accelerations */
	if((AGS_WAVE_EDIT_KEY_L_CONTROL & (wave_edit->key_mask)) != 0 || (AGS_WAVE_EDIT_KEY_R_CONTROL & (wave_edit->key_mask)) != 0){
	  ags_wave_editor_cut(wave_editor);
	}
      }
      break;
    }
  }

  return(retval);
}

gboolean
ags_wave_edit_drawing_area_key_release_event(GtkWidget *widget, GdkEventKey *event, AgsWaveEdit *wave_edit)
{
  AgsWaveEditor *wave_editor;
  AgsWaveToolbar *wave_toolbar;
  AgsMachine *machine;

  double zoom_factor;
  gint i;
  gboolean retval;

  wave_editor = (AgsWaveEditor *) gtk_widget_get_ancestor(GTK_WIDGET(wave_edit),
							  AGS_TYPE_WAVE_EDITOR);

  wave_toolbar = wave_editor->wave_toolbar;

  machine = wave_editor->selected_machine;
  
  if(event->keyval == GDK_KEY_Tab ||
     event->keyval == GDK_ISO_Left_Tab ||
     event->keyval == GDK_KEY_Shift_L ||
     event->keyval == GDK_KEY_Shift_R ||
     event->keyval == GDK_KEY_Alt_L ||
     event->keyval == GDK_KEY_Alt_R ||
     event->keyval == GDK_KEY_Control_L ||
     event->keyval == GDK_KEY_Control_R ){
    retval = FALSE;
  }else{
    retval = TRUE;
  }

  if(machine != NULL){    
    /* zoom */
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) wave_toolbar->zoom));

    /* check key value */
    switch(event->keyval){
    case GDK_KEY_Control_L:
      {
	wave_edit->key_mask &= (~AGS_WAVE_EDIT_KEY_L_CONTROL);
      }
      break;
    case GDK_KEY_Control_R:
      {
	wave_edit->key_mask &= (~AGS_WAVE_EDIT_KEY_R_CONTROL);
      }
      break;
    case GDK_KEY_Shift_L:
      {
	wave_edit->key_mask &= (~AGS_WAVE_EDIT_KEY_L_SHIFT);
      }
      break;
    case GDK_KEY_Shift_R:
      {
	wave_edit->key_mask &= (~AGS_WAVE_EDIT_KEY_R_SHIFT);
      }
      break;
    case GDK_KEY_Left:
    case GDK_KEY_leftarrow:
      {
	gdouble x0_offset;

	/* position cursor */
	if(wave_edit->cursor_position_x > 0){
	  if(wave_edit->cursor_position_x - (zoom_factor * wave_edit->control_width) > 0){
	    wave_edit->cursor_position_x -= (zoom_factor * wave_edit->control_width);
	  }else{
	    wave_edit->cursor_position_x = 0;
	  }
	}

	x0_offset = wave_edit->cursor_position_x / zoom_factor;
      
	if(x0_offset < GTK_RANGE(wave_edit->hscrollbar)->adjustment->value){
	  gtk_range_set_value(GTK_RANGE(wave_edit->hscrollbar),
			      x0_offset);
	}
      }
      break;
    case GDK_KEY_Right:
    case GDK_KEY_rightarrow:
      {
	gdouble x0_offset;
	  
	/* position cursor */      
	if(wave_edit->cursor_position_x < AGS_WAVE_EDITOR_MAX_CONTROLS){
	  wave_edit->cursor_position_x += (zoom_factor * wave_edit->control_width);
	}

	x0_offset = wave_edit->cursor_position_x / zoom_factor;
      
	if(x0_offset + wave_edit->control_width > GTK_RANGE(wave_edit->hscrollbar)->adjustment->value + GTK_WIDGET(wave_edit->drawing_area)->allocation.width){
	  gtk_range_set_value(GTK_RANGE(wave_edit->hscrollbar),
			      x0_offset);
	}
      }
      break;
    }

    gtk_widget_queue_draw((GtkWidget *) wave_edit);
  }
  
  return(retval);
}

void
ags_wave_edit_vscrollbar_value_changed(GtkRange *range, AgsWaveEdit *wave_edit)
{
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) wave_edit->drawing_area);
}

void
ags_wave_edit_hscrollbar_value_changed(GtkRange *range, AgsWaveEdit *wave_edit)
{
  AgsConfig *config;

  gchar *str;
  
  gdouble gui_scale_factor;
  gdouble value;

  config = ags_config_get_instance();
  
  /* scale factor */
  gui_scale_factor = 1.0;
  
  str = ags_config_get_value(config,
			     AGS_CONFIG_GENERIC,
			     "gui-scale");

  if(str != NULL){
    gui_scale_factor = g_ascii_strtod(str,
				      NULL);

    g_free(str);
  }

  value = GTK_RANGE(wave_edit->hscrollbar)->adjustment->value / (gui_scale_factor * 64.0);
  gtk_adjustment_set_value(wave_edit->ruler->adjustment,
			   value);
  gtk_widget_queue_draw((GtkWidget *) wave_edit->ruler);
  
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) wave_edit->drawing_area);
}

