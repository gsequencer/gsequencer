/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/GSequencer/editor/ags_wave_edit_callbacks.h>

#include <ags/GSequencer/ags_ui_provider.h>
#include <ags/GSequencer/ags_wave_editor.h>
#include <ags/GSequencer/ags_composite_editor.h>

#include <ags/GSequencer/editor/ags_composite_edit.h>

#include <math.h>
#include <gdk/gdkkeysyms.h>

void ags_wave_edit_drawing_area_button_press_position_cursor(GtkWidget *editor,
							     GtkWidget *toolbar,
							     AgsWaveEdit *wave_edit,
							     AgsMachine *machine,
							     GdkEventButton *event);
void ags_wave_edit_drawing_area_button_press_select_buffer(GtkWidget *editor,
							   GtkWidget *toolbar,
							   AgsWaveEdit *wave_edit,
							   AgsMachine *machine,
							   GdkEventButton *event);

void ags_wave_edit_drawing_area_button_release_position_cursor(GtkWidget *editor,
							       GtkWidget *toolbar,
							       AgsWaveEdit *wave_edit,
							       AgsMachine *machine,
							       GdkEventButton *event);
void ags_wave_edit_drawing_area_button_release_select_buffer(GtkWidget *editor,
							     GtkWidget *toolbar,
							     AgsWaveEdit *wave_edit,
							     AgsMachine *machine,
							     GdkEventButton *event);

void ags_wave_edit_drawing_area_motion_notify_position_cursor(GtkWidget *editor,
							      GtkWidget *toolbar,
							      AgsWaveEdit *wave_edit,
							      AgsMachine *machine,
							      GdkEventMotion *event);
void ags_wave_edit_drawing_area_motion_notify_select_buffer(GtkWidget *editor,
							    GtkWidget *toolbar,
							    AgsWaveEdit *wave_edit,
							    AgsMachine *machine,
							    GdkEventMotion *event);

gboolean
ags_wave_edit_draw_callback(GtkWidget *drawing_area, cairo_t *cr, AgsWaveEdit *wave_edit)
{
  ags_wave_edit_draw(wave_edit, cr);

  return(FALSE);
}

gboolean
ags_wave_edit_drawing_area_configure_event(GtkWidget *widget, GdkEventConfigure *event, AgsWaveEdit *wave_edit)
{
  ags_wave_edit_reset_vscrollbar(wave_edit);
  ags_wave_edit_reset_hscrollbar(wave_edit);
  
  gtk_widget_queue_draw(wave_edit);
  
  return(FALSE);
}

void
ags_wave_edit_drawing_area_button_press_position_cursor(GtkWidget *editor,
							GtkWidget *toolbar,
							AgsWaveEdit *wave_edit,
							AgsMachine *machine,
							GdkEventButton *event)
{
  GtkAdjustment *vscrollbar_adjustment;
    
  GtkAllocation allocation;

  AgsApplicationContext *application_context;
  
  gboolean use_composite_editor;
  gdouble c_range;
  guint g_range;
  double zoom_factor, zoom;
  double zoom_correction;

  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));

  /* zoom */
  if(use_composite_editor){
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));
    zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom) - 2.0);
  }else{
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_WAVE_TOOLBAR(toolbar)->zoom));
    zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) AGS_WAVE_TOOLBAR(toolbar)->zoom) - 2.0);
  }
  
  gtk_widget_get_allocation(GTK_WIDGET(wave_edit->drawing_area),
			    &allocation);

  vscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(wave_edit->vscrollbar));

  c_range = wave_edit->upper - wave_edit->lower;

  g_range = gtk_adjustment_get_upper(vscrollbar_adjustment) + allocation.height;

  /* cursor position */
  zoom_correction = 1.0 / 16;

  wave_edit->cursor_position_x = (guint) ((zoom_factor * event->x + (gtk_range_get_value(GTK_RANGE(wave_edit->hscrollbar)) / zoom / zoom_correction)));
    
  wave_edit->cursor_position_y = (((allocation.height - event->y) / g_range) * c_range);

  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) wave_edit);
}

