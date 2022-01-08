/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_notation_editor.h>
#include <ags/X/ags_composite_editor.h>

#include <ags/X/editor/ags_composite_edit.h>

#include <math.h>

#include <gdk/gdkkeysyms.h>

void ags_notation_edit_drawing_area_button_press_position_cursor(GtkWidget *editor,
								 GtkWidget *toolbar,
								 AgsNotationEdit *notation_edit,
								 AgsMachine *machine,
								 GdkEventButton *event);
void ags_notation_edit_drawing_area_button_press_add_note(GtkWidget *editor,
							  GtkWidget *toolbar,
							  AgsNotationEdit *notation_edit,
							  AgsMachine *machine,
							  GdkEventButton *event);
void ags_notation_edit_drawing_area_button_press_select_note(GtkWidget *editor,
							     GtkWidget *toolbar,
							     AgsNotationEdit *notation_edit,
							     AgsMachine *machine,
							     GdkEventButton *event);

void ags_notation_edit_drawing_area_button_release_position_cursor(GtkWidget *editor,
								   GtkWidget *toolbar,
								   AgsNotationEdit *notation_edit,
								   AgsMachine *machine,
								   GdkEventButton *event);
void ags_notation_edit_drawing_area_button_release_add_note(GtkWidget *editor,
							    GtkWidget *toolbar,
							    AgsNotationEdit *notation_edit,
							    AgsMachine *machine,
							    GdkEventButton *event);
void ags_notation_edit_drawing_area_button_release_delete_note(GtkWidget *editor,
							       GtkWidget *toolbar,
							       AgsNotationEdit *notation_edit,
							       AgsMachine *machine,
							       GdkEventButton *event);
void ags_notation_edit_drawing_area_button_release_select_note(GtkWidget *editor,
							       GtkWidget *toolbar,
							       AgsNotationEdit *notation_edit,
							       AgsMachine *machine,
							       GdkEventButton *event);

void ags_notation_edit_drawing_area_motion_notify_position_cursor(GtkWidget *editor,
								  GtkWidget *toolbar,
								  AgsNotationEdit *notation_edit,
								  AgsMachine *machine,
								  GdkEventMotion *event);
void ags_notation_edit_drawing_area_motion_notify_add_note(GtkWidget *editor,
							   GtkWidget *toolbar,
							   AgsNotationEdit *notation_edit,
							   AgsMachine *machine,
							   GdkEventMotion *event);
void ags_notation_edit_drawing_area_motion_notify_select_note(GtkWidget *editor,
							      GtkWidget *toolbar,
							      AgsNotationEdit *notation_edit,
							      AgsMachine *machine,
							      GdkEventMotion *event);

gboolean
ags_notation_edit_draw_callback(GtkWidget *drawing_area, cairo_t *cr, AgsNotationEdit *notation_edit)
{
  ags_notation_edit_draw(notation_edit, cr);

  return(FALSE);
}

gboolean
ags_notation_edit_drawing_area_configure_event(GtkWidget *widget, GdkEventConfigure *event, AgsNotationEdit *notation_edit)
{
  GtkAdjustment *vadjustment, *hadjustment;
  
  gdouble vscrollbar_value, hscrollbar_value;
  gdouble vnew_upper, vold_upper;
  gdouble hnew_upper, hold_upper;

  vadjustment = gtk_range_get_adjustment((GtkRange *) notation_edit->vscrollbar);
  hadjustment = gtk_range_get_adjustment((GtkRange *) notation_edit->hscrollbar);
  
  vscrollbar_value = gtk_range_get_value((GtkRange *) notation_edit->vscrollbar);
  hscrollbar_value = gtk_range_get_value((GtkRange *) notation_edit->hscrollbar);
  
  vold_upper = gtk_adjustment_get_upper(vadjustment);
  hold_upper = gtk_adjustment_get_upper(hadjustment);
  
  ags_notation_edit_reset_vscrollbar(notation_edit);
  ags_notation_edit_reset_hscrollbar(notation_edit);
  
  vnew_upper = gtk_adjustment_get_upper(vadjustment);
  hnew_upper = gtk_adjustment_get_upper(hadjustment);
  
  gtk_range_set_value((GtkRange *) notation_edit->vscrollbar,
		      vscrollbar_value * (1.0 / (vold_upper / vnew_upper)));
  
  gtk_range_set_value((GtkRange *) notation_edit->hscrollbar,
		      hscrollbar_value * (1.0 / (hold_upper / hnew_upper)));
  
  gtk_widget_queue_draw(notation_edit->drawing_area);

  return(FALSE);
}

void
ags_notation_edit_drawing_area_button_press_position_cursor(GtkWidget *editor,
							    GtkWidget *toolbar,
							    AgsNotationEdit *notation_edit,
							    AgsMachine *machine,
							    GdkEventButton *event)
{
  AgsApplicationContext *application_context;
  
  gboolean use_composite_editor;
  double zoom_factor;

  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));

  /* zoom */
  if(use_composite_editor){
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));
  }else{
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_NOTATION_TOOLBAR(toolbar)->zoom));
  }
  
  /* cursor position */
  notation_edit->cursor_position_x = (guint) (zoom_factor * (event->x + gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar)))) / notation_edit->control_width;
  notation_edit->cursor_position_x = zoom_factor * floor(notation_edit->cursor_position_x / zoom_factor);
    
  notation_edit->cursor_position_y = (guint) ((event->y + gtk_range_get_value(GTK_RANGE(notation_edit->vscrollbar))) / notation_edit->control_height);

  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}
  
