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

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_wave_editor.h>

#include <ags/X/editor/ags_wave_toolbar.h>

#include <ags/X/machine/ags_waverec.h>

#include <ags/X/thread/ags_gui_thread.h>

#include <math.h>
#include <gdk/gdkkeysyms.h>

void
ags_wave_edit_set_audio_channels_callback(AgsAudio *audio,
					  guint audio_channels, guint audio_channels_old,
					  AgsWaveEdit *wave_edit)
{
  AgsWindow *window;
  AgsWaveEditor *wave_editor;
  AgsNotebook *notebook;

  GList *list;

  guint pads;
  guint i, j;

  window = (AgsWindow *) AGS_WAVE_WINDOW(gtk_widget_get_toplevel((GtkWidget *) wave_edit))->parent_window;
  
  wave_editor = (AgsWaveEditor *) gtk_widget_get_ancestor(GTK_WIDGET(wave_edit),
							  AGS_TYPE_WAVE_EDITOR);

  editor_child = NULL;
  list = wave_editor->wave_editor_child;
  
  while(list != NULL){
    if(AGS_WAVE_EDITOR_CHILD(list->data)->wave_edit == (GtkWidget *) wave_edit){
      editor_child = AGS_WAVE_EDITOR_CHILD(list->data);

      notebook = editor_child->notebook;

      pads = audio->output_pads;

      break;
    }
    
    list = list->next;
  }

  if(editor_child == NULL){    
    return;
  }

  if(audio_channels_old < audio_channels){
    for(j = 0; j < pads; j++){
      for(i = audio_channels_old; i < audio_channels; i++){
	ags_notebook_insert_tab(notebook,
				j * audio_channels + i);
      }
    }
    
    gtk_widget_show_all((GtkWidget *) notebook);
  }else if(audio_channels_old > audio_channels){
    for(j = 0; j < pads; j++){
      for(i = audio_channels; i < audio_channels_old; i++){
	ags_notebook_remove_tab(notebook,
				pads * audio_channels);
      }
    }
  }
}

void
ags_wave_edit_set_pads_callback(AgsAudio *audio,
				GType channel_type,
				guint pads, guint pads_old,
				AgsWaveEdit *wave_edit)
{
  AgsWindow *window;
  AgsWaveEditor *wave_editor;
  AgsWaveEditorChild *editor_child;
  AgsNotebook *notebook;

  GList *list;

  guint audio_channels;
  guint i, j;
  
  window = (AgsWindow *) AGS_WAVE_WINDOW(gtk_widget_get_toplevel((GtkWidget *) wave_edit))->parent_window;

  wave_editor = (AgsWaveEditor *) gtk_widget_get_ancestor(GTK_WIDGET(wave_edit),
							  AGS_TYPE_WAVE_EDITOR);

  editor_child = NULL;
  list = wave_editor->wave_editor_child;

  audio_channels = audio->audio_channels;
  
  while(list != NULL){
    if(AGS_WAVE_EDITOR_CHILD(list->data)->output_wave_edit == (GtkWidget *) wave_edit){
      editor_child = AGS_WAVE_EDITOR_CHILD(list->data);
      
      notebook = editor_child->output_notebook;
      
      break;
    }
    
    list = list->next;
  }

  if(editor_child == NULL){    
    return;
  }

  if(pads_old < pads){
    for(j = pads_old; j < pads; j++){
      for(i = 0; i < audio_channels; i++){
	ags_notebook_insert_tab(notebook,
				j * audio_channels + i);
      }
    }
    
    gtk_widget_show_all((GtkWidget *) notebook);
  }else if(pads_old > pads){
    for(j = pads; j < pads_old; j++){
      for(i = 0; i < audio_channels; i++){
	ags_notebook_remove_tab(notebook,
				j * audio_channels + i);
      }
    }
  }
}

gboolean
ags_wave_edit_drawing_area_expose_event(GtkWidget *widget, GdkEventExpose *event,
					AgsWaveEdit *wave_edit)
{
  cairo_t *cr;
  
  cr = gdk_cairo_create(widget->window);
  cairo_push_group(cr);
    
  ags_wave_edit_paint(wave_edit,
		      cr);

  cairo_pop_group_to_source(cr);
  cairo_paint(cr);
  
  cairo_surface_mark_dirty(cairo_get_target(cr));
  cairo_destroy(cr);

  return(TRUE);
}

