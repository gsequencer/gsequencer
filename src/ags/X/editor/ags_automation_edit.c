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
  gtk_widget_set_events (GTK_WIDGET (automation_edit->drawing_area), GDK_EXPOSURE_MASK
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

  automation_edit->map_width = AGS_AUTOMATION_EDIT_MAX_CONTROLS * 64;
  automation_edit->map_height = 0;

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

  /*  */

  //TODO:JK: implement me
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
  if((AGS_AUTOMATION_EDIT_RESET_VSCROLLBAR & flags) != 0){
    GtkAdjustment *vadjustment;
    guint height;

    height = GTK_WIDGET(automation_edit->drawing_area)->allocation.height;

    vadjustment = GTK_RANGE(automation_edit->vscrollbar)->adjustment;

    gtk_adjustment_set_upper(vadjustment,
			     (gdouble) (automation_edit->map_height - height));

    if(vadjustment->value > vadjustment->upper){
      gtk_adjustment_set_value(vadjustment, vadjustment->upper);
    }
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
  if((AGS_AUTOMATION_EDIT_RESET_HSCROLLBAR & flags) != 0){
    GtkAdjustment *hadjustment;
    guint width;

    width = GTK_WIDGET(automation_edit->drawing_area)->allocation.width;

    hadjustment = GTK_RANGE(automation_edit->hscrollbar)->adjustment;

    gtk_adjustment_set_upper(hadjustment,
			     (gdouble) (automation_edit->map_width - width));

    if(hadjustment->value > hadjustment->upper){
      gtk_adjustment_set_value(hadjustment, hadjustment->upper);
    }
  }
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
ags_automation_edit_paint(AgsAutomationEdit *automation_edit)
{
  GList *automation_area;

  automation_area = automation_edit->automation_area;

  while(automation_area != NULL){
    ags_automation_area_paint(automation_area->data);

    automation_area = automation_area->next;
  }
}

void
ags_automation_edit_add_area(AgsAutomationEdit *automation_edit,
			     AgsAutomationArea *automation_area)
{
  guint y;

  g_object_ref(automation_area);

  automation_area->drawing_area = (GtkDrawingArea *) automation_edit->drawing_area;

  if(automation_edit->automation_area->data != NULL){
    y = AGS_AUTOMATION_AREA(automation_edit->automation_area->data)->y;
  }else{
    y = 0;
  }

  automation_area->y = y;
  automation_area->height = AGS_AUTOMATION_AREA_DEFAULT_HEIGHT;

  automation_edit->automation_area = g_list_prepend(automation_edit->automation_area,
						    automation_area);
}

void
ags_automation_edit_remove_area(AgsAutomationEdit *automation_edit,
				AgsAutomationArea *automation_area)
{
  automation_edit->automation_area = g_list_remove(automation_edit->automation_area,
						   automation_area);
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