void
ags_wave_edit_drawing_area_button_press_select_buffer(GtkWidget *editor,
						      GtkWidget *toolbar,
						      AgsWaveEdit *wave_edit,
						      AgsMachine *machine,
						      GdkEventButton *event)
{
  AgsApplicationContext *application_context;
  
  gboolean use_composite_editor;
  double zoom_factor, zoom;
  double zoom_correction;

  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));

  /* zoom */
  if(use_composite_editor){
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));
    zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom) - 2.0);
  }else{
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_WAVE_TOOLBAR(toolbar)->zoom));
    zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) AGS_WAVE_TOOLBAR(toolbar)->zoom) - 2.0);
  }
  
  zoom_correction = 1.0 / 16;

  wave_edit->selection_x0 = (guint) (zoom_factor * event->x) + (gtk_range_get_value(GTK_RANGE(wave_edit->hscrollbar)) / zoom / zoom_correction);
  wave_edit->selection_x1 = wave_edit->selection_x0;
    
  wave_edit->selection_y0 = (guint) event->y + gtk_range_get_value(GTK_RANGE(wave_edit->vscrollbar));
  wave_edit->selection_y1 = wave_edit->selection_y0;

  gtk_widget_queue_draw((GtkWidget *) wave_edit);
}

gboolean
ags_wave_edit_drawing_area_button_press_event(GtkWidget *widget, GdkEventButton *event, AgsWaveEdit *wave_edit)
{
  GtkWidget *editor;
  GtkWidget *toolbar;
  AgsMachine *machine;

  AgsApplicationContext *application_context;
  
  gboolean use_composite_editor;
  gboolean selected_position_cursor, selected_select;

  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));

  selected_position_cursor = FALSE;
  selected_select = FALSE;
  
  if(use_composite_editor){
    AgsCompositeToolbar *composite_toolbar;
    
    editor = gtk_widget_get_ancestor(GTK_WIDGET(wave_edit),
				     AGS_TYPE_COMPOSITE_EDITOR);
    
    toolbar = AGS_COMPOSITE_EDITOR(editor)->toolbar;

    machine = AGS_COMPOSITE_EDITOR(editor)->selected_machine;

    composite_toolbar = (AgsCompositeToolbar *) toolbar;
    
    selected_position_cursor = (composite_toolbar->selected_tool == composite_toolbar->position) ? TRUE: FALSE;
    selected_select = (composite_toolbar->selected_tool == composite_toolbar->select) ? TRUE: FALSE;

    AGS_COMPOSITE_EDITOR(editor)->wave_edit->focused_edit = wave_edit;
  }else{
    AgsWaveToolbar *wave_toolbar;
    
    editor = gtk_widget_get_ancestor(GTK_WIDGET(wave_edit),
				     AGS_TYPE_WAVE_EDITOR);
    
    toolbar = AGS_WAVE_EDITOR(editor)->wave_toolbar;

    machine = AGS_WAVE_EDITOR(editor)->selected_machine;

    wave_toolbar = (AgsWaveToolbar *) toolbar;
    
    selected_position_cursor = (wave_toolbar->selected_edit_mode == wave_toolbar->position) ? TRUE: FALSE;
    selected_select = (wave_toolbar->selected_edit_mode == wave_toolbar->select) ? TRUE: FALSE;
    
    AGS_WAVE_EDITOR(editor)->focused_wave_edit = wave_edit;
  }
  
  gtk_widget_grab_focus((GtkWidget *) wave_edit->drawing_area);

  if(machine != NULL &&
     event->button == 1){    
    wave_edit->button_mask = AGS_WAVE_EDIT_BUTTON_1;
    
    if(selected_position_cursor){
      wave_edit->mode = AGS_WAVE_EDIT_POSITION_CURSOR;

      ags_wave_edit_drawing_area_button_press_position_cursor(editor,
							      toolbar,
							      wave_edit,
							      machine,
							      event);
    }else if(selected_select){
      wave_edit->mode = AGS_WAVE_EDIT_SELECT_BUFFER;

      ags_wave_edit_drawing_area_button_press_select_buffer(editor,
							    toolbar,
							    wave_edit,
							    machine,
							    event);
    }
  }
  
  return(FALSE);
}

