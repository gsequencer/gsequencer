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

#include <ags/X/ags_editor_callbacks.h>

#include <ags/X/ags_window.h>

#include <ags/X/editor/ags_toolbar.h>
#include <ags/X/editor/ags_notebook.h>
#include <ags/X/editor/ags_meter.h>

#include <ags/X/machine/ags_panel.h>
#include <ags/X/machine/ags_mixer.h>
#include <ags/X/machine/ags_drum.h>
#include <ags/X/machine/ags_matrix.h>
#include <ags/X/machine/ags_synth.h>
#include <ags/X/machine/ags_ffplayer.h>

#include <math.h>
#include <string.h>
#include <cairo.h>

void ags_editor_link_index_callback(GtkRadioButton *radio_button, GtkDialog *dialog);
void ags_editor_link_index_response_callback(GtkDialog *dialog, int response, AgsEditor *editor);

#define AGS_EDITOR_LINK_INDEX_VBOX "AgsEditorLinkIndexVBox"

void
ags_editor_parent_set_callback(GtkWidget  *widget, GtkObject *old_parent, AgsEditor *editor)
{
  if(old_parent != NULL)
    return;
  
  editor->flags |= AGS_EDITOR_RESETING_HORIZONTALLY;
  ags_editor_reset_horizontally(editor, AGS_EDITOR_RESET_HSCROLLBAR);
  editor->flags &= (~AGS_EDITOR_RESETING_HORIZONTALLY);
}

gboolean
ags_editor_destroy_callback(GtkObject *object, AgsEditor *editor)
{
  ags_editor_destroy(object);

  return(TRUE);
}

void
ags_editor_show_callback(GtkWidget *widget, AgsEditor *editor)
{
  ags_editor_show(widget);
}

void
ags_editor_index_callback(GtkRadioButton *radio_button, AgsEditor *editor)
{
  AgsMachine *machine;

  editor->selected = radio_button;

  if(editor->selected != NULL)
    machine = (AgsMachine *) g_object_get_data((GObject *) editor->selected, (char *) g_type_name(AGS_TYPE_MACHINE));
  else
    machine = NULL;

  ags_editor_change_machine(editor, machine);

  if(machine != NULL){
    guint pads;

    if((AGS_AUDIO_NOTATION_DEFAULT & (machine->audio->flags)) != 0){
      pads = machine->audio->input_pads;
    }else{
      pads = machine->audio->output_pads;
    }

    editor->map_height = pads * editor->control_height;
  }

  editor->flags |= AGS_EDITOR_RESETING_VERTICALLY;
  ags_editor_reset_vertically(editor, AGS_EDITOR_RESET_VSCROLLBAR);
  editor->flags &= (~AGS_EDITOR_RESETING_VERTICALLY);
}

gboolean
ags_editor_button_press_callback(GtkWidget *widget, GdkEventButton *event, AgsEditor *editor)
{
  if(event->button == 3)
    gtk_menu_popup (GTK_MENU (editor->popup),
                    NULL, NULL, NULL, NULL,
                    event->button, event->time);

  return(TRUE);
}

gboolean
ags_editor_drawing_area_expose_event(GtkWidget *widget, GdkEventExpose *event, AgsEditor *editor)
{
  if(editor->selected != NULL){
    AgsMachine *machine;

    machine = (AgsMachine *) g_object_get_data((GObject *) editor->selected, g_type_name(AGS_TYPE_MACHINE));

    if(machine != NULL){
      cairo_t *cr;

      cr = gdk_cairo_create(widget->window);
      cairo_push_group(cr);

      if(AGS_IS_PANEL(machine)){
      }else if(AGS_IS_MIXER(machine)){
      }else if(AGS_IS_DRUM(machine)){
	ags_meter_paint(editor->meter);
	ags_editor_draw_segment(editor, cr);
	ags_editor_draw_notation(editor, cr);
      }else if(AGS_IS_MATRIX(machine)){
	ags_meter_paint(editor->meter);
	ags_editor_draw_segment(editor, cr);
	ags_editor_draw_notation(editor, cr);
      }else if(AGS_IS_SYNTH(machine)){
	ags_meter_paint(editor->meter);
	ags_editor_draw_segment(editor, cr);
	ags_editor_draw_notation(editor, cr);
      }else if(AGS_IS_FFPLAYER(machine)){
	ags_meter_paint(editor->meter);
	ags_editor_draw_segment(editor, cr);
	ags_editor_draw_notation(editor, cr);
      }

      if(editor->toolbar->selected_edit_mode == editor->toolbar->position){
	ags_editor_draw_position(editor, cr);
      }

      cairo_pop_group_to_source(cr);
      cairo_paint(cr);
    }
  }

  return(TRUE);
}