void
ags_notation_edit_drawing_area_button_press_add_note(GtkWidget *editor,
						     GtkWidget *toolbar,
						     AgsNotationEdit *notation_edit,
						     AgsMachine *machine,
						     GdkEventButton *event)
{
  AgsNote *note;

  AgsApplicationContext *application_context;
  
  gboolean use_composite_editor;
  gboolean pattern_mode;
  double zoom_factor;

  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));
    
  note = ags_note_new();

  /* zoom */
  if(use_composite_editor){    
    pattern_mode = (AGS_COMPOSITE_EDIT_PATTERN_MODE == AGS_COMPOSITE_EDITOR(editor)->notation_edit->edit_mode) ? TRUE: FALSE;
    
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));
  }else{
    pattern_mode = ((AGS_NOTATION_EDITOR_PATTERN_MODE & (AGS_NOTATION_EDITOR(editor)->flags)) != 0) ? TRUE: FALSE;

    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_NOTATION_TOOLBAR(toolbar)->zoom));
  }

  /* note */
  note->x[0] = (guint) (zoom_factor * (event->x + gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar)))) / notation_edit->control_width;
  note->x[0] = zoom_factor * floor(note->x[0] / zoom_factor);

  if(!pattern_mode){
    note->x[1] = note->x[0] + zoom_factor;
  }else{
    note->x[1] = note->x[0] + 1;
  }
    
  note->y = (guint) ((event->y + gtk_range_get_value(GTK_RANGE(notation_edit->vscrollbar))) / notation_edit->control_height);

  /* current note */
  if(notation_edit->current_note != NULL){
    g_object_unref(notation_edit->current_note);

    notation_edit->current_note = NULL;
  }

  notation_edit->current_note = note;
  g_object_ref(note);

  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}

void
ags_notation_edit_drawing_area_button_press_select_note(GtkWidget *editor,
							GtkWidget *toolbar,
							AgsNotationEdit *notation_edit,
							AgsMachine *machine,
							GdkEventButton *event)
{
  notation_edit->selection_x0 = (guint) event->x + gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar));
  notation_edit->selection_x1 = notation_edit->selection_x0;
    
  notation_edit->selection_y0 = (guint) event->y + gtk_range_get_value(GTK_RANGE(notation_edit->vscrollbar));
  notation_edit->selection_y1 = notation_edit->selection_y0;

  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}

gboolean
ags_notation_edit_drawing_area_button_press_event(GtkWidget *widget, GdkEventButton *event, AgsNotationEdit *notation_edit)
{
  GtkWidget *editor;
  GtkWidget *toolbar;
  AgsMachine *machine;

  AgsApplicationContext *application_context;
  
  gboolean use_composite_editor;
  gboolean selected_position_cursor, selected_edit, selected_clear, selected_select;

  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));

  selected_position_cursor = FALSE;
  selected_edit = FALSE;
  selected_clear = FALSE;
  selected_select = FALSE;
  
  if(use_composite_editor){
    AgsCompositeToolbar *composite_toolbar;
    
    editor = gtk_widget_get_ancestor(GTK_WIDGET(notation_edit),
				     AGS_TYPE_COMPOSITE_EDITOR);
    
    toolbar = AGS_COMPOSITE_EDITOR(editor)->toolbar;

    machine = AGS_COMPOSITE_EDITOR(editor)->selected_machine;

    composite_toolbar = (AgsCompositeToolbar *) toolbar;
    
    selected_position_cursor = (composite_toolbar->selected_tool == composite_toolbar->position) ? TRUE: FALSE;
    selected_edit = (composite_toolbar->selected_tool == composite_toolbar->edit) ? TRUE: FALSE;
    selected_clear = (composite_toolbar->selected_tool == composite_toolbar->clear) ? TRUE: FALSE;
    selected_select = (composite_toolbar->selected_tool == composite_toolbar->select) ? TRUE: FALSE;
  }else{
    AgsNotationToolbar *notation_toolbar;
    
    editor = gtk_widget_get_ancestor(GTK_WIDGET(notation_edit),
				     AGS_TYPE_NOTATION_EDITOR);
    
    toolbar = AGS_NOTATION_EDITOR(editor)->notation_toolbar;

    machine = AGS_NOTATION_EDITOR(editor)->selected_machine;

    notation_toolbar = (AgsNotationToolbar *) toolbar;
    
    selected_position_cursor = (notation_toolbar->selected_edit_mode == notation_toolbar->position) ? TRUE: FALSE;
    selected_edit = (notation_toolbar->selected_edit_mode == notation_toolbar->edit) ? TRUE: FALSE;
    selected_clear = (notation_toolbar->selected_edit_mode == notation_toolbar->clear) ? TRUE: FALSE;
    selected_select = (notation_toolbar->selected_edit_mode == notation_toolbar->select) ? TRUE: FALSE;
  }
  
  gtk_widget_grab_focus((GtkWidget *) notation_edit->drawing_area);

  if(machine != NULL &&
     event->button == 1){
    notation_edit->button_mask |= AGS_NOTATION_EDIT_BUTTON_1;
    
    if(selected_position_cursor){
      notation_edit->mode = AGS_NOTATION_EDIT_POSITION_CURSOR;
      
      ags_notation_edit_drawing_area_button_press_position_cursor(editor,
								  toolbar,
								  notation_edit,
								  machine,
								  event);
    }else if(selected_edit){
      notation_edit->mode = AGS_NOTATION_EDIT_ADD_NOTE;

      ags_notation_edit_drawing_area_button_press_add_note(editor,
							   toolbar,
							   notation_edit,
							   machine,
							   event);
    }else if(selected_clear){
      notation_edit->mode = AGS_NOTATION_EDIT_DELETE_NOTE;

      //NOTE:JK: only takes action on release
    }else if(selected_select){
      notation_edit->mode = AGS_NOTATION_EDIT_SELECT_NOTE;

      ags_notation_edit_drawing_area_button_press_select_note(editor,
							      toolbar,
							      notation_edit,
							      machine,
							      event);
    }
  }
  
  return(FALSE);
}

