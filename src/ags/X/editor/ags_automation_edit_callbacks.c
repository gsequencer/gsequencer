/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#include <ags/X/editor/ags_automation_edit_callbacks.h>

#include <ags/X/ags_automation_editor.h>

#include <ags/X/editor/ags_notebook.h>
#include <ags/X/editor/ags_automation_toolbar.h>

#include <ags/X/machine/ags_panel.h>
#include <ags/X/machine/ags_mixer.h>
#include <ags/X/machine/ags_drum.h>
#include <ags/X/machine/ags_matrix.h>
#include <ags/X/machine/ags_synth.h>
#include <ags/X/machine/ags_ffplayer.h>

#include <math.h>

gboolean
ags_automation_edit_drawing_area_expose_event(GtkWidget *widget, GdkEventExpose *event,
					      AgsAutomationEdit *automation_edit)
{
  cairo_t *cr;
  
  cr = gdk_cairo_create(widget->window);
  cairo_push_group(cr);
    
  ags_automation_edit_paint(automation_edit,
			    cr);

  cairo_pop_group_to_source(cr);
  cairo_paint(cr);

  return(TRUE);
}

gboolean
ags_automation_edit_drawing_area_configure_event(GtkWidget *widget, GdkEventConfigure *event,
						 AgsAutomationEdit *automation_edit)
{
  cairo_t *cr;
  
  automation_edit->flags |= AGS_AUTOMATION_EDIT_RESETING_VERTICALLY;
  ags_automation_edit_reset_vertically(automation_edit,
				       AGS_AUTOMATION_EDIT_RESET_VSCROLLBAR);
  automation_edit->flags &= (~AGS_AUTOMATION_EDIT_RESETING_VERTICALLY);

  automation_edit->flags |= AGS_AUTOMATION_EDIT_RESETING_HORIZONTALLY;
  ags_automation_edit_reset_horizontally(automation_edit,
					 AGS_AUTOMATION_EDIT_RESET_HSCROLLBAR |
					 AGS_AUTOMATION_EDIT_RESET_WIDTH);
  automation_edit->flags &= (~AGS_AUTOMATION_EDIT_RESETING_HORIZONTALLY);  

  cr = gdk_cairo_create(widget->window);
  cairo_push_group(cr);
    
  ags_automation_edit_paint(automation_edit,
			    cr);

  cairo_pop_group_to_source(cr);
  cairo_paint(cr);

  return(FALSE);
}