gboolean
ags_editor_drawing_area_configure_event(GtkWidget *widget, GdkEventConfigure *event, AgsEditor *editor)
{
  editor->flags |= AGS_EDITOR_RESETING_VERTICALLY;
  ags_editor_reset_vertically(editor, AGS_EDITOR_RESET_VSCROLLBAR);
  editor->flags &= (~AGS_EDITOR_RESETING_VERTICALLY);

  editor->flags |= AGS_EDITOR_RESETING_HORIZONTALLY;
  ags_editor_reset_horizontally(editor, AGS_EDITOR_RESET_HSCROLLBAR);
  editor->flags &= (~AGS_EDITOR_RESETING_HORIZONTALLY);  

  return(FALSE);
}

gboolean
ags_editor_drawing_area_button_press_event (GtkWidget *widget, GdkEventButton *event, AgsEditor *editor)
{
  AgsMachine *machine;
  double tact, zoom;
  void ags_editor_drawing_area_button_press_event_set_control(){
    AgsNote *note;
    guint note_offset_x0, note_offset_y0;
    guint note_x, note_y;

    if(editor->control.y0 >= editor->map_height || editor->control.x0 >= editor->map_width)
      return;

    note_offset_x0 = (guint) (ceil((double) (editor->control.x0_offset) / (double) (editor->control_current.control_width)));

    if(editor->control.x0 >= editor->control_current.x0)
      note_x = (guint) (floor((double) (editor->control.x0 - editor->control_current.x0) / (double) (editor->control_current.control_width)));
    else{
      note_offset_x0 -= 1;
      note_x = 0;
    }

    note_offset_y0 = (guint) ceil((double) (editor->control.y0_offset) / (double) (editor->control_height));

    if(editor->control.y0 >= editor->y0)
      note_y = (guint) floor((double) (editor->control.y0 - editor->y0) / (double) (editor->control_height));
    else{
      note_offset_y0 -= 1;
      note_y = 0;
    }

    note = editor->control.note;
    note->flags = AGS_NOTE_GUI;
    note->x[0] = (note_x * tact) + (note_offset_x0 * tact);
    note->x[1] = (guint) note->x[0] + 1;
    note->y = note_y + note_offset_y0;
  }

  if(editor->selected != NULL &&
     event->button == 1 &&
     (machine = (AgsMachine *) g_object_get_data((GObject *) editor->selected, (char *) g_type_name(AGS_TYPE_MACHINE))) != NULL){
    AgsToolbar *toolbar;

    toolbar = editor->toolbar;

    if(toolbar->selected_edit_mode == toolbar->position){
      editor->flags |= AGS_EDITOR_POSITION_CURSOR;
    }else if(toolbar->selected_edit_mode == toolbar->edit){
      editor->flags |= AGS_EDITOR_ADDING_NOTE;
    }else if(toolbar->selected_edit_mode == toolbar->clear){
      editor->flags |= AGS_EDITOR_DELETING_NOTE;
    }else if(toolbar->selected_edit_mode == toolbar->select){
      editor->flags |= AGS_EDITOR_SELECTING_NOTES;
    }

    /* store the events position */
    editor->control.x0_offset = (guint) round((double) editor->hscrollbar->scrollbar.range.adjustment->value);
    editor->control.y0_offset = (guint) round((double) editor->vscrollbar->scrollbar.range.adjustment->value);

    editor->control.x0 = (guint) event->x;
    editor->control.y0 = (guint) event->y;

    if((AGS_EDITOR_ADDING_NOTE & (editor->flags)) != 0 ||
       (AGS_EDITOR_POSITION_CURSOR & (editor->flags)) != 0){
      tact = exp2(8.0 - (double) gtk_option_menu_get_history(editor->toolbar->tact));
      
      if(AGS_IS_PANEL(machine)){
      }else if(AGS_IS_MIXER(machine)){
      }else if(AGS_IS_DRUM(machine)){
	ags_editor_drawing_area_button_press_event_set_control();
      }else if(AGS_IS_MATRIX(machine)){
	ags_editor_drawing_area_button_press_event_set_control();
      }else if(AGS_IS_SYNTH(machine)){
	ags_editor_drawing_area_button_press_event_set_control();
      }else if(AGS_IS_FFPLAYER(machine)){
	ags_editor_drawing_area_button_press_event_set_control();
      }
    }
  }

  return(TRUE);
}

