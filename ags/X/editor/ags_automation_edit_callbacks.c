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

#include <ags/X/editor/ags_automation_edit_callbacks.h>

#include <ags/X/ags_automation_editor.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <math.h>

#include <gdk/gdkkeysyms.h>

gboolean
ags_automation_edit_drawing_area_expose_event(GtkWidget *widget, GdkEventExpose *event, AgsAutomationEdit *automation_edit)
{
  ags_automation_edit_reset_vscrollbar(automation_edit);
  ags_automation_edit_reset_hscrollbar(automation_edit);
  
  ags_automation_edit_draw(automation_edit);
  
  return(TRUE);
}

gboolean
ags_automation_edit_drawing_area_configure_event(GtkWidget *widget, GdkEventConfigure *event, AgsAutomationEdit *automation_edit)
{
  ags_automation_edit_reset_vscrollbar(automation_edit);
  ags_automation_edit_reset_hscrollbar(automation_edit);

  ags_automation_edit_draw(automation_edit);

  return(FALSE);
}

gboolean
ags_automation_edit_drawing_area_button_press_event(GtkWidget *widget, GdkEventButton *event, AgsAutomationEdit *automation_edit)
{
  AgsAutomationEditor *automation_editor;
  AgsAutomationToolbar *automation_toolbar;
  AgsMachine *machine;

  auto void ags_automation_edit_drawing_area_button_press_position_cursor();
  auto void ags_automation_edit_drawing_area_button_press_add_acceleration();
  auto void ags_automation_edit_drawing_area_button_press_select_acceleration();

  void ags_automation_edit_drawing_area_button_press_position_cursor()
  {
    gdouble c_range;
    guint g_range;
    double zoom_factor;

    if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
      c_range = exp(automation_edit->upper) - exp(automation_edit->lower);
    }else{
      c_range = automation_edit->upper - automation_edit->lower;
    }

    g_range = GTK_RANGE(automation_edit->vscrollbar)->adjustment->upper + GTK_WIDGET(automation_edit->drawing_area)->allocation_height;

    /* zoom */
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom));

    /* cursor position */
    automation_edit->cursor_position_x = (guint) zoom_factor * ((event->x + GTK_RANGE(automation_edit->hscrollbar)->adjustment->value) / automation_edit->control_width);
    
    if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
      automation_edit->cursor_position_y = log((event->y / g_range) * c_range);
    }else{
      automation_edit->cursor_position_y = ((event->y / g_range) * c_range);
    }

    /* queue draw */
    gtk_widget_queue_draw(automation_edit);
  }
  
  void ags_automation_edit_drawing_area_button_press_add_acceleration()
  {
    AgsAcceleration *acceleration;

    gdouble c_range;
    guint g_range;
    double zoom_factor;
    
    acceleration = ags_acceleration_new();

    if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
      c_range = exp(automation_edit->upper) - exp(automation_edit->lower);
    }else{
      c_range = automation_edit->upper - automation_edit->lower;
    }

    g_range = GTK_RANGE(automation_edit->vscrollbar)->adjustment->upper + GTK_WIDGET(automation_edit->drawing_area)->allocation_height;

    /* zoom */
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom));

    /* acceleration */
    acceleration->x = (guint) zoom_factor * ((event->x + GTK_RANGE(automation_edit->hscrollbar)->adjustment->value));
    
    if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
      acceleration->y = log((event->y / g_range) * c_range);
    }else{
      acceleration->y = ((event->y / g_range) * c_range);
    }
    
    /* current acceleration */
    if(automation_edit->current_acceleration != NULL){
      g_object_unref(automation_edit->current_acceleration);

      automation_edit->current_acceleration = NULL;
    }

    automation_edit->current_acceleration = acceleration;
    g_object_ref(acceleration);

    /* queue draw */
    gtk_widget_queue_draw(automation_edit);
  }

  void ags_automation_edit_drawing_area_button_press_select_acceleration()
  {
    automation_edit->selection_x0 = (guint) event->x + GTK_RANGE(automation_edit->hscrollbar)->adjustment->value;
    automation_edit->selection_x1 = automation_edit->selection_x0;
    
    automation_edit->selection_y0 = (guint) event->y + GTK_RANGE(automation_edit->vscrollbar)->adjustment->value;
    automation_edit->selection_y1 = automation_edit->selection_y0;

    gtk_widget_queue_draw(automation_edit);
  }

  automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor(GTK_WIDGET(automation_edit),
								      AGS_TYPE_AUTOMATION_EDITOR);

  automation_toolbar = automation_editor->automation_toolbar;

  gtk_widget_grab_focus((GtkWidget *) automation_edit->drawing_area);

  if((machine = automation_editor->selected_machine) != NULL &&
     event->button == 1){    
    if(automation_toolbar->selected_edit_mode == automation_toolbar->position){
      automation_edit->mode = AGS_AUTOMATION_EDIT_POSITION_CURSOR;

      ags_automation_edit_drawing_area_button_press_position_cursor();
    }else if(automation_toolbar->selected_edit_mode == automation_toolbar->edit){
      automation_edit->mode = AGS_AUTOMATION_EDIT_ADD_ACCELERATION;

      ags_automation_edit_drawing_area_button_press_add_acceleration();
    }else if(automation_toolbar->selected_edit_mode == automation_toolbar->clear){
      automation_edit->mode = AGS_AUTOMATION_EDIT_DELETE_ACCELERATION;

      //ACCELERATION:JK: only takes action on release
    }else if(automation_toolbar->selected_edit_mode == automation_toolbar->select){
      automation_edit->mode = AGS_AUTOMATION_EDIT_SELECT_ACCELERATION;

      ags_automation_edit_drawing_area_button_press_select_acceleration();
    }
  }
  
  return(TRUE);
}

