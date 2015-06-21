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

#include <ags/X/editor/ags_toolbar_callbacks.h>

#include <ags/X/ags_editor.h>

#include <ags/X/editor/ags_note_edit.h>
#include <ags/X/editor/ags_pattern_edit.h>

#include <math.h>

void
ags_toolbar_show_callback(GtkWidget *widget, AgsToolbar *toolbar)
{
  ags_toolbar_show(widget);
}

void
ags_toolbar_position_callback(GtkToggleButton *toggle_button, AgsToolbar *toolbar)
{
  AgsEditor *editor;

  editor = (AgsEditor *) gtk_widget_get_ancestor(GTK_WIDGET(toolbar), AGS_TYPE_EDITOR);

  if(toggle_button == toolbar->selected_edit_mode){
    GdkRectangle *rectangle;
    gint width, height;
    
    rectangle = g_new(GdkRectangle, 1);
    rectangle->x = 0;
    rectangle->y = 0;
    
    if(AGS_IS_NOTE_EDIT(editor->edit_widget)){
      /* refresh editor */
      gtk_widget_get_size_request(GTK_WIDGET(AGS_NOTE_EDIT(editor->edit_widget)->drawing_area),
				  &width,
				  &height);
    
      rectangle->width = width;
      rectangle->height = height;

      gdk_window_invalidate_rect(GTK_WIDGET(AGS_NOTE_EDIT(editor->edit_widget)->drawing_area)->window,
				 rectangle,
				 TRUE);
      gdk_window_process_updates(GTK_WIDGET(AGS_NOTE_EDIT(editor->edit_widget)->drawing_area)->window,
				 TRUE);
    }else if(AGS_IS_PATTERN_EDIT(editor->edit_widget)){
      /* refresh editor */
      gtk_widget_get_size_request(GTK_WIDGET(AGS_PATTERN_EDIT(editor->edit_widget)->drawing_area),
				  &width,
				  &height);
    
      rectangle->width = width;
      rectangle->height = height;

      gdk_window_invalidate_rect(GTK_WIDGET(AGS_PATTERN_EDIT(editor->edit_widget)->drawing_area)->window,
				 rectangle,
				 TRUE);
      gdk_window_process_updates(GTK_WIDGET(AGS_PATTERN_EDIT(editor->edit_widget)->drawing_area)->window,
				 TRUE);
    }
    
    g_free(rectangle);
    
    if(!gtk_toggle_button_get_active(toggle_button)){
      gtk_toggle_button_set_active(toggle_button, TRUE);
    }
  }else if(gtk_toggle_button_get_active(toggle_button)){
    GtkToggleButton *old_selected_edit_mode;
    cairo_t *cr;
    
    old_selected_edit_mode = toolbar->selected_edit_mode;
    toolbar->selected_edit_mode = toggle_button;
    gtk_toggle_button_set_active(old_selected_edit_mode, FALSE);

    /* refresh note_edit */
    if(AGS_IS_NOTE_EDIT(editor->edit_widget)){
      cr = gdk_cairo_create(GTK_WIDGET(AGS_NOTE_EDIT(editor->edit_widget)->drawing_area)->window);
    
      ags_note_edit_draw_position(AGS_NOTE_EDIT(editor->edit_widget), cr);
    }else if(AGS_IS_PATTERN_EDIT(editor->edit_widget)){
      cr = gdk_cairo_create(GTK_WIDGET(AGS_PATTERN_EDIT(editor->edit_widget)->drawing_area)->window);
    
      ags_pattern_edit_draw_position(AGS_PATTERN_EDIT(editor->edit_widget), cr);
    }
  }
}

void
ags_toolbar_edit_callback(GtkToggleButton *toggle_button, AgsToolbar *toolbar)
{
  if(toggle_button == toolbar->selected_edit_mode){
    if(!gtk_toggle_button_get_active(toggle_button)){
      gtk_toggle_button_set_active(toggle_button, TRUE);
    }
  }else if(gtk_toggle_button_get_active(toggle_button)){
    GtkToggleButton *old_selected_edit_mode;
    
    old_selected_edit_mode = toolbar->selected_edit_mode;
    toolbar->selected_edit_mode = toggle_button;
    gtk_toggle_button_set_active(old_selected_edit_mode, FALSE);
  }
}

void
ags_toolbar_clear_callback(GtkToggleButton *toggle_button, AgsToolbar *toolbar)
{
  if(toggle_button == toolbar->selected_edit_mode){
    if(!gtk_toggle_button_get_active(toggle_button)){
      gtk_toggle_button_set_active(toggle_button, TRUE);
    }
  }else if(gtk_toggle_button_get_active(toggle_button)){
    GtkToggleButton *old_selected_edit_mode;
    
    old_selected_edit_mode = toolbar->selected_edit_mode;
    toolbar->selected_edit_mode = toggle_button;
    gtk_toggle_button_set_active(old_selected_edit_mode, FALSE);
  }
}

void
ags_toolbar_select_callback(GtkToggleButton *toggle_button, AgsToolbar *toolbar)
{
  if(toggle_button == toolbar->selected_edit_mode){
    if(!gtk_toggle_button_get_active(toggle_button)){
      gtk_toggle_button_set_active(toggle_button, TRUE);
    }
  }else if(gtk_toggle_button_get_active(toggle_button)){
    GtkToggleButton *old_selected_edit_mode;
    
    old_selected_edit_mode = toolbar->selected_edit_mode;
    toolbar->selected_edit_mode = toggle_button;
    gtk_toggle_button_set_active(old_selected_edit_mode, FALSE);
  }
}