void
ags_wave_edit_drawing_area_button_release_position_cursor(GtkWidget *editor,
							  GtkWidget *toolbar,
							  AgsWaveEdit *wave_edit,
							  AgsMachine *machine,
							  GdkEventButton *event)
{
  GtkAdjustment *vscrollbar_adjustment;

  GtkAllocation allocation;

  AgsApplicationContext *application_context;
  
  gboolean use_composite_editor;
  gdouble c_range;
  guint g_range;
  double zoom_factor, zoom;
  double zoom_correction;

  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));
  
  /* zoom */
  if(use_composite_editor){
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));
    zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom) - 2.0);
  }else{
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_WAVE_TOOLBAR(toolbar)->zoom));
    zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) AGS_WAVE_TOOLBAR(toolbar)->zoom) - 2.0);
  }
  
  gtk_widget_get_allocation(GTK_WIDGET(wave_edit->drawing_area),
			    &allocation);

  vscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(wave_edit->vscrollbar));

  c_range = wave_edit->upper - wave_edit->lower;

  g_range = gtk_adjustment_get_upper(vscrollbar_adjustment) + allocation.height;

  /* cursor position */
  zoom_correction = 1.0 / 16;

  wave_edit->cursor_position_x = (guint) ((zoom_factor * event->x + (gtk_range_get_value(GTK_RANGE(wave_edit->hscrollbar)) / zoom / zoom_correction)));
    
  wave_edit->cursor_position_y = (((allocation.height - event->y) / g_range) * c_range);
    
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) wave_edit);
}

void
ags_wave_edit_drawing_area_button_release_select_buffer(GtkWidget *editor,
							GtkWidget *toolbar,
							AgsWaveEdit *wave_edit,
							AgsMachine *machine,
							GdkEventButton *event)
{
  GtkAdjustment *vscrollbar_adjustment;

  GtkAllocation allocation;

  AgsApplicationContext *application_context;

  gboolean use_composite_editor;
  gdouble c_range;
  guint g_range;
  double zoom_factor, zoom;
  double zoom_correction;
  guint x0, x1;
  gdouble y0, y1;

  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));
  
  /* zoom */
  if(use_composite_editor){
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));
    zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom) - 2.0);
  }else{
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_WAVE_TOOLBAR(toolbar)->zoom));
    zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) AGS_WAVE_TOOLBAR(toolbar)->zoom) - 2.0);
  }
  
  gtk_widget_get_allocation(GTK_WIDGET(wave_edit->drawing_area),
			    &allocation);

  vscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(wave_edit->vscrollbar));

  c_range = wave_edit->upper - wave_edit->lower;

  g_range = gtk_adjustment_get_upper(vscrollbar_adjustment) + allocation.height;

  //FIXME:JK: this won't work
//  wave_edit->selection_x1 = (guint) (zoom_factor * event->x) + (gtk_range_get_value(GTK_RANGE(wave_edit->hscrollbar)) / zoom / zoom_correction);

  /* region */
  x0 = (guint) wave_edit->selection_x0;

  y0 = ((gdouble) (allocation.height - wave_edit->selection_y0) / g_range) * c_range;
  
  x1 = (guint) wave_edit->selection_x1;
    
  y1 = (((allocation.height - event->y) + gtk_range_get_value(GTK_RANGE(wave_edit->vscrollbar))) / g_range) * c_range;
    
  /* select region */
  if(use_composite_editor){
    ags_composite_editor_select_region(editor,
				       x0, y0,
				       x1, y1);
  }else{
    ags_wave_editor_select_region(editor,
				  x0, y0,
				  x1, y1);
  }
}