gboolean
ags_automation_edit_drawing_area_button_press_event(GtkWidget *widget, GdkEventButton *event,
						    AgsAutomationEdit *automation_edit)
{
  AgsAutomationEditor *automation_editor;
  AgsAutomationToolbar *automation_toolbar;

  double tact_factor, zoom_factor;
  double tact;
  
  cairo_t *cr;
  
  guint x, y;

  auto void ags_automation_edit_drawing_area_button_press_event_add_point();

  void ags_automation_edit_drawing_area_button_press_event_add_point(){
    AgsAcceleration *current_acceleration, *acceleration;

    GList *automation, *list;

    guint i;
    gboolean is_audio, is_output, is_input;
  
    automation = automation_editor->selected_machine->audio->automation;

    if(automation_edit == automation_editor->audio_automation_edit){
      is_audio = TRUE;
    }else if(automation_edit == automation_editor->output_automation_edit){
      is_output = TRUE;
    }else if(automation_edit == automation_editor->input_automation_edit){
      is_input = TRUE;
    }

    /* find automation area */
    list = automation_edit->automation_area;
    i = 0;
      
    while(list != NULL){
      if(i + AGS_AUTOMATION_AREA(list->data)->height > y){
	automation_edit->current_area = AGS_AUTOMATION_AREA(list->data);
	break;
      }

      i += i + AGS_AUTOMATION_AREA(list->data)->height + AGS_AUTOMATION_EDIT_DEFAULT_MARGIN;
      list = list->next;
    }

    if(list == NULL){
      return;
    }
      
    /* match specifier */
    while((automation = ags_automation_find_specifier(automation,
						      automation_edit->current_area->control_name)) != NULL){
      /* skip different target */
      if(is_output){
	GList *list;

	list = g_list_nth(automation_editor->output_notebook->tabs,
			  AGS_AUTOMATION(automation->data)->line);
	  
	if(!gtk_toggle_button_get_active(AGS_NOTEBOOK_TAB(list->data)->toggle)){
	  automation = automation->next;
	    
	  continue;
	}
      }

      if(is_input){
	GList *list;

	list = g_list_nth(automation_editor->input_notebook->tabs,
			  AGS_AUTOMATION(automation->data)->line);
	  
	if(!gtk_toggle_button_get_active(AGS_NOTEBOOK_TAB(list->data)->toggle)){
	  automation = automation->next;
	    
	  continue;
	}
      }

      /* check steps */
      if(AGS_AUTOMATION(automation->data)->steps < automation_edit->current_area->height){
	y = (guint) round(y / (automation_edit->current_area->height / AGS_AUTOMATION(automation->data)->steps));
      }
      
      current_acceleration = ags_automation_find_point(AGS_AUTOMATION(automation->data),
						       x * tact, y,
						       FALSE);

      /* add acceleration */
      if(current_acceleration == NULL){
	acceleration = ags_acceleration_new();
	acceleration->x = x * tact;
	acceleration->y = y;
      
	ags_automation_add_acceleration(automation->data,
					acceleration,
					FALSE);
      }

      automation = automation->next;
    }
  }
  
  automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor(GTK_WIDGET(automation_edit),
								      AGS_TYPE_AUTOMATION_EDITOR);

  if(automation_editor->selected_machine != NULL &&
     event->button == 1){
    automation_toolbar = automation_editor->automation_toolbar;
    
    zoom_factor = 1.0 / 4.0;

    tact_factor = exp2(8.0 - (double) gtk_combo_box_get_active(automation_toolbar->zoom));
    tact = exp2((double) gtk_combo_box_get_active(automation_toolbar->zoom) - 4.0);

    x = (guint) GTK_RANGE(automation_edit->hscrollbar)->adjustment->value + (guint) event->x;
    y = (guint) GTK_RANGE(automation_edit->vscrollbar)->adjustment->value + (guint) event->y;

    if(automation_toolbar->selected_edit_mode == automation_toolbar->position){
      automation_edit->flags |= AGS_AUTOMATION_EDIT_POSITION_CURSOR;
    }else if(automation_toolbar->selected_edit_mode == automation_toolbar->edit){
      automation_edit->flags |= AGS_AUTOMATION_EDIT_ADDING_ACCELERATION;

      cr = gdk_cairo_create(widget->window);
      cairo_push_group(cr);
    
      ags_automation_edit_drawing_area_button_press_event_add_point();
      ags_automation_edit_paint(automation_edit,
				cr);
      
      cairo_pop_group_to_source(cr);
      cairo_paint(cr);
      
      automation_edit->edit_x = x * tact;
      automation_edit->edit_y = y * tact;
    }else if(automation_toolbar->selected_edit_mode == automation_toolbar->clear){
      automation_edit->flags |= AGS_AUTOMATION_EDIT_DELETING_ACCELERATION;
    }else if(automation_toolbar->selected_edit_mode == automation_toolbar->select){
      automation_edit->flags |= AGS_AUTOMATION_EDIT_SELECTING_ACCELERATIONS;

      automation_edit->select_x0 = x;
      automation_edit->select_y0 = y;
    }
  }

  return(TRUE);
}

