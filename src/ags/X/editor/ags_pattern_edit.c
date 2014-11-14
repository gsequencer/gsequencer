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

#include <ags/X/editor/ags_pattern_edit.h>
#include <ags/X/editor/ags_pattern_edit_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/X/ags_editor.h>

#include <ags/X/editor/ags_pattern_edit.h>

void ags_pattern_edit_class_init(AgsPatternEditClass *pattern_edit);
void ags_pattern_edit_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_pattern_edit_init(AgsPatternEdit *pattern_edit);
void ags_pattern_edit_connect(AgsConnectable *connectable);
void ags_pattern_edit_disconnect(AgsConnectable *connectable);

void ags_pattern_edit_paint(AgsPatternEdit *pattern_edit);

/**
 * SECTION:ags_pattern_edit
 * @short_description: edit notes
 * @title: AgsPatternEdit
 * @section_id:
 * @include: ags/X/editor/ags_pattern_edit.h
 *
 * The #AgsPatternEdit lets you edit notes.
 */

GtkStyle *pattern_edit_style;

GType
ags_pattern_edit_get_type(void)
{
  static GType ags_type_pattern_edit = 0;

  if(!ags_type_pattern_edit){
    static const GTypeInfo ags_pattern_edit_info = {
      sizeof (AgsPatternEditClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_pattern_edit_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPatternEdit),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_pattern_edit_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_pattern_edit_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_pattern_edit = g_type_register_static(GTK_TYPE_TABLE,
						"AgsPatternEdit\0", &ags_pattern_edit_info,
						0);
    
    g_type_add_interface_static(ags_type_pattern_edit,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_pattern_edit);
}

void
ags_pattern_edit_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_pattern_edit_connect;
  connectable->disconnect = ags_pattern_edit_disconnect;
}

void
ags_pattern_edit_class_init(AgsPatternEditClass *pattern_edit)
{
}

void
ags_pattern_edit_init(AgsPatternEdit *pattern_edit)
{
  GtkAdjustment *adjustment;

  pattern_edit->flags = 0;

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, 1.0, 1.0);

  pattern_edit->ruler = ags_ruler_new();
  gtk_table_attach(GTK_TABLE(pattern_edit), (GtkWidget *) pattern_edit->ruler,
		   0, 1, 0, 1,
		   GTK_FILL|GTK_EXPAND, GTK_FILL,
		   0, 0);

  pattern_edit->drawing_area = (GtkDrawingArea *) gtk_drawing_area_new();
  gtk_widget_set_style((GtkWidget *) pattern_edit->drawing_area, pattern_edit_style);
  gtk_widget_set_events(GTK_WIDGET (pattern_edit->drawing_area), GDK_EXPOSURE_MASK
			| GDK_LEAVE_NOTIFY_MASK
			| GDK_BUTTON_PRESS_MASK
			| GDK_BUTTON_RELEASE_MASK
			| GDK_POINTER_MOTION_MASK
			| GDK_POINTER_MOTION_HINT_MASK
			);
  
  gtk_table_attach(GTK_TABLE(pattern_edit), (GtkWidget *) pattern_edit->drawing_area,
		   0, 1, 1, 2,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);

  pattern_edit->control.note = ags_note_new();

  pattern_edit->width = 0;
  pattern_edit->height = 0;
  pattern_edit->map_width = AGS_PATTERN_EDIT_MAX_CONTROLS * 64;
  pattern_edit->map_height = 78;

  pattern_edit->control_height = 14;
  pattern_edit->control_margin_y = 2;

  pattern_edit->control_width = 16;

  pattern_edit->y0 = 0;
  pattern_edit->y1 = 0;

  pattern_edit->nth_y = 0;
  pattern_edit->stop_y = 0;

  /* AgsPatternEditControlCurrent is used by ags_pattern_edit_draw_segment */
  pattern_edit->control_current.control_count = AGS_PATTERN_EDIT_MAX_CONTROLS;
  pattern_edit->control_current.control_width = 64;

  pattern_edit->control_current.x0 = 0;
  pattern_edit->control_current.x1 = 0;

  pattern_edit->control_current.nth_x = 0;

  /* AgsPatternEditControlUnit is used by ags_pattern_edit_draw_notation */
  pattern_edit->control_unit.control_count = 16 * AGS_PATTERN_EDIT_MAX_CONTROLS;
  pattern_edit->control_unit.control_width = 1 * 4;

  pattern_edit->control_unit.x0 = 0;
  pattern_edit->control_unit.x1 = 0;

  pattern_edit->control_unit.nth_x = 0;
  pattern_edit->control_unit.stop_x = 0;

  /* offset for pasting from clipboard */
  pattern_edit->selected_x = 0;
  pattern_edit->selected_y = 0;

  /* GtkScrollbars */
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, 1.0, 1.0);
  pattern_edit->vscrollbar = (GtkVScrollbar *) gtk_vscrollbar_new(adjustment);
  gtk_table_attach(GTK_TABLE(pattern_edit), (GtkWidget *) pattern_edit->vscrollbar,
		   1, 2, 1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, 1.0, 1.0);
  pattern_edit->hscrollbar = (GtkHScrollbar *) gtk_hscrollbar_new(adjustment);
  gtk_table_attach(GTK_TABLE(pattern_edit), (GtkWidget *) pattern_edit->hscrollbar,
		   0, 1, 2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);
}