gboolean
ags_wave_edit_drawing_area_configure_event(GtkWidget *widget, GdkEventConfigure *event,
					   AgsWaveEdit *wave_edit)
{
  cairo_t *cr;
  
  wave_edit->flags |= AGS_WAVE_EDIT_RESETING_VERTICALLY;
  ags_wave_edit_reset_vertically(wave_edit,
				 AGS_WAVE_EDIT_RESET_VSCROLLBAR);
  wave_edit->flags &= (~AGS_WAVE_EDIT_RESETING_VERTICALLY);

  wave_edit->flags |= AGS_WAVE_EDIT_RESETING_HORIZONTALLY;
  ags_wave_edit_reset_horizontally(wave_edit,
				   AGS_WAVE_EDIT_RESET_HSCROLLBAR |
				   AGS_WAVE_EDIT_RESET_WIDTH);
  wave_edit->flags &= (~AGS_WAVE_EDIT_RESETING_HORIZONTALLY);  

  cr = gdk_cairo_create(widget->window);
  cairo_push_group(cr);
  
  ags_wave_edit_paint(wave_edit,
		      cr);
  
  cairo_pop_group_to_source(cr);
  cairo_paint(cr);
  
  cairo_surface_mark_dirty(cairo_get_target(cr));
  cairo_destroy(cr);
  
  return(FALSE);
}

gboolean
ags_wave_edit_drawing_area_button_press_event(GtkWidget *widget, GdkEventButton *event,
					      AgsWaveEdit *wave_edit)
{
  AgsWaveEditor *wave_editor;
  AgsWaveToolbar *wave_toolbar;

  AgsMutexManager *mutex_manager;

  double tact_factor, zoom_factor;
  double tact;
  
  cairo_t *cr;
  
  guint x, y;
  
  wave_editor = (AgsWaveEditor *) gtk_widget_get_ancestor(GTK_WIDGET(wave_edit),
							  AGS_TYPE_WAVE_EDITOR);
  gtk_widget_grab_focus((GtkWidget *) wave_edit->drawing_area);

  if(wave_editor->selected_machine != NULL &&
     event->button == 1){
    wave_toolbar = wave_editor->wave_toolbar;
    
    zoom_factor = 1.0 / 4.0;

    tact_factor = exp2(6.0 - (double) gtk_combo_box_get_active(wave_toolbar->zoom));
    tact = exp2((double) gtk_combo_box_get_active(wave_toolbar->zoom) - 2.0);

    x = (guint) (GTK_RANGE(wave_edit->hscrollbar)->adjustment->value + (guint) event->x) / tact;
    y = (guint) GTK_RANGE(wave_edit->vscrollbar)->adjustment->value + (guint) event->y;

    /* do it so */    
    if(wave_toolbar->selected_edit_mode == wave_toolbar->position){
      wave_edit->flags |= AGS_WAVE_EDIT_POSITION_CURSOR;

      wave_edit->edit_x = x;
      wave_edit->edit_y = y;
      
      gtk_widget_queue_draw((GtkWidget *) wave_edit->drawing_area);
    }else if(wave_toolbar->selected_edit_mode == wave_toolbar->select){
      wave_edit->flags |= AGS_WAVE_EDIT_SELECTING_AUDIO_DATA;

      wave_edit->select_x0 = x;
      wave_edit->select_y0 = y;
    }
  }

  return(TRUE);
}