gboolean
ags_automation_edit_drawing_area_button_release_event(GtkWidget *widget, GdkEventButton *event,
						      AgsAutomationEdit *automation_edit)
{
  AgsAutomationEditor *automation_editor;
  AgsAutomationToolbar *automation_toolbar;
  AgsNotebook *notebook;

  cairo_t *cr;
  
  double tact_factor, zoom_factor;
  double tact;
  guint x, y;

  auto void ags_automation_edit_drawing_area_button_press_event_move_point(cairo_t *cr);

  void ags_automation_edit_drawing_area_button_press_event_move_point(cairo_t *cr){
    AgsAcceleration *current_acceleration, *prev_acceleration, *next_acceleration;

    GList *automation, *current_list;
      
    guint i;
    gboolean is_audio, is_output, is_input;
      
    /* set acceleration value */
    automation = automation_editor->selected_machine->audio->automation;
    i = automation_edit->current_area->y;

    if(automation_edit == automation_editor->audio_automation_edit){
      is_audio = TRUE;
    }else if(automation_edit == automation_editor->output_automation_edit){
      is_output = TRUE;
    }else if(automation_edit == automation_editor->input_automation_edit){
      is_input = TRUE;
    }
      
    while((automation = ags_automation_find_specifier(automation,
						      automation_edit->current_area->control_name)) != NULL){
      /* skip different target */
      if(is_output){
	GList *list;

	list = g_list_nth(automation_editor->output_notebook->tabs,
			  AGS_AUTOMATION(automation->data)->line);
	  
	if(!gtk_toggle_button_get_active(AGS_NOTEBOOK_TAB(list->data)->toggle)){
	  automation = automation->next;
	    
	  continue;
	}
      }

      if(is_input){
	GList *list;

	list = g_list_nth(automation_editor->input_notebook->tabs,
			  AGS_AUTOMATION(automation->data)->line);
	  
	if(!gtk_toggle_button_get_active(AGS_NOTEBOOK_TAB(list->data)->toggle)){
	  automation = automation->next;
	    
	  continue;
	}
      }
      
      /* apply boundaries */
      if(automation_edit->current_area->y > automation_edit->edit_y){
	current_acceleration->y = AGS_AUTOMATION(automation->data)->lower;
      }else if(automation_edit->current_area->y < automation_edit->edit_y + automation_edit->current_area->height){
	current_acceleration->y = AGS_AUTOMATION(automation->data)->upper;
      }else{
	if(AGS_AUTOMATION(automation->data)->steps < automation_edit->current_area->height){
	  y = (guint) round(y / (automation_edit->current_area->height / AGS_AUTOMATION(automation->data)->steps));
	}
      }

      /* move/position acceleration */
      current_acceleration = ags_automation_find_point(AGS_AUTOMATION(automation->data),
						       automation_edit->edit_x, automation_edit->edit_y,
						       FALSE);
      current_acceleration->y = y;

      current_list = g_list_find(AGS_AUTOMATION(automation->data)->acceleration,
				 current_acceleration);

      if(current_list->prev != NULL && current_list->next != NULL){
	prev_acceleration = current_list->prev->data;
	next_acceleration = current_list->next->data;

	if(x * tact > prev_acceleration->x){
	  if(x * tact < next_acceleration->x){
	    current_acceleration->x = x * tact;
	  }else{
	    current_acceleration->x = next_acceleration->x - 1;
	  }
	}else{
	  current_acceleration->x = prev_acceleration->x + 1;
	}
      }

      /* iterate */
      automation = automation->next;
    }

    automation_edit->current_area = NULL;
  }
  
  automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor(GTK_WIDGET(automation_edit),
								      AGS_TYPE_AUTOMATION_EDITOR);

  if(automation_editor->selected_machine != NULL &&
     event->button == 1){
    automation_toolbar = automation_editor->automation_toolbar;
    
    zoom_factor = 1.0 / 4.0;

    tact_factor = exp2(8.0 - (double) gtk_combo_box_get_active(automation_toolbar->zoom));
    tact = exp2((double) gtk_combo_box_get_active(automation_toolbar->zoom) - 4.0);

    x = (guint) GTK_RANGE(automation_edit->hscrollbar)->adjustment->value + (guint) event->x;
    y = (guint) GTK_RANGE(automation_edit->vscrollbar)->adjustment->value + (guint) event->y;

    if((AGS_AUTOMATION_EDIT_POSITION_CURSOR & (automation_edit->flags)) != 0){
      automation_edit->flags &= (~AGS_AUTOMATION_EDIT_POSITION_CURSOR);

      //TODO:JK: implement me
    }else if((AGS_AUTOMATION_EDIT_ADDING_ACCELERATION & (automation_edit->flags)) != 0){
      automation_edit->flags &= (~AGS_AUTOMATION_EDIT_ADDING_ACCELERATION);

      if(automation_edit->current_area != NULL){
	cr = gdk_cairo_create(widget->window);
	cairo_push_group(cr);
    
	ags_automation_edit_drawing_area_button_press_event_move_point(cr);
	ags_automation_edit_paint(automation_edit,
				  cr);

	cairo_pop_group_to_source(cr);
	cairo_paint(cr);

	automation_edit->current_area = NULL;
	
	automation_edit->edit_x = 0;
	automation_edit->edit_y = 0;
      }
    }else if((AGS_AUTOMATION_EDIT_DELETING_ACCELERATION & (automation_edit->flags)) != 0){
      automation_edit->flags &= (~AGS_AUTOMATION_EDIT_DELETING_ACCELERATION);

      //TODO:JK: implement me
    }else if((AGS_AUTOMATION_EDIT_SELECTING_ACCELERATIONS & (automation_edit->flags)) != 0){
      automation_edit->flags &= (~AGS_AUTOMATION_EDIT_SELECTING_ACCELERATIONS);

      //TODO:JK: implement me
    }
  }
  
  return(FALSE);
}

