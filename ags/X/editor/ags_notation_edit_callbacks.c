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

#include <ags/X/editor/ags_notation_edit_callbacks.h>

#include <ags/X/ags_notation_editor.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <math.h>

gboolean
ags_notation_edit_drawing_area_expose_event(GtkWidget *widget, GdkEventExpose *event, AgsNotationEdit *notation_edit)
{
  ags_notation_edit_reset_vscrollbar(notation_edit);
  ags_notation_edit_reset_hscrollbar(notation_edit);
  
  ags_notation_edit_draw(notation_edit);
  
  return(TRUE);
}

gboolean
ags_notation_edit_drawing_area_configure_event(GtkWidget *widget, GdkEventConfigure *event, AgsNotationEdit *notation_edit)
{
  ags_notation_edit_reset_vscrollbar(notation_edit);
  ags_notation_edit_reset_hscrollbar(notation_edit);

  ags_notation_edit_draw(notation_edit);

  return(FALSE);
}

gboolean
ags_notation_edit_drawing_area_button_press_event(GtkWidget *widget, GdkEventButton *event, AgsNotationEdit *notation_edit)
{
  AgsNotationEditor *notation_editor;
  AgsNotationToolbar *notation_toolbar;
  AgsMachine *machine;

  auto void ags_notation_edit_drawing_area_button_press_position_cursor();
  auto void ags_notation_edit_drawing_area_button_press_add_note();
  auto void ags_notation_edit_drawing_area_button_press_select_note();

  void ags_notation_edit_drawing_area_button_press_position_cursor()
  {
    double zoom_factor;

    /* zoom */
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) notation_toolbar->zoom));

    /* cursor position */
    notation_edit->cursor_position_x = (guint) zoom_factor * ((event->x + GTK_RANGE(notation_edit->hscrollbar)->adjustment->value) / notation_edit->control_width);
    notation_edit->cursor_position_x = zoom_factor * floor(notation_edit->cursor_position_x / zoom_factor);
    
    notation_edit->cursor_position_y = (guint) ((event->y + GTK_RANGE(notation_edit->vscrollbar)->adjustment->value) / notation_edit->control_height);

    /* queue draw */
    gtk_widget_queue_draw(notation_edit);
  }
  
  void ags_notation_edit_drawing_area_button_press_add_note()
  {
    AgsNote *note;

    double zoom_factor;
    
    note = ags_note_new();

    /* zoom */
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) notation_toolbar->zoom));

    /* note */
    note->x[0] = (guint) zoom_factor * ((event->x + GTK_RANGE(notation_edit->hscrollbar)->adjustment->value) / notation_edit->control_width);
    note->x[0] = zoom_factor * floor(note->x[0] / zoom_factor);

    note->x[1] = note->x[0] + zoom_factor;
    
    note->y = (guint) ((event->y + GTK_RANGE(notation_edit->vscrollbar)->adjustment->value) / notation_edit->control_height);

    /* current note */
    if(notation_edit->current_note != NULL){
      g_object_unref(notation_edit->current_note);

      notation_edit->current_note = NULL;
    }

    notation_edit->current_note = note;
    g_object_ref(note);

    /* queue draw */
    gtk_widget_queue_draw(notation_edit);
  }

  void ags_notation_edit_drawing_area_button_press_select_note()
  {
    notation_edit->selection_x0 = (guint) event->x + GTK_RANGE(notation_edit->hscrollbar)->adjustment->value;
    notation_edit->selection_x1 = notation_edit->selection_x0;
    
    notation_edit->selection_y0 = (guint) event->y + GTK_RANGE(notation_edit->vscrollbar)->adjustment->value;
    notation_edit->selection_y1 = notation_edit->selection_y0;

    gtk_widget_queue_draw(notation_edit);
  }

  notation_editor = (AgsNotationEditor *) gtk_widget_get_ancestor(GTK_WIDGET(notation_edit),
								  AGS_TYPE_NOTATION_EDITOR);

  notation_toolbar = notation_editor->notation_toolbar;

  gtk_widget_grab_focus((GtkWidget *) notation_edit->drawing_area);

  if((machine = notation_editor->selected_machine) != NULL &&
     event->button == 1){    
    if(notation_toolbar->selected_edit_mode == notation_toolbar->position){
      notation_edit->mode = AGS_NOTATION_EDIT_POSITION_CURSOR;

      ags_notation_edit_drawing_area_button_press_position_cursor();
    }else if(notation_toolbar->selected_edit_mode == notation_toolbar->edit){
      notation_edit->mode = AGS_NOTATION_EDIT_ADD_NOTE;

      ags_notation_edit_drawing_area_button_press_add_note();
    }else if(notation_toolbar->selected_edit_mode == notation_toolbar->clear){
      notation_edit->mode = AGS_NOTATION_EDIT_DELETE_NOTE;

      //NOTE:JK: only takes action on release
    }else if(notation_toolbar->selected_edit_mode == notation_toolbar->select){
      notation_edit->mode = AGS_NOTATION_EDIT_SELECT_NOTE;

      ags_notation_edit_drawing_area_button_press_select_note();
    }
  }
  
  return(TRUE);
}