gboolean
ags_wave_edit_drawing_area_button_release_event(GtkWidget *widget, GdkEventButton *event, AgsWaveEdit *wave_edit)
{
  GtkWidget *editor;
  GtkWidget *toolbar;
  AgsMachine *machine;

  AgsApplicationContext *application_context;
  
  gboolean use_composite_editor;
  gboolean selected_position_cursor, selected_select;

  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));

  editor = NULL;
  toolbar = NULL;
  
  machine = NULL;
  
  selected_position_cursor = FALSE;
  selected_select = FALSE;

  if(use_composite_editor){
    AgsCompositeToolbar *composite_toolbar;
    
    editor = gtk_widget_get_ancestor(GTK_WIDGET(wave_edit),
				     AGS_TYPE_COMPOSITE_EDITOR);
    
    toolbar = AGS_COMPOSITE_EDITOR(editor)->toolbar;

    machine = AGS_COMPOSITE_EDITOR(editor)->selected_machine;

    composite_toolbar = (AgsCompositeToolbar *) toolbar;
    
    selected_position_cursor = (composite_toolbar->selected_tool == composite_toolbar->position) ? TRUE: FALSE;
    selected_select = (composite_toolbar->selected_tool == composite_toolbar->select) ? TRUE: FALSE;
  }else{
    AgsWaveToolbar *wave_toolbar;
    
    editor = gtk_widget_get_ancestor(GTK_WIDGET(wave_edit),
				     AGS_TYPE_WAVE_EDITOR);
    
    toolbar = AGS_WAVE_EDITOR(editor)->wave_toolbar;

    machine = AGS_WAVE_EDITOR(editor)->selected_machine;

    wave_toolbar = (AgsWaveToolbar *) toolbar;
    
    selected_position_cursor = (wave_toolbar->selected_edit_mode == wave_toolbar->position) ? TRUE: FALSE;
    selected_select = (wave_toolbar->selected_edit_mode == wave_toolbar->select) ? TRUE: FALSE;
  }
  
  if(machine != NULL &&
     event->button == 1){
    wave_edit->button_mask &= (~AGS_WAVE_EDIT_BUTTON_1);
    
    if(selected_position_cursor){
      ags_wave_edit_drawing_area_button_release_position_cursor(editor,
								toolbar,
								wave_edit,
								machine,
								event);

      wave_edit->mode = AGS_WAVE_EDIT_NO_EDIT_MODE;
    }else if(selected_select){
      ags_wave_edit_drawing_area_button_release_select_buffer(editor,
							      toolbar,
							      wave_edit,
							      machine,
							      event);

      wave_edit->mode = AGS_WAVE_EDIT_NO_EDIT_MODE;
    }
  }

  return(FALSE);
}

void
ags_wave_edit_drawing_area_motion_notify_position_cursor(GtkWidget *editor,
							 GtkWidget *toolbar,
							 AgsWaveEdit *wave_edit,
							 AgsMachine *machine,
							 GdkEventMotion *event)
{
  GtkAdjustment *vscrollbar_adjustment;
    
  GtkAllocation allocation;

  AgsApplicationContext *application_context;
  
  gboolean use_composite_editor;
  gdouble c_range;
  guint g_range;
  double zoom_factor, zoom;
  double zoom_correction;

  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));

  /* zoom */
  if(use_composite_editor){
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));
    zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom) - 2.0);
  }else{
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_WAVE_TOOLBAR(toolbar)->zoom));
    zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) AGS_WAVE_TOOLBAR(toolbar)->zoom) - 2.0);
  }
  
  gtk_widget_get_allocation(GTK_WIDGET(wave_edit->drawing_area),
			    &allocation);

  vscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(wave_edit->vscrollbar));

  c_range = wave_edit->upper - wave_edit->lower;

  g_range = gtk_adjustment_get_upper(vscrollbar_adjustment) + allocation.height;

  /* cursor position */
  zoom_correction = 1.0 / 16;

  wave_edit->cursor_position_x = (guint) ((zoom_factor * event->x + (gtk_range_get_value(GTK_RANGE(wave_edit->hscrollbar)) / zoom / zoom_correction)));

  wave_edit->cursor_position_y = (((allocation.height - event->y) / g_range) * c_range);

#ifdef AGS_DEBUG
  g_message("%lu %f", wave_edit->cursor_position_x, wave_edit->cursor_position_y);
#endif
    
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) wave_edit);
}

void
ags_wave_edit_drawing_area_motion_notify_select_buffer(GtkWidget *editor,
						       GtkWidget *toolbar,
						       AgsWaveEdit *wave_edit,
						       AgsMachine *machine,
						       GdkEventMotion *event)
{
  AgsApplicationContext *application_context;
  
  gboolean use_composite_editor;
  double zoom_factor, zoom;
  double zoom_correction;

  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));

  /* zoom */
  if(use_composite_editor){
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));
    zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom) - 2.0);
  }else{
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_WAVE_TOOLBAR(toolbar)->zoom));
    zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) AGS_WAVE_TOOLBAR(toolbar)->zoom) - 2.0);
  }

  zoom_correction = 1.0 / 16;
  
  wave_edit->selection_x1 = (guint) (zoom_factor * event->x) + (gtk_range_get_value(GTK_RANGE(wave_edit->hscrollbar)) / zoom / zoom_correction);
  
  if(event->y + gtk_range_get_value(GTK_RANGE(wave_edit->vscrollbar)) >= 0.0){
    wave_edit->selection_y1 = (guint) event->y + gtk_range_get_value(GTK_RANGE(wave_edit->vscrollbar));
  }else{
    wave_edit->selection_y1 = 0.0;
  }
    
  gtk_widget_queue_draw((GtkWidget *) wave_edit);
}