void
ags_notation_edit_drawing_area_button_release_position_cursor(GtkWidget *editor,
							      GtkWidget *toolbar,
							      AgsNotationEdit *notation_edit,
							      AgsMachine *machine,
							      GdkEventButton *event)
{
  AgsApplicationContext *application_context;
  
  gboolean use_composite_editor;
  double zoom_factor;

  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));
  
  /* zoom */
  if(use_composite_editor){
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));
  }else{
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_NOTATION_TOOLBAR(toolbar)->zoom));
  }

  /* cursor position */
  notation_edit->cursor_position_x = (guint) (zoom_factor * (event->x + gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar)))) / notation_edit->control_width;
  notation_edit->cursor_position_x = zoom_factor * floor(notation_edit->cursor_position_x / zoom_factor);

  notation_edit->cursor_position_y = (guint) ((event->y + gtk_range_get_value(GTK_RANGE(notation_edit->vscrollbar))) / notation_edit->control_height);
    
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}

void
ags_notation_edit_drawing_area_button_release_add_note(GtkWidget *editor,
						       GtkWidget *toolbar,
						       AgsNotationEdit *notation_edit,
						       AgsMachine *machine,
						       GdkEventButton *event)
{
  AgsNote *note;
    
  AgsApplicationContext *application_context;

  gboolean use_composite_editor;
  gboolean pattern_mode;
  double zoom_factor;
  guint new_x;
    
  note = notation_edit->current_note;
    
  if(note == NULL){
    return;
  }

  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));
  
  /* zoom */
  if(use_composite_editor){
    pattern_mode = (AGS_COMPOSITE_EDIT_PATTERN_MODE == AGS_COMPOSITE_EDITOR(editor)->notation_edit->edit_mode) ? TRUE: FALSE;

    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));
  }else{
    pattern_mode = ((AGS_NOTATION_EDITOR_PATTERN_MODE & (AGS_NOTATION_EDITOR(editor)->flags)) != 0) ? TRUE: FALSE;

    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_NOTATION_TOOLBAR(toolbar)->zoom));
  }

  /* new x[1] */
  if(!pattern_mode){
    new_x = (guint) (zoom_factor * (event->x + gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar)))) / notation_edit->control_width;
    new_x = zoom_factor * floor((new_x + zoom_factor) / zoom_factor);
    
    if(new_x >= note->x[0] + zoom_factor){
      note->x[1] = new_x;
    }
  }else{
    note->x[1] = note->x[0] + 1;
  }
    
#ifdef AGS_DEBUG
  g_message("%lu-%lu %lu", note->x[0], note->x[1], note->y);
#endif

  /* add note */
  if(use_composite_editor){
    ags_composite_editor_add_note(editor,
				  note);
  }else{
    ags_notation_editor_add_note(editor,
				 note);
  }
  
  notation_edit->current_note = NULL;
  g_object_unref(note);
}
  
void
ags_notation_edit_drawing_area_button_release_delete_note(GtkWidget *editor,
							  GtkWidget *toolbar,
							  AgsNotationEdit *notation_edit,
							  AgsMachine *machine,
							  GdkEventButton *event)
{
  AgsApplicationContext *application_context;

  gboolean use_composite_editor;
  double zoom_factor;
  guint x, y;

  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));
  
  /* zoom */
  if(use_composite_editor){
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));
  }else{
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_NOTATION_TOOLBAR(toolbar)->zoom));
  }

  /* note */
  x = (guint) (zoom_factor * (event->x + gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar)))) / notation_edit->control_width;
  x = zoom_factor * floor(x / zoom_factor);
    
  y = (guint) ((event->y + gtk_range_get_value(GTK_RANGE(notation_edit->vscrollbar))) / notation_edit->control_height);

  /* delete note */
  if(use_composite_editor){
    ags_composite_editor_delete_note(editor,
				     x, y);
  }else{
    ags_notation_editor_delete_note(editor,
				    x, y);
  }
}
  
void
ags_notation_edit_drawing_area_button_release_select_note(GtkWidget *editor,
							  GtkWidget *toolbar,
							  AgsNotationEdit *notation_edit,
							  AgsMachine *machine,
							  GdkEventButton *event)
{
  AgsApplicationContext *application_context;

  gboolean use_composite_editor;
  double zoom_factor;
  guint x0, x1, y0, y1;
    
  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));
  
  /* zoom */
  if(use_composite_editor){
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));
  }else{
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_NOTATION_TOOLBAR(toolbar)->zoom));
  }

  /* region */
  x0 = (guint) (zoom_factor * notation_edit->selection_x0) / notation_edit->control_width;

  y0 = (guint) (notation_edit->selection_y0 / notation_edit->control_height);
    
  x1 = (guint) (zoom_factor * (event->x + gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar)))) / notation_edit->control_width;
    
  y1 = (guint) ((event->y + gtk_range_get_value(GTK_RANGE(notation_edit->vscrollbar))) / notation_edit->control_height);

  /* select region */
  if(use_composite_editor){
    ags_composite_editor_select_region(editor,
				       x0, y0,
				       x1, y1);
  }else{
    ags_notation_editor_select_region(editor,
				      x0, y0,
				      x1, y1);
  }
}

