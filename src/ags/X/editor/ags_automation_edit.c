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

#include <ags/X/editor/ags_automation_edit.h>
#include <ags/X/editor/ags_automation_edit_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/X/ags_automation_editor.h>

#include <ags/X/editor/ags_automation_edit.h>

#include <cairo.h>
#include <math.h>

void ags_automation_edit_class_init(AgsAutomationEditClass *automation_edit);
void ags_automation_edit_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_automation_edit_init(AgsAutomationEdit *automation_edit);
void ags_automation_edit_connect(AgsConnectable *connectable);
void ags_automation_edit_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_automation_edit
 * @short_description: edit automations
 * @title: AgsAutomationEdit
 * @section_id:
 * @include: ags/X/editor/ags_automation_edit.h
 *
 * The #AgsAutomationEdit lets you edit automations.
 */

static gpointer ags_automation_edit_parent_class = NULL;

GtkStyle *automation_edit_style;

GType
ags_automation_edit_get_type(void)
{
  static GType ags_type_automation_edit = 0;

  if(!ags_type_automation_edit){
    static const GTypeInfo ags_automation_edit_info = {
      sizeof (AgsAutomationEditClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_automation_edit_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAutomationEdit),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_automation_edit_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_automation_edit_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_automation_edit = g_type_register_static(GTK_TYPE_TABLE,
						      "AgsAutomationEdit\0", &ags_automation_edit_info,
						      0);
    
    g_type_add_interface_static(ags_type_automation_edit,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_automation_edit);
}

void
ags_automation_edit_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_automation_edit_connect;
  connectable->disconnect = ags_automation_edit_disconnect;
}

void
ags_automation_edit_class_init(AgsAutomationEditClass *automation_edit)
{
  ags_automation_edit_parent_class = g_type_class_peek_parent(automation_edit);
}

void
ags_automation_edit_init(AgsAutomationEdit *automation_edit)
{
  GtkAdjustment *adjustment;

  automation_edit->flags = 0;
  
  automation_edit->map_width = AGS_AUTOMATION_EDIT_MAX_CONTROLS;
  automation_edit->map_height = 0;
  
  automation_edit->ruler = ags_ruler_new();
  gtk_table_attach(GTK_TABLE(automation_edit),
		   (GtkWidget *) automation_edit->ruler,
		   0, 1,
		   0, 1,
		   GTK_FILL|GTK_EXPAND,
		   GTK_FILL,
		   0, 0);

  automation_edit->drawing_area = (GtkDrawingArea *) gtk_drawing_area_new();
  gtk_widget_set_style((GtkWidget *) automation_edit->drawing_area, automation_edit_style);
  gtk_widget_set_events(GTK_WIDGET (automation_edit->drawing_area), GDK_EXPOSURE_MASK
			| GDK_LEAVE_NOTIFY_MASK
			| GDK_BUTTON_PRESS_MASK
			| GDK_BUTTON_RELEASE_MASK
			| GDK_POINTER_MOTION_MASK
			| GDK_POINTER_MOTION_HINT_MASK
			);
    
  gtk_table_attach(GTK_TABLE(automation_edit),
		   (GtkWidget *) automation_edit->drawing_area,
		   0, 1,
		   1, 2,
		   GTK_FILL|GTK_EXPAND,
		   GTK_FILL|GTK_EXPAND,
		   0, 0);

  automation_edit->automation_area = NULL;
  
  /* GtkScrollbars */
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, 1.0, 1.0);
  automation_edit->vscrollbar = (GtkVScrollbar *) gtk_vscrollbar_new(adjustment);
  gtk_table_attach(GTK_TABLE(automation_edit),
		   (GtkWidget *) automation_edit->vscrollbar,
		   1, 2,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, 1.0, 1.0);
  automation_edit->hscrollbar = (GtkHScrollbar *) gtk_hscrollbar_new(adjustment);
  gtk_table_attach(GTK_TABLE(automation_edit),
		   (GtkWidget *) automation_edit->hscrollbar,
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);
}