void
ags_pattern_edit_connect(AgsConnectable *connectable)
{
  AgsPatternEdit *pattern_edit;

  pattern_edit = AGS_PATTERN_EDIT(connectable);

  g_signal_connect_after((GObject *) pattern_edit->drawing_area, "expose_event\0",
			 G_CALLBACK (ags_pattern_edit_drawing_area_expose_event), (gpointer) pattern_edit);

  g_signal_connect_after((GObject *) pattern_edit->drawing_area, "configure_event\0",
			 G_CALLBACK (ags_pattern_edit_drawing_area_configure_event), (gpointer) pattern_edit);

  g_signal_connect((GObject *) pattern_edit->drawing_area, "button_press_event\0",
		   G_CALLBACK (ags_pattern_edit_drawing_area_button_press_event), (gpointer) pattern_edit);

  g_signal_connect((GObject *) pattern_edit->drawing_area, "button_release_event\0",
		   G_CALLBACK (ags_pattern_edit_drawing_area_button_release_event), (gpointer) pattern_edit);

  g_signal_connect((GObject *) pattern_edit->drawing_area, "motion_notify_event\0",
		   G_CALLBACK (ags_pattern_edit_drawing_area_motion_notify_event), (gpointer) pattern_edit);

  g_signal_connect_after((GObject *) pattern_edit->vscrollbar, "value-changed\0",
			 G_CALLBACK (ags_pattern_edit_vscrollbar_value_changed), (gpointer) pattern_edit);

  g_signal_connect_after((GObject *) pattern_edit->hscrollbar, "value-changed\0",
			 G_CALLBACK (ags_pattern_edit_hscrollbar_value_changed), (gpointer) pattern_edit);
}

void
ags_pattern_edit_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

/**
 * ags_pattern_edit_set_map_height:
 * @pattern_edit: the #AgsPatternEdit
 * @map_height: the new height
 *
 * Set the map height in pixel.
 *
 * Since: 0.4
 */
void
ags_pattern_edit_set_map_height(AgsPatternEdit *pattern_edit, guint map_height)
{
  pattern_edit->map_height = map_height;
  
  pattern_edit->flags |= AGS_PATTERN_EDIT_RESETING_VERTICALLY;
  ags_pattern_edit_reset_vertically(pattern_edit, AGS_PATTERN_EDIT_RESET_VSCROLLBAR);
  pattern_edit->flags &= (~AGS_PATTERN_EDIT_RESETING_VERTICALLY);
  
  pattern_edit->flags |= AGS_PATTERN_EDIT_RESETING_HORIZONTALLY;
  ags_pattern_edit_reset_horizontally(pattern_edit, AGS_PATTERN_EDIT_RESET_HSCROLLBAR);
  pattern_edit->flags &= (~AGS_PATTERN_EDIT_RESETING_HORIZONTALLY);
}

/**
 * ags_pattern_edit_reset_vertically:
 * @pattern_edit: the #AgsPatternEdit
 * @flags: the #AgsPatternEditResetFlags
 *
 * Reset @pattern_edit as configured vertically.
 *
 * Since: 0.4
 */