gboolean
ags_notation_edit_drawing_area_button_release_event(GtkWidget *widget, GdkEventButton *event, AgsNotationEdit *notation_edit)
{
  GtkWidget *editor;
  GtkWidget *toolbar;
  AgsMachine *machine;

  AgsApplicationContext *application_context;
  
  gboolean use_composite_editor;
  gboolean selected_position_cursor, selected_edit, selected_clear, selected_select;

  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));

  selected_position_cursor = FALSE;
  selected_edit = FALSE;
  selected_clear = FALSE;
  selected_select = FALSE;

  if(use_composite_editor){
    AgsCompositeToolbar *composite_toolbar;
    
    editor = gtk_widget_get_ancestor(GTK_WIDGET(notation_edit),
				     AGS_TYPE_COMPOSITE_EDITOR);
    
    toolbar = AGS_COMPOSITE_EDITOR(editor)->toolbar;

    machine = AGS_COMPOSITE_EDITOR(editor)->selected_machine;

    composite_toolbar = (AgsCompositeToolbar *) toolbar;
    
    selected_position_cursor = (composite_toolbar->selected_tool == composite_toolbar->position) ? TRUE: FALSE;
    selected_edit = (composite_toolbar->selected_tool == composite_toolbar->edit) ? TRUE: FALSE;
    selected_clear = (composite_toolbar->selected_tool == composite_toolbar->clear) ? TRUE: FALSE;
    selected_select = (composite_toolbar->selected_tool == composite_toolbar->select) ? TRUE: FALSE;
  }else{
    AgsNotationToolbar *notation_toolbar;
    
    editor = gtk_widget_get_ancestor(GTK_WIDGET(notation_edit),
				     AGS_TYPE_NOTATION_EDITOR);
    
    toolbar = AGS_NOTATION_EDITOR(editor)->notation_toolbar;

    machine = AGS_NOTATION_EDITOR(editor)->selected_machine;

    notation_toolbar = (AgsNotationToolbar *) toolbar;
    
    selected_position_cursor = (notation_toolbar->selected_edit_mode == notation_toolbar->position) ? TRUE: FALSE;
    selected_edit = (notation_toolbar->selected_edit_mode == notation_toolbar->edit) ? TRUE: FALSE;
    selected_clear = (notation_toolbar->selected_edit_mode == notation_toolbar->clear) ? TRUE: FALSE;
    selected_select = (notation_toolbar->selected_edit_mode == notation_toolbar->select) ? TRUE: FALSE;
  }

  if(machine != NULL &&
     event->button == 1){    
    notation_edit->button_mask &= (~AGS_NOTATION_EDIT_BUTTON_1);
    
    if(selected_position_cursor){
      ags_notation_edit_drawing_area_button_release_position_cursor(editor,
								    toolbar,
								    notation_edit,
								    machine,
								    event);
      
      //      notation_edit->mode = AGS_NOTATION_EDIT_NO_EDIT_MODE;
    }else if(selected_edit){
      ags_notation_edit_drawing_area_button_release_add_note(editor,
							     toolbar,
							     notation_edit,
							     machine,
							     event);

      notation_edit->mode = AGS_NOTATION_EDIT_NO_EDIT_MODE;
    }else if(selected_clear){
      ags_notation_edit_drawing_area_button_release_delete_note(editor,
								toolbar,
								notation_edit,
								machine,
								event);

      notation_edit->mode = AGS_NOTATION_EDIT_NO_EDIT_MODE;
    }else if(selected_select){
      ags_notation_edit_drawing_area_button_release_select_note(editor,
								toolbar,
								notation_edit,
								machine,
								event);

      notation_edit->mode = AGS_NOTATION_EDIT_NO_EDIT_MODE;
    }
  }

  return(FALSE);
}

void
ags_notation_edit_drawing_area_motion_notify_position_cursor(GtkWidget *editor,
							     GtkWidget *toolbar,
							     AgsNotationEdit *notation_edit,
							     AgsMachine *machine,
							     GdkEventMotion *event)
{
  AgsApplicationContext *application_context;
  
  gboolean use_composite_editor;
  double zoom_factor;

  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));

  /* zoom */
  if(use_composite_editor){
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));
  }else{
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_NOTATION_TOOLBAR(toolbar)->zoom));
  }

  /* cursor position */
  notation_edit->cursor_position_x = (guint) (zoom_factor * (event->x + gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar)))) / notation_edit->control_width;
  notation_edit->cursor_position_x = zoom_factor * floor(notation_edit->cursor_position_x / zoom_factor);

  notation_edit->cursor_position_y = (guint) ((event->y + gtk_range_get_value(GTK_RANGE(notation_edit->vscrollbar))) / notation_edit->control_height);

#ifdef AGS_DEBUG
  g_message("%lu %lu", notation_edit->cursor_position_x, notation_edit->cursor_position_y);
#endif
    
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}

void
ags_notation_edit_drawing_area_motion_notify_add_note(GtkWidget *editor,
						      GtkWidget *toolbar,
						      AgsNotationEdit *notation_edit,
						      AgsMachine *machine,
						      GdkEventMotion *event)
{
  AgsNote *note;
    
  AgsApplicationContext *application_context;
  
  gboolean use_composite_editor;
  gboolean pattern_mode;
  double zoom_factor;
  guint new_x;
    
  note = notation_edit->current_note;
    
  if(note == NULL){
    return;
  }

  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));

  /* zoom */
  if(use_composite_editor){
    pattern_mode = (AGS_COMPOSITE_EDIT_PATTERN_MODE == AGS_COMPOSITE_EDITOR(editor)->notation_edit->edit_mode) ? TRUE: FALSE;
    
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));
  }else{
    pattern_mode = ((AGS_NOTATION_EDITOR_PATTERN_MODE & (AGS_NOTATION_EDITOR(editor)->flags)) != 0) ? TRUE: FALSE;

    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_NOTATION_TOOLBAR(toolbar)->zoom));
  }

  /* new x[1] */
  if(!pattern_mode){
    new_x = (guint) (zoom_factor * (event->x + gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar)))) / notation_edit->control_width;
    new_x = zoom_factor * floor((new_x + zoom_factor) / zoom_factor);
    
    if(new_x >= note->x[0] + zoom_factor){
      note->x[1] = new_x;
    }
  }else{
    note->x[1] = note->x[0] + 1;
  }
    