void
ags_toolbar_copy_or_cut_callback(GtkWidget *widget, AgsToolbar *toolbar)
{
  AgsMachine *machine;
  AgsEditor *editor;

  AgsNotation *notation;

  GList *list_notation;
  xmlDocPtr clipboard;
  xmlNodePtr audio_node, notation_node;

  xmlChar *buffer;
  int size;
  gint i;
  gint selected_channel;

  /* add notation to root node */
  editor = AGS_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(toolbar), AGS_TYPE_EDITOR));

  if((machine = editor->selected_machine) != NULL){
    if(widget == (GtkWidget *) toolbar->copy){
      ags_editor_copy(editor);
    }else{
      ags_editor_cut(editor);
    }
  }
}

void
ags_toolbar_paste_callback(GtkWidget *widget, AgsToolbar *toolbar)
{
  AgsEditor *editor;

  editor = (AgsEditor *) gtk_widget_get_ancestor(GTK_WIDGET(toolbar), AGS_TYPE_EDITOR);

  ags_editor_paste(editor);
}

void
ags_toolbar_zoom_callback(GtkComboBox *combo_box, AgsToolbar *toolbar)
{
  AgsEditor *editor;
  GtkWidget *widget;
  GtkAdjustment *adjustment;
  double zoom, zoom_old;
  double tact_factor, zoom_factor;
  double tact;
  gdouble old_upper, new_upper;
  gdouble position;
  guint history;

  editor = (AgsEditor *) gtk_widget_get_ancestor((GtkWidget *) toolbar, AGS_TYPE_EDITOR);

  history = gtk_combo_box_get_active(combo_box);

  zoom = exp2((double) history - 2.0);
  zoom_old = exp2((double) toolbar->zoom_history - 2.0);

  zoom_factor = 0.25;

  tact_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) editor->toolbar->zoom));
  tact = exp2((double) gtk_combo_box_get_active((GtkComboBox *) editor->toolbar->zoom) - 2.0);

  toolbar->zoom_history = history;

  if(AGS_IS_NOTE_EDIT(editor->edit_widget)){
    position = GTK_RANGE(AGS_NOTE_EDIT(editor->edit_widget)->hscrollbar)->adjustment->value;
    old_upper = GTK_RANGE(AGS_NOTE_EDIT(editor->edit_widget)->hscrollbar)->adjustment->upper;
  
    AGS_NOTE_EDIT(editor->edit_widget)->flags |= AGS_NOTE_EDIT_RESETING_HORIZONTALLY;
    ags_note_edit_reset_horizontally(AGS_NOTE_EDIT(editor->edit_widget), AGS_NOTE_EDIT_RESET_HSCROLLBAR |
				     AGS_NOTE_EDIT_RESET_WIDTH);
    AGS_NOTE_EDIT(editor->edit_widget)->flags &= (~AGS_NOTE_EDIT_RESETING_HORIZONTALLY);

    new_upper = GTK_RANGE(AGS_NOTE_EDIT(editor->edit_widget)->hscrollbar)->adjustment->upper;
  
    gtk_adjustment_set_value(GTK_RANGE(AGS_NOTE_EDIT(editor->edit_widget)->hscrollbar)->adjustment,
			     position / old_upper * new_upper);
  }else if(AGS_IS_PATTERN_EDIT(editor->edit_widget)){
    position = GTK_RANGE(AGS_PATTERN_EDIT(editor->edit_widget)->hscrollbar)->adjustment->value;
    old_upper = GTK_RANGE(AGS_PATTERN_EDIT(editor->edit_widget)->hscrollbar)->adjustment->upper;
  
    AGS_PATTERN_EDIT(editor->edit_widget)->flags |= AGS_PATTERN_EDIT_RESETING_HORIZONTALLY;
    ags_pattern_edit_reset_horizontally(AGS_PATTERN_EDIT(editor->edit_widget), AGS_PATTERN_EDIT_RESET_HSCROLLBAR |
				     AGS_PATTERN_EDIT_RESET_WIDTH);
    AGS_PATTERN_EDIT(editor->edit_widget)->flags &= (~AGS_PATTERN_EDIT_RESETING_HORIZONTALLY);

    new_upper = GTK_RANGE(AGS_PATTERN_EDIT(editor->edit_widget)->hscrollbar)->adjustment->upper;
  
    gtk_adjustment_set_value(GTK_RANGE(AGS_PATTERN_EDIT(editor->edit_widget)->hscrollbar)->adjustment,
			     position / old_upper * new_upper);
  }
}

void
ags_toolbar_mode_callback(GtkWidget *widget, AgsToolbar *toolbar)
{
  AgsEditor *editor;
  AgsNotebook *notebook;
  GList *list;
  gint history;

  /* retrieve some variables */
  editor = (AgsEditor *) gtk_widget_get_ancestor(GTK_WIDGET(toolbar),
						 AGS_TYPE_EDITOR);

  notebook = editor->notebook;

  /* toggle AgsNotebook */
  history = gtk_combo_box_get_active((GtkComboBox *) toolbar->mode);

  switch(history){
  case 0:
    {
      list = g_list_reverse(notebook->tabs);

      if(list != NULL){
	gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(list->data)->toggle,
				     TRUE);

	list = list->next;
      }

      while(list != NULL){
	gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(list->data)->toggle,
				     FALSE);	

	list = list->next;
      }
    }
    break;
  case 1:
    {
      //NOTE: you're on your own
    }
    break;
  case 2:
    {
      list = g_list_reverse(notebook->tabs);

      while(list != NULL){
	gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(list->data)->toggle,
				     TRUE);

	list = list->next;
      }
    }
    break;
  default:
    g_message("unknown editor mode\0");
  }
}