gboolean
ags_notation_edit_drawing_area_button_release_event(GtkWidget *widget, GdkEventButton *event, AgsNotationEdit *notation_edit)
{
  AgsNotationEditor *notation_editor;
  AgsNotationToolbar *notation_toolbar;
  AgsMachine *machine;

  auto void ags_notation_edit_drawing_area_button_release_position_cursor();
  auto void ags_notation_edit_drawing_area_button_release_add_note();
  auto void ags_notation_edit_drawing_area_button_release_delete_note();
  auto void ags_notation_edit_drawing_area_button_release_select_note();

  void ags_notation_edit_drawing_area_button_release_position_cursor()
  {
    double zoom_factor;

    /* zoom */
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) notation_toolbar->zoom));

    /* cursor position */
    notation_edit->cursor_position_x = (guint) zoom_factor * ((event->x + GTK_RANGE(notation_edit->hscrollbar)->adjustment->value) / notation_edit->control_width);
    notation_edit->cursor_position_x = zoom_factor * floor(notation_edit->cursor_position_x / zoom_factor);

    notation_edit->cursor_position_y = (guint) ((event->y + GTK_RANGE(notation_edit->vscrollbar)->adjustment->value) / notation_edit->control_height);
    
    /* queue draw */
    gtk_widget_queue_draw(notation_edit);
  }

  void ags_notation_edit_drawing_area_button_release_add_note()
  {
    AgsNote *note;
    
    double zoom_factor;
    guint new_x;
    
    note = notation_edit->current_note;
    
    if(note == NULL){
      return;
    }

    /* zoom */
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) notation_toolbar->zoom));

    /* new x[1] */
    new_x = (guint) zoom_factor * ((event->x + GTK_RANGE(notation_edit->hscrollbar)->adjustment->value) / notation_edit->control_width);
    new_x = zoom_factor * floor((new_x + zoom_factor) / zoom_factor);
    
    if(new_x >= note->x[0] + zoom_factor){
      note->x[1] = new_x;
    }
    
    //#ifdef AGS_DEBUG
    g_message("%lu-%lu %lu", note->x[0], note->x[1], note->y);
    //#endif

    /* add note */
    ags_notation_editor_add_note(notation_editor,
				 note);

    notation_edit->current_note = NULL;
    g_object_unref(note);
  }
  
  void ags_notation_edit_drawing_area_button_release_delete_note()
  {
    double zoom_factor;
    guint x, y;
    
    /* zoom */
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) notation_toolbar->zoom));

    /* note */
    x = (guint) zoom_factor * ((event->x + GTK_RANGE(notation_edit->hscrollbar)->adjustment->value) / notation_edit->control_width);
    x = zoom_factor * floor(x / zoom_factor);
    
    y = (guint) ((event->y + GTK_RANGE(notation_edit->vscrollbar)->adjustment->value) / notation_edit->control_height);

    /* delete note */
    ags_notation_editor_delete_note(notation_editor,
				    x, y);
  }
  
  void ags_notation_edit_drawing_area_button_release_select_note()
  {
    double zoom_factor;
    guint x0, x1, y0, y1;
    
    /* zoom */
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) notation_toolbar->zoom));

    /* region */
    x0 = (guint) zoom_factor * (notation_edit->selection_x0 / notation_edit->control_width);
    x0 = zoom_factor * floor(x0 / zoom_factor);

    y0 = (guint) (notation_edit->selection_y0 / notation_edit->control_height);
    
    x1 = (guint) zoom_factor * ((event->x + GTK_RANGE(notation_edit->hscrollbar)->adjustment->value) / notation_edit->control_width);
    x1 = zoom_factor * floor(x1 / zoom_factor);
    
    y1 = (guint) ((event->y + GTK_RANGE(notation_edit->vscrollbar)->adjustment->value) / notation_edit->control_height);

    /* select region */
    ags_notation_editor_select_region(notation_editor,
				      x0, y0,
				      x1, y1);
  }

  notation_editor = (AgsNotationEditor *) gtk_widget_get_ancestor(GTK_WIDGET(notation_edit),
								  AGS_TYPE_NOTATION_EDITOR);

  notation_toolbar = notation_editor->notation_toolbar;

  if((machine = notation_editor->selected_machine) != NULL &&
     event->button == 1){    
    if(notation_edit->mode == AGS_NOTATION_EDIT_POSITION_CURSOR){
      ags_notation_edit_drawing_area_button_release_position_cursor();
    }else if(notation_edit->mode == AGS_NOTATION_EDIT_ADD_NOTE){
      ags_notation_edit_drawing_area_button_release_add_note();
    }else if(notation_edit->mode == AGS_NOTATION_EDIT_DELETE_NOTE){
      ags_notation_edit_drawing_area_button_release_delete_note();
    }else if(notation_edit->mode == AGS_NOTATION_EDIT_SELECT_NOTE){
      ags_notation_edit_drawing_area_button_release_select_note();
    }
  }

  return(FALSE);
}

