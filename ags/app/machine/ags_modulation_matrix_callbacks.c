/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#include <ags/app/machine/ags_modulation_matrix_callbacks.h>

#include <ags/app/ags_window.h>

#include <math.h>

void
ags_modulation_matrix_draw_callback(GtkWidget *drawing_area,
				    cairo_t *cr,
				    int width, int height,
				    AgsModulationMatrix *modulation_matrix)
{
  ags_modulation_matrix_draw(modulation_matrix,
			     cr);
}

gboolean
ags_modulation_matrix_gesture_click_pressed_callback(GtkGestureClick *event_controller,
						     gint n_press,
						     gdouble x,
						     gdouble y,
						     AgsModulationMatrix *modulation_matrix)
{
  AgsMachine *machine;

  AgsAudio *audio;
    
  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) modulation_matrix,
						   AGS_TYPE_MACHINE);

  audio = machine->audio;

  if(x >= (double) AGS_MODULATION_MATRIX_DEFAULT_CONTROL_WIDTH &&
     x < (double) AGS_MODULATION_MATRIX_DEFAULT_CONTROL_WIDTH + (double) (AGS_MODULATION_MATRIX_DEFAULT_CONTROLS_HORIZONTALLY * modulation_matrix->cell_width) &&
     y >= 2.0 + AGS_MODULATION_MATRIX_ROTATED_CONTROL_WIDTH &&
     y < 2.0 + AGS_MODULATION_MATRIX_ROTATED_CONTROL_WIDTH + (double) (AGS_MODULATION_MATRIX_DEFAULT_CONTROLS_VERTICALLY * modulation_matrix->cell_height)){
    modulation_matrix->flags |= AGS_MODULATION_MATRIX_CURSOR_ON;
    
    modulation_matrix->cursor_x = (guint) floor((x - (double) AGS_MODULATION_MATRIX_DEFAULT_CONTROL_WIDTH) / (double) modulation_matrix->cell_width);
    modulation_matrix->cursor_y = (guint) floor((y - (2.0 + AGS_MODULATION_MATRIX_ROTATED_CONTROL_WIDTH)) / (double) modulation_matrix->cell_height);
  }
    
  return(TRUE);
}

gboolean
ags_modulation_matrix_gesture_click_released_callback(GtkGestureClick *event_controller,
						      gint n_press,
						      gdouble x,
						      gdouble y,
						      AgsModulationMatrix *modulation_matrix)
{
  gint cursor_x, cursor_y;

  cursor_x = modulation_matrix->cursor_x;
  cursor_y = modulation_matrix->cursor_y;

  //  g_message("cursor %d | %d", cursor_x, cursor_y);

  if((AGS_MODULATION_MATRIX_CURSOR_ON & (modulation_matrix->flags)) != 0){
    modulation_matrix->flags &= (~AGS_MODULATION_MATRIX_CURSOR_ON);
    
    if(!ags_modulation_matrix_get_enabled(modulation_matrix, cursor_x, cursor_y)){
      ags_modulation_matrix_set_enabled(modulation_matrix,
					cursor_x, cursor_y,
					TRUE);
    }else{
      ags_modulation_matrix_set_enabled(modulation_matrix,
					cursor_x, cursor_y,
					FALSE);
    }
  }

  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) modulation_matrix->drawing_area);
  
  return(FALSE);
}

gboolean
ags_modulation_matrix_key_pressed_callback(GtkEventControllerKey *event_controller,
					   guint keyval,
					   guint keycode,
					   GdkModifierType state,
					   AgsModulationMatrix *modulation_matrix)
{
  if(keyval == GDK_KEY_Tab ||
     keyval == GDK_KEY_ISO_Left_Tab){
    return(FALSE);
  }

  switch(keyval){
  case GDK_KEY_Control_L:
    {
      modulation_matrix->key_mask |= AGS_MODULATION_MATRIX_KEY_L_CONTROL;
    }
    break;
  case GDK_KEY_Control_R:
    {
      modulation_matrix->key_mask |= AGS_MODULATION_MATRIX_KEY_R_CONTROL;
    }
    break;
  }

  return(TRUE);
}

gboolean
ags_modulation_matrix_key_released_callback(GtkEventControllerKey *event_controller,
					    guint keyval,
					    guint keycode,
					    GdkModifierType state,
					    AgsModulationMatrix *modulation_matrix)
{
  AgsMachine *machine;
  
  AgsAudio *audio;

  if(keyval == GDK_KEY_Tab ||
     keyval == GDK_KEY_ISO_Left_Tab){
    return(FALSE);
  }

  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) modulation_matrix,
						   AGS_TYPE_MACHINE);

  audio = machine->audio;
  
  switch(keyval){
  case GDK_KEY_Control_L:
    {
      modulation_matrix->key_mask &= (~AGS_MODULATION_MATRIX_KEY_L_CONTROL);
    }
    break;
  case GDK_KEY_Control_R:
    {
      modulation_matrix->key_mask &= (~AGS_MODULATION_MATRIX_KEY_R_CONTROL);
    }
    break;
  case GDK_KEY_Left:
  case GDK_KEY_leftarrow:
    {
      //TODO:JK: implement me
    }
    break;
  case GDK_KEY_Right:
  case GDK_KEY_rightarrow:
    {
      //TODO:JK: implement me
    }
    break;
  case GDK_KEY_Up:
  case GDK_KEY_uparrow:
    {
      //TODO:JK: implement me
    }
    break;
  case GDK_KEY_Down:
  case GDK_KEY_downarrow:
    {
      //TODO:JK: implement me
    }
    break;
  case GDK_KEY_space:
    {      
      /* queue draw */
      gtk_widget_queue_draw((GtkWidget *) modulation_matrix->drawing_area);
    }
    break;
  }

  return(TRUE);
}

gboolean
ags_modulation_matrix_modifiers_callback(GtkEventControllerKey *event_controller,
					 GdkModifierType keyval,
					 AgsModulationMatrix *modulation_matrix)
{
  //empty
  
  return(FALSE);
}