#ifdef AGS_DEBUG
  g_message("%lu-%lu %lu", note->x[0], note->x[1], note->y);
#endif
    
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}

void
ags_notation_edit_drawing_area_motion_notify_select_note(GtkWidget *editor,
							 GtkWidget *toolbar,
							 AgsNotationEdit *notation_edit,
							 AgsMachine *machine,
							 GdkEventMotion *event)
{
  if(event->x + gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar)) >= 0.0){
    notation_edit->selection_x1 = (guint) event->x + gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar));
  }else{
    notation_edit->selection_x1 = 0.0;
  }
    
  if(event->y + gtk_range_get_value(GTK_RANGE(notation_edit->vscrollbar)) >= 0.0){
    notation_edit->selection_y1 = (guint) event->y + gtk_range_get_value(GTK_RANGE(notation_edit->vscrollbar));
  }else{
    notation_edit->selection_y1 = 0.0;
  }

  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}

gboolean
ags_notation_edit_drawing_area_motion_notify_event(GtkWidget *widget, GdkEventMotion *event, AgsNotationEdit *notation_edit)
{
  GtkWidget *editor;
  GtkWidget *toolbar;
  AgsMachine *machine;

  AgsApplicationContext *application_context;
  
  gboolean use_composite_editor;
  gboolean selected_position_cursor, selected_edit, selected_clear, selected_select;

  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));

  selected_position_cursor = FALSE;
  selected_edit = FALSE;
  selected_clear = FALSE;
  selected_select = FALSE;

  if(use_composite_editor){
    AgsCompositeToolbar *composite_toolbar;
    
    editor = gtk_widget_get_ancestor(GTK_WIDGET(notation_edit),
				     AGS_TYPE_COMPOSITE_EDITOR);
    
    toolbar = AGS_COMPOSITE_EDITOR(editor)->toolbar;

    machine = AGS_COMPOSITE_EDITOR(editor)->selected_machine;

    composite_toolbar = (AgsCompositeToolbar *) toolbar;

    selected_position_cursor = (composite_toolbar->selected_tool == composite_toolbar->position) ? TRUE: FALSE;
    selected_edit = (composite_toolbar->selected_tool == composite_toolbar->edit) ? TRUE: FALSE;
    selected_clear = (composite_toolbar->selected_tool == composite_toolbar->clear) ? TRUE: FALSE;
    selected_select = (composite_toolbar->selected_tool == composite_toolbar->select) ? TRUE: FALSE;
  }else{
    AgsNotationToolbar *notation_toolbar;
    
    editor = gtk_widget_get_ancestor(GTK_WIDGET(notation_edit),
				     AGS_TYPE_NOTATION_EDITOR);
    
    toolbar = AGS_NOTATION_EDITOR(editor)->notation_toolbar;

    machine = AGS_NOTATION_EDITOR(editor)->selected_machine;

    notation_toolbar = (AgsNotationToolbar *) toolbar;
    
    selected_position_cursor = (notation_toolbar->selected_edit_mode == notation_toolbar->position) ? TRUE: FALSE;
    selected_edit = (notation_toolbar->selected_edit_mode == notation_toolbar->edit) ? TRUE: FALSE;
    selected_clear = (notation_toolbar->selected_edit_mode == notation_toolbar->clear) ? TRUE: FALSE;
    selected_select = (notation_toolbar->selected_edit_mode == notation_toolbar->select) ? TRUE: FALSE;
  }

  gtk_widget_grab_focus((GtkWidget *) notation_edit->drawing_area);

  if(machine != NULL &&
     (AGS_NOTATION_EDIT_BUTTON_1 & (notation_edit->button_mask)) != 0){
    if(selected_position_cursor){
      ags_notation_edit_drawing_area_motion_notify_position_cursor(editor,
								   toolbar,
								   notation_edit,
								   machine,
								   event);
    }else if(selected_edit){
      ags_notation_edit_drawing_area_motion_notify_add_note(editor,
							    toolbar,
							    notation_edit,
							    machine,
							    event);
    }else if(selected_clear){
      //NOTE:JK: only takes action on release
    }else if(selected_select){
      ags_notation_edit_drawing_area_motion_notify_select_note(editor,
							       toolbar,
							       notation_edit,
							       machine,
							       event);
    }
  }

  return(FALSE);
}