gboolean
ags_wave_edit_drawing_area_button_release_event(GtkWidget *widget, GdkEventButton *event,
						AgsWaveEdit *wave_edit)
{
  AgsWaveEditor *wave_editor;
  AgsWaveToolbar *wave_toolbar;
  AgsNotebook *notebook;

  AgsMutexManager *mutex_manager;

  cairo_t *cr;
  
  double tact_factor, zoom_factor;
  double tact;
  guint x, y;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;

  auto void ags_wave_edit_drawing_area_button_release_event_select(cairo_t *cr);

  void ags_wave_edit_drawing_area_button_release_event_select(cairo_t *cr){
    AgsNotebook *notebook;

    guint x0, x1;
    guint y0, y1;
    
    /* get boundaries */
    if(wave_edit->select_x0 > wave_edit->select_x1){
      x0 = wave_edit->select_x1;
      x1 = wave_edit->select_x0;
    }else{
      x0 = wave_edit->select_x0;
      x1 = wave_edit->select_x1;
    }

    if(wave_edit->select_y0 > wave_edit->select_y1){
      y0 = wave_edit->select_y1;
      y1 = wave_edit->select_y0;
    }else{
      y0 = wave_edit->select_y0;
      y1 = wave_edit->select_y1;
    }

    //TODO:JK: implement me
  }

  wave_editor = (AgsWaveEditor *) gtk_widget_get_ancestor(GTK_WIDGET(wave_edit),
							  AGS_TYPE_WAVE_EDITOR);

  if(wave_editor->selected_machine != NULL &&
     event->button == 1){
    wave_toolbar = wave_editor->wave_toolbar;
    
    zoom_factor = 1.0 / 4.0;

    tact_factor = exp2(6.0 - (double) gtk_combo_box_get_active(wave_toolbar->zoom));
    tact = exp2((double) gtk_combo_box_get_active(wave_toolbar->zoom) - 2.0);

    x = (guint) (GTK_RANGE(wave_edit->hscrollbar)->adjustment->value + (guint) event->x) / tact;
    y = (guint) GTK_RANGE(wave_edit->vscrollbar)->adjustment->value + (guint) event->y;

    /* get mutex manager and application mutex */
    mutex_manager = ags_mutex_manager_get_instance();
    application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

    /* get audio mutex */
    pthread_mutex_lock(application_mutex);

    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) wave_editor->selected_machine->audio);
  
    pthread_mutex_unlock(application_mutex);

    /* do it so */
    if((AGS_WAVE_EDIT_POSITION_CURSOR & (wave_edit->flags)) != 0){
      wave_edit->flags &= (~AGS_WAVE_EDIT_POSITION_CURSOR);

      //TODO:JK: implement me
    }else if((AGS_WAVE_EDIT_SELECTING_AUDIO_DATA & (wave_edit->flags)) != 0){
      wave_edit->flags &= (~AGS_WAVE_EDIT_SELECTING_AUDIO_DATA);

      wave_edit->select_x1 = x;
      wave_edit->select_y1 = y;

      /* select acceleration */
      pthread_mutex_lock(audio_mutex);
      
      ags_wave_edit_drawing_area_button_release_event_select(cr);

      pthread_mutex_unlock(audio_mutex);

      /* redraw */
      gtk_widget_queue_draw((GtkWidget *) wave_edit->drawing_area);
    }
  }
  
  return(FALSE);
}