gboolean
ags_editor_drawing_area_button_release_event(GtkWidget *widget, GdkEventButton *event, AgsEditor *editor)
{
  AgsMachine *machine;
  AgsNote *note, *note0;
  double tact;
  void ags_editor_drawing_area_button_release_event_set_control(){
    GList *list_notation;
    guint note_x, note_y;
    guint note_offset_x1;

    if(editor->control.x0 >= editor->map_width)
      editor->control.x0 = editor->map_width - 1;

    note_offset_x1 = (guint) (ceil((double) (editor->control.x1_offset)  / (double) (editor->control_current.control_width)));

    if(editor->control.x1 >= editor->control_current.x0)
      note_x = (guint) (ceil((double) (editor->control.x1 - editor->control_current.x0) / (double) (editor->control_current.control_width)));
    else{
      note_offset_x1 -= 1;
      note_x = 0;
    }

    note->x[1] = (note_x * tact) + (note_offset_x1 * tact);

    list_notation = machine->audio->notation;

    if(gtk_option_menu_get_history(editor->toolbar->mode) == 0){
      if(gtk_notebook_get_n_pages((GtkNotebook *) editor->notebook) > 0){
	list_notation = g_list_nth(list_notation, gtk_notebook_get_current_page((GtkNotebook *) editor->notebook));

	note0 = ags_note_duplicate(note);

	ags_notation_add_note(AGS_NOTATION(list_notation->data), note0, FALSE);
      }
    }else{
      while(list_notation != NULL ){
	note0 = ags_note_duplicate(note);

	ags_notation_add_note(AGS_NOTATION(list_notation->data), note0, FALSE);

	list_notation = list_notation->next;
      }
    }

    fprintf(stdout, "x0 = %llu\nx1 = %llu\ny  = %llu\n\n\0", (long long unsigned int) note->x[0], (long long unsigned int) note->x[1], (long long unsigned int) note->y);
  }
  void ags_editor_drawing_area_button_release_event_draw_control(cairo_t *cr){
    guint x, y, width, height;

    widget = (GtkWidget *) editor->drawing_area;
    //    cr = gdk_cairo_create(widget->window);

    x = note->x[0] * editor->control_unit.control_width;
    width = note->x[1] * editor->control_unit.control_width;

    if(x < editor->control.x1_offset){
      if(width > editor->control.x1_offset){
	width -= (guint) x;
	x = 0;
      }else{
	return;
      }
    }else if(x < editor->control.x1_offset + widget->allocation.width){
      width -= x;
      x -= editor->control.x1_offset;
    }else{
      return;
    }

    if(x + width > widget->allocation.width)
      width = widget->allocation.width - x;

    y = note->y * editor->control_height;

    if(y < editor->control.y1_offset){
      if(y + editor->control_height - editor->control_margin_y < editor->control.y1_offset){
	return;
      }else{
	if(y + editor->control_margin_y < editor->control.y1_offset){
	  height = editor->control_height;
	  y = y + editor->control_margin_y - editor->control.y1_offset;
	}else{
	  height = editor->y0;
	  y -= editor->control.y1_offset;
	}
      }
    }else if(y < editor->control.y1_offset + widget->allocation.height - editor->control_height){
      height = editor->control_height - 2 * editor->control_margin_y;
      y = y - editor->control.y1_offset + editor->control_margin_y;
    }else{
      if(y > editor->control.y1_offset + widget->allocation.height - editor->y1 + editor->control_margin_y){
	return;
      }else{
	height = editor->y0;
	y = y - editor->control.y1_offset + editor->control_margin_y;
      }
    }

    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
    cairo_fill(cr);
  }
  void ags_editor_drawing_area_button_release_event_delete_point(){
    GList *list_notation;
    guint x, y;

    x = editor->control.x0_offset + editor->control.x0;
    y = editor->control.y0_offset + editor->control.y0;

    x = (guint) ceil((double) x / (double) (editor->control_unit.control_width));
    y = (guint) floor((double) y / (double) (editor->control_height));

    /* select notes */
    list_notation = machine->audio->notation;

    if(gtk_option_menu_get_history(editor->toolbar->mode) == 0){
      if(gtk_notebook_get_n_pages((GtkNotebook *) editor->notebook) > 0){
	list_notation = g_list_nth(list_notation, gtk_notebook_get_current_page((GtkNotebook *) editor->notebook));

	ags_notation_remove_note_at_position(AGS_NOTATION(list_notation->data),
					     x, y);
      }
    }else{
      while(list_notation != NULL ){
	ags_notation_remove_note_at_position(AGS_NOTATION(list_notation->data),
					     x, y);

	list_notation = list_notation->next;
      }
    }
  }
  void ags_editor_drawing_area_button_release_event_select_region(){
    GList *list_notation;
    guint x0, x1, y0, y1;

    /* get real size and offset */
    x0 = editor->control.x0_offset + editor->control.x0;
    x1 = editor->control.x1_offset + editor->control.x1;

    if(x0 > x1){
      guint tmp;

      tmp = x1;
      x1 = x0;
      x0 = tmp;
    }

    /* convert to region */
    x0 = (guint) (floor((double) x0 / (double) (editor->control_current.control_width)) * tact);
    x1 = (guint) (ceil((double) x1 / (double) (editor->control_current.control_width)) * tact);

    /* get real size and offset */
    y0 = editor->control.y0_offset + editor->control.y0;
    y1 = editor->control.y1_offset + editor->control.y1;

    if(y0 > y1){
      guint tmp;

      tmp = y1;
      y1 = y0;
      y0 = tmp;
    }

    /* convert to region */
    y0 = (guint) floor((double) y0 / (double) (editor->control_height));
    y1 = (guint) ceil((double) y1 / (double) (editor->control_height));

    /* select notes */
    list_notation = machine->audio->notation;

    if(gtk_option_menu_get_history(editor->toolbar->mode) == 0){
      if(gtk_notebook_get_n_pages((GtkNotebook *) editor->notebook) > 0){
	list_notation = g_list_nth(list_notation, gtk_notebook_get_current_page((GtkNotebook *) editor->notebook));

	ags_notation_add_region_to_selection(AGS_NOTATION(list_notation->data),
					     x0, y0,
					     x1, y1,
					     TRUE);
      }
    }else{
      while(list_notation != NULL ){
	ags_notation_add_region_to_selection(AGS_NOTATION(list_notation->data),
					     x0, y0,
					     x1, y1,
					     TRUE);

	list_notation = list_notation->next;
      }
    }

  }

  if(editor->selected != NULL && event->button == 1){
    cairo_t *cr;

    editor->control.x1 = (guint) event->x;
    editor->control.y1 = (guint) event->y;

    machine = AGS_MACHINE(g_object_get_data((GObject *) editor->selected, (char *) g_type_name(AGS_TYPE_MACHINE)));
    note = editor->control.note;

    /* store the events position */
    editor->control.x1_offset = (guint) round((double) editor->hscrollbar->scrollbar.range.adjustment->value);
    editor->control.y1_offset = (guint) round((double) editor->vscrollbar->scrollbar.range.adjustment->value);

    tact = exp2(8.0 - (double) gtk_option_menu_get_history(editor->toolbar->tact));

    cr = gdk_cairo_create(widget->window);
    cairo_push_group(cr);

    if((AGS_EDITOR_POSITION_CURSOR & (editor->flags)) != 0){
      editor->flags &= (~AGS_EDITOR_POSITION_CURSOR);

      ags_editor_draw_segment(editor, cr);
      ags_editor_draw_notation(editor, cr);

      editor->selected_x = editor->control.note->x[0];
      editor->selected_y = editor->control.note->y;

      if(AGS_IS_PANEL(machine)){
      }else if(AGS_IS_MIXER(machine)){
      }else if(AGS_IS_DRUM(machine)){
	ags_editor_draw_position(editor, cr);
      }else if(AGS_IS_MATRIX(machine)){
	ags_editor_draw_position(editor, cr);
      }else if(AGS_IS_FFPLAYER(machine)){
	ags_editor_draw_position(editor, cr);
      }else if(AGS_IS_SYNTH(machine)){
	ags_editor_draw_position(editor, cr);
      }
    }else if((AGS_EDITOR_ADDING_NOTE & (editor->flags)) != 0){
      editor->flags &= (~AGS_EDITOR_ADDING_NOTE);

      ags_editor_draw_segment(editor, cr);
      ags_editor_draw_notation(editor, cr);

      if(AGS_IS_PANEL(machine)){
      }else if(AGS_IS_MIXER(machine)){
      }else if(AGS_IS_DRUM(machine)){
	ags_editor_drawing_area_button_release_event_set_control();
	ags_editor_drawing_area_button_release_event_draw_control(cr);
      }else if(AGS_IS_MATRIX(machine)){
	ags_editor_drawing_area_button_release_event_set_control();
	ags_editor_drawing_area_button_release_event_draw_control(cr);
      }else if(AGS_IS_FFPLAYER(machine)){
	ags_editor_drawing_area_button_release_event_set_control();
	ags_editor_drawing_area_button_release_event_draw_control(cr);
      }else if(AGS_IS_SYNTH(machine)){
	ags_editor_drawing_area_button_release_event_set_control();
	ags_editor_drawing_area_button_release_event_draw_control(cr);
      }
    }else if((AGS_EDITOR_DELETING_NOTE & (editor->flags)) != 0){
      editor->flags &= (~AGS_EDITOR_DELETING_NOTE);

      ags_editor_drawing_area_button_release_event_delete_point();

      ags_editor_draw_segment(editor, cr);
      ags_editor_draw_notation(editor, cr);
    }else if((AGS_EDITOR_SELECTING_NOTES & (editor->flags)) != 0){
      editor->flags &= (~AGS_EDITOR_SELECTING_NOTES);

      ags_editor_drawing_area_button_release_event_select_region();

      ags_editor_draw_segment(editor, cr);
      ags_editor_draw_notation(editor, cr);
    }

    cairo_pop_group_to_source(cr);
    cairo_paint(cr);
  }

  return(FALSE);
}

