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

  automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor(GTK_WIDGET(automation_edit),
								      AGS_TYPE_AUTOMATION_EDITOR);

  if(automation_editor->selected_machine != NULL &&
     event->button == 1){
    AgsAutomationToolbar *automation_toolbar;

    automation_toolbar = automation_editor->automation_toolbar;

    if(automation_toolbar->selected_edit_mode == automation_toolbar->position){
      automation_edit->flags |= AGS_AUTOMATION_EDIT_POSITION_CURSOR;
    }else if(automation_toolbar->selected_edit_mode == automation_toolbar->edit){
      automation_edit->flags |= AGS_AUTOMATION_EDIT_ADDING_ACCELERATION;
    }else if(automation_toolbar->selected_edit_mode == automation_toolbar->clear){
      automation_edit->flags |= AGS_AUTOMATION_EDIT_DELETING_ACCELERATION;
    }else if(automation_toolbar->selected_edit_mode == automation_toolbar->select){
      automation_edit->flags |= AGS_AUTOMATION_EDIT_SELECTING_ACCELERATIONS;

      automation_edit->select_x0 = (guint) GTK_RANGE(automation_edit->hscrollbar)->adjustment->value + event->x;
      automation_edit->select_y0 = (guint) GTK_RANGE(automation_edit->vscrollbar)->adjustment->value + event->y;
    }
  }

  return(TRUE);
}

gboolean
ags_automation_edit_drawing_area_button_release_event(GtkWidget *widget, GdkEventButton *event,
						      AgsAutomationEdit *automation_edit)
{
  AgsAutomationEditor *automation_editor;
  AgsNotebook *notebook;
  
  guint x, y;

  automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor(GTK_WIDGET(automation_edit),
								      AGS_TYPE_AUTOMATION_EDITOR);

  if(automation_editor->selected_machine != NULL &&
     event->button == 1){
    x = (guint) event->x;
    y = (guint) event->y;

    if((AGS_AUTOMATION_EDIT_POSITION_CURSOR & (automation_edit->flags)) != 0){
      automation_edit->flags &= (~AGS_AUTOMATION_EDIT_POSITION_CURSOR);

      //TODO:JK: implement me
    }else if((AGS_AUTOMATION_EDIT_ADDING_ACCELERATION & (automation_edit->flags)) != 0){
      AgsAutomationArea *automation_area;
      
      AgsAcceleration *acceleration;

      GList *automation;

      gboolean is_audio, is_output, is_input;
  
      automation_edit->flags &= (~AGS_AUTOMATION_EDIT_ADDING_ACCELERATION);

      automation = automation_editor->selected_machine->audio->automation;

      if(automation_edit == automation_editor->audio_automation_edit){
	is_audio = TRUE;
      }else if(automation_edit == automation_editor->output_automation_edit){
	is_output = TRUE;
      }else if(automation_edit == automation_editor->input_automation_edit){
	is_input = TRUE;
      }
      
      while((automation = ags_automation_find_specifier(automation,
							automation_area->control_name)) != NULL){
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
	
	acceleration = ags_acceleration_new();
	acceleration->x = x;
	acceleration->y = y;

	ags_automation_add_acceleration(automation->data,
					acceleration,
					FALSE);

	automation = automation->next;
      }
    }else if((AGS_AUTOMATION_EDIT_DELETING_ACCELERATION & (automation_edit->flags)) != 0){
      automation_edit->flags &= (~AGS_AUTOMATION_EDIT_DELETING_ACCELERATION);

      //TODO:JK: implement me
    }else if((AGS_AUTOMATION_EDIT_SELECTING_ACCELERATIONS & (automation_edit->flags)) != 0){
      automation_edit->flags &= (~AGS_AUTOMATION_EDIT_SELECTING_ACCELERATIONS);

      //TODO:JK: implement me
    }

    gtk_widget_queue_draw(automation_edit->drawing_area);
  }
  
  return(FALSE);
}

gboolean
ags_automation_edit_drawing_area_motion_notify_event(GtkWidget *widget, GdkEventMotion *event,
						     AgsAutomationEdit *automation_edit)
{
  cairo_t *cr;
  
  auto void ags_automation_edit_drawing_area_motion_notify_event_draw_selection(cairo_t *cr);

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

  if((AGS_AUTOMATION_EDIT_SELECTING_ACCELERATIONS & (automation_edit->flags)) != 0){
    cr = gdk_cairo_create(widget->window);
    cairo_push_group(cr);
    
    ags_automation_edit_paint(automation_edit,
			      cr);
    ags_automation_edit_drawing_area_motion_notify_event_draw_selection(cr);

    cairo_pop_group_to_source(cr);
    cairo_paint(cr);

    automation_edit->select_x1 = (guint) GTK_RANGE(automation_edit->hscrollbar)->adjustment->value + event->x;
    automation_edit->select_y1 = (guint) GTK_RANGE(automation_edit->vscrollbar)->adjustment->value + event->y;
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