void
ags_pattern_edit_reset_vertically(AgsPatternEdit *pattern_edit, guint flags)
{
  AgsEditor *editor;

  editor = (AgsEditor *) gtk_widget_get_ancestor(GTK_WIDGET(pattern_edit),
						 AGS_TYPE_EDITOR);

  if(editor->selected_machine != NULL){
    cairo_t *cr;
    gdouble value;

    value = GTK_RANGE(pattern_edit->vscrollbar)->adjustment->value;

    if((AGS_PATTERN_EDIT_RESET_VSCROLLBAR & flags) != 0){
      GtkWidget *widget;
      GtkAdjustment *adjustment;
      guint height;

      widget = GTK_WIDGET(pattern_edit->drawing_area);
      adjustment = GTK_RANGE(pattern_edit->vscrollbar)->adjustment;
      
      if(pattern_edit->map_height > widget->allocation.height){
	height = widget->allocation.height;
	gtk_adjustment_set_upper(adjustment,
				 (gdouble) (pattern_edit->map_height - height));
	gtk_adjustment_set_value(adjustment, 0.0);
      }else{
	height = pattern_edit->map_height;
	
	gtk_adjustment_set_upper(adjustment, 0.0);
	gtk_adjustment_set_value(adjustment, 0.0);
      }
      
      pattern_edit->height = height;
    }

    pattern_edit->y0 = ((guint) round((double) value)) % pattern_edit->control_height;

    if(pattern_edit->y0 != 0){
      pattern_edit->y0 = pattern_edit->control_height - pattern_edit->y0;
    }

    pattern_edit->y1 = (pattern_edit->height - pattern_edit->y0) % pattern_edit->control_height;

    pattern_edit->nth_y = (guint) ceil(round((double) value) / (double)(pattern_edit->control_height));
    pattern_edit->stop_y = pattern_edit->nth_y + (pattern_edit->height - pattern_edit->y0 - pattern_edit->y1) / pattern_edit->control_height;

    /* refresh display */
    if(GTK_WIDGET_VISIBLE(editor)){
      cr = gdk_cairo_create(GTK_WIDGET(pattern_edit->drawing_area)->window);
      cairo_push_group(cr);

      ags_pattern_edit_draw_segment(pattern_edit, cr);
      ags_pattern_edit_draw_notation(pattern_edit, cr);

      if(editor->toolbar->selected_edit_mode == editor->toolbar->position){
	ags_pattern_edit_draw_position(pattern_edit, cr);
      }

      cairo_pop_group_to_source(cr);
      cairo_paint(cr);

      cairo_destroy(cr);
    }
  }
}

/**
 * ags_pattern_edit_reset_horizontally:
 * @pattern_edit: the #AgsPatternEdit
 * @flags: the #AgsPatternEditResetFlags
 *
 * Reset @pattern_edit as configured horizontally.
 *
 * Since: 0.4
 */