gboolean
ags_editor_drawing_area_motion_notify_event (GtkWidget *widget, GdkEventMotion *event, AgsEditor *editor)
{
  AgsMachine *machine;
  AgsNote *note, *note0;
  double value[2];
  double tact;
  guint note_x1;
  guint prev_x1;
  void ags_editor_drawing_area_motion_notify_event_set_control(){
    GList *list_notation;
    guint note_x, note_y;
    guint note_offset_x1;

    if(editor->control.x0 >= editor->map_width)
      editor->control.x0 = editor->map_width - 1;

    note_offset_x1 = (guint) (ceil(editor->control.x1_offset / (double) (editor->control_current.control_width)));

    if(editor->control.x1 >= editor->control_current.x0)
      note_x = (guint) (ceil((double) (editor->control.x1 - editor->control_current.x0) / (double) (editor->control_current.control_width)));
    else{
      note_offset_x1 -= 1;
      note_x = 0;
    }

    note_x1 = (note_x * tact) + (note_offset_x1 * tact);

    list_notation = machine->audio->notation;

    fprintf(stdout, "x0 = %llu\nx1 = %llu\ny  = %llu\n\n\0", (long long unsigned int) note->x[0], (long long unsigned int) note->x[1], (long long unsigned int) note->y);
  }
  void ags_editor_drawing_area_motion_notify_event_draw_control(cairo_t *cr){
    guint x, y, width, height;

    widget = (GtkWidget *) editor->drawing_area;

    x = note->x[0] * editor->control_unit.control_width;
    width = note_x1 * editor->control_unit.control_width;

    if(x < editor->control.x1_offset){
      if(width > editor->control.x1_offset){
	width -= x;
	x = 0;
      }else{
	return;
      }
    }else if(x < editor->control.x1_offset + widget->allocation.width){
      width -= x;
      x -= editor->control.x1_offset;
    }else{
      return;
    }

    if(x + width > widget->allocation.width)
      width = widget->allocation.width - x;

    y = note->y * editor->control_height;

    if(y < editor->control.y1_offset){
      if(y + editor->control_height - editor->control_margin_y < editor->control.y1_offset){
	return;
      }else{
	if(y + editor->control_margin_y < editor->control.y1_offset){
	  height = editor->control_height;
	  y = y + editor->control_margin_y - editor->control.y1_offset;
	}else{
	  height = editor->y0;
	  y -= editor->control.y1_offset;
	}
      }
    }else if(y < editor->control.y1_offset + widget->allocation.height - editor->control_height){
      height = editor->control_height - 2 * editor->control_margin_y;
      y = y - editor->control.y1_offset + editor->control_margin_y;
    }else{
      if(y > editor->control.y1_offset + widget->allocation.height - editor->y1 + editor->control_margin_y){
	return;
      }else{
	height = editor->y0;
	y = y - editor->control.y1_offset + editor->control_margin_y;
      }
    }

    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
    cairo_fill(cr);
  }
  void ags_editor_drawing_area_motion_notify_event_draw_selection(cairo_t *cr){
    GtkAllocation allocation;
    guint x0_offset, x1_offset, y0_offset, y1_offset;
    guint x0, x1, y0, y1, width, height;
    guint x0_viewport, x1_viewport, y0_viewport, y1_viewport;

    /* get viewport */
    gtk_widget_get_allocation(widget, &allocation);

    x0_viewport = editor->control.x1_offset;
    x1_viewport = editor->control.x1_offset + allocation.width;

    y0_viewport = editor->control.y1_offset;
    y1_viewport = editor->control.y1_offset + allocation.height;

    /* get real size and offset */
    x0 = editor->control.x0_offset + editor->control.x0;
    x1 = editor->control.x1_offset + editor->control.x1;

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
      x0 = 0;
      width = x1_offset - x0_viewport;
    }else{
      x0 -= x0_viewport;
      width = x1 - x0;
    }

    if(x1 > x1_viewport){
      width -= (x1 - x1_viewport);
    }

    /* get real size and offset */
    y0 = editor->control.y0_offset + editor->control.y0;
    y1 = editor->control.y1_offset + editor->control.y1;

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
    }

    cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 0.3);
    cairo_rectangle(cr, (double) x0, (double) y0, (double) width, (double) height);
    cairo_fill(cr);
  }
    
  if(editor->selected != NULL){
    cairo_t *cr;

    prev_x1 = editor->control.x1;
    editor->control.x1 = (guint) event->x;
    editor->control.y1 = (guint) event->y;

    machine = AGS_MACHINE(g_object_get_data((GObject *) editor->selected, (char *) g_type_name(AGS_TYPE_MACHINE)));
    note = editor->control.note;

    editor->control.x1_offset = (guint) round((double) editor->hscrollbar->scrollbar.range.adjustment->value);
    editor->control.y1_offset = (guint) round((double) editor->vscrollbar->scrollbar.range.adjustment->value);

    tact = exp2(8.0 - (double) gtk_option_menu_get_history(editor->toolbar->tact));

    cr = gdk_cairo_create(widget->window);
    cairo_push_group(cr);

    if((AGS_EDITOR_ADDING_NOTE & (editor->flags)) != 0){
      if(prev_x1 > editor->control.x1){
	ags_editor_draw_segment(editor, cr);
	ags_editor_draw_notation(editor, cr);
      }

      if(AGS_IS_PANEL(machine)){
      }else if(AGS_IS_MIXER(machine)){
      }else if(AGS_IS_DRUM(machine)){
	ags_editor_drawing_area_motion_notify_event_set_control();
	ags_editor_drawing_area_motion_notify_event_draw_control(cr);
      }else if(AGS_IS_MATRIX(machine)){
	ags_editor_drawing_area_motion_notify_event_set_control();
	ags_editor_drawing_area_motion_notify_event_draw_control(cr);
      }else if(AGS_IS_FFPLAYER(machine)){
	ags_editor_drawing_area_motion_notify_event_set_control();
	ags_editor_drawing_area_motion_notify_event_draw_control(cr);
      }
    }else if((AGS_EDITOR_SELECTING_NOTES & (editor->flags)) != 0){
      ags_editor_draw_segment(editor, cr);
      ags_editor_draw_notation(editor, cr);

      ags_editor_drawing_area_motion_notify_event_draw_selection(cr);
    }
    
    cairo_pop_group_to_source(cr);
    cairo_paint(cr);
  }

  return(FALSE);
}