gboolean
ags_automation_edit_drawing_area_motion_notify_event(GtkWidget *widget, GdkEventMotion *event,
						     AgsAutomationEdit *automation_edit)
{
  AgsAutomationEditor *automation_editor;
  AgsAutomationToolbar *automation_toolbar;
  
  cairo_t *cr;

  double tact_factor, zoom_factor;
  double tact;
  guint x, y;
  
  auto void ags_automation_edit_drawing_area_motion_notify_event_move_point(cairo_t *cr);
  auto void ags_automation_edit_drawing_area_motion_notify_event_draw_selection(cairo_t *cr);

  void ags_automation_edit_drawing_area_motion_notify_event_move_point(cairo_t *cr){
    AgsAcceleration *current_acceleration, *prev_acceleration, *next_acceleration;

    GList *automation, *current_list;
      
    guint i;
    gboolean is_audio, is_output, is_input;
      
    /* set acceleration value */
    automation = automation_editor->selected_machine->audio->automation;
    i = automation_edit->current_area->y;

    if(automation_edit == automation_editor->audio_automation_edit){
      is_audio = TRUE;
    }else if(automation_edit == automation_editor->output_automation_edit){
      is_output = TRUE;
    }else if(automation_edit == automation_editor->input_automation_edit){
      is_input = TRUE;
    }
      
    while((automation = ags_automation_find_specifier(automation,
						      automation_edit->current_area->control_name)) != NULL){
      /* skip different target */
      if(is_output){
	GList *list;

	list = g_list_nth(automation_editor->output_notebook->tabs,
			  AGS_AUTOMATION(automation->data)->line);
	  
	if(!gtk_toggle_button_get_active(AGS_NOTEBOOK_TAB(list->data)->toggle)){
	  automation = automation->next;
	    
	  continue;
	}
      }

      if(is_input){
	GList *list;

	list = g_list_nth(automation_editor->input_notebook->tabs,
			  AGS_AUTOMATION(automation->data)->line);
	  
	if(!gtk_toggle_button_get_active(AGS_NOTEBOOK_TAB(list->data)->toggle)){
	  automation = automation->next;
	    
	  continue;
	}
      }
	
      /* apply boundaries */
      if(automation_edit->current_area->y > automation_edit->edit_y){
	current_acceleration->y = AGS_AUTOMATION(automation->data)->lower;
      }else if(automation_edit->current_area->y < automation_edit->edit_y + automation_edit->current_area->height){
	current_acceleration->y = AGS_AUTOMATION(automation->data)->upper;
      }else{
	if(AGS_AUTOMATION(automation->data)->steps < automation_edit->current_area->height){
	  y = (guint) round(y / (automation_edit->current_area->height / AGS_AUTOMATION(automation->data)->steps));
	}
      }

      /* move/position acceleration */
      current_acceleration = ags_automation_find_point(AGS_AUTOMATION(automation->data),
						       automation_edit->edit_x, automation_edit->edit_y,
						       FALSE);
      current_acceleration->y = y;

      current_list = g_list_find(AGS_AUTOMATION(automation->data)->acceleration,
				 current_acceleration);

      if(current_list->prev != NULL && current_list->next != NULL){
	prev_acceleration = current_list->prev->data;
	next_acceleration = current_list->next->data;

	if(x * tact > prev_acceleration->x){
	  if(x * tact < next_acceleration->x){
	    current_acceleration->x = x * tact;
	  }else{
	    current_acceleration->x = (next_acceleration->x - 1);
	  }
	}else{
	  current_acceleration->x = prev_acceleration->x + 1;
	}
      }

      /* iterate */
      automation = automation->next;
    }

    automation_edit->current_area = NULL;
  }

  void ags_automation_edit_drawing_area_motion_notify_event_draw_selection(cairo_t *cr){
    GtkAllocation allocation;
    guint x0_offset, x1_offset, y0_offset, y1_offset;
    guint x0, x1, y0, y1, width, height;
    guint x0_viewport, x1_viewport, y0_viewport, y1_viewport;

    /* get viewport */
    gtk_widget_get_allocation(widget, &allocation);

    x0_viewport = (guint) GTK_RANGE(automation_edit->hscrollbar)->adjustment->value;
    x1_viewport = (guint) GTK_RANGE(automation_edit->hscrollbar)->adjustment->value + allocation.width;

    y0_viewport = (guint) GTK_RANGE(automation_edit->vscrollbar)->adjustment->value;
    y1_viewport = (guint) GTK_RANGE(automation_edit->vscrollbar)->adjustment->value + allocation.height;

    /* get real size and offset */
    x0 = automation_edit->select_x0;
    x1 = x0_viewport + event->x;

    if(x0 > x1){
      x0_offset = x1;
      x1_offset = x0;

      x1 = x0_offset;
      x0 = x1_offset;
    }else{
      x0_offset = x0;
      x1_offset = x1;
    }

    /* get drawable size and offset */
    if(x0 < x0_viewport){
      x0 -= x0_viewport;
      width = x1 - x0;
    }else{
      x0 -= x0_viewport;
      width = x1 - x0;
    }

    if(x1 > x1_viewport){
      width -= (x1 - x1_viewport);
    }else{
      width -= x0_viewport;
    }

    /* get real size and offset */
    y0 = automation_edit->select_y0;
    y1 = y0_viewport + event->y;

    if(y0 > y1){
      y0_offset = y1;
      y1_offset = y0;

      y1 = y0_offset;
      y0 = y1_offset;
    }else{
      y0_offset = y0;
      y1_offset = y1;
    }

    /* get drawable size and offset */
    if(y0 < y0_viewport){
      y0 = 0;
      height = y1_offset - y0_viewport;
    }else{
      y0 -= y0_viewport;
      height = y1 - y0;
    }

    if(y1 > y1_viewport){
      height -= (y1 - y1_viewport);
    }else{
      height -= y0_viewport;
    }

    cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 0.3);
    cairo_rectangle(cr, (double) x0, (double) y0, (double) width, (double) height);
    cairo_fill(cr);
  }

  automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor(GTK_WIDGET(automation_edit),
								      AGS_TYPE_AUTOMATION_EDITOR);

  if(automation_editor->selected_machine != NULL){
    automation_toolbar = automation_editor->automation_toolbar;

    zoom_factor = 1.0 / 4.0;

    tact_factor = exp2(8.0 - (double) gtk_combo_box_get_active(automation_toolbar->zoom));
    tact = exp2((double) gtk_combo_box_get_active(automation_toolbar->zoom) - 4.0);

    x = (guint) GTK_RANGE(automation_edit->hscrollbar)->adjustment->value + (guint) event->x;
    y = (guint) GTK_RANGE(automation_edit->vscrollbar)->adjustment->value + (guint) event->y;
    
    if((AGS_AUTOMATION_EDIT_POSITION_CURSOR & (automation_edit->flags)) != 0){
      //TODO:JK: implement me
    }else if((AGS_AUTOMATION_EDIT_ADDING_ACCELERATION & (automation_edit->flags)) != 0){
      if(automation_edit->current_area != NULL){
	cr = gdk_cairo_create(widget->window);
	cairo_push_group(cr);
    
	ags_automation_edit_drawing_area_motion_notify_event_move_point(cr);
	ags_automation_edit_paint(automation_edit,
				  cr);

	cairo_pop_group_to_source(cr);
	cairo_paint(cr);

	automation_edit->edit_x = x * tact;
	automation_edit->edit_y = y;
      }
    }else if((AGS_AUTOMATION_EDIT_SELECTING_ACCELERATIONS & (automation_edit->flags)) != 0){
      cr = gdk_cairo_create(widget->window);
      cairo_push_group(cr);
    
      ags_automation_edit_paint(automation_edit,
				cr);
      ags_automation_edit_drawing_area_motion_notify_event_draw_selection(cr);

      cairo_pop_group_to_source(cr);
      cairo_paint(cr);

      automation_edit->select_x1 = x;
      automation_edit->select_y1 = y;
    }
  }
  
  return(FALSE);
}