void
ags_automation_edit_connect(AgsConnectable *connectable)
{
  AgsAutomationEdit *automation_edit;

  automation_edit = AGS_AUTOMATION_EDIT(connectable);

  /*  */
  g_signal_connect_after((GObject *) automation_edit->drawing_area, "expose_event\0",
			 G_CALLBACK (ags_automation_edit_drawing_area_expose_event), (gpointer) automation_edit);

  g_signal_connect_after((GObject *) automation_edit->drawing_area, "configure_event\0",
			 G_CALLBACK (ags_automation_edit_drawing_area_configure_event), (gpointer) automation_edit);

  g_signal_connect((GObject *) automation_edit->drawing_area, "button_press_event\0",
		   G_CALLBACK (ags_automation_edit_drawing_area_button_press_event), (gpointer) automation_edit);

  g_signal_connect((GObject *) automation_edit->drawing_area, "button_release_event\0",
		   G_CALLBACK (ags_automation_edit_drawing_area_button_release_event), (gpointer) automation_edit);
  
  g_signal_connect((GObject *) automation_edit->drawing_area, "motion_notify_event\0",
		   G_CALLBACK (ags_automation_edit_drawing_area_motion_notify_event), (gpointer) automation_edit);

  /*  */
  g_signal_connect_after((GObject *) automation_edit->vscrollbar, "value-changed\0",
			 G_CALLBACK (ags_automation_edit_vscrollbar_value_changed), (gpointer) automation_edit);

  g_signal_connect_after((GObject *) automation_edit->hscrollbar, "value-changed\0",
			 G_CALLBACK (ags_automation_edit_hscrollbar_value_changed), (gpointer) automation_edit);
}

void
ags_automation_edit_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

/**
 * ags_automation_edit_reset_vertically:
 * @automation_edit: the #AgsAutomationEdit
 * @flags: the #AgsAutomationEditResetFlags
 *
 * Reset @automation_edit as configured vertically.
 *
 * Since: 0.4.3
 */
void
ags_automation_edit_reset_vertically(AgsAutomationEdit *automation_edit, guint flags)
{
  AgsAutomationEditor *automation_editor;

  automation_editor = gtk_widget_get_ancestor(automation_edit,
					      AGS_TYPE_AUTOMATION_EDITOR);
  
  if((AGS_AUTOMATION_EDIT_RESET_VSCROLLBAR & flags) != 0){
    GtkAdjustment *vadjustment;
    guint height;

    height = GTK_WIDGET(automation_edit->drawing_area)->allocation.height;

    vadjustment = GTK_RANGE(automation_edit->vscrollbar)->adjustment;

    if(height > automation_edit->map_height){
      gtk_adjustment_set_upper(vadjustment,
			       1.0);
    }else{
      gtk_adjustment_set_upper(vadjustment,
			       (gdouble) (automation_edit->map_height - height));
    }
    
    if(vadjustment->value > vadjustment->upper){
      gtk_adjustment_set_value(vadjustment, vadjustment->upper);
    }
  }

  gtk_widget_queue_draw(automation_edit->drawing_area);

  /* reset scale */
  if(automation_editor->audio_automation_edit == automation_edit){
    automation_editor->audio_scale->y_offset = GTK_RANGE(automation_edit->vscrollbar)->adjustment->value;
    gtk_widget_queue_draw(automation_editor->audio_scale);
  }else if(automation_editor->output_automation_edit == automation_edit){
    automation_editor->output_scale->y_offset = GTK_RANGE(automation_edit->vscrollbar)->adjustment->value;
    gtk_widget_queue_draw(automation_editor->output_scale);
  }else if(automation_editor->input_automation_edit == automation_edit){
    automation_editor->input_scale->y_offset = GTK_RANGE(automation_edit->vscrollbar)->adjustment->value;
    gtk_widget_queue_draw(automation_editor->input_scale);
  }
}

