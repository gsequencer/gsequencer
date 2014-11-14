/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011, 2014 Joël Krähemann
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

#include <ags/X/editor/ags_note_edit.h>
#include <ags/X/editor/ags_note_edit_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/X/ags_editor.h>

#include <ags/X/editor/ags_note_edit.h>

void ags_note_edit_class_init(AgsNoteEditClass *note_edit);
void ags_note_edit_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_note_edit_init(AgsNoteEdit *note_edit);
void ags_note_edit_connect(AgsConnectable *connectable);
void ags_note_edit_disconnect(AgsConnectable *connectable);

void ags_note_edit_paint(AgsNoteEdit *note_edit);

/**
 * SECTION:ags_note_edit
 * @short_description: edit notes
 * @title: AgsNoteEdit
 * @section_id:
 * @include: ags/X/editor/ags_note_edit.h
 *
 * The #AgsNoteEdit lets you edit notes.
 */

GtkStyle *note_edit_style;

GType
ags_note_edit_get_type(void)
{
  static GType ags_type_note_edit = 0;

  if(!ags_type_note_edit){
    static const GTypeInfo ags_note_edit_info = {
      sizeof (AgsNoteEditClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_note_edit_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsNoteEdit),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_note_edit_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_note_edit_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_note_edit = g_type_register_static(GTK_TYPE_TABLE,
						"AgsNoteEdit\0", &ags_note_edit_info,
						0);
    
    g_type_add_interface_static(ags_type_note_edit,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_note_edit);
}

void
ags_note_edit_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_note_edit_connect;
  connectable->disconnect = ags_note_edit_disconnect;
}

void
ags_note_edit_class_init(AgsNoteEditClass *note_edit)
{
}

void
ags_note_edit_init(AgsNoteEdit *note_edit)
{
  GtkAdjustment *adjustment;

  note_edit->flags = 0;

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, 1.0, 1.0);

  note_edit->ruler = ags_ruler_new();
  gtk_table_attach(GTK_TABLE(note_edit), (GtkWidget *) note_edit->ruler,
		   0, 1, 0, 1,
		   GTK_FILL|GTK_EXPAND, GTK_FILL,
		   0, 0);

  note_edit->drawing_area = (GtkDrawingArea *) gtk_drawing_area_new();
  gtk_widget_set_style((GtkWidget *) note_edit->drawing_area, note_edit_style);
  gtk_widget_set_events(GTK_WIDGET (note_edit->drawing_area), GDK_EXPOSURE_MASK
			| GDK_LEAVE_NOTIFY_MASK
			| GDK_BUTTON_PRESS_MASK
			| GDK_BUTTON_RELEASE_MASK
			| GDK_POINTER_MOTION_MASK
			| GDK_POINTER_MOTION_HINT_MASK
			);
  
  gtk_table_attach(GTK_TABLE(note_edit), (GtkWidget *) note_edit->drawing_area,
		   0, 1, 1, 2,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);

  note_edit->control.note = ags_note_new();

  note_edit->width = 0;
  note_edit->height = 0;
  note_edit->map_width = AGS_NOTE_EDIT_MAX_CONTROLS * 64;
  note_edit->map_height = 78;

  note_edit->control_height = 14;
  note_edit->control_margin_y = 2;

  note_edit->control_width = 16;

  note_edit->y0 = 0;
  note_edit->y1 = 0;

  note_edit->nth_y = 0;
  note_edit->stop_y = 0;

  /* AgsNoteEditControlCurrent is used by ags_note_edit_draw_segment */
  note_edit->control_current.control_count = AGS_NOTE_EDIT_MAX_CONTROLS;
  note_edit->control_current.control_width = 64;

  note_edit->control_current.x0 = 0;
  note_edit->control_current.x1 = 0;

  note_edit->control_current.nth_x = 0;

  /* AgsNoteEditControlUnit is used by ags_note_edit_draw_notation */
  note_edit->control_unit.control_count = 16 * AGS_NOTE_EDIT_MAX_CONTROLS;
  note_edit->control_unit.control_width = 1 * 4;

  note_edit->control_unit.x0 = 0;
  note_edit->control_unit.x1 = 0;

  note_edit->control_unit.nth_x = 0;
  note_edit->control_unit.stop_x = 0;

  /* offset for pasting from clipboard */
  note_edit->selected_x = 0;
  note_edit->selected_y = 0;

  /* GtkScrollbars */
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, 1.0, 1.0);
  note_edit->vscrollbar = (GtkVScrollbar *) gtk_vscrollbar_new(adjustment);
  gtk_table_attach(GTK_TABLE(note_edit), (GtkWidget *) note_edit->vscrollbar,
		   1, 2, 1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, 1.0, 1.0);
  note_edit->hscrollbar = (GtkHScrollbar *) gtk_hscrollbar_new(adjustment);
  gtk_table_attach(GTK_TABLE(note_edit), (GtkWidget *) note_edit->hscrollbar,
		   0, 1, 2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);
}