gboolean
ags_notation_edit_drawing_area_motion_notify_event(GtkWidget *widget, GdkEventMotion *event, AgsNotationEdit *notation_edit)
{
  AgsNotationEditor *notation_editor;
  AgsNotationToolbar *notation_toolbar;
  AgsMachine *machine;

  auto void ags_notation_edit_drawing_area_motion_notify_position_cursor();
  auto void ags_notation_edit_drawing_area_motion_notify_add_note();
  auto void ags_notation_edit_drawing_area_motion_notify_select_note();

  void ags_notation_edit_drawing_area_motion_notify_position_cursor()
  {
    double zoom_factor;

    /* zoom */
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) notation_toolbar->zoom));

    /* cursor position */
    notation_edit->cursor_position_x = (guint) zoom_factor * ((event->x + GTK_RANGE(notation_edit->hscrollbar)->adjustment->value) / notation_edit->control_width);
    notation_edit->cursor_position_x = zoom_factor * floor(notation_edit->cursor_position_x / zoom_factor);

    notation_edit->cursor_position_y = (guint) ((event->y + GTK_RANGE(notation_edit->vscrollbar)->adjustment->value) / notation_edit->control_height);

#ifdef AGS_DEBUG
    g_message("%lu %lu", notation_edit->cursor_position_x, notation_edit->cursor_position_y);