void
ags_editor_popup_add_tab_callback(GtkWidget *widget, GtkMenu *popup)
{
}

void
ags_editor_popup_remove_tab_callback(GtkWidget *widget, GtkMenu *popup)
{
}

void
ags_editor_popup_add_index_callback(GtkWidget *widget, GtkMenu *popup)
{
  AgsEditor *editor;
  GtkRadioButton *radio_button;
  GList *list;

  editor = AGS_EDITOR(g_object_get_data((GObject *) popup, g_type_name(AGS_TYPE_EDITOR)));

  radio_button = (GtkRadioButton *) gtk_radio_button_new_with_label_from_widget(editor->selected, g_strdup(AGS_EDITOR_DEFAULT));
  g_object_set_data((GObject *) radio_button, (char *) g_type_name(AGS_TYPE_MACHINE), NULL);
  g_object_set_data((GObject *) radio_button, (char *) g_type_name(AGS_TYPE_CHANNEL), GUINT_TO_POINTER(0));
  g_signal_connect((GObject *) radio_button, "toggled\0",
		   G_CALLBACK(ags_editor_index_callback), editor);
  gtk_box_pack_start((GtkBox *) editor->index_radio, (GtkWidget *) radio_button, FALSE, FALSE, 0);

  gtk_widget_show((GtkWidget *) radio_button);

  if(editor->selected == NULL)
    editor->selected = radio_button;
}