void
ags_note_edit_connect(AgsConnectable *connectable)
{
  AgsNoteEdit *note_edit;

  note_edit = AGS_NOTE_EDIT(connectable);

  g_signal_connect_after((GObject *) note_edit->drawing_area, "expose_event\0",
			 G_CALLBACK (ags_note_edit_drawing_area_expose_event), (gpointer) note_edit);

  g_signal_connect_after((GObject *) note_edit->drawing_area, "configure_event\0",
			 G_CALLBACK (ags_note_edit_drawing_area_configure_event), (gpointer) note_edit);

  g_signal_connect((GObject *) note_edit->drawing_area, "button_press_event\0",
		   G_CALLBACK (ags_note_edit_drawing_area_button_press_event), (gpointer) note_edit);

  g_signal_connect((GObject *) note_edit->drawing_area, "button_release_event\0",
		   G_CALLBACK (ags_note_edit_drawing_area_button_release_event), (gpointer) note_edit);

  g_signal_connect((GObject *) note_edit->drawing_area, "motion_notify_event\0",
		   G_CALLBACK (ags_note_edit_drawing_area_motion_notify_event), (gpointer) note_edit);

  g_signal_connect_after((GObject *) note_edit->vscrollbar, "value-changed\0",
			 G_CALLBACK (ags_note_edit_vscrollbar_value_changed), (gpointer) note_edit);

  g_signal_connect_after((GObject *) note_edit->hscrollbar, "value-changed\0",
			 G_CALLBACK (ags_note_edit_hscrollbar_value_changed), (gpointer) note_edit);

}

void
ags_note_edit_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

/**
 * ags_note_edit_set_map_height:
 * @note_edit: the #AgsNoteEdit
 * @map_height: the new height
 *
 * Set the map height in pixel.
 *
 * Since: 0.4
 */
void
ags_note_edit_set_map_height(AgsNoteEdit *note_edit, guint map_height)
{
  note_edit->map_height = map_height;
  
  note_edit->flags |= AGS_NOTE_EDIT_RESETING_VERTICALLY;
  ags_note_edit_reset_vertically(note_edit, AGS_NOTE_EDIT_RESET_VSCROLLBAR);
  note_edit->flags &= (~AGS_NOTE_EDIT_RESETING_VERTICALLY);
  
  note_edit->flags |= AGS_NOTE_EDIT_RESETING_HORIZONTALLY;
  ags_note_edit_reset_horizontally(note_edit, AGS_NOTE_EDIT_RESET_HSCROLLBAR);
  note_edit->flags &= (~AGS_NOTE_EDIT_RESETING_HORIZONTALLY);
}

/**
 * ags_note_edit_reset_vertically:
 * @note_edit: the #AgsNoteEdit
 * @flags: the #AgsNoteEditResetFlags
 *
 * Reset @note_edit as configured vertically.
 *
 * Since: 0.4
 */