gboolean
ags_notation_edit_drawing_area_key_press_event(GtkWidget *widget, GdkEventKey *event, AgsNotationEdit *notation_edit)
{
  GtkWidget *editor;
  AgsMachine *machine;

  AgsApplicationContext *application_context;
  
  gboolean use_composite_editor;
  gboolean retval;
  
  if(event->keyval == GDK_KEY_Tab ||
     event->keyval == GDK_KEY_ISO_Left_Tab ||
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

  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));

  machine = NULL;
  
  if(use_composite_editor){
    editor = gtk_widget_get_ancestor(GTK_WIDGET(notation_edit),
				     AGS_TYPE_COMPOSITE_EDITOR);
    
    machine = AGS_COMPOSITE_EDITOR(editor)->selected_machine;
  }else{
    editor = gtk_widget_get_ancestor(GTK_WIDGET(notation_edit),
				     AGS_TYPE_NOTATION_EDITOR);
    
    machine = AGS_NOTATION_EDITOR(editor)->selected_machine;
  }
  
  if(machine != NULL){
    switch(event->keyval){
    case GDK_KEY_Control_L:
      {
	notation_edit->key_mask |= AGS_NOTATION_EDIT_KEY_L_CONTROL;
      }
      break;
    case GDK_KEY_Control_R:
      {
	notation_edit->key_mask |= AGS_NOTATION_EDIT_KEY_R_CONTROL;
      }
      break;
    case GDK_KEY_Shift_L:
      {
	notation_edit->key_mask |= AGS_NOTATION_EDIT_KEY_L_SHIFT;
      }
      break;
    case GDK_KEY_Shift_R:
      {
	notation_edit->key_mask |= AGS_NOTATION_EDIT_KEY_R_SHIFT;
      }
      break;
    case GDK_KEY_a:
      {
	/* select all notes */
	if((AGS_NOTATION_EDIT_KEY_L_CONTROL & (notation_edit->key_mask)) != 0 || (AGS_NOTATION_EDIT_KEY_R_CONTROL & (notation_edit->key_mask)) != 0){
	  if(use_composite_editor){
 	    ags_composite_editor_select_all(editor);
	  }else{
	    ags_notation_editor_select_all(editor);
	  }
	}
      }
      break;
    case GDK_KEY_c:
      {
	/* copy notes */
	if((AGS_NOTATION_EDIT_KEY_L_CONTROL & (notation_edit->key_mask)) != 0 || (AGS_NOTATION_EDIT_KEY_R_CONTROL & (notation_edit->key_mask)) != 0){
	  if(use_composite_editor){
	    ags_composite_editor_copy(editor);
	  }else{
	    ags_notation_editor_copy(editor);
	  }
	}
      }
      break;
    case GDK_KEY_v:
      {
	/* paste notes */
	if((AGS_NOTATION_EDIT_KEY_L_CONTROL & (notation_edit->key_mask)) != 0 || (AGS_NOTATION_EDIT_KEY_R_CONTROL & (notation_edit->key_mask)) != 0){
	  if(use_composite_editor){
	    ags_composite_editor_paste(editor);
	  }else{
	    ags_notation_editor_paste(editor);
	  }
	}
      }
      break;
    case GDK_KEY_x:
      {
	/* cut notes */
	if((AGS_NOTATION_EDIT_KEY_L_CONTROL & (notation_edit->key_mask)) != 0 || (AGS_NOTATION_EDIT_KEY_R_CONTROL & (notation_edit->key_mask)) != 0){
	  if(use_composite_editor){
	    ags_composite_editor_cut(editor);
	  }else{
	    ags_notation_editor_cut(editor);
	  }
	}
      }
      break;
    case GDK_KEY_i:
      {
	/* invert notes */
	if((AGS_NOTATION_EDIT_KEY_L_CONTROL & (notation_edit->key_mask)) != 0 || (AGS_NOTATION_EDIT_KEY_R_CONTROL & (notation_edit->key_mask)) != 0){
	  if(use_composite_editor){
	    ags_composite_editor_invert(editor);
	  }else{
	    ags_notation_editor_invert(editor);
	  }
	}
      }
      break;
    case GDK_KEY_m:
      {
	/* meta */
	if((AGS_NOTATION_EDIT_KEY_L_CONTROL & (notation_edit->key_mask)) != 0 || (AGS_NOTATION_EDIT_KEY_R_CONTROL & (notation_edit->key_mask)) != 0){
	  AgsNotationMeta *notation_meta;

	  notation_meta = NULL;
	  
	  if(use_composite_editor){
	    notation_meta = AGS_COMPOSITE_EDITOR(editor)->notation_edit->edit_meta;
	  }else{
	    notation_meta = AGS_NOTATION_EDITOR(editor)->notation_meta;
	  }

	  if((AGS_NOTATION_META_ENABLED & (notation_meta->flags)) != 0){
	    notation_meta->flags &= (~AGS_NOTATION_META_ENABLED);

	    gtk_widget_hide(notation_meta);
	  }else{
	    notation_meta->flags |= AGS_NOTATION_META_ENABLED;

	    gtk_widget_show_all(notation_meta);

	    ags_notation_meta_refresh(notation_meta);
	  }
	}
      }
      break;
    }
  }

  return(retval);
}