void
ags_editor_popup_remove_index_callback(GtkWidget *widget, GtkMenu *popup)
{
}

void
ags_editor_popup_link_index_callback(GtkWidget *widget, GtkMenu *popup)
{
  AgsWindow *window;
  AgsEditor *editor;
  AgsMachine *machine;
  GtkDialog *dialog;
  GtkScrolledWindow *scrolled_window;
  GtkVBox *vbox_radio;
  GtkRadioButton *radio_button0, *radio_button;
  GList *list;

  editor = AGS_EDITOR(g_object_get_data((GObject *) popup, (char *) g_type_name(AGS_TYPE_EDITOR)));
  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) editor);

  dialog = (GtkDialog *) gtk_dialog_new_with_buttons(g_strdup("select machine\0"),
						     (GtkWindow *) window,
						     GTK_DIALOG_DESTROY_WITH_PARENT,
						     GTK_STOCK_OK,
						     GTK_RESPONSE_ACCEPT,
						     GTK_STOCK_CANCEL,
						     GTK_RESPONSE_REJECT,
						     NULL);
  g_object_set_data((GObject *) dialog, g_type_name(AGS_TYPE_EDITOR), editor);
  g_signal_connect_after((GObject *) dialog, "response\0",
			 G_CALLBACK(ags_editor_link_index_response_callback), editor);

  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
  gtk_box_pack_start((GtkBox *) dialog->vbox, (GtkWidget *) scrolled_window, TRUE, TRUE, 0);

  vbox_radio = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  g_object_set_data((GObject *) dialog, AGS_EDITOR_LINK_INDEX_VBOX, (gpointer) vbox_radio);
  gtk_scrolled_window_add_with_viewport(scrolled_window, (GtkWidget *) vbox_radio);

  list = gtk_container_get_children((GtkContainer *) (window->machines));

  if(list != NULL){
    radio_button =
      radio_button0 = (GtkRadioButton *) gtk_radio_button_new_from_widget(NULL);
    goto ags_editor_popup_link_index_callback0;
  }

  while(list != NULL){
    radio_button = (GtkRadioButton *) gtk_radio_button_new_from_widget(radio_button0);
  ags_editor_popup_link_index_callback0:
    machine = AGS_MACHINE(list->data);
    gtk_button_set_label((GtkButton *) radio_button, g_strconcat(G_OBJECT_TYPE_NAME((GObject *) machine), ": \0", machine->name, NULL));
    g_object_set_data((GObject *) radio_button, (char *) g_type_name(AGS_TYPE_MACHINE), list->data);
    gtk_box_pack_start((GtkBox *) vbox_radio, (GtkWidget *) radio_button, FALSE, FALSE, 0);

    list = list->next;
  }

  gtk_widget_show_all((GtkWidget *) dialog);
}