void
ags_note_edit_reset_vertically(AgsNoteEdit *note_edit, guint flags)
{
  AgsEditor *editor;

  editor = (AgsEditor *) gtk_widget_get_ancestor(GTK_WIDGET(note_edit),
						 AGS_TYPE_EDITOR);

  if(editor->selected_machine != NULL){
    cairo_t *cr;
    gdouble value;

    value = GTK_RANGE(note_edit->vscrollbar)->adjustment->value;

    if((AGS_NOTE_EDIT_RESET_VSCROLLBAR & flags) != 0){
      GtkWidget *widget;
      GtkAdjustment *adjustment;
      guint height;

      widget = GTK_WIDGET(note_edit->drawing_area);
      adjustment = GTK_RANGE(note_edit->vscrollbar)->adjustment;
      
      if(note_edit->map_height > widget->allocation.height){
	height = widget->allocation.height;
	gtk_adjustment_set_upper(adjustment,
				 (gdouble) (note_edit->map_height - height));
	gtk_adjustment_set_value(adjustment, 0.0);
      }else{
	height = note_edit->map_height;
	
	gtk_adjustment_set_upper(adjustment, 0.0);
	gtk_adjustment_set_value(adjustment, 0.0);
      }
      
      note_edit->height = height;
    }

    note_edit->y0 = ((guint) round((double) value)) % note_edit->control_height;

    if(note_edit->y0 != 0){
      note_edit->y0 = note_edit->control_height - note_edit->y0;
    }

    note_edit->y1 = (note_edit->height - note_edit->y0) % note_edit->control_height;

    note_edit->nth_y = (guint) ceil(round((double) value) / (double)(note_edit->control_height));
    note_edit->stop_y = note_edit->nth_y + (note_edit->height - note_edit->y0 - note_edit->y1) / note_edit->control_height;

    /* refresh display */
    if(GTK_WIDGET_VISIBLE(editor)){
      cr = gdk_cairo_create(GTK_WIDGET(note_edit->drawing_area)->window);
      cairo_push_group(cr);

      ags_note_edit_draw_segment(note_edit, cr);
      ags_note_edit_draw_notation(note_edit, cr);

      if(editor->toolbar->selected_edit_mode == editor->toolbar->position){
	ags_note_edit_draw_position(note_edit, cr);
      }

      cairo_pop_group_to_source(cr);
      cairo_paint(cr);

      cairo_destroy(cr);
    }
  }
}

/**
 * ags_note_edit_reset_horizontally:
 * @note_edit: the #AgsNoteEdit
 * @flags: the #AgsNoteEditResetFlags
 *
 * Reset @note_edit as configured horizontally.
 *
 * Since: 0.4
 */