void
ags_pattern_edit_reset_horizontally(AgsPatternEdit *pattern_edit, guint flags)
{
  AgsEditor *editor;
  double tact_factor, zoom_factor;
  double tact;

  editor = (AgsEditor *) gtk_widget_get_ancestor(GTK_WIDGET(pattern_edit),
						 AGS_TYPE_EDITOR);

  zoom_factor = 0.25;

  tact_factor = exp2(8.0 - (double) gtk_combo_box_get_active(editor->toolbar->zoom));
  tact = exp2((double) gtk_combo_box_get_active(editor->toolbar->zoom) - 4.0);

  if((AGS_PATTERN_EDIT_RESET_WIDTH & flags) != 0){
    pattern_edit->control_unit.control_width = (guint) (((double) pattern_edit->control_width * zoom_factor * tact));

    pattern_edit->control_current.control_count = (guint) ((double) pattern_edit->control_unit.control_count * tact);
    pattern_edit->control_current.control_width = (pattern_edit->control_width * zoom_factor * tact_factor * tact);

    pattern_edit->map_width = (guint) ((double) pattern_edit->control_current.control_count * (double) pattern_edit->control_current.control_width);
    /* reset ruler */
    pattern_edit->ruler->factor = tact_factor;
    pattern_edit->ruler->precision = tact;
    pattern_edit->ruler->scale_precision = 1.0 / tact;

    gtk_widget_queue_draw(pattern_edit->ruler);
  }

  if(editor->selected_machine != NULL){
    cairo_t *cr;
    gdouble value;

    value = GTK_RANGE(pattern_edit->hscrollbar)->adjustment->value;

    if((AGS_PATTERN_EDIT_RESET_HSCROLLBAR & flags) != 0){
      GtkWidget *widget;
      GtkAdjustment *adjustment;
      guint width;

      widget = GTK_WIDGET(pattern_edit->drawing_area);
      adjustment = GTK_RANGE(pattern_edit->hscrollbar)->adjustment;

      if(pattern_edit->map_width > widget->allocation.width){
	width = widget->allocation.width;
	//	gtk_adjustment_set_upper(adjustment, (double) (pattern_edit->map_width - width));
	gtk_adjustment_set_upper(adjustment,
				 (gdouble) (pattern_edit->map_width - width));
	gtk_adjustment_set_upper(pattern_edit->ruler->adjustment,
				 (gdouble) (pattern_edit->map_width - width) / pattern_edit->control_current.control_width);

	if(adjustment->value > adjustment->upper){
	  gtk_adjustment_set_value(adjustment, adjustment->upper);

	  /* reset ruler */
	  gtk_adjustment_set_value(pattern_edit->ruler->adjustment, pattern_edit->ruler->adjustment->upper);
	  gtk_widget_queue_draw(pattern_edit->ruler);
	}
      }else{
	width = pattern_edit->map_width;

	gtk_adjustment_set_upper(adjustment, 0.0);
	gtk_adjustment_set_value(adjustment, 0.0);
	
	/* reset ruler */
	gtk_adjustment_set_upper(pattern_edit->ruler->adjustment, 0.0);
	gtk_adjustment_set_value(pattern_edit->ruler->adjustment, 0.0);
	gtk_widget_queue_draw(pattern_edit->ruler);
      }

      pattern_edit->width = width;
    }

    /* reset AgsPatternEditControlCurrent */
    if(pattern_edit->map_width > pattern_edit->width){
      pattern_edit->control_current.x0 = ((guint) round((double) value)) % pattern_edit->control_current.control_width;

      if(pattern_edit->control_current.x0 != 0){
	pattern_edit->control_current.x0 = pattern_edit->control_current.control_width - pattern_edit->control_current.x0;
      }

      pattern_edit->control_current.x1 = (pattern_edit->width - pattern_edit->control_current.x0) % pattern_edit->control_current.control_width;

      pattern_edit->control_current.nth_x = (guint) ceil((double)(value) / (double)(pattern_edit->control_current.control_width));
    }else{
      pattern_edit->control_current.x0 = 0;
      pattern_edit->control_current.x1 = 0;
      pattern_edit->control_current.nth_x = 0;
    }

    /* reset AgsPatternEditControlUnit */
    if(pattern_edit->map_width > pattern_edit->width){
      pattern_edit->control_unit.x0 = ((guint)round((double) value)) % pattern_edit->control_unit.control_width;

      if(pattern_edit->control_unit.x0 != 0)
	pattern_edit->control_unit.x0 = pattern_edit->control_unit.control_width - pattern_edit->control_unit.x0;
      
      pattern_edit->control_unit.x1 = (pattern_edit->width - pattern_edit->control_unit.x0) % pattern_edit->control_unit.control_width;
      
      pattern_edit->control_unit.nth_x = (guint) ceil(round((double) value) / (double) (pattern_edit->control_unit.control_width));
      pattern_edit->control_unit.stop_x = pattern_edit->control_unit.nth_x + (pattern_edit->width - pattern_edit->control_unit.x0 - pattern_edit->control_unit.x1) / pattern_edit->control_unit.control_width;
    }else{
      pattern_edit->control_unit.x0 = 0;
      pattern_edit->control_unit.x1 = 0;
      pattern_edit->control_unit.nth_x = 0;
    }

    /* refresh display */
    if(GTK_WIDGET_VISIBLE(editor)){
      gdouble position;
      
      cr = gdk_cairo_create(GTK_WIDGET(pattern_edit->drawing_area)->window);
      cairo_push_group(cr);

      ags_pattern_edit_draw_segment(pattern_edit, cr);
      ags_pattern_edit_draw_notation(pattern_edit, cr);

      if(editor->toolbar->selected_edit_mode == editor->toolbar->position){
	ags_pattern_edit_draw_position(pattern_edit, cr);
      }

      //TODO:JK: implement me
      //      position = gtk_range_get_value(GTK_RANGE(pattern_edit->hscrollbar));
      //      position -= floor(position / pattern_edit->control_current.control_width);
      //      ags_pattern_edit_draw_scroll(pattern_edit, cr,
      //				position);

      cairo_pop_group_to_source(cr);
      cairo_paint(cr);
    }
  }
}