gboolean
ags_wave_edit_drawing_area_motion_notify_event(GtkWidget *widget, GdkEventMotion *event, AgsWaveEdit *wave_edit)
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
  selected_select = FALSE;

  if(use_composite_editor){
    AgsCompositeToolbar *composite_toolbar;
    
    editor = gtk_widget_get_ancestor(GTK_WIDGET(wave_edit),
				     AGS_TYPE_COMPOSITE_EDITOR);
    
    toolbar = AGS_COMPOSITE_EDITOR(editor)->toolbar;

    machine = AGS_COMPOSITE_EDITOR(editor)->selected_machine;

    composite_toolbar = (AgsCompositeToolbar *) toolbar;

    selected_position_cursor = (composite_toolbar->selected_tool == composite_toolbar->position) ? TRUE: FALSE;
    selected_select = (composite_toolbar->selected_tool == composite_toolbar->select) ? TRUE: FALSE;
  }else{
    AgsWaveToolbar *wave_toolbar;
    
    editor = gtk_widget_get_ancestor(GTK_WIDGET(wave_edit),
				     AGS_TYPE_WAVE_EDITOR);
    
    toolbar = AGS_WAVE_EDITOR(editor)->wave_toolbar;

    machine = AGS_WAVE_EDITOR(editor)->selected_machine;

    wave_toolbar = (AgsWaveToolbar *) toolbar;
    
    selected_position_cursor = (wave_toolbar->selected_edit_mode == wave_toolbar->position) ? TRUE: FALSE;
    selected_select = (wave_toolbar->selected_edit_mode == wave_toolbar->select) ? TRUE: FALSE;
  }
  
  gtk_widget_grab_focus((GtkWidget *) wave_edit->drawing_area);

  if(machine != NULL &&
     (AGS_WAVE_EDIT_BUTTON_1 & (wave_edit->button_mask)) != 0){
    if(wave_edit->mode == AGS_WAVE_EDIT_POSITION_CURSOR){
      ags_wave_edit_drawing_area_motion_notify_position_cursor(editor,
							       toolbar,
							       wave_edit,
							       machine,
							       event);
    }else if(wave_edit->mode == AGS_WAVE_EDIT_SELECT_BUFFER){
      ags_wave_edit_drawing_area_motion_notify_select_buffer(editor,
							     toolbar,
							     wave_edit,
							     machine,
							     event);
    }
  }

  return(FALSE);
}