gboolean
ags_notation_edit_drawing_area_key_release_event(GtkWidget *widget, GdkEventKey *event, AgsNotationEdit *notation_edit)
{
  AgsMachine *machine;
  GtkWidget *editor;
  AgsNotebook *channel_selector;
  
  AgsApplicationContext *application_context;
  
  GtkAllocation allocation;

  gboolean use_composite_editor;
  gboolean pattern_mode;
  double zoom_factor;
  gint i;
  gboolean retval;
  gboolean do_feedback;

  GRecMutex *audio_mutex;

  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));

  machine = NULL;
  
  if(use_composite_editor){
    editor = gtk_widget_get_ancestor(GTK_WIDGET(notation_edit),
				     AGS_TYPE_COMPOSITE_EDITOR);

    channel_selector = AGS_COMPOSITE_EDITOR(editor)->notation_edit->channel_selector;
    
    machine = AGS_COMPOSITE_EDITOR(editor)->selected_machine;
    
    pattern_mode = (AGS_COMPOSITE_EDIT_PATTERN_MODE == AGS_COMPOSITE_EDITOR(editor)->notation_edit->edit_mode) ? TRUE: FALSE;    
  }else{
    editor = gtk_widget_get_ancestor(GTK_WIDGET(notation_edit),
				     AGS_TYPE_NOTATION_EDITOR);
    
    channel_selector = AGS_NOTATION_EDITOR(editor)->notebook;

    machine = AGS_NOTATION_EDITOR(editor)->selected_machine;

    pattern_mode = ((AGS_NOTATION_EDITOR_PATTERN_MODE & (AGS_NOTATION_EDITOR(editor)->flags)) != 0) ? TRUE: FALSE;
  }
  
  if(event->keyval == GDK_KEY_Tab ||
     event->keyval == GDK_KEY_ISO_Left_Tab ||
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

  if(use_composite_editor){
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(AGS_COMPOSITE_EDITOR(editor)->toolbar)->zoom));
  }else{
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_NOTATION_TOOLBAR(AGS_NOTATION_EDITOR(editor)->notation_toolbar)->zoom));
  }

  gtk_widget_get_allocation(GTK_WIDGET(notation_edit->drawing_area),
			    &allocation);

  if(machine != NULL){
    /* get audio mutex */
    audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(machine->audio);

    /* do feedback - initial set */
    do_feedback = FALSE;

    /* check key value */
    switch(event->keyval){
    case GDK_KEY_Control_L:
      {
	notation_edit->key_mask &= (~AGS_NOTATION_EDIT_KEY_L_CONTROL);
      }
      break;
    case GDK_KEY_Control_R:
      {
	notation_edit->key_mask &= (~AGS_NOTATION_EDIT_KEY_R_CONTROL);
      }
      break;
    case GDK_KEY_Shift_L:
      {
	notation_edit->key_mask &= (~AGS_NOTATION_EDIT_KEY_L_SHIFT);
      }
      break;
    case GDK_KEY_Shift_R:
      {
	notation_edit->key_mask &= (~AGS_NOTATION_EDIT_KEY_R_SHIFT);
      }
      break;
    case GDK_KEY_Left:
    case GDK_KEY_leftarrow:
      {
	if((AGS_NOTATION_EDIT_KEY_L_SHIFT & (notation_edit->key_mask)) != 0 ||
	   (AGS_NOTATION_EDIT_KEY_R_SHIFT & (notation_edit->key_mask)) != 0){
	  AgsTimestamp *timestamp;

	  if(pattern_mode){
	    return(retval);
	  }
	  
	  /* shrink note */
	  timestamp = ags_timestamp_new();

	  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
	  timestamp->flags |= AGS_TIMESTAMP_OFFSET;

	  timestamp->timer.ags_offset.offset = AGS_NOTATION_DEFAULT_OFFSET * floor(notation_edit->cursor_position_x / AGS_NOTATION_DEFAULT_OFFSET);
	  
	  i = 0;
	  do_feedback = TRUE;
	
	  while((i = ags_notebook_next_active_tab(channel_selector,
						  i)) != -1){
	    GList *list_notation;
	    
	    g_rec_mutex_lock(audio_mutex);
	  
	    list_notation = ags_notation_find_near_timestamp(machine->audio->notation, i,
							     timestamp);
	    
	    if(list_notation != NULL){
	      AgsNote *note;
	      
	      note = ags_notation_find_point(AGS_NOTATION(list_notation->data),
					     notation_edit->cursor_position_x, notation_edit->cursor_position_y,
					     FALSE);

	      if(note != NULL &&
		 note->x[1] - note->x[0] - zoom_factor >= zoom_factor){
		note->x[1] -= zoom_factor;
	      }
	    }

	    g_rec_mutex_unlock(audio_mutex);
	  
	    i++;
	  }

	  g_object_unref(timestamp);
	}else{
	  gdouble x0_offset;

	  /* position cursor */
	  if(notation_edit->cursor_position_x > 0){
	    if(notation_edit->cursor_position_x - (zoom_factor) > 0){
	      notation_edit->cursor_position_x -= (zoom_factor);
	    }else{
	      notation_edit->cursor_position_x = 0;
	    }
	  
	    do_feedback = TRUE;
	  }

	  x0_offset = notation_edit->cursor_position_x * notation_edit->control_width;
      
	  if(x0_offset / zoom_factor < gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar))){
	    gtk_range_set_value(GTK_RANGE(notation_edit->hscrollbar),
				x0_offset / zoom_factor);
	  }
	}
      }
      break;
    case GDK_KEY_Right:
    case GDK_KEY_rightarrow:
      {
	if((AGS_NOTATION_EDIT_KEY_L_SHIFT & (notation_edit->key_mask)) != 0 ||
	   (AGS_NOTATION_EDIT_KEY_R_SHIFT & (notation_edit->key_mask)) != 0){
	  AgsTimestamp *timestamp;

	  if(pattern_mode){
	    return(retval);
	  }

	  /* shrink note */
	  timestamp = ags_timestamp_new();

	  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
	  timestamp->flags |= AGS_TIMESTAMP_OFFSET;

	  timestamp->timer.ags_offset.offset = AGS_NOTATION_DEFAULT_OFFSET * floor(notation_edit->cursor_position_x / AGS_NOTATION_DEFAULT_OFFSET);
	  
	  i = 0;
	  do_feedback = TRUE;
	
	  while((i = ags_notebook_next_active_tab(channel_selector,
						  i)) != -1){
	    GList *list_notation;
	    
	    g_rec_mutex_lock(audio_mutex);
	  
	    list_notation = ags_notation_find_near_timestamp(machine->audio->notation, i,
							     timestamp);
	    
	    if(list_notation != NULL){
	      AgsNote *note;
	      
	      note = ags_notation_find_point(AGS_NOTATION(list_notation->data),
					     notation_edit->cursor_position_x, notation_edit->cursor_position_y,
					     FALSE);

	      if(note != NULL){
		note->x[1] += zoom_factor;
	      }
	    }

	    g_rec_mutex_unlock(audio_mutex);
	  
	    i++;
	  }

	  g_object_unref(timestamp);
	}else{
	  gdouble x0_offset;
	  
	  /* position cursor */      
	  if(notation_edit->cursor_position_x < AGS_NOTATION_EDITOR_MAX_CONTROLS){
	    notation_edit->cursor_position_x += (zoom_factor);
	  
	    do_feedback = TRUE;
	  }

	  x0_offset = notation_edit->cursor_position_x * notation_edit->control_width;
      
	  if((x0_offset + notation_edit->control_width) / zoom_factor > gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar)) + allocation.width){
	    gtk_range_set_value(GTK_RANGE(notation_edit->hscrollbar),
				x0_offset / zoom_factor);
	  }
	}
      }
      break;
    case GDK_KEY_Up:
    case GDK_KEY_uparrow:
      {
	gdouble y0_offset;
      
	if(notation_edit->cursor_position_y > 0){
	  notation_edit->cursor_position_y -= 1;
	
	  do_feedback = TRUE;
	}

	y0_offset = notation_edit->cursor_position_y * notation_edit->control_height;
      
	if(y0_offset < gtk_range_get_value(GTK_RANGE(notation_edit->vscrollbar))){
	  gtk_range_set_value(GTK_RANGE(notation_edit->vscrollbar),
			      y0_offset);
	}
      }
      break;
    case GDK_KEY_Down:
    case GDK_KEY_downarrow:
      {
	gdouble y0_offset;
      
	if(notation_edit->cursor_position_y > 0){
	  notation_edit->cursor_position_y += 1;
	
	  do_feedback = TRUE;
	}

	y0_offset = notation_edit->cursor_position_y * notation_edit->control_height;
      
	if(y0_offset < gtk_range_get_value(GTK_RANGE(notation_edit->vscrollbar))){
	  gtk_range_set_value(GTK_RANGE(notation_edit->vscrollbar),
			      y0_offset);
	}
      }
      break;
    case GDK_KEY_space:
      {
	AgsNote *note;
	
	do_feedback = TRUE;

	note = ags_note_new();

	note->x[0] = notation_edit->cursor_position_x;

	if(!pattern_mode){
	  note->x[1] = notation_edit->cursor_position_x + zoom_factor;
	}else{
	  note->x[1] = note->x[0] + 1;
	}
	
	note->y = notation_edit->cursor_position_y;

	/* add note */
	if(use_composite_editor){
	  ags_composite_editor_add_note(editor,
					note);
	}else{
	  ags_notation_editor_add_note(editor,
				       note);
	}
      }
      break;
    case GDK_KEY_Delete:
      {
	/* delete note */
	if(use_composite_editor){
	  ags_composite_editor_delete_note(editor,
					   notation_edit->cursor_position_x, notation_edit->cursor_position_y);
	}else{
	  ags_notation_editor_delete_note(editor,
					  notation_edit->cursor_position_x, notation_edit->cursor_position_y);
	}
      }
      break;
    }

    gtk_widget_queue_draw((GtkWidget *) notation_edit);

    /* do feedback */
    if(do_feedback){
      if(use_composite_editor){
	ags_composite_editor_do_feedback(editor);
      }else{
	ags_notation_editor_do_feedback(editor);
      }
    }
  }
  
  return(retval);
}