void
ags_automation_edit_vscrollbar_value_changed(GtkRange *range, AgsAutomationEdit *automation_edit)
{
  if((AGS_AUTOMATION_EDIT_RESETING_VERTICALLY & automation_edit->flags) != 0){
    return;
  }

  automation_edit->flags |= AGS_AUTOMATION_EDIT_RESETING_VERTICALLY;
  ags_automation_edit_reset_vertically(automation_edit, 0);
  automation_edit->flags &= (~AGS_AUTOMATION_EDIT_RESETING_VERTICALLY);
}

void
ags_automation_edit_hscrollbar_value_changed(GtkRange *range, AgsAutomationEdit *automation_edit)
{
  if((AGS_AUTOMATION_EDIT_RESETING_HORIZONTALLY & automation_edit->flags) != 0){
    return;
  }

  g_message("%f\0", GTK_RANGE(automation_edit->hscrollbar)->adjustment->value);
  
  /* reset ruler */
  gtk_adjustment_set_value(automation_edit->ruler->adjustment,
			   GTK_RANGE(automation_edit->hscrollbar)->adjustment->value / 64.0);
  gtk_widget_queue_draw(automation_edit->ruler);

  /* update automation edit */
  automation_edit->flags |= AGS_AUTOMATION_EDIT_RESETING_HORIZONTALLY;
  ags_automation_edit_reset_horizontally(automation_edit, 0);
  automation_edit->flags &= (~AGS_AUTOMATION_EDIT_RESETING_HORIZONTALLY);
}
