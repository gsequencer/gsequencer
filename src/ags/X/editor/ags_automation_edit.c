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
#include <ags/X/editor/ags_automation_area.h>

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

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, 1.0, 1.0);

  automation_edit->ruler = ags_ruler_new();
  gtk_table_attach(GTK_TABLE(automation_edit),
		   (GtkWidget *) automation_edit->ruler,
		   0, 1, 0, 1,
		   GTK_FILL|GTK_EXPAND,
		   GTK_FILL,
		   0, 0);

  automation_edit->map_width = AGS_AUTOMATION_EDIT_MAX_CONTROLS * 64;
  automation_edit->map_height = 0;

  /* drawing area box and its viewport */
  automation_edit->scrolled_window = gtk_scrolled_window_new(NULL,
							     NULL);
  gtk_widget_set_events(GTK_WIDGET(automation_edit->scrolled_window), GDK_EXPOSURE_MASK
			| GDK_LEAVE_NOTIFY_MASK
			| GDK_BUTTON_PRESS_MASK
			| GDK_BUTTON_RELEASE_MASK
			| GDK_POINTER_MOTION_MASK
			| GDK_POINTER_MOTION_HINT_MASK
			);
  gtk_table_attach(GTK_TABLE(automation_edit),
		   (GtkWidget *) automation_edit->scrolled_window,
		   0, 1,
		   1, 2,
		   GTK_FILL|GTK_EXPAND,
		   GTK_FILL|GTK_EXPAND,
		   0, 0);

  automation_edit->automation_area = gtk_vbox_new(FALSE, 0);
  gtk_scrolled_window_add_with_viewport(automation_edit->scrolled_window,
					automation_edit->automation_area);
}

void
ags_automation_edit_connect(AgsConnectable *connectable)
{
  AgsAutomationEdit *automation_edit;

  automation_edit = AGS_AUTOMATION_EDIT(connectable);

  /*  */
  g_signal_connect_after((GObject *) automation_edit->scrolled_window, "expose_event\0",
			 G_CALLBACK (ags_automation_edit_scrolled_window_expose_event), (gpointer) automation_edit);

  g_signal_connect_after((GObject *) automation_edit->scrolled_window, "configure_event\0",
			 G_CALLBACK (ags_automation_edit_scrolled_window_configure_event), (gpointer) automation_edit);

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

    height = GTK_WIDGET(automation_edit->automation_area)->allocation.height;

    vadjustment = gtk_scrolled_window_get_vadjustment(automation_edit->scrolled_window);

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

    width = GTK_WIDGET(automation_edit->automation_area)->allocation.width;

    hadjustment = gtk_scrolled_window_get_hadjustment(automation_edit->scrolled_window);

    gtk_adjustment_set_upper(hadjustment,
			     (gdouble) (automation_edit->map_width - width));

    if(hadjustment->value > hadjustment->upper){
      gtk_adjustment_set_value(hadjustment, hadjustment->upper);
    }
  }
}

/**
 * ags_automation_edit_draw_position:
 * @automation_area: the #GtkVBox containing automation area
 *
 * Draws the cursor.
 *
 * Since: 0.4.3
 */
void
ags_automation_edit_draw_position(GtkVBox *automation_area)
{
  AgsAutomationEdit *automation_edit;
  gdouble width, height;

  automation_edit = gtk_widget_get_ancestor(automation_area,
					    AGS_TYPE_AUTOMATION_EDIT);

  width = (gdouble) GTK_WIDGET(automation_area)->allocation.width;
  height = (gdouble) GTK_WIDGET(automation_area)->allocation.height;

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
ags_automation_edit_draw_scroll(GtkVBox *automation_area,
				gdouble position)
{
  AgsAutomationEdit *automation_edit;
  gdouble width, height;

  automation_edit = gtk_widget_get_ancestor(automation_area,
					    AGS_TYPE_AUTOMATION_EDIT);

  width = (gdouble) GTK_WIDGET(automation_area)->allocation.width;
  height = (gdouble) GTK_WIDGET(automation_area)->allocation.height;

  //TODO:JK: implement me
}

/**
 * ags_automation_edit_add_automation_area:
 * @automation_edit:
 * @automation:
 * 
 * 
 */
GtkDrawingArea*
ags_automation_edit_add_drawing_area(AgsAutomationEdit *automation_edit,
				     AgsAutomation *automation)
{
  GtkDrawingArea *drawing_area;
  guint width;
  
  width = GTK_WIDGET(automation_edit->automation_area)->allocation.width;

  /* resize */
  automation_edit->map_height += AGS_AUTOMATION_AREA_DEFAULT_HEIGHT;

  automation_edit->flags |= AGS_AUTOMATION_EDIT_RESETING_VERTICALLY;
  ags_automation_edit_reset_vertically(automation_edit, AGS_AUTOMATION_EDIT_RESET_VSCROLLBAR);
  automation_edit->flags &= (~AGS_AUTOMATION_EDIT_RESETING_VERTICALLY);

  /* add */
  drawing_area = (GtkDrawingArea *) g_object_new(AGS_TYPE_AUTOMATION_AREA,
						 "automation\0", automation,
						 NULL);
  gtk_widget_set_size_request(drawing_area,
			      width, AGS_AUTOMATION_AREA_DEFAULT_HEIGHT);
  gtk_box_pack_start(automation_edit->automation_area,
		     drawing_area,
		     FALSE, FALSE,
		     0);
  ags_connectable_connect(AGS_CONNECTABLE(drawing_area));
  gtk_widget_show(drawing_area);

  return(drawing_area);
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