void
ags_note_edit_reset_horizontally(AgsNoteEdit *note_edit, guint flags)
{
  AgsEditor *editor;
  double tact_factor, zoom_factor;
  double tact;

  editor = (AgsEditor *) gtk_widget_get_ancestor(GTK_WIDGET(note_edit),
						 AGS_TYPE_EDITOR);

  zoom_factor = 0.25;

  tact_factor = exp2(8.0 - (double) gtk_combo_box_get_active(editor->toolbar->zoom));
  tact = exp2((double) gtk_combo_box_get_active(editor->toolbar->zoom) - 4.0);

  if((AGS_NOTE_EDIT_RESET_WIDTH & flags) != 0){
    note_edit->control_unit.control_width = (guint) (((double) note_edit->control_width * zoom_factor * tact));

    note_edit->control_current.control_count = (guint) ((double) note_edit->control_unit.control_count * tact);
    note_edit->control_current.control_width = (note_edit->control_width * zoom_factor * tact_factor * tact);

    note_edit->map_width = (guint) ((double) note_edit->control_current.control_count * (double) note_edit->control_current.control_width);
    /* reset ruler */
    note_edit->ruler->factor = tact_factor;
    note_edit->ruler->precision = tact;
    note_edit->ruler->scale_precision = 1.0 / tact;

    gtk_widget_queue_draw(note_edit->ruler);
  }

  if(editor->selected_machine != NULL){
    cairo_t *cr;
    gdouble value;

    value = GTK_RANGE(note_edit->hscrollbar)->adjustment->value;

    if((AGS_NOTE_EDIT_RESET_HSCROLLBAR & flags) != 0){
      GtkWidget *widget;
      GtkAdjustment *adjustment;
      guint width;

      widget = GTK_WIDGET(note_edit->drawing_area);
      adjustment = GTK_RANGE(note_edit->hscrollbar)->adjustment;

      if(note_edit->map_width > widget->allocation.width){
	width = widget->allocation.width;
	//	gtk_adjustment_set_upper(adjustment, (double) (note_edit->map_width - width));
	gtk_adjustment_set_upper(adjustment,
				 (gdouble) (note_edit->map_width - width));
	gtk_adjustment_set_upper(note_edit->ruler->adjustment,
				 (gdouble) (note_edit->map_width - width) / note_edit->control_current.control_width);

	if(adjustment->value > adjustment->upper){
	  gtk_adjustment_set_value(adjustment, adjustment->upper);

	  /* reset ruler */
	  gtk_adjustment_set_value(note_edit->ruler->adjustment, note_edit->ruler->adjustment->upper);
	  gtk_widget_queue_draw(note_edit->ruler);
	}
      }else{
	width = note_edit->map_width;

	gtk_adjustment_set_upper(adjustment, 0.0);
	gtk_adjustment_set_value(adjustment, 0.0);
	
	/* reset ruler */
	gtk_adjustment_set_upper(note_edit->ruler->adjustment, 0.0);
	gtk_adjustment_set_value(note_edit->ruler->adjustment, 0.0);
	gtk_widget_queue_draw(note_edit->ruler);
      }

      note_edit->width = width;
    }

    /* reset AgsNoteEditControlCurrent */
    if(note_edit->map_width > note_edit->width){
      note_edit->control_current.x0 = ((guint) round((double) value)) % note_edit->control_current.control_width;

      if(note_edit->control_current.x0 != 0){
	note_edit->control_current.x0 = note_edit->control_current.control_width - note_edit->control_current.x0;
      }

      note_edit->control_current.x1 = (note_edit->width - note_edit->control_current.x0) % note_edit->control_current.control_width;

      note_edit->control_current.nth_x = (guint) ceil((double)(value) / (double)(note_edit->control_current.control_width));
    }else{
      note_edit->control_current.x0 = 0;
      note_edit->control_current.x1 = 0;
      note_edit->control_current.nth_x = 0;
    }

    /* reset AgsNoteEditControlUnit */
    if(note_edit->map_width > note_edit->width){
      note_edit->control_unit.x0 = ((guint)round((double) value)) % note_edit->control_unit.control_width;

      if(note_edit->control_unit.x0 != 0)
	note_edit->control_unit.x0 = note_edit->control_unit.control_width - note_edit->control_unit.x0;
      
      note_edit->control_unit.x1 = (note_edit->width - note_edit->control_unit.x0) % note_edit->control_unit.control_width;
      
      note_edit->control_unit.nth_x = (guint) ceil(round((double) value) / (double) (note_edit->control_unit.control_width));
      note_edit->control_unit.stop_x = note_edit->control_unit.nth_x + (note_edit->width - note_edit->control_unit.x0 - note_edit->control_unit.x1) / note_edit->control_unit.control_width;
    }else{
      note_edit->control_unit.x0 = 0;
      note_edit->control_unit.x1 = 0;
      note_edit->control_unit.nth_x = 0;
    }

    /* refresh display */
    if(GTK_WIDGET_VISIBLE(editor)){
      gdouble position;
      
      cr = gdk_cairo_create(GTK_WIDGET(note_edit->drawing_area)->window);
      cairo_push_group(cr);

      ags_note_edit_draw_segment(note_edit, cr);
      ags_note_edit_draw_notation(note_edit, cr);

      if(editor->toolbar->selected_edit_mode == editor->toolbar->position){
	ags_note_edit_draw_position(note_edit, cr);
      }

      //TODO:JK: implement me
      //      position = gtk_range_get_value(GTK_RANGE(note_edit->hscrollbar));
      //      position -= floor(position / note_edit->control_current.control_width);
      //      ags_note_edit_draw_scroll(note_edit, cr,
      //				position);

      cairo_pop_group_to_source(cr);
      cairo_paint(cr);
    }
  }
}

/**
 * ags_note_edit_draw_segment:
 * @note_edit: the #AgsNoteEdit
 * @cr: the #cairo_t surface
 *
 * Draws horizontal and vertical lines.
 *
 * Since: 0.4
 */
