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

#define AGS_EDITOR_LINK_INDEX_VBOX "AgsEditorLinkIndexVBox\0"

void
ags_editor_parent_set_callback(GtkWidget  *widget, GtkObject *old_parent, AgsEditor *editor)
{
  if(old_parent != NULL)
    return;
  
  editor->flags |= AGS_EDITOR_RESETING_HORIZONTALLY;
  ags_editor_reset_horizontally(editor, AGS_EDITOR_RESET_HSCROLLBAR);
  editor->flags &= (~AGS_EDITOR_RESETING_HORIZONTALLY);


  /*
  editor->width = (editor->map_width < widget->allocation.width) ? editor->map_width: widget->allocation.width;
  
  editor->control_current.x1 = (editor->width - editor->control_current.x0) % editor->control_current.control_width;

  editor->control_unit.x1 = (editor->width - editor->control_unit.x0) % editor->control_unit.control_width;
  editor->control_unit.stop_x = (editor->width - editor->control_unit.x0 - editor->control_unit.x1) / editor->control_unit.control_width;
*/
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

  if(machine != NULL)
    editor->map_height = machine->audio->input_pads * editor->control_height;

  editor->flags |= AGS_EDITOR_RESETING_VERTICALLY;
  ags_editor_reset_vertically(editor, AGS_EDITOR_RESET_VSCROLLBAR);
  editor->flags &= (~AGS_EDITOR_RESETING_VERTICALLY);

  /*
  GtkWidget *widget;
  GtkAdjustment *adjustment;
  guint height;
  guint i;

  editor->selected = radio_button;


  if(editor->selected != NULL)
    machine = (AgsMachine *) g_object_get_data((GObject *) editor->selected, (char *) g_type_name(AGS_TYPE_MACHINE));
  else
    machine = NULL;

  widget = (GtkWidget *) editor->drawing_area;

  if(machine != NULL){
    editor->map_height = machine->audio->input_pads * editor->control_height;

    /* reset GtkVScrollbar * /
    editor->flags |= AGS_EDITOR_RESET_VSCROLLBAR;
    adjustment = GTK_RANGE(editor->vscrollbar)->adjustment;

    if(editor->map_height > widget->allocation.height){
      height = widget->allocation.height;
      //      gtk_adjustment_set_upper(adjustment, (double) (editor->map_height - height));
      gtk_adjustment_set_upper(adjustment,
			       (gdouble) (editor->map_height - height));

      if(adjustment->value > adjustment->upper)
	gtk_adjustment_set_value(adjustment, adjustment->value);
    }else{
      height = editor->map_height;

      gtk_adjustment_set_upper(adjustment, 0.0);
      gtk_adjustment_set_value(adjustment, 0.0);
    }

    editor->height = height;
    editor->flags &= (~AGS_EDITOR_RESET_VSCROLLBAR);

    editor->y0 = ((guint) round((double) editor->vscrollbar->scrollbar.range.adjustment->value)) % editor->control_height;

    if(editor->y0 != 0){
      editor->y0 = editor->control_height - editor->y0;
    }

    editor->y1 = (editor->height - editor->y0) % editor->control_height;
  }



  ags_editor_draw_segment(editor);
  ags_editor_draw_notation(editor);
*/
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
      if(AGS_IS_PANEL(machine)){
      }else if(AGS_IS_MIXER(machine)){
      }else if(AGS_IS_DRUM(machine)){
	ags_meter_paint(editor->meter);
	ags_editor_draw_segment(editor);
	ags_editor_draw_notation(editor);
      }else if(AGS_IS_MATRIX(machine)){
	ags_meter_paint(editor->meter);
	ags_editor_draw_segment(editor);
	ags_editor_draw_notation(editor);
      }else if(AGS_IS_SYNTH(machine)){
	ags_meter_paint(editor->meter);
	ags_editor_draw_segment(editor);
	ags_editor_draw_notation(editor);
      }else if(AGS_IS_FFPLAYER(machine)){
	ags_meter_paint(editor->meter);
	ags_editor_draw_segment(editor);
	ags_editor_draw_notation(editor);
      }
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

    /*
  if(editor->selected != NULL){
    AgsMachine *machine;
    GtkAdjustment *adjustment;
    guint width, height;

    machine = (AgsMachine *) g_object_get_data((GObject *) editor->selected, g_type_name(AGS_TYPE_MACHINE));

    if(machine != NULL){
      /* reset GtkHScrollbar * /
      editor->flags |= AGS_EDITOR_RESET_HSCROLLBAR;
      adjustment = GTK_RANGE(editor->hscrollbar)->adjustment;

      if(editor->map_width > widget->allocation.width){
	width = widget->allocation.width;
	//	gtk_adjustment_set_upper(adjustment, (double) (editor->map_width - width));
	gtk_adjustment_set_upper(adjustment,
				 (gdouble) (editor->map_width - width));

	if(adjustment->value > adjustment->upper)
	  gtk_adjustment_set_value(adjustment, adjustment->upper);
      }else{
	width = editor->map_width;

	gtk_adjustment_set_upper(adjustment, 0.0);
	gtk_adjustment_set_value(adjustment, 0.0);
      }

      editor->width = width;
      editor->flags &= (~AGS_EDITOR_RESET_HSCROLLBAR);

      /* reset GtkVScrollbar * /
      editor->flags |= AGS_EDITOR_RESET_VSCROLLBAR;
      adjustment = GTK_RANGE(editor->vscrollbar)->adjustment;

      if(editor->map_height > widget->allocation.height){
	height = widget->allocation.height;
	//	gtk_adjustment_set_upper(adjustment, (double) (editor->map_height - height));
	gtk_adjustment_set_upper(adjustment,
				 (gdouble) (editor->map_height - height));

	if(adjustment->value > adjustment->upper)
	  gtk_adjustment_set_value(adjustment, adjustment->value);
      }else{
	height = editor->map_height;
	//	gtk_adjustment_set_upper(adjustment, 0.0);
	gtk_adjustment_set_upper(adjustment,
				 0.0);
	gtk_adjustment_set_value(adjustment, 0.0);
      }

      editor->height = height;
      editor->flags &= (~AGS_EDITOR_RESET_VSCROLLBAR);

      /* reset AgsEditorControlCurrent * /
      if(editor->map_width > editor->width){
	editor->control_current.x0 = ((guint) round((double) adjustment->value) % editor->control_current.control_width);

	if(editor->control_current.x0 != 0)
	  editor->control_current.x0 = editor->control_current.control_width - editor->control_current.x0;

	editor->control_current.x1 = (editor->width - editor->control_current.x0) % editor->control_current.control_width;

	editor->control_current.nth_x = (guint) ceil(adjustment->value / (double) editor->control_current.control_width);
      }else{
	editor->control_current.x0 = 0;
	editor->control_current.x1 = 0;

	editor->control_current.nth_x = 0;
      }

      /* reset AgsEditorControlUnit * /
      if(editor->map_width > editor->width){
	editor->control_unit.x0 = ((guint) round((double) adjustment->value) % editor->control_unit.control_width);

	if(editor->control_unit.x0 != 0)
	  editor->control_unit.x0 = editor->control_unit.control_width - editor->control_unit.x0;

	editor->control_unit.x1 = (editor->width - editor->control_unit.x0) % editor->control_unit.control_width;

	editor->control_unit.nth_x = (guint) ceil(adjustment->value / (double) editor->control_unit.control_width);
	editor->control_unit.stop_x = (editor->width - editor->control_unit.x0 - editor->control_unit.x1) / editor->control_unit.control_width;
      }else{
	editor->control_unit.x0 = 0;
	editor->control_unit.x1 = 0;

	editor->control_unit.nth_x = 0;
	editor->control_unit.stop_x = editor->control_unit.control_width * editor->control_unit.control_count;
      }

      /* refresh display * /
      if(AGS_IS_PANEL(machine)){
      }else if(AGS_IS_MIXER(machine)){
      }else if(AGS_IS_DRUM(machine)){
	ags_meter_paint(editor->meter);
	ags_editor_draw_segment(editor);
	ags_editor_draw_notation(editor);
      }else if(AGS_IS_MATRIX(machine)){
	ags_meter_paint(editor->meter);
	ags_editor_draw_segment(editor);
	ags_editor_draw_notation(editor);
      }else if(AGS_IS_SYNTH(machine)){
	ags_meter_paint(editor->meter);
	ags_editor_draw_segment(editor);
	ags_editor_draw_notation(editor);
      }else if(AGS_IS_FFPLAYER(machine)){
	ags_meter_paint(editor->meter);
	ags_editor_draw_segment(editor);
	ags_editor_draw_notation(editor);
      }
    }
  }
    */
  return(FALSE);
}