gboolean
ags_automation_edit_drawing_area_button_release_event(GtkWidget *widget, GdkEventButton *event, AgsAutomationEdit *automation_edit)
{
  AgsAutomationEditor *automation_editor;
  AgsAutomationToolbar *automation_toolbar;
  AgsMachine *machine;

  auto void ags_automation_edit_drawing_area_button_release_position_cursor();
  auto void ags_automation_edit_drawing_area_button_release_add_acceleration();
  auto void ags_automation_edit_drawing_area_button_release_delete_acceleration();
  auto void ags_automation_edit_drawing_area_button_release_select_acceleration();

  void ags_automation_edit_drawing_area_button_release_position_cursor()
  {
    gdouble c_range;
    guint g_range;
    double zoom_factor;

    if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
      c_range = exp(automation_edit->upper) - exp(automation_edit->lower);
    }else{
      c_range = automation_edit->upper - automation_edit->lower;
    }

    g_range = GTK_RANGE(automation_edit->vscrollbar)->adjustment->upper + GTK_WIDGET(automation_edit->drawing_area)->allocation_height;

    /* zoom */
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom));

    /* cursor position */
    automation_edit->cursor_position_x = (guint) zoom_factor * ((event->x + GTK_RANGE(automation_edit->hscrollbar)->adjustment->value) / automation_edit->control_width);
    
    if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
      automation_edit->cursor_position_y = log((event->y / g_range) * c_range);
    }else{
      automation_edit->cursor_position_y = ((event->y / g_range) * c_range);
    }
    
    /* queue draw */
    gtk_widget_queue_draw(automation_edit);
  }

  void ags_automation_edit_drawing_area_button_release_add_acceleration()
  {
    AgsAcceleration *acceleration;
    
    gdouble c_range;
    guint g_range;
    double zoom_factor;
    guint new_x;
    
    acceleration = automation_edit->current_acceleration;
    
    if(acceleration == NULL){
      return;
    }

    if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
      c_range = exp(automation_edit->upper) - exp(automation_edit->lower);
    }else{
      c_range = automation_edit->upper - automation_edit->lower;
    }

    g_range = GTK_RANGE(automation_edit->vscrollbar)->adjustment->upper + GTK_WIDGET(automation_edit->drawing_area)->allocation_height;

    /* zoom */
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom));

    /* acceleration */
    acceleration->x = (guint) zoom_factor * ((event->x + GTK_RANGE(automation_edit->hscrollbar)->adjustment->value));
    
    if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
      acceleration->y = log((event->y / g_range) * c_range);
    }else{
      acceleration->y = ((event->y / g_range) * c_range);
    }
    
#ifdef AGS_DEBUG
    g_message("%lu %f", acceleration->x, acceleration->y);