/**
 * ags_pattern_edit_draw_segment:
 * @pattern_edit: the #AgsPatternEdit
 * @cr: the #cairo_t surface
 *
 * Draws horizontal and vertical lines.
 *
 * Since: 0.4
 */
void
ags_pattern_edit_draw_segment(AgsPatternEdit *pattern_edit, cairo_t *cr)
{
  AgsEditor *editor;
  GtkWidget *widget;
  double tact;
  guint i, j;
  guint j_set;

  widget = (GtkWidget *) pattern_edit->drawing_area;

  editor = (AgsEditor *) gtk_widget_get_ancestor(GTK_WIDGET(pattern_edit),
						 AGS_TYPE_EDITOR);

  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
  cairo_rectangle(cr, 0.0, 0.0, (double) widget->allocation.width, (double) widget->allocation.height);
  cairo_fill(cr);

  cairo_set_line_width(cr, 1.0);

  cairo_set_source_rgb(cr, 0.8, 0.8, 0.8);

  for(i = pattern_edit->y0 ; i < pattern_edit->height;){
    cairo_move_to(cr, 0.0, (double) i);
    cairo_line_to(cr, (double) pattern_edit->width, (double) i);
    cairo_stroke(cr);

    i += pattern_edit->control_height;
  }

  cairo_move_to(cr, 0.0, (double) i);
  cairo_line_to(cr, (double) pattern_edit->width, (double) i);
  cairo_stroke(cr);

  tact = exp2((double) gtk_combo_box_get_active(editor->toolbar->zoom) - 4.0);

  i = pattern_edit->control_current.x0;
  
  if(i < pattern_edit->width &&
     tact > 1.0 ){
    j_set = pattern_edit->control_current.nth_x % ((guint) tact);
    cairo_set_source_rgb(cr, 0.6, 0.6, 0.6);

    if(j_set != 0){
      j = j_set;
      goto ags_pattern_edit_draw_segment0;
    }
  }

  for(; i < pattern_edit->width; ){
    cairo_set_source_rgb(cr, 1.0, 1.0, 0.0);
    
    cairo_move_to(cr, (double) i, 0.0);
    cairo_line_to(cr, (double) i, (double) pattern_edit->height);
    cairo_stroke(cr);
    
    i += pattern_edit->control_current.control_width;
    
    cairo_set_source_rgb(cr, 0.6, 0.6, 0.6);
    
    for(j = 1; i < pattern_edit->width && j < tact; j++){
    ags_pattern_edit_draw_segment0:
      cairo_move_to(cr, (double) i, 0.0);
      cairo_line_to(cr, (double) i, (double) pattern_edit->height);
      cairo_stroke(cr);
      
      i += pattern_edit->control_current.control_width;
    }
  }
}

/**
 * ags_pattern_edit_draw_position:
 * @pattern_edit: the #AgsPatternEdit
 * @cr: the #cairo_t surface
 *
 * Draws the cursor.
 *
 * Since: 0.4
 */