gboolean
ags_wave_edit_drawing_area_motion_notify_event(GtkWidget *widget, GdkEventMotion *event,
					       AgsWaveEdit *wave_edit)
{
  AgsWaveEditor *wave_editor;
  AgsWaveToolbar *wave_toolbar;
  
  cairo_t *cr;

  double tact_factor, zoom_factor;
  double tact;
  guint x, y;
  
  auto void ags_wave_edit_drawing_area_motion_notify_event_draw_selection(cairo_t *cr);

  void ags_wave_edit_drawing_area_motion_notify_event_draw_selection(cairo_t *cr){
    GtkAllocation allocation;
    guint x0_offset, x1_offset, y0_offset, y1_offset;
    guint x0, x1, y0, y1, width, height;
    guint x0_viewport, x1_viewport, y0_viewport, y1_viewport;

    /* get viewport */
    gtk_widget_get_allocation((GtkWidget *) widget, &allocation);

    x0_viewport = (guint) GTK_RANGE(wave_edit->hscrollbar)->adjustment->value / tact;
    x1_viewport = (guint) (GTK_RANGE(wave_edit->hscrollbar)->adjustment->value + allocation.width) / tact;

    y0_viewport = (guint) GTK_RANGE(wave_edit->vscrollbar)->adjustment->value;
    y1_viewport = (guint) GTK_RANGE(wave_edit->vscrollbar)->adjustment->value + allocation.height;

    /* get real size and offset */
    x0 = wave_edit->select_x0;

    if(event->x >= 0.0){
      x1 = x0_viewport + (event->x / tact);
    }else{
      x1 = x0_viewport;
    }
    
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
      //      x0 = 0;
      //      width = x1_offset - x0_viewport;
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
    y0 = wave_edit->select_y0;

    if(event->y >= 0.0){
      y1 = y0_viewport + event->y;
    }else{
      y1 = y0_viewport;
    }
    
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
      //      y0 = 0;
      //      height = y1_offset - y0_viewport;
      y0 -= y0_viewport;
      height = y1 - y0;
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
    cairo_rectangle(cr, (double) x0 * tact, (double) y0, (double) width * tact, (double) height);
    cairo_fill(cr);
  }

  wave_editor = (AgsWaveEditor *) gtk_widget_get_ancestor(GTK_WIDGET(wave_edit),
							  AGS_TYPE_WAVE_EDITOR);

  if(wave_editor->selected_machine != NULL){
    wave_toolbar = wave_editor->wave_toolbar;

    zoom_factor = 1.0 / 4.0;

    tact_factor = exp2(6.0 - (double) gtk_combo_box_get_active(wave_toolbar->zoom));
    tact = exp2((double) gtk_combo_box_get_active(wave_toolbar->zoom) - 2.0);

    if(event->x >= 0.0){
      x = (guint) event->x;
    }else{
      x = 0;
    }

    if(event->y >= 0.0){
      y = (guint) event->y;
    }else{
      y = 0;
    }
    
    if((AGS_WAVE_EDIT_POSITION_CURSOR & (wave_edit->flags)) != 0){
      //TODO:JK: implement me
    }else if((AGS_WAVE_EDIT_SELECTING_AUDIO_DATA & (wave_edit->flags)) != 0){
      wave_edit->select_x1 = x / tact;
      wave_edit->select_y1 = GTK_RANGE(wave_edit->vscrollbar)->adjustment->value + y;

      cr = gdk_cairo_create(widget->window);
      cairo_push_group(cr);
    
      ags_wave_edit_paint(wave_edit,
			  cr);
      ags_wave_edit_drawing_area_motion_notify_event_draw_selection(cr);

      cairo_pop_group_to_source(cr);
      cairo_paint(cr);
      
      cairo_surface_mark_dirty(cairo_get_target(cr));
      cairo_destroy(cr);
    }
  }
  
  return(FALSE);
}