/**
 * ags_automation_edit_reset_horizontally:
 * @automation_edit: the #AgsAutomationEdit
 * @flags: the #AgsAutomationEditResetFlags
 *
 * Reset @automation_edit as configured horizontally.
 *
 * Since: 0.4.3
 */
void
ags_automation_edit_reset_horizontally(AgsAutomationEdit *automation_edit, guint flags)
{
  AgsAutomationEditor *automation_editor;

  double tact_factor, zoom_factor;
  double tact;

  automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor(GTK_WIDGET(automation_edit),
								      AGS_TYPE_AUTOMATION_EDITOR);

  zoom_factor = 1.0 / 4.0;

  tact_factor = exp2(8.0 - (double) gtk_combo_box_get_active(automation_editor->automation_toolbar->zoom));
  tact = exp2((double) gtk_combo_box_get_active(automation_editor->automation_toolbar->zoom) - 4.0);

  if((AGS_AUTOMATION_EDIT_RESET_WIDTH & flags) != 0){
    automation_edit->map_width = (guint) ((double) (64 * AGS_AUTOMATION_EDIT_MAX_CONTROLS) * zoom_factor * tact);
    
    /* reset ruler */
    automation_edit->ruler->factor = tact_factor;
    automation_edit->ruler->precision = tact;
    automation_edit->ruler->scale_precision = 1.0 / tact;

    gtk_widget_queue_draw(automation_edit->ruler);
  }

  g_message("%d\0",
	    automation_edit->map_width);

  if((AGS_AUTOMATION_EDIT_RESET_HSCROLLBAR & flags) != 0){
    GtkWidget *widget;
    
    GtkAdjustment *hadjustment;
    
    guint width;

    widget = GTK_WIDGET(automation_edit->drawing_area);
    hadjustment = GTK_RANGE(automation_edit->hscrollbar)->adjustment;

    if(automation_edit->map_width > widget->allocation.width){
      width = widget->allocation.width;
      
      gtk_adjustment_set_upper(hadjustment,
			       (gdouble) (automation_edit->map_width - width));
      gtk_adjustment_set_upper(automation_edit->ruler->adjustment,
			       (gdouble) (automation_edit->map_width - width) / 64.0);
      
      if(hadjustment->value > hadjustment->upper){
	gtk_adjustment_set_value(hadjustment, hadjustment->upper);

	/* reset ruler */
	gtk_adjustment_set_value(automation_edit->ruler->adjustment,
			       automation_edit->ruler->adjustment->upper);
	gtk_widget_queue_draw(automation_edit->ruler);

      }
    }else{
      gtk_adjustment_set_upper(hadjustment, 0.0);
      gtk_adjustment_set_value(hadjustment, 0.0);
      
      /* reset ruler */
      gtk_adjustment_set_upper(automation_edit->ruler->adjustment, 0.0);
      gtk_adjustment_set_value(automation_edit->ruler->adjustment, 0.0);
      gtk_widget_queue_draw(automation_edit->ruler);
    }
  }

  gtk_widget_queue_draw(automation_edit->drawing_area);
}

/**
 * ags_automation_edit_draw_position:
 * @automation_edit: the #AgsAutomationEdit
 *
 * Draws the cursor.
 *
 * Since: 0.4.3
 */
void
ags_automation_edit_draw_position(AgsAutomationEdit *automation_edit)
{
  gdouble width, height;

  width = (gdouble) GTK_WIDGET(automation_edit->drawing_area)->allocation.width;
  height = (gdouble) GTK_WIDGET(automation_edit->drawing_area)->allocation.height;

  //TODO:JK: implement me
}

/**
 * ags_automation_edit_draw_scroll:
 * @automation_edit: the #AgsAutomationEdit
 * @cr: the #cairo_t surface
 * @position: the new position
 *
 * Change visible x-position of @automation_edit.
 *
 * Since: 0.4.3
 */