gboolean
ags_wave_edit_drawing_area_key_press_event(GtkWidget *widget, GdkEventKey *event, AgsWaveEdit *wave_edit)
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
    editor = gtk_widget_get_ancestor(GTK_WIDGET(wave_edit),
				     AGS_TYPE_COMPOSITE_EDITOR);
    
    machine = AGS_COMPOSITE_EDITOR(editor)->selected_machine;
  }else{
    editor = gtk_widget_get_ancestor(GTK_WIDGET(wave_edit),
				     AGS_TYPE_WAVE_EDITOR);
    
    machine = AGS_WAVE_EDITOR(editor)->selected_machine;
  }
  
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
	  if(use_composite_editor){
	    ags_composite_editor_select_all(editor);
	  }else{
	    ags_wave_editor_select_all(editor);
	  }
	}
      }
      break;
    case GDK_KEY_c:
      {
	/* copy accelerations */
	if((AGS_WAVE_EDIT_KEY_L_CONTROL & (wave_edit->key_mask)) != 0 || (AGS_WAVE_EDIT_KEY_R_CONTROL & (wave_edit->key_mask)) != 0){
	  if(use_composite_editor){
	    ags_composite_editor_copy(editor);
	  }else{
	    ags_wave_editor_copy(editor);
	  }
	}
      }
      break;
    case GDK_KEY_v:
      {
	/* paste accelerations */
	if((AGS_WAVE_EDIT_KEY_L_CONTROL & (wave_edit->key_mask)) != 0 || (AGS_WAVE_EDIT_KEY_R_CONTROL & (wave_edit->key_mask)) != 0){
	  if(use_composite_editor){
	    ags_composite_editor_paste(editor);
	  }else{
	    ags_wave_editor_paste(editor);
	  }
	}
      }
      break;
    case GDK_KEY_x:
      {
	/* cut accelerations */
	if((AGS_WAVE_EDIT_KEY_L_CONTROL & (wave_edit->key_mask)) != 0 || (AGS_WAVE_EDIT_KEY_R_CONTROL & (wave_edit->key_mask)) != 0){
	  if(use_composite_editor){
	    ags_composite_editor_cut(editor);
	  }else{
	    ags_wave_editor_cut(editor);
	  }
	}
      }
      break;
    case GDK_KEY_m:
      {
	/* meta */
	if((AGS_WAVE_EDIT_KEY_L_CONTROL & (wave_edit->key_mask)) != 0 || (AGS_WAVE_EDIT_KEY_R_CONTROL & (wave_edit->key_mask)) != 0){
	  AgsWaveMeta *wave_meta;

	  wave_meta = NULL;
	  
	  if(use_composite_editor){
	    wave_meta = AGS_COMPOSITE_EDITOR(editor)->wave_edit->edit_meta;
	  }else{
	    wave_meta = AGS_WAVE_EDITOR(editor)->wave_meta;
	  }
	  
	  if((AGS_WAVE_META_ENABLED & (wave_meta->flags)) != 0){
	    wave_meta->flags &= (~AGS_WAVE_META_ENABLED);

	    gtk_widget_hide(wave_meta);
	  }else{
	    wave_meta->flags |= AGS_WAVE_META_ENABLED;

	    gtk_widget_show_all(wave_meta);

	    ags_wave_meta_refresh(wave_meta);
	  }
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
  GtkWidget *editor;
  GtkWidget *toolbar;
  AgsMachine *machine;
  AgsNotebook *channel_selector;
  
  AgsApplicationContext *application_context;
  
  GtkAllocation allocation;

  gboolean use_composite_editor;
  double zoom_factor;
  gint i;
  gboolean retval;

  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));

  machine = NULL;
  
  if(use_composite_editor){
    editor = gtk_widget_get_ancestor(GTK_WIDGET(wave_edit),
				     AGS_TYPE_COMPOSITE_EDITOR);

    channel_selector = AGS_COMPOSITE_EDITOR(editor)->wave_edit->channel_selector;
    
    machine = AGS_COMPOSITE_EDITOR(editor)->selected_machine;  
  }else{
    editor = gtk_widget_get_ancestor(GTK_WIDGET(wave_edit),
				     AGS_TYPE_WAVE_EDITOR);
    
    channel_selector = AGS_WAVE_EDITOR(editor)->notebook;

    machine = AGS_WAVE_EDITOR(editor)->selected_machine;
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
    zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_WAVE_TOOLBAR(AGS_WAVE_EDITOR(editor)->wave_toolbar)->zoom));
  }

  gtk_widget_get_allocation(GTK_WIDGET(wave_edit->drawing_area),
			    &allocation);

  if(machine != NULL){    
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
      
	if(x0_offset / zoom_factor < gtk_range_get_value(GTK_RANGE(wave_edit->hscrollbar))){
	  gtk_range_set_value(GTK_RANGE(wave_edit->hscrollbar),
			      x0_offset / zoom_factor);
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
      
	if((x0_offset + wave_edit->control_width) / zoom_factor > gtk_range_get_value(GTK_RANGE(wave_edit->hscrollbar)) + allocation.width){
	  gtk_range_set_value(GTK_RANGE(wave_edit->hscrollbar),
			      x0_offset / zoom_factor);
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
  GtkWidget *editor;
  
  AgsApplicationContext *application_context;
  
  gboolean use_composite_editor;
  gdouble gui_scale_factor;
  gdouble value;

  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  value = gtk_range_get_value(GTK_RANGE(wave_edit->hscrollbar)) / (gui_scale_factor * 64.0);
  gtk_adjustment_set_value(wave_edit->ruler->adjustment,
			   gui_scale_factor * value);
  gtk_widget_queue_draw((GtkWidget *) wave_edit->ruler);

  if(use_composite_editor){
    editor = gtk_widget_get_ancestor(GTK_WIDGET(wave_edit),
				     AGS_TYPE_COMPOSITE_EDITOR);
    
    gtk_adjustment_set_value(AGS_COMPOSITE_EDITOR(editor)->wave_edit->ruler->adjustment,
			     gui_scale_factor * value);
    gtk_widget_queue_draw((GtkWidget *) AGS_COMPOSITE_EDITOR(editor)->wave_edit->ruler);
  }
  
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) wave_edit->drawing_area);
}