#endif

    /* add acceleration */
    ags_automation_editor_add_acceleration(automation_editor,
					   acceleration);

    automation_edit->current_acceleration = NULL;
    g_object_unref(acceleration);
  }
  
  void ags_automation_edit_drawing_area_button_release_delete_acceleration()
  {
    gdouble c_range;
    guint g_range;
    double zoom_factor;
    guint x;
    gdouble y;

    if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
      c_range = exp(automation_edit->upper) - exp(automation_edit->lower);
    }else{
      c_range = automation_edit->upper - automation_edit->lower;
    }

    g_range = GTK_RANGE(automation_edit->vscrollbar)->adjustment->upper + GTK_WIDGET(automation_edit->drawing_area)->allocation_height;
    
    /* zoom */
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom));

    /* acceleration */
    x = (guint) zoom_factor * ((event->x + GTK_RANGE(automation_edit->hscrollbar)->adjustment->value));
    
    if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
      y = log((event->y / g_range) * c_range);
    }else{
      y = ((event->y / g_range) * c_range);
    }
    
    /* delete acceleration */
    ags_automation_editor_delete_acceleration(automation_editor,
					      x, y);
  }
  
  void ags_automation_edit_drawing_area_button_release_select_acceleration()
  {
    double zoom_factor;
    guint x0, x1, y0, y1;
    
    /* zoom */
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom));

    /* region */
    x0 = (guint) zoom_factor * (automation_edit->selection_x0 / automation_edit->control_width);
    x0 = zoom_factor * floor(x0 / zoom_factor);

    y0 = (guint) (automation_edit->selection_y0 / automation_edit->control_height);
    
    x1 = (guint) zoom_factor * ((event->x + GTK_RANGE(automation_edit->hscrollbar)->adjustment->value) / automation_edit->control_width);
    x1 = zoom_factor * floor(x1 / zoom_factor);
    
    y1 = (guint) ((event->y + GTK_RANGE(automation_edit->vscrollbar)->adjustment->value) / automation_edit->control_height);

    /* select region */
    ags_automation_editor_select_region(automation_editor,
					x0, y0,
					x1, y1);
  }

  automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor(GTK_WIDGET(automation_edit),
								      AGS_TYPE_AUTOMATION_EDITOR);

  automation_toolbar = automation_editor->automation_toolbar;

  if((machine = automation_editor->selected_machine) != NULL &&
     event->button == 1){    
    if(automation_edit->mode == AGS_AUTOMATION_EDIT_POSITION_CURSOR){
      ags_automation_edit_drawing_area_button_release_position_cursor();
    }else if(automation_edit->mode == AGS_AUTOMATION_EDIT_ADD_ACCELERATION){
      ags_automation_edit_drawing_area_button_release_add_acceleration();

      automation_edit->mode = AGS_AUTOMATION_EDIT_NO_EDIT_MODE;
    }else if(automation_edit->mode == AGS_AUTOMATION_EDIT_DELETE_ACCELERATION){
      ags_automation_edit_drawing_area_button_release_delete_acceleration();

      automation_edit->mode = AGS_AUTOMATION_EDIT_NO_EDIT_MODE;
    }else if(automation_edit->mode == AGS_AUTOMATION_EDIT_SELECT_ACCELERATION){
      ags_automation_edit_drawing_area_button_release_select_acceleration();

      automation_edit->mode = AGS_AUTOMATION_EDIT_NO_EDIT_MODE;
    }
  }

  return(FALSE);
}

gboolean
ags_automation_edit_drawing_area_motion_notify_event(GtkWidget *widget, GdkEventMotion *event, AgsAutomationEdit *automation_edit)
{
  AgsAutomationEditor *automation_editor;
  AgsAutomationToolbar *automation_toolbar;
  AgsMachine *machine;

  auto void ags_automation_edit_drawing_area_motion_notify_position_cursor();
  auto void ags_automation_edit_drawing_area_motion_notify_add_acceleration();
  auto void ags_automation_edit_drawing_area_motion_notify_select_acceleration();

  void ags_automation_edit_drawing_area_motion_notify_position_cursor()
  {
    gdouble c_range;
    guint g_range;
    double zoom_factor;

    if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
      c_range = exp(automation_edit->upper) - exp(automation_edit->lower);
    }else{
      c_range = automation_edit->upper - automation_edit->lower;
    }

    g_range = GTK_RANGE(automation_edit->vscrollbar)->adjustment->upper + GTK_WIDGET(automation_edit->drawing_area)->allocation_height;

    /* zoom */
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom));

    /* cursor position */
    automation_edit->cursor_position_x = (guint) zoom_factor * ((event->x + GTK_RANGE(automation_edit->hscrollbar)->adjustment->value));

    if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
      automation_edit->cursor_position_y = log((event->y / g_range) * c_range);
    }else{
      automation_edit->cursor_position_y = ((event->y / g_range) * c_range);
    }