void
ags_automation_edit_draw_scroll(AgsAutomationEdit *automation_edit,
				gdouble position)
{
  gdouble width, height;

  width = (gdouble) GTK_WIDGET(automation_edit->drawing_area)->allocation.width;
  height = (gdouble) GTK_WIDGET(automation_edit->drawing_area)->allocation.height;

  //TODO:JK: implement me
}

void
ags_automation_edit_paint(AgsAutomationEdit *automation_edit,
			  cairo_t *cr)
{
  GList *automation_area;

  gdouble x_offset, y_offset;
  
  x_offset = GTK_RANGE(automation_edit->hscrollbar)->adjustment->value;
  y_offset = GTK_RANGE(automation_edit->vscrollbar)->adjustment->value;

  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_rectangle(cr, 0.0, 0.0, (double) GTK_WIDGET(automation_edit->drawing_area)->allocation.width, (double) GTK_WIDGET(automation_edit->drawing_area)->allocation.height);
  cairo_fill(cr);

  automation_area = automation_edit->automation_area;

  while(automation_area != NULL){
    ags_automation_area_paint(automation_area->data,
			      cr,
			      x_offset, y_offset);

    automation_area = automation_area->next;
  }
}

/**
 * ags_automation_edit_add_area:
 * @automation_edit: an #AgsAutomationEdit
 * @automation_area: the #AgsAutomationArea to add
 *
 * Adds @automation_area to @automation_edit.
 * 
 * Since: 0.4.3
 */
void
ags_automation_edit_add_area(AgsAutomationEdit *automation_edit,
			     AgsAutomationArea *automation_area)
{
  guint y;

  g_object_ref(automation_area);

  automation_edit->map_height += AGS_AUTOMATION_AREA_DEFAULT_HEIGHT + AGS_AUTOMATION_EDIT_DEFAULT_MARGIN;
  
  automation_area->drawing_area = (GtkDrawingArea *) automation_edit->drawing_area;

  if(automation_edit->automation_area != NULL){
    y = AGS_AUTOMATION_AREA(automation_edit->automation_area->data)->y + AGS_AUTOMATION_AREA_DEFAULT_HEIGHT + AGS_AUTOMATION_EDIT_DEFAULT_MARGIN;
  }else{
    y = 0;
  }

  automation_area->y = y;
  automation_area->height = AGS_AUTOMATION_AREA_DEFAULT_HEIGHT;

  automation_edit->automation_area = g_list_prepend(automation_edit->automation_area,
						    automation_area);
}

/**
 * ags_automation_edit_remove_area:
 * @automation_edit: an #AgsAutomationEdit
 * @automation_area: the #AgsAutomationArea to remove
 *
 * Removes @automation_area of @automation_edit.
 * 
 * Since: 0.4.3
 */
void
ags_automation_edit_remove_area(AgsAutomationEdit *automation_edit,
				AgsAutomationArea *automation_area)
{
  GList *list;
  guint y;

  automation_edit->automation_area = g_list_remove(automation_edit->automation_area,
						   automation_area);

  /* configure y */
  list = g_list_last(automation_edit->automation_area);
  y = 0;

  while(list != NULL){
    AGS_AUTOMATION_AREA(list->data)->y = y;

    y += AGS_AUTOMATION_AREA_DEFAULT_HEIGHT + AGS_AUTOMATION_EDIT_DEFAULT_MARGIN;
    list = list->prev;
  }
  
  automation_edit->map_height -= AGS_AUTOMATION_AREA_DEFAULT_HEIGHT + AGS_AUTOMATION_EDIT_DEFAULT_MARGIN;

  g_object_unref(automation_area);
}

/**
 * ags_automation_edit_new:
 *
 * Create a new #AgsAutomationEdit.
 *
 * Since: 0.4.3
 */
AgsAutomationEdit*
ags_automation_edit_new()
{
  AgsAutomationEdit *automation_edit;

  automation_edit = (AgsAutomationEdit *) g_object_new(AGS_TYPE_AUTOMATION_EDIT, NULL);

  return(automation_edit);
}