gboolean
ags_wave_edit_drawing_area_key_press_event(GtkWidget *widget, GdkEventKey *event,
					   AgsWaveEdit *wave_edit)
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
	/* select all notes */
	if((AGS_WAVE_EDIT_KEY_L_CONTROL & (wave_edit->key_mask)) != 0 || (AGS_WAVE_EDIT_KEY_R_CONTROL & (wave_edit->key_mask)) != 0){
	  ags_wave_editor_select_all(wave_editor);
	}
      }
      break;
    case GDK_KEY_c:
      {
	/* copy notes */
	if((AGS_WAVE_EDIT_KEY_L_CONTROL & (wave_edit->key_mask)) != 0 || (AGS_WAVE_EDIT_KEY_R_CONTROL & (wave_edit->key_mask)) != 0){
	  ags_wave_editor_copy(wave_editor);
	}
      }
      break;
    case GDK_KEY_v:
      {
	/* paste notes */
	if((AGS_WAVE_EDIT_KEY_L_CONTROL & (wave_edit->key_mask)) != 0 || (AGS_WAVE_EDIT_KEY_R_CONTROL & (wave_edit->key_mask)) != 0){
	  ags_wave_editor_paste(wave_editor);
	}
      }
      break;
    case GDK_KEY_x:
      {
	/* cut notes */
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
ags_wave_edit_drawing_area_key_release_event(GtkWidget *widget, GdkEventKey *event,
					     AgsWaveEdit *wave_edit)
{
  AgsWaveEditor *wave_editor;
  AgsMachine *machine;

  AgsMutexManager *mutex_manager;

  GList *list, *list_start;
  
  double tact_factor, zoom_factor;
  double tact;
  guint x, y;
  gboolean retval;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
    
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
  
  if(machine == NULL){
    return(retval);
  }

  zoom_factor = 1.0 / 4.0;
  
  tact_factor = exp2(6.0 - (double) gtk_combo_box_get_active(wave_editor->wave_toolbar->zoom));
  tact = exp2((double) gtk_combo_box_get_active(wave_editor->wave_toolbar->zoom) - 2.0);

  /* get mutex manager and application mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) wave_editor->selected_machine->audio);
  
  pthread_mutex_unlock(application_mutex);

  /* do it so */
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
      if((AGS_WAVE_EDIT_KEY_L_SHIFT & wave_edit->key_mask) != 0 ||
	 (AGS_WAVE_EDIT_KEY_R_SHIFT & wave_edit->key_mask) != 0){
	if(wave_edit->edit_x > (1.0 / tact)){
	  x = wave_edit->edit_x - (1.0 / tact);
	}else{
	  x = 0;
	}
	
	y = wave_edit->edit_y;
      }else{
	if(wave_edit->edit_x > (AGS_WAVE_EDIT_DEFAULT_WIDTH / tact)){
	  x = wave_edit->edit_x - (AGS_WAVE_EDIT_DEFAULT_WIDTH / tact);
	}else{
	  x = 0;
	}

	y = wave_edit->edit_y;
      }

      //TODO:JK: implement me
      
      gtk_widget_queue_draw((GtkWidget *) wave_edit->drawing_area);
    }
    break;
  case GDK_KEY_Right:
  case GDK_KEY_rightarrow:
    {
      if((AGS_WAVE_EDIT_KEY_L_SHIFT & wave_edit->key_mask) != 0 ||
	 (AGS_WAVE_EDIT_KEY_R_SHIFT & wave_edit->key_mask) != 0){
	x = wave_edit->edit_x + (1.0 / tact);
	y = wave_edit->edit_y;
      }else{
	x = wave_edit->edit_x + (AGS_WAVE_EDIT_DEFAULT_WIDTH / tact);
	y = wave_edit->edit_y;
      }

      //TODO:JK: implement me
      
      gtk_widget_queue_draw((GtkWidget *) wave_edit->drawing_area);
    }
    break;
  }
  
  return(retval);
}

void
ags_wave_edit_vscrollbar_value_changed(GtkRange *range, AgsWaveEdit *wave_edit)
{
  if((AGS_WAVE_EDIT_RESETING_VERTICALLY & wave_edit->flags) != 0){
    return;
  }

  wave_edit->flags |= AGS_WAVE_EDIT_RESETING_VERTICALLY;
  ags_wave_edit_reset_vertically(wave_edit, 0);
  wave_edit->flags &= (~AGS_WAVE_EDIT_RESETING_VERTICALLY);
}

void
ags_wave_edit_hscrollbar_value_changed(GtkRange *range, AgsWaveEdit *wave_edit)
{
  AgsWaveEditor *wave_editor;

  double tact_factor, zoom_factor;
  double tact;

  if((AGS_WAVE_EDIT_RESETING_HORIZONTALLY & wave_edit->flags) != 0){
    return;
  }

  wave_editor = (AgsWaveEditor *) gtk_widget_get_ancestor(GTK_WIDGET(wave_edit),
							  AGS_TYPE_WAVE_EDITOR);

  zoom_factor = 0.25;

  tact_factor = exp2(6.0 - (double) gtk_combo_box_get_active(wave_editor->wave_toolbar->zoom));
  tact = exp2((double) gtk_combo_box_get_active(wave_editor->wave_toolbar->zoom) - 2.0);

  //  g_message("%f", GTK_RANGE(wave_edit->hscrollbar)->adjustment->value);
  
  /* reset ruler */
  gtk_adjustment_set_value(wave_edit->ruler->adjustment,
			   GTK_RANGE(wave_edit->hscrollbar)->adjustment->value / AGS_WAVE_EDIT_DEFAULT_WIDTH);
  gtk_widget_queue_draw((GtkWidget *) wave_edit->ruler);

  /* update wave edit */
  wave_edit->flags |= AGS_WAVE_EDIT_RESETING_HORIZONTALLY;
  ags_wave_edit_reset_horizontally(wave_edit, 0);
  wave_edit->flags &= (~AGS_WAVE_EDIT_RESETING_HORIZONTALLY);
}