#ifdef AGS_DEBUG
    g_message("%lu %f", automation_edit->cursor_position_x, automation_edit->cursor_position_y);
#endif
    
    /* queue draw */
    gtk_widget_queue_draw(automation_edit);
  }

  void ags_automation_edit_drawing_area_motion_notify_add_acceleration()
  {
    AgsAcceleration *acceleration;
    
    gdouble c_range;
    guint g_range;
    double zoom_factor;
    
    acceleration = automation_edit->current_acceleration;
    
    if(acceleration == NULL){
      return;
    }
    
    if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
      c_range = exp(automation_edit->upper) - exp(automation_edit->lower);
    }else{
      c_range = automation_edit->upper - automation_edit->lower;
    }

    g_range = GTK_RANGE(automation_edit->vscrollbar)->adjustment->upper + GTK_WIDGET(automation_edit->drawing_area)->allocation_height;

    /* zoom */
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom));

    /* acceleration */
    acceleration->x = (guint) zoom_factor * ((event->x + GTK_RANGE(automation_edit->hscrollbar)->adjustment->value));
    
    if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
      acceleration->y = log((event->y / g_range) * c_range);
    }else{
      acceleration->y = ((event->y / g_range) * c_range);
    }
    
#ifdef AGS_DEBUG
    g_message("%lu %f", acceleration->x, acceleration->y);
#endif
    
    /* queue draw */
    gtk_widget_queue_draw(automation_edit);
  }

  void ags_automation_edit_drawing_area_motion_notify_select_acceleration()
  {
    automation_edit->selection_x1 = (guint) event->x + GTK_RANGE(automation_edit->hscrollbar)->adjustment->value;
    
    automation_edit->selection_y1 = (guint) event->y + GTK_RANGE(automation_edit->vscrollbar)->adjustment->value;

    gtk_widget_queue_draw(automation_edit);
  }

  automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor(GTK_WIDGET(automation_edit),
								      AGS_TYPE_AUTOMATION_EDITOR);

  automation_toolbar = automation_editor->automation_toolbar;

  gtk_widget_grab_focus((GtkWidget *) automation_edit->drawing_area);

  if((machine = automation_editor->selected_machine) != NULL){
    if(automation_edit->mode == AGS_AUTOMATION_EDIT_POSITION_CURSOR){
      ags_automation_edit_drawing_area_motion_notify_position_cursor();
    }else if(automation_edit->mode == AGS_AUTOMATION_EDIT_ADD_ACCELERATION){
      ags_automation_edit_drawing_area_motion_notify_add_acceleration();
    }else if(automation_edit->mode == AGS_AUTOMATION_EDIT_DELETE_ACCELERATION){
      //ACCELERATION:JK: only takes action on release
    }else if(automation_edit->mode == AGS_AUTOMATION_EDIT_SELECT_ACCELERATION){
      ags_automation_edit_drawing_area_motion_notify_select_acceleration();
    }
  }

  return(FALSE);
}