void
ags_note_edit_draw_segment(AgsNoteEdit *note_edit, cairo_t *cr)
{
  AgsEditor *editor;
  GtkWidget *widget;
  double tact;
  guint i, j;
  guint j_set;

  widget = (GtkWidget *) note_edit->drawing_area;

  editor = (AgsEditor *) gtk_widget_get_ancestor(GTK_WIDGET(note_edit),
						 AGS_TYPE_EDITOR);

  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
  cairo_rectangle(cr, 0.0, 0.0, (double) widget->allocation.width, (double) widget->allocation.height);
  cairo_fill(cr);

  cairo_set_line_width(cr, 1.0);

  cairo_set_source_rgb(cr, 0.8, 0.8, 0.8);

  for(i = note_edit->y0 ; i < note_edit->height;){
    cairo_move_to(cr, 0.0, (double) i);
    cairo_line_to(cr, (double) note_edit->width, (double) i);
    cairo_stroke(cr);

    i += note_edit->control_height;
  }

  cairo_move_to(cr, 0.0, (double) i);
  cairo_line_to(cr, (double) note_edit->width, (double) i);
  cairo_stroke(cr);

  tact = exp2((double) gtk_combo_box_get_active(editor->toolbar->zoom) - 4.0);

  i = note_edit->control_current.x0;
  
  if(i < note_edit->width &&
     tact > 1.0 ){
    j_set = note_edit->control_current.nth_x % ((guint) tact);
    cairo_set_source_rgb(cr, 0.6, 0.6, 0.6);

    if(j_set != 0){
      j = j_set;
      goto ags_note_edit_draw_segment0;
    }
  }

  for(; i < note_edit->width; ){
    cairo_set_source_rgb(cr, 1.0, 1.0, 0.0);
    
    cairo_move_to(cr, (double) i, 0.0);
    cairo_line_to(cr, (double) i, (double) note_edit->height);
    cairo_stroke(cr);
    
    i += note_edit->control_current.control_width;
    
    cairo_set_source_rgb(cr, 0.6, 0.6, 0.6);
    
    for(j = 1; i < note_edit->width && j < tact; j++){
    ags_note_edit_draw_segment0:
      cairo_move_to(cr, (double) i, 0.0);
      cairo_line_to(cr, (double) i, (double) note_edit->height);
      cairo_stroke(cr);
      
      i += note_edit->control_current.control_width;
    }
  }
}

/**
 * ags_note_edit_draw_position:
 * @note_edit: the #AgsNoteEdit
 * @cr: the #cairo_t surface
 *
 * Draws the cursor.
 *
 * Since: 0.4
 */
void
ags_note_edit_draw_position(AgsNoteEdit *note_edit, cairo_t *cr)
{
  guint selected_x, selected_y;
  guint x_offset[2], y_offset[2];
  guint x, y, width, height;
  gint size_width, size_height;

  selected_x = note_edit->selected_x * note_edit->control_unit.control_width;
  selected_y = note_edit->selected_y * note_edit->control_height;

  size_width = GTK_WIDGET(note_edit->drawing_area)->allocation.width;
  size_height = GTK_WIDGET(note_edit->drawing_area)->allocation.height;

  x_offset[0] = (guint) gtk_range_get_value(GTK_RANGE(note_edit->hscrollbar));
  x_offset[1] = x_offset[0] + (guint) size_width;

  y_offset[0] = (guint) gtk_range_get_value(GTK_RANGE(note_edit->vscrollbar));
  y_offset[1] = y_offset[0] + (guint) size_height;

  /* calculate horizontally values */
  if(selected_x < x_offset[0]){
    if(selected_x + note_edit->control_current.control_width > x_offset[0]){
      x = 0;
      width = selected_x + note_edit->control_current.control_width - x_offset[0];
    }else{
      return;
    }
  }else if(selected_x > x_offset[1]){
    return;
  }else{
    x = selected_x - x_offset[0];

    if(selected_x + note_edit->control_current.control_width < x_offset[1]){
      width = note_edit->control_current.control_width;
    }else{
      width = x_offset[1] - (selected_x + note_edit->control_current.control_width);
    }
  }

  /* calculate vertically values */
  if(selected_y < y_offset[0]){
    if(selected_y + note_edit->control_height > y_offset[0]){
      y = 0;
      height = selected_y + note_edit->control_height - y_offset[0];
    }else{
      return;
    }
  }else if(selected_y > y_offset[1]){
    return;
  }else{
    y = selected_y - y_offset[0];

    if(selected_y + note_edit->control_height < y_offset[1]){
      height = note_edit->control_height;
    }else{
      height = y_offset[1] - (selected_y + note_edit->control_height);
    }
  }

  /* draw */
  cairo_set_source_rgba(cr, 0.25, 0.5, 1.0, 0.5);
  cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
  cairo_fill(cr);
}