void
ags_editor_link_index_callback(GtkRadioButton *radio_button, GtkDialog *dialog)
{
}

void
ags_editor_link_index_response_callback(GtkDialog *dialog, gint response, AgsEditor *editor)
{
  if(response == GTK_RESPONSE_ACCEPT){
    AgsMachine *machine0, *machine1;
    GList *list;
    
    list = gtk_container_get_children((GtkContainer *) GTK_VBOX(g_object_get_data((GObject *) dialog, AGS_EDITOR_LINK_INDEX_VBOX)));
    
    if(list == NULL)
      return;

    while(list != NULL && !(GTK_TOGGLE_BUTTON(list->data)->active))
      list = list->next;
    
    machine0 = (AgsMachine *) g_object_get_data(G_OBJECT(list->data), (char *) g_type_name(AGS_TYPE_MACHINE));
    machine1 = (AgsMachine *) g_object_get_data((GObject *) editor->selected, (char *) g_type_name(AGS_TYPE_MACHINE));
    
    ags_editor_change_machine(editor, machine0);
        
    if(machine0 != NULL){
      guint pads;

      gtk_button_set_label(GTK_BUTTON(editor->selected), g_strconcat(G_OBJECT_TYPE_NAME((GObject *) machine0), ": \0", machine0->name, NULL));

      if((AGS_AUDIO_NOTATION_DEFAULT & (machine0->audio->flags)) != 0){
	pads = machine0->audio->input_pads;
      }else{
	pads = machine0->audio->output_pads;
      }

      editor->map_height = pads * editor->control_height;
    
      editor->flags |= AGS_EDITOR_RESETING_VERTICALLY;
      ags_editor_reset_vertically(editor, AGS_EDITOR_RESET_VSCROLLBAR);
      editor->flags &= (~AGS_EDITOR_RESETING_VERTICALLY);
      
      editor->flags |= AGS_EDITOR_RESETING_HORIZONTALLY;
      ags_editor_reset_horizontally(editor, AGS_EDITOR_RESET_HSCROLLBAR);
      editor->flags &= (~AGS_EDITOR_RESETING_HORIZONTALLY);  
    }
  }

  gtk_widget_destroy((GtkWidget *) dialog);
}

void
ags_editor_vscrollbar_value_changed(GtkRange *range, AgsEditor *editor)
{
  if((AGS_EDITOR_RESETING_VERTICALLY & editor->flags) != 0){
    return;
  }

  editor->flags |= AGS_EDITOR_RESETING_VERTICALLY;
  ags_editor_reset_vertically(editor, 0);
  editor->flags &= (~AGS_EDITOR_RESETING_VERTICALLY);
}

void
ags_editor_hscrollbar_value_changed(GtkRange *range, AgsEditor *editor)
{
  if((AGS_EDITOR_RESETING_HORIZONTALLY & editor->flags) != 0){
    return;
  }

  editor->flags |= AGS_EDITOR_RESETING_HORIZONTALLY;
  ags_editor_reset_horizontally(editor, 0);
  editor->flags &= (~AGS_EDITOR_RESETING_HORIZONTALLY);
}