void
ags_pattern_edit_draw_position(AgsPatternEdit *pattern_edit, cairo_t *cr)
{
  guint selected_x, selected_y;
  guint x_offset[2], y_offset[2];
  guint x, y, width, height;
  gint size_width, size_height;

  selected_x = pattern_edit->selected_x * pattern_edit->control_unit.control_width;
  selected_y = pattern_edit->selected_y * pattern_edit->control_height;

  size_width = GTK_WIDGET(pattern_edit->drawing_area)->allocation.width;
  size_height = GTK_WIDGET(pattern_edit->drawing_area)->allocation.height;

  x_offset[0] = (guint) gtk_range_get_value(GTK_RANGE(pattern_edit->hscrollbar));
  x_offset[1] = x_offset[0] + (guint) size_width;

  y_offset[0] = (guint) gtk_range_get_value(GTK_RANGE(pattern_edit->vscrollbar));
  y_offset[1] = y_offset[0] + (guint) size_height;

  /* calculate horizontally values */
  if(selected_x < x_offset[0]){
    if(selected_x + pattern_edit->control_current.control_width > x_offset[0]){
      x = 0;
      width = selected_x + pattern_edit->control_current.control_width - x_offset[0];
    }else{
      return;
    }
  }else if(selected_x > x_offset[1]){
    return;
  }else{
    x = selected_x - x_offset[0];

    if(selected_x + pattern_edit->control_current.control_width < x_offset[1]){
      width = pattern_edit->control_current.control_width;
    }else{
      width = x_offset[1] - (selected_x + pattern_edit->control_current.control_width);
    }
  }

  /* calculate vertically values */
  if(selected_y < y_offset[0]){
    if(selected_y + pattern_edit->control_height > y_offset[0]){
      y = 0;
      height = selected_y + pattern_edit->control_height - y_offset[0];
    }else{
      return;
    }
  }else if(selected_y > y_offset[1]){
    return;
  }else{
    y = selected_y - y_offset[0];

    if(selected_y + pattern_edit->control_height < y_offset[1]){
      height = pattern_edit->control_height;
    }else{
      height = y_offset[1] - (selected_y + pattern_edit->control_height);
    }
  }

  /* draw */
  cairo_set_source_rgba(cr, 0.25, 0.5, 1.0, 0.5);
  cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
  cairo_fill(cr);
}

/**
 * ags_pattern_edit_draw_notation:
 * @pattern_edit: the #AgsPatternEdit
 * @cr: the #cairo_t surface
 *
 * Draw the #AgsNotation of selected #AgsMachine on @pattern_edit.
 *
 * Since: 0.4
 */