#endif
    
    /* queue draw */
    gtk_widget_queue_draw(notation_edit);
  }

  void ags_notation_edit_drawing_area_motion_notify_add_note()
  {
    AgsNote *note;
    
    double zoom_factor;
    guint new_x;
    
    note = notation_edit->current_note;
    
    if(note == NULL){
      return;
    }

    /* zoom */
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) notation_toolbar->zoom));

    /* new x[1] */
    new_x = (guint) zoom_factor * ((event->x + GTK_RANGE(notation_edit->hscrollbar)->adjustment->value) / notation_edit->control_width);
    new_x = zoom_factor * floor((new_x + zoom_factor) / zoom_factor);
    
    if(new_x >= note->x[0] + zoom_factor){
      note->x[1] = new_x;
    }

#ifdef AGS_DEBUG
    g_message("%lu-%lu %lu", note->x[0], note->x[1], note->y);
#endif
    
    /* queue draw */
    gtk_widget_queue_draw(notation_edit);
  }

  void ags_notation_edit_drawing_area_motion_notify_select_note()
  {
    notation_edit->selection_x1 = (guint) event->x + GTK_RANGE(notation_edit->hscrollbar)->adjustment->value;
    
    notation_edit->selection_y1 = (guint) event->y + GTK_RANGE(notation_edit->vscrollbar)->adjustment->value;

    gtk_widget_queue_draw(notation_edit);
  }

  notation_editor = (AgsNotationEditor *) gtk_widget_get_ancestor(GTK_WIDGET(notation_edit),
								  AGS_TYPE_NOTATION_EDITOR);

  notation_toolbar = notation_editor->notation_toolbar;

  gtk_widget_grab_focus((GtkWidget *) notation_edit->drawing_area);

  if((machine = notation_editor->selected_machine) != NULL){
    if(notation_edit->mode == AGS_NOTATION_EDIT_POSITION_CURSOR){
      ags_notation_edit_drawing_area_motion_notify_position_cursor();
    }else if(notation_edit->mode == AGS_NOTATION_EDIT_ADD_NOTE){
      ags_notation_edit_drawing_area_motion_notify_add_note();
    }else if(notation_edit->mode == AGS_NOTATION_EDIT_DELETE_NOTE){
      //NOTE:JK: only takes action on release
    }else if(notation_edit->mode == AGS_NOTATION_EDIT_SELECT_NOTE){
      ags_notation_edit_drawing_area_motion_notify_select_note();
    }
  }

  return(FALSE);
}

gboolean
ags_notation_edit_drawing_area_key_press_event(GtkWidget *widget, GdkEventKey *event, AgsNotationEdit *notation_edit)
{
  //TODO:JK: implement me
}

gboolean
ags_notation_edit_drawing_area_key_release_event(GtkWidget *widget, GdkEventKey *event, AgsNotationEdit *notation_edit)
{
  //TODO:JK: implement me
}

void
ags_notation_edit_vscrollbar_value_changed(GtkRange *range, AgsNotationEdit *notation_edit)
{
  AgsNotationEditor *notation_editor;

  GtkAdjustment *piano_adjustment;
  
  notation_editor = gtk_widget_get_ancestor(notation_edit,
					    AGS_TYPE_NOTATION_EDITOR);

  g_object_get(notation_editor->scrolled_piano->viewport,
	       "vadjustment", &piano_adjustment,
	       NULL);
  gtk_adjustment_set_value(piano_adjustment,
			   range->adjustment->value);
  gtk_widget_queue_draw(notation_editor->scrolled_piano->piano);

  /* queue draw */
  gtk_widget_queue_draw(notation_edit->drawing_area);
}

void
ags_notation_edit_hscrollbar_value_changed(GtkRange *range, AgsNotationEdit *notation_edit)
{
  gdouble value;

  value = GTK_RANGE(notation_edit->hscrollbar)->adjustment->value / 64.0;
  gtk_adjustment_set_value(notation_edit->ruler->adjustment,
			   value);
  gtk_widget_queue_draw(notation_edit->ruler);
  
  /* queue draw */
  gtk_widget_queue_draw(notation_edit->drawing_area);
}