gboolean
ags_automation_edit_drawing_area_key_press_event(GtkWidget *widget, GdkEventKey *event, AgsAutomationEdit *automation_edit)
{
  AgsAutomationEditor *automation_editor;
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

  automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor(GTK_WIDGET(automation_edit),
								      AGS_TYPE_AUTOMATION_EDITOR);

  machine = automation_editor->selected_machine;
  
  if(machine != NULL){
    switch(event->keyval){
    case GDK_KEY_Control_L:
      {
	automation_edit->key_mask |= AGS_AUTOMATION_EDIT_KEY_L_CONTROL;
      }
      break;
    case GDK_KEY_Control_R:
      {
	automation_edit->key_mask |= AGS_AUTOMATION_EDIT_KEY_R_CONTROL;
      }
      break;
    case GDK_KEY_Shift_L:
      {
	automation_edit->key_mask |= AGS_AUTOMATION_EDIT_KEY_L_SHIFT;
      }
      break;
    case GDK_KEY_Shift_R:
      {
	automation_edit->key_mask |= AGS_AUTOMATION_EDIT_KEY_R_SHIFT;
      }
      break;
    case GDK_KEY_a:
      {
	/* select all accelerations */
	if((AGS_AUTOMATION_EDIT_KEY_L_CONTROL & (automation_edit->key_mask)) != 0 || (AGS_AUTOMATION_EDIT_KEY_R_CONTROL & (automation_edit->key_mask)) != 0){
	  ags_automation_editor_select_all(automation_editor);
	}
      }
      break;
    case GDK_KEY_c:
      {
	/* copy accelerations */
	if((AGS_AUTOMATION_EDIT_KEY_L_CONTROL & (automation_edit->key_mask)) != 0 || (AGS_AUTOMATION_EDIT_KEY_R_CONTROL & (automation_edit->key_mask)) != 0){
	  ags_automation_editor_copy(automation_editor);
	}
      }
      break;
    case GDK_KEY_v:
      {
	/* paste accelerations */
	if((AGS_AUTOMATION_EDIT_KEY_L_CONTROL & (automation_edit->key_mask)) != 0 || (AGS_AUTOMATION_EDIT_KEY_R_CONTROL & (automation_edit->key_mask)) != 0){
	  ags_automation_editor_paste(automation_editor);
	}
      }
      break;
    case GDK_KEY_x:
      {
	/* cut accelerations */
	if((AGS_AUTOMATION_EDIT_KEY_L_CONTROL & (automation_edit->key_mask)) != 0 || (AGS_AUTOMATION_EDIT_KEY_R_CONTROL & (automation_edit->key_mask)) != 0){
	  ags_automation_editor_cut(automation_editor);
	}
      }
      break;
    case GDK_KEY_i:
      {
	/* invert accelerations */
	if((AGS_AUTOMATION_EDIT_KEY_L_CONTROL & (automation_edit->key_mask)) != 0 || (AGS_AUTOMATION_EDIT_KEY_R_CONTROL & (automation_edit->key_mask)) != 0){
	  ags_automation_editor_invert(automation_editor);
	}
      }
      break;
    }
  }

  return(retval);
}