void
ags_pattern_edit_draw_notation(AgsPatternEdit *pattern_edit, cairo_t *cr)
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

  editor = (AgsEditor *) gtk_widget_get_ancestor(GTK_WIDGET(pattern_edit),
						 AGS_TYPE_EDITOR);

  if(editor->selected_machine == NULL ||
     (machine = editor->selected_machine) == NULL ||
     machine->audio->notation == NULL)
    return;

  widget = (GtkWidget *) pattern_edit->drawing_area;

  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);

  i = 0;

  while((selected_channel = ags_notebook_next_active_tab(editor->notebook,
							 i)) != -1){
    list_notation = g_list_nth(machine->audio->notation,
			       selected_channel);
    list_note = AGS_NOTATION(list_notation->data)->notes;

    control_height = pattern_edit->control_height - 2 * pattern_edit->control_margin_y;

    x_offset = (guint) GTK_RANGE(pattern_edit->hscrollbar)->adjustment->value;

    /* draw controls smaller than pattern_edit->nth_x */
    while(list_note != NULL && (note = (AgsNote *) list_note->data)->x[0] < pattern_edit->control_unit.nth_x){
      if(note->x[1] >= pattern_edit->control_unit.nth_x){
	if(note->y >= pattern_edit->nth_y && note->y <= pattern_edit->stop_y){
	  x = 0;
	  y = (note->y - pattern_edit->nth_y) * pattern_edit->control_height + pattern_edit->y0 + pattern_edit->control_margin_y;

	  width = (guint) ((double) note->x[1] * pattern_edit->control_unit.control_width - (double) x_offset);

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
	}else if(note->y == (pattern_edit->nth_y - 1) && pattern_edit->y0 != 0){
	  if(pattern_edit->y0 > pattern_edit->control_margin_y){
	    x = 0;
	    width = (guint) ((double) note->x[1] * (double) pattern_edit->control_unit.control_width - x_offset);

	    if(width > widget->allocation.width)
	      width = widget->allocation.width;

	    if(pattern_edit->y0 > control_height + pattern_edit->control_margin_y){
	      y = pattern_edit->y0 - (control_height + pattern_edit->control_margin_y);
	      height = control_height;
	    }else{
	      y = 0;
	      height = pattern_edit->y0 - pattern_edit->control_margin_y;
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
	}else if(note->y == (pattern_edit->stop_y + 1) && pattern_edit->y1 != 0){
	  if(pattern_edit->y1 > pattern_edit->control_margin_y){
	    x = 0;
	    width = note->x[1] * pattern_edit->control_unit.control_width - x_offset;

	    if(width > widget->allocation.width)
	      width = widget->allocation.width;

	    y = (note->y - pattern_edit->nth_y) * pattern_edit->control_height + pattern_edit->control_margin_y;

	    if(pattern_edit->y1 > control_height + pattern_edit->control_margin_y){
	      height = control_height;
	    }else{
	      height = pattern_edit->y1 - pattern_edit->control_margin_y;
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

    /* draw controls equal or greater than pattern_edit->nth_x */
    while(list_note != NULL && (note = (AgsNote *) list_note->data)->x[0] <= pattern_edit->control_unit.stop_x){
      if(note->y >= pattern_edit->nth_y && note->y <= pattern_edit->stop_y){
	x = (guint) note->x[0] * pattern_edit->control_unit.control_width;
	y = (note->y - pattern_edit->nth_y) * pattern_edit->control_height +
	  pattern_edit->y0 +
	  pattern_edit->control_margin_y;

	width = note->x[1] * pattern_edit->control_unit.control_width - x;
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
      }else if(note->y == (pattern_edit->nth_y - 1) && pattern_edit->y0 != 0){
	if(pattern_edit->y0 > pattern_edit->control_margin_y){
	  x = note->x[0] * pattern_edit->control_unit.control_width - x_offset;
	  width = note->x[1] * pattern_edit->control_unit.control_width - x_offset - x;
      
	  if(x + width > widget->allocation.width)
	    width = widget->allocation.width - x;

	  if(pattern_edit->y0 > control_height + pattern_edit->control_margin_y){
	    y = pattern_edit->y0 - (control_height + pattern_edit->control_margin_y);
	    height = control_height;
	  }else{
	    y = 0;
	    height = pattern_edit->y0 - pattern_edit->control_margin_y;
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
      }else if(note->y == (pattern_edit->stop_y + 1) && pattern_edit->y1 != 0){
	if(pattern_edit->y1 > pattern_edit->control_margin_y){
	  x = note->x[0] * pattern_edit->control_unit.control_width - x_offset;
	  width = note->x[1] * pattern_edit->control_unit.control_width - x_offset - x;
      
	  if(x + width > widget->allocation.width)
	    width = widget->allocation.width - x;

	  y = (note->y - pattern_edit->nth_y) * pattern_edit->control_height + pattern_edit->control_margin_y;

	  if(pattern_edit->y1 > control_height + pattern_edit->control_margin_y){
	    height = control_height;
	  }else{
	    height = pattern_edit->y1 - pattern_edit->control_margin_y;
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
 * ags_pattern_edit_draw_scroll:
 * @pattern_edit: the #AgsPatternEdit
 * @cr: the #cairo_t surface
 * @position: the new position
 *
 * Change visible x-position of @pattern_edit.
 *
 * Since: 0.4
 */
void
ags_pattern_edit_draw_scroll(AgsPatternEdit *pattern_edit, cairo_t *cr,
			  gdouble position)
{
  double x, y;
  double width, height;

  y = 0.0;
  x = (position) - (GTK_RANGE(pattern_edit->hscrollbar)->adjustment->value * pattern_edit->control_current.control_width);

  height = (double) GTK_WIDGET(pattern_edit->drawing_area)->allocation.height;
  width = 3.0;

  /* draw */
  cairo_set_source_rgba(cr, 0.79, 0.0, 1.0, 0.5);
  cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
  cairo_fill(cr);
}

/**
 * ags_pattern_edit_new:
 *
 * Create a new #AgsPatternEdit.
 *
 * Since: 0.4
 */
AgsPatternEdit*
ags_pattern_edit_new()
{
  AgsPatternEdit *pattern_edit;

  pattern_edit = (AgsPatternEdit *) g_object_new(AGS_TYPE_PATTERN_EDIT, NULL);

  return(pattern_edit);
}