/**
 * ags_note_edit_draw_notation:
 * @note_edit: the #AgsNoteEdit
 * @cr: the #cairo_t surface
 *
 * Draw the #AgsNotation of selected #AgsMachine on @note_edit.
 *
 * Since: 0.4
 */
void
ags_note_edit_draw_notation(AgsNoteEdit *note_edit, cairo_t *cr)
{
  AgsMachine *machine;
  AgsEditor *editor;
  GtkWidget *widget;
  AgsNote *note;
  GList *list_notation, *list_note;
  guint x_offset;
  guint control_height;
  guint x, y, width, height;
  gint selected_channel;
  gint i;

  editor = (AgsEditor *) gtk_widget_get_ancestor(GTK_WIDGET(note_edit),
						 AGS_TYPE_EDITOR);

  if(editor->selected_machine == NULL ||
     (machine = editor->selected_machine) == NULL ||
     machine->audio->notation == NULL)
    return;

  widget = (GtkWidget *) note_edit->drawing_area;

  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);

  i = 0;

  while((selected_channel = ags_notebook_next_active_tab(editor->notebook,
							 i)) != -1){
    list_notation = g_list_nth(machine->audio->notation,
			       selected_channel);
    list_note = AGS_NOTATION(list_notation->data)->notes;

    control_height = note_edit->control_height - 2 * note_edit->control_margin_y;

    x_offset = (guint) GTK_RANGE(note_edit->hscrollbar)->adjustment->value;

    /* draw controls smaller than note_edit->nth_x */
    while(list_note != NULL && (note = (AgsNote *) list_note->data)->x[0] < note_edit->control_unit.nth_x){
      if(note->x[1] >= note_edit->control_unit.nth_x){
	if(note->y >= note_edit->nth_y && note->y <= note_edit->stop_y){
	  x = 0;
	  y = (note->y - note_edit->nth_y) * note_edit->control_height + note_edit->y0 + note_edit->control_margin_y;

	  width = (guint) ((double) note->x[1] * note_edit->control_unit.control_width - (double) x_offset);

	  if(width > widget->allocation.width)
	    width = widget->allocation.width;

	  height = control_height;

	  /* draw note */
	  cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	  cairo_fill(cr);

	  /* check if note is selected */
	  if((AGS_NOTE_IS_SELECTED & (note->flags)) != 0){
	    cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 0.7);

	    cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	    cairo_stroke(cr);

	    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	  }
	}else if(note->y == (note_edit->nth_y - 1) && note_edit->y0 != 0){
	  if(note_edit->y0 > note_edit->control_margin_y){
	    x = 0;
	    width = (guint) ((double) note->x[1] * (double) note_edit->control_unit.control_width - x_offset);

	    if(width > widget->allocation.width)
	      width = widget->allocation.width;

	    if(note_edit->y0 > control_height + note_edit->control_margin_y){
	      y = note_edit->y0 - (control_height + note_edit->control_margin_y);
	      height = control_height;
	    }else{
	      y = 0;
	      height = note_edit->y0 - note_edit->control_margin_y;
	    }

	    /* draw note */
	    cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	    cairo_fill(cr);

	    /* check if note is selected */
	    if((AGS_NOTE_IS_SELECTED & (note->flags)) != 0){
	      cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 0.7);
	    
	      cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	      cairo_stroke(cr);
	    
	      cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	    }
	  }
	}else if(note->y == (note_edit->stop_y + 1) && note_edit->y1 != 0){
	  if(note_edit->y1 > note_edit->control_margin_y){
	    x = 0;
	    width = note->x[1] * note_edit->control_unit.control_width - x_offset;

	    if(width > widget->allocation.width)
	      width = widget->allocation.width;

	    y = (note->y - note_edit->nth_y) * note_edit->control_height + note_edit->control_margin_y;

	    if(note_edit->y1 > control_height + note_edit->control_margin_y){
	      height = control_height;
	    }else{
	      height = note_edit->y1 - note_edit->control_margin_y;
	    }

	    /* draw note */
	    cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	    cairo_fill(cr);
	  
	    /* check if note is selected */
	    if((AGS_NOTE_IS_SELECTED & (note->flags)) != 0){
	      cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 0.7);
	    
	      cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	      cairo_stroke(cr);
	    
	      cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	    }
	  }
	}
      }

      list_note = list_note->next;
    }

    /* draw controls equal or greater than note_edit->nth_x */
    while(list_note != NULL && (note = (AgsNote *) list_note->data)->x[0] <= note_edit->control_unit.stop_x){
      if(note->y >= note_edit->nth_y && note->y <= note_edit->stop_y){
	x = (guint) note->x[0] * note_edit->control_unit.control_width;
	y = (note->y - note_edit->nth_y) * note_edit->control_height +
	  note_edit->y0 +
	  note_edit->control_margin_y;

	width = note->x[1] * note_edit->control_unit.control_width - x;
	x -= x_offset;

	if(x + width > widget->allocation.width)
	  width = widget->allocation.width - x;

	height = control_height;

	/* draw note*/
	cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	cairo_fill(cr);

	/* check if note is selected */
	if((AGS_NOTE_IS_SELECTED & (note->flags)) != 0){
	  cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 0.7);
	
	  cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	  cairo_stroke(cr);
	
	  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	}
      }else if(note->y == (note_edit->nth_y - 1) && note_edit->y0 != 0){
	if(note_edit->y0 > note_edit->control_margin_y){
	  x = note->x[0] * note_edit->control_unit.control_width - x_offset;
	  width = note->x[1] * note_edit->control_unit.control_width - x_offset - x;
      
	  if(x + width > widget->allocation.width)
	    width = widget->allocation.width - x;

	  if(note_edit->y0 > control_height + note_edit->control_margin_y){
	    y = note_edit->y0 - (control_height + note_edit->control_margin_y);
	    height = control_height;
	  }else{
	    y = 0;
	    height = note_edit->y0 - note_edit->control_margin_y;
	  }

	  /* draw note */
	  cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	  cairo_fill(cr);

	  /* check if note is selected */
	  if((AGS_NOTE_IS_SELECTED & (note->flags)) != 0){
	    cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 0.7);

	    cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	    cairo_stroke(cr);

	    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	  }
	}
      }else if(note->y == (note_edit->stop_y + 1) && note_edit->y1 != 0){
	if(note_edit->y1 > note_edit->control_margin_y){
	  x = note->x[0] * note_edit->control_unit.control_width - x_offset;
	  width = note->x[1] * note_edit->control_unit.control_width - x_offset - x;
      
	  if(x + width > widget->allocation.width)
	    width = widget->allocation.width - x;

	  y = (note->y - note_edit->nth_y) * note_edit->control_height + note_edit->control_margin_y;

	  if(note_edit->y1 > control_height + note_edit->control_margin_y){
	    height = control_height;
	  }else{
	    height = note_edit->y1 - note_edit->control_margin_y;
	  }

	  /* draw note */
	  cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	  cairo_fill(cr);

	  /* check if note is selected */
	  if((AGS_NOTE_IS_SELECTED & (note->flags)) != 0){
	    cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 0.7);

	    cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	    cairo_stroke(cr);

	    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	  }
	}
      }

      list_note = list_note->next;
    }

    i++;
  }
}

/**
 * ags_note_edit_draw_scroll:
 * @note_edit: the #AgsNoteEdit
 * @cr: the #cairo_t surface
 * @position: the new position
 *
 * Change visible x-position of @note_edit.
 *
 * Since: 0.4
 */
void
ags_note_edit_draw_scroll(AgsNoteEdit *note_edit, cairo_t *cr,
			  gdouble position)
{
  double x, y;
  double width, height;

  y = 0.0;
  x = (position) - (GTK_RANGE(note_edit->hscrollbar)->adjustment->value * note_edit->control_current.control_width);

  height = (double) GTK_WIDGET(note_edit->drawing_area)->allocation.height;
  width = 3.0;

  /* draw */
  cairo_set_source_rgba(cr, 0.79, 0.0, 1.0, 0.5);
  cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
  cairo_fill(cr);
}

/**
 * ags_note_edit_new:
 *
 * Create a new #AgsNoteEdit.
 *
 * Since: 0.4
 */
AgsNoteEdit*
ags_note_edit_new()
{
  AgsNoteEdit *note_edit;

  note_edit = (AgsNoteEdit *) g_object_new(AGS_TYPE_NOTE_EDIT, NULL);

  return(note_edit);
}