gboolean
ags_editor_drawing_area_button_press_event (GtkWidget *widget, GdkEventButton *event, AgsEditor *editor)
{
  AgsMachine *machine;
  void ags_editor_drawing_area_button_press_event_set_control(){
    AgsNote *note;
    guint tact;
    guint offset_x, offset_y;
    guint note_x, note_y;
    double value[2];

    if(editor->control.y0 >= editor->map_height || editor->control.x0 >= editor->map_width)
      return;

    tact = gtk_option_menu_get_history(editor->toolbar->tact);
    tact = (guint) exp2(8 - tact);

    value[0] = (double) round((double) editor->hscrollbar->scrollbar.range.adjustment->value);
    value[1] = (double) round((double) editor->vscrollbar->scrollbar.range.adjustment->value);

    offset_x = (guint) ceil(value[0] / (double) (editor->control_current.control_width));

    if(editor->control.x0 >= editor->control_current.x0)
      note_x = (guint) floor((double) (editor->control.x0 - editor->control_current.x0) / (double) (editor->control_current.control_width));
    else{
      offset_x -= 1;
      note_x = 0;
    }

    offset_y = (guint) ceil(value[1] / (double) (editor->control_height));

    if(editor->control.y0 >= editor->y0)
      note_y = (guint) floor((double) (editor->control.y0 - editor->y0) / (double) (editor->control_height));
    else{
      offset_y -= 1;
      note_y = 0;
    }

    note = &(editor->control.note);
    note->flags = AGS_NOTE_GUI;
    note->x[0] = (guint) (tact * note_x) + (tact * offset_x);
    note->x[1] = (guint) note->x[0] + 1;
    note->y = (guint) note_y + offset_y;

    fprintf(stdout, "x0 = %llu\nx1 = %llu\ny  = %llu\n\n\0", note->x[0], note->x[1], note->y);
  }

  if(editor->selected != NULL &&
     event->button == 1 &&
     (machine = (AgsMachine *) g_object_get_data((GObject *) editor->selected, (char *) g_type_name(AGS_TYPE_MACHINE))) != NULL){
    /* store the events position */
    editor->control.x0 = (guint) event->x;
    editor->control.y0 = (guint) event->y;

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

  return(TRUE);
}

gboolean
ags_editor_drawing_area_button_release_event(GtkWidget *widget, GdkEventButton *event, AgsEditor *editor)
{
  AgsMachine *machine;
  AgsNote *note, *note0;
  double value[2];
  guint tact;
  void ags_editor_drawing_area_button_release_event_set_control(){
    GList *list_notation;
    guint note_x, note_y;
    guint offset_x, offset_y;

    if(editor->control.x0 >= editor->map_width)
      editor->control.x0 = editor->map_width - 1;

    offset_x = (guint) ceil(value[0] / (double) (editor->control_current.control_width));

    if(editor->control.x1 >= editor->control_current.x0)
      note_x = (guint) ceil((double) (editor->control.x1 - editor->control_current.x0) / (double) (editor->control_current.control_width));
    else{
      offset_x -= 1;
      note_x = 0;
    }

    note->x[1] = (guint) (tact * (note_x)) + (tact * offset_x);

    list_notation = machine->audio->notation;

    if(gtk_option_menu_get_history(editor->toolbar->mode) == 0){
      if(gtk_notebook_get_n_pages((GtkNotebook *) editor->notebook) > 0){
	list_notation = g_list_nth(list_notation, gtk_notebook_get_current_page((GtkNotebook *) editor->notebook));

	note0 = (AgsNote *) malloc(sizeof(AgsNote));
	memcpy(note0, note, sizeof(AgsNote));

	ags_notation_add_note(AGS_NOTATION(list_notation->data), note0);
      }
    }else{
      while(list_notation != NULL ){
	note0 = (AgsNote *) malloc(sizeof(AgsNote));
	memcpy(note0, note, sizeof(AgsNote));

	ags_notation_add_note(AGS_NOTATION(list_notation->data), note0);
	list_notation = list_notation->next;
      }
    }

    fprintf(stdout, "x0 = %llu\nx1 = %llu\ny  = %llu\n\n\0", note->x[0], note->x[1], note->y);
  }
  void ags_editor_drawing_area_button_release_event_draw_control(){
    cairo_t *cr;
    guint x, y, width, height;

    widget = (GtkWidget *) editor->drawing_area;
    cr = gdk_cairo_create(widget->window);

    x = (note->x[0] / tact) * editor->control_current.control_width;
    width = (note->x[1] / tact) * editor->control_current.control_width;

    if(x < (guint) value[0]){
      if(width > (guint) value[0]){
	width -= (guint) x;
	x = 0;
      }else{
	return;
      }
    }else if(x < (guint) value[0] + widget->allocation.width){
      width -= x;
      x -= (guint) value[0];
    }else{
      return;
    }

    if(x + width > widget->allocation.width)
      width = widget->allocation.width - x;

    y = note->y * editor->control_height;

    if(y < (guint) value[1]){
      if(y + editor->control_height - editor->control_margin_y < (guint) value[1]){
	return;
      }else{
	if(y + editor->control_margin_y < (guint) value[1]){
	  height = editor->control_height;
	  y = y + editor->control_margin_y - (guint) value[1];
	}else{
	  height = editor->y0;
	  y -= (guint) value[1];
	}
      }
    }else if(y < (guint) value[1] + widget->allocation.height - editor->control_height){
      height = editor->control_height - 2 * editor->control_margin_y;
      y = y - (guint) value[1] + editor->control_margin_y;
    }else{
      if(y > value[1] + widget->allocation.height - editor->y1 + editor->control_margin_y){
	return;
      }else{
	height = editor->y0;
	y = y - (guint) value[1] + editor->control_margin_y;
      }
    }

    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
    cairo_fill(cr);
  }

  if(editor->selected != NULL && event->button == 1){
    editor->control.x1 = (guint) event->x;

    machine = AGS_MACHINE(g_object_get_data((GObject *) editor->selected, (char *) g_type_name(AGS_TYPE_MACHINE)));
    note = &(editor->control.note);

    tact = gtk_option_menu_get_history(editor->toolbar->tact);
    tact = (guint) exp2(8 - tact);

    value[0] = (double) round((double) editor->hscrollbar->scrollbar.range.adjustment->value);
    value[1] = (double) round((double) editor->vscrollbar->scrollbar.range.adjustment->value);

    if(AGS_IS_PANEL(machine)){
    }else if(AGS_IS_MIXER(machine)){
    }else if(AGS_IS_DRUM(machine)){
      ags_editor_drawing_area_button_release_event_set_control();
      ags_editor_drawing_area_button_release_event_draw_control();
    }else if(AGS_IS_MATRIX(machine)){
      ags_editor_drawing_area_button_release_event_set_control();
      ags_editor_drawing_area_button_release_event_draw_control();
    }else if(AGS_IS_FFPLAYER(machine)){
      ags_editor_drawing_area_button_release_event_set_control();
      ags_editor_drawing_area_button_release_event_draw_control();
    }
  }

  return(FALSE);
}

gboolean
ags_editor_drawing_area_motion_notify_event (GtkWidget *widget, GdkEventMotion *event, AgsEditor *editor)
{
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
      gtk_button_set_label(GTK_BUTTON(editor->selected), g_strconcat(G_OBJECT_TYPE_NAME((GObject *) machine0), ": \0", machine0->name, NULL));

      editor->map_height = machine0->audio->input_pads * editor->control_height;
    
      editor->flags |= AGS_EDITOR_RESETING_VERTICALLY;
      ags_editor_reset_vertically(editor, AGS_EDITOR_RESET_VSCROLLBAR);
      editor->flags &= (~AGS_EDITOR_RESETING_VERTICALLY);
      
      editor->flags |= AGS_EDITOR_RESETING_HORIZONTALLY;
      ags_editor_reset_horizontally(editor, AGS_EDITOR_RESET_HSCROLLBAR);
      editor->flags &= (~AGS_EDITOR_RESETING_HORIZONTALLY);  
    }
    /*
    if(machine0 != NULL){
      GtkWidget *widget;
      GtkAdjustment *adjustment;
      guint height;

      widget = (GtkWidget *) editor->drawing_area;
      adjustment = GTK_RANGE(editor->vscrollbar)->adjustment;

      editor->map_height = machine0->audio->input_pads * editor->control_height;
      
      /* reset GtkVScrollbar * /
      editor->flags |= AGS_EDITOR_RESET_VSCROLLBAR;
      adjustment = GTK_RANGE(editor->vscrollbar)->adjustment;

      if(editor->map_height > widget->allocation.height){
	height = widget->allocation.height;
	gtk_adjustment_set_upper(adjustment,
				 (gdouble) (editor->map_height - height));
	// adjustment->upper = (double) (editor->map_height - height);

	//	if(adjustment->value > adjustment->upper)
	  gtk_adjustment_set_value(adjustment, 0.0);
      }else{
	height = editor->map_height;

	gtk_adjustment_set_upper(adjustment, 0.0);
	//	adjustment->upper = 0.0;
	gtk_adjustment_set_value(adjustment, 0.0);
      }

      editor->height = height;
      editor->flags &= (~AGS_EDITOR_RESET_VSCROLLBAR);

      editor->y0 = ((guint) round((double) editor->vscrollbar->scrollbar.range.adjustment->value)) % editor->control_height;

      if(editor->y0 != 0){
	editor->y0 = editor->control_height - editor->y0;
      }

      editor->y1 = (editor->height - editor->y0) % editor->control_height;
      editor->stop_y = editor->nth_y + (editor->height - editor->y0 - editor->y1) / editor->control_height;

      gtk_button_set_label(GTK_BUTTON(editor->selected), g_strconcat(G_OBJECT_TYPE_NAME((GObject *) machine0), ": \0", machine0->name, NULL));

      if(AGS_IS_PANEL(machine0)){
      }else if(AGS_IS_MIXER(machine0)){
      }else if(AGS_IS_DRUM(machine0)){
	ags_meter_paint(editor->meter);
	ags_editor_draw_segment(editor);
	//	ags_editor_draw_notation(editor);
      }else if(AGS_IS_MATRIX(machine0)){
	ags_meter_paint(editor->meter);
	ags_editor_draw_segment(editor);
	//	ags_editor_draw_notation(editor);
      }else if(AGS_IS_SYNTH(machine0)){
	ags_meter_paint(editor->meter);
	ags_editor_draw_segment(editor);
	//	ags_editor_draw_notation(editor);
      }else if(AGS_IS_FFPLAYER(machine0)){
	ags_meter_paint(editor->meter);
	ags_editor_draw_segment(editor);
	//	ags_editor_draw_notation(editor);
      }
    }
*/
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