gboolean
ags_automation_edit_drawing_area_key_release_event(GtkWidget *widget, GdkEventKey *event, AgsAutomationEdit *automation_edit)
{
  AgsAutomationEditor *automation_editor;
  AgsAutomationToolbar *automation_toolbar;
  AgsMachine *machine;
  AgsNotebook *notebook;	  

  AgsMutexManager *mutex_manager;

  double zoom_factor;
  gint i;
  gboolean retval;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;

  automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor(GTK_WIDGET(automation_edit),
								      AGS_TYPE_AUTOMATION_EDITOR);

  automation_toolbar = automation_editor->automation_toolbar;

  machine = automation_editor->selected_machine;
  
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
    if(automation_edit->channel_type == G_TYPE_NONE){
      notebook = NULL;
    }else if(channel_type == AGS_TYPE_OUTPUT){
      notebook = automation_editor->output_notebook;
    }else if(channel_type == AGS_TYPE_INPUT){
      notebook = automation_editor->input_notebook;
    }
    
    mutex_manager = ags_mutex_manager_get_instance();
    application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

    /* get audio mutex */
    pthread_mutex_lock(application_mutex);  
    
    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) machine->audio);
  
    pthread_mutex_unlock(application_mutex);

    /* zoom */
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom));

    /* check key value */
    switch(event->keyval){
    case GDK_KEY_Control_L:
      {
	automation_edit->key_mask &= (~AGS_AUTOMATION_EDIT_KEY_L_CONTROL);
      }
      break;
    case GDK_KEY_Control_R:
      {
	automation_edit->key_mask &= (~AGS_AUTOMATION_EDIT_KEY_R_CONTROL);
      }
      break;
    case GDK_KEY_Shift_L:
      {
	automation_edit->key_mask &= (~AGS_AUTOMATION_EDIT_KEY_L_SHIFT);
      }
      break;
    case GDK_KEY_Shift_R:
      {
	automation_edit->key_mask &= (~AGS_AUTOMATION_EDIT_KEY_R_SHIFT);
      }
      break;
    case GDK_KEY_Left:
    case GDK_KEY_leftarrow:
      {
	gdouble x0_offset;

	/* position cursor */
	if(automation_edit->cursor_position_x > 0){
	  if(automation_edit->cursor_position_x - (zoom_factor) > 0){
	    automation_edit->cursor_position_x -= (zoom_factor);
	  }else{
	    automation_edit->cursor_position_x = 0;
	  }
	  
	  do_feedback = TRUE;
	}

	x0_offset = automation_edit->cursor_position_x * automation_edit->control_width;
      
	if(x0_offset < GTK_RANGE(automation_edit->hscrollbar)->adjustment->value){
	  gtk_range_set_value(GTK_RANGE(automation_edit->hscrollbar),
			      x0_offset);
	}
      }
      break;
    case GDK_KEY_Right:
    case GDK_KEY_rightarrow:
      {
	gdouble x0_offset;
	  
	/* position cursor */      
	if(automation_edit->cursor_position_x < AGS_AUTOMATION_EDITOR_MAX_CONTROLS){
	  automation_edit->cursor_position_x += (zoom_factor);
	  
	  do_feedback = TRUE;
	}

	x0_offset = automation_edit->cursor_position_x * automation_edit->control_width;
      
	if(x0_offset + automation_edit->control_width > GTK_RANGE(automation_edit->hscrollbar)->adjustment->value + GTK_WIDGET(automation_edit->drawing_area)->allocation.width){
	  gtk_range_set_value(GTK_RANGE(automation_edit->hscrollbar),
			      x0_offset);
	}
      }
      break;
    case GDK_KEY_Up:
    case GDK_KEY_uparrow:
      {
	gdouble y0_offset;
      
	if(automation_edit->cursor_position_y > 0){
	  automation_edit->cursor_position_y -= 1;
	
	  do_feedback = TRUE;
	}

	y0_offset = automation_edit->cursor_position_y * automation_edit->control_height;
      
	if(y0_offset < GTK_RANGE(automation_edit->vscrollbar)->adjustment->value){
	  gtk_range_set_value(GTK_RANGE(automation_edit->vscrollbar),
			      y0_offset);
	}
      }
      break;
    case GDK_KEY_Down:
    case GDK_KEY_downarrow:
      {
	gdouble y0_offset;
      
	if(automation_edit->cursor_position_y > 0){
	  automation_edit->cursor_position_y -= 1;
	
	  do_feedback = TRUE;
	}

	y0_offset = automation_edit->cursor_position_y * automation_edit->control_height;
      
	if(y0_offset < GTK_RANGE(automation_edit->vscrollbar)->adjustment->value){
	  gtk_range_set_value(GTK_RANGE(automation_edit->vscrollbar),
			      y0_offset);
	}
      }
      break;
    case GDK_KEY_space:
      {
	AgsAcceleration *acceleration;
	
	do_feedback = TRUE;

	acceleration = ags_acceleration_new();

	acceleration->x = automation_edit->cursor_position_x;
	acceleration->y = automation_edit->cursor_position_y;

	/* add acceleration */
	ags_automation_editor_add_acceleration(automation_editor,
					       acceleration);
      }
      break;
    case GDK_KEY_Delete:
      {
	/* delete acceleration */
	ags_automation_editor_delete_acceleration(automation_editor,
						  automation_edit->cursor_position_x, automation_edit->cursor_position_y);
      }
      break;
    }

    gtk_widget_queue_draw(automation_edit);
  }
  
  return(retval);
}

void
ags_automation_edit_vscrollbar_value_changed(GtkRange *range, AgsAutomationEdit *automation_edit)
{
  AgsAutomationEditor *automation_editor;

  GtkAdjustment *piano_adjustment;
  
  automation_editor = gtk_widget_get_ancestor(automation_edit,
					      AGS_TYPE_AUTOMATION_EDITOR);

  /* queue draw */
  gtk_widget_queue_draw(automation_edit->drawing_area);
}

void
ags_automation_edit_hscrollbar_value_changed(GtkRange *range, AgsAutomationEdit *automation_edit)
{
  gdouble value;

  value = GTK_RANGE(automation_edit->hscrollbar)->adjustment->value / 64.0;
  gtk_adjustment_set_value(automation_edit->ruler->adjustment,
			   value);
  gtk_widget_queue_draw(automation_edit->ruler);
  
  /* queue draw */
  gtk_widget_queue_draw(automation_edit->drawing_area);
}