void
ags_notation_edit_vscrollbar_value_changed(GtkRange *range, AgsNotationEdit *notation_edit)
{
  GtkWidget *editor;
  AgsScrolledPiano *scrolled_piano;
  
  GtkAdjustment *piano_adjustment;
  
  AgsApplicationContext *application_context;
  
  gboolean use_composite_editor;

  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));

  if(use_composite_editor){
    editor = gtk_widget_get_ancestor((GtkWidget *) notation_edit,
				     AGS_TYPE_COMPOSITE_EDITOR);

    scrolled_piano = AGS_COMPOSITE_EDITOR(editor)->notation_edit->edit_control;
  }else{
    editor = gtk_widget_get_ancestor((GtkWidget *) notation_edit,
				     AGS_TYPE_NOTATION_EDITOR);

    scrolled_piano = AGS_NOTATION_EDITOR(editor)->scrolled_piano;
  }    
    
  g_object_get(scrolled_piano->viewport,
	       "vadjustment", &piano_adjustment,
	       NULL);
  
  gtk_adjustment_set_value(piano_adjustment,
			   gtk_range_get_value(range));
  
  gtk_widget_queue_draw((GtkWidget *) scrolled_piano);

  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) notation_edit->drawing_area);
}

void
ags_notation_edit_hscrollbar_value_changed(GtkRange *range, AgsNotationEdit *notation_edit)
{
  GtkWidget *editor;

  AgsApplicationContext *application_context;
  
  gboolean use_composite_editor;
  gdouble gui_scale_factor;
  gdouble value;

  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  value = gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar)) / (guint) (gui_scale_factor * 64.0);

  gtk_adjustment_set_value(notation_edit->ruler->adjustment,
			   gui_scale_factor * value);
  gtk_widget_queue_draw((GtkWidget *) notation_edit->ruler);

  if(use_composite_editor){
    editor = gtk_widget_get_ancestor(GTK_WIDGET(notation_edit),
				     AGS_TYPE_COMPOSITE_EDITOR);
    
    gtk_adjustment_set_value(AGS_COMPOSITE_EDITOR(editor)->notation_edit->ruler->adjustment,
			     gui_scale_factor * value);
    gtk_widget_queue_draw((GtkWidget *) AGS_COMPOSITE_EDITOR(editor)->notation_edit->ruler);
  }
  
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) notation_edit->drawing_area);
}
