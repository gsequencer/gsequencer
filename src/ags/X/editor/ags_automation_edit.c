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

#include <ags-lib/object/ags_connectable.h>

#include <ags/X/ags_editor.h>

#include <ags/X/editor/ags_automation_edit.h>

void ags_automation_edit_class_init(AgsAutomationEditClass *automation_edit);
void ags_automation_edit_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_automation_edit_init(AgsAutomationEdit *automation_edit);
void ags_automation_edit_connect(AgsConnectable *connectable);
void ags_automation_edit_disconnect(AgsConnectable *connectable);

void ags_automation_edit_paint(AgsAutomationEdit *automation_edit);
void ags_automation_edit_draw_surface(GtkDrawingArea *drawing_area, cairo_t *cr,
				      gdouble x0, gdouble x1,
				      gdouble azimut);
void ags_automation_edit_draw_strip(GtkDrawingArea *drawing_area,
				    AgsChannel *channel, gchar *control_name,
				    cairo_t *cr);

/**
 * SECTION:ags_automation_edit
 * @short_description: edit automations
 * @title: AgsAutomationEdit
 * @section_id:
 * @include: ags/X/editor/ags_automation_edit.h
 *
 * The #AgsAutomationEdit lets you edit automations.
 */

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
}

void
ags_automation_edit_init(AgsAutomationEdit *automation_edit)
{
  GtkAdjustment *adjustment;

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, 1.0, 1.0);

  automation_edit->ruler = ags_ruler_new();
  gtk_table_attach(GTK_TABLE(automation_edit), (GtkWidget *) automation_edit->ruler,
		   0, 1, 0, 1,
		   GTK_FILL|GTK_EXPAND, GTK_FILL,
		   0, 0);

  automation_edit->drawing_area = gtk_vbox_new(FALSE, 0);
  gtk_table_attach(GTK_TABLE(automation_edit), (GtkWidget *) automation_edit->drawing_area,
		   0, 1, 1, 2,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);

  /* GtkScrollbars */
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, 1.0, 1.0);
  automation_edit->vscrollbar = (GtkVScrollbar *) gtk_vscrollbar_new(adjustment);
  gtk_table_attach(GTK_TABLE(automation_edit), (GtkWidget *) automation_edit->vscrollbar,
		   1, 2, 1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, 1.0, 1.0);
  automation_edit->hscrollbar = (GtkHScrollbar *) gtk_hscrollbar_new(adjustment);
  gtk_table_attach(GTK_TABLE(automation_edit), (GtkWidget *) automation_edit->hscrollbar,
		   0, 1, 2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);
}

void
ags_automation_edit_connect(AgsConnectable *connectable)
{
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
 * Since: 0.4
 */
void
ags_automation_edit_reset_vertically(AgsAutomationEdit *automation_edit, guint flags)
{
  //TODO:JK: implement me
}

/**
 * ags_automation_edit_reset_horizontally:
 * @automation_edit: the #AgsAutomationEdit
 * @flags: the #AgsAutomationEditResetFlags
 *
 * Reset @automation_edit as configured horizontally.
 *
 * Since: 0.4
 */
void
ags_automation_edit_reset_horizontally(AgsAutomationEdit *automation_edit, guint flags)
{
  //TODO:JK: implement me
}

/**
 * ags_automation_edit_draw_surface:
 * @automation_edit: the #AgsAutomationEdit
 * @cr: the #cairo_t surface
 * @x0: x offset
 * @x1: x offset
 * @azimut: y axis
 *
 * Draw a portion of data.
 *
 * Since: 0.4
 */
void
ags_automation_edit_draw_surface(GtkDrawingArea *drawing_area, cairo_t *cr,
				 gdouble x0, gdouble x1,
				 gdouble azimut)
{
  AgsAutomationEdit *automation_edit;
  gdouble width, height;

  automation_edit = gtk_widget_get_ancestor(drawing_area,
					    AGS_TYPE_AUTOMATION_EDIT);

  width = (gdouble) GTK_WIDGET(drawing_area)->allocation.width;
  height = (gdouble) GTK_WIDGET(drawing_area)->allocation.height;

  //TODO:JK: implement me
}

/**
 * ags_automation_edit_draw_strip:
 * @automation_edit: the #AgsAutomationEdit
 * @cr: the #cairo_t surface
 * @channel: the #AgsChannel
 *
 * Plot data.
 *
 * Since: 0.4
 */
void
ags_automation_edit_draw_strip(GtkDrawingArea *drawing_area,
			       AgsChannel *channel, gchar *control_name,
			       cairo_t *cr)
{
  AgsAutomationEdit *automation_edit;
  gdouble width, height;

  automation_edit = gtk_widget_get_ancestor(drawing_area,
					    AGS_TYPE_AUTOMATION_EDIT);

  width = (gdouble) GTK_WIDGET(drawing_area)->allocation.width;
  height = (gdouble) GTK_WIDGET(drawing_area)->allocation.height;

  /* background */
  cairo_set_source_rgb(cr, 0.5, 0.4, 0.0);
  cairo_rectangle(cr, 0.0, 0.0, width, height);
  cairo_fill(cr);

  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
  cairo_set_line_width(cr, 1.0);
  cairo_rectangle(cr, 0.0, 0.0, width, height);
  cairo_stroke(cr);

  //TODO:JK: implement me
}

/**
 * ags_automation_edit_draw_scale:
 * @automation_edit: the #AgsAutomationEdit
 * @cr: the #cairo_t surface
 * @lower: the lower of scale
 * @upper: the upper of scale
 * @ground: the adjusting point
 *
 * Draw a scale and its boundaries.
 *
 * Since: 0.4
 */
void
ags_automation_edit_draw_scale(GtkDrawingArea *drawing_area, cairo_t *cr,
			       gdouble lower, gdouble upper,
			       gdouble ground)
{
  AgsAutomationEdit *automation_edit;
  gdouble width, height;

  automation_edit = gtk_widget_get_ancestor(drawing_area,
					    AGS_TYPE_AUTOMATION_EDIT);

  width = (gdouble) GTK_WIDGET(drawing_area)->allocation.width;
  height = (gdouble) GTK_WIDGET(drawing_area)->allocation.height;

  //TODO:JK: implement me
}

/**
 * ags_automation_edit_draw_position:
 * @automation_edit: the #AgsAutomationEdit
 * @cr: the #cairo_t surface
 *
 * Draws the cursor.
 *
 * Since: 0.4
 */
void
ags_automation_edit_draw_position(GtkDrawingArea *drawing_area, cairo_t *cr)
{
  AgsAutomationEdit *automation_edit;
  gdouble width, height;

  automation_edit = gtk_widget_get_ancestor(drawing_area,
					    AGS_TYPE_AUTOMATION_EDIT);

  width = (gdouble) GTK_WIDGET(drawing_area)->allocation.width;
  height = (gdouble) GTK_WIDGET(drawing_area)->allocation.height;

  //TODO:JK: implement me
}

/**
 * ags_automation_edit_draw_automation:
 * @automation_edit: the #AgsAutomationEdit
 * @cr: the #cairo_t surface
 *
 * Draw the #AgsAutomation of selected #AgsMachine on @automation_edit.
 *
 * Since: 0.4
 */
void
ags_automation_edit_draw_automation(GtkDrawingArea *drawing_area,
				    AgsAutomation *automation,
				    cairo_t *cr)
{
  AgsEditor *editor;
  AgsAutomationEdit *automation_edit;

  editor = (AgsEditor *) gtk_widget_get_ancestor(GTK_WIDGET(automation_edit),
						 AGS_TYPE_EDITOR);

  automation_edit = gtk_widget_get_ancestor(drawing_area,
					    AGS_TYPE_AUTOMATION_EDIT);

  if(editor->selected_machine != NULL){
    AgsMachine *machine;
    AgsChannel *channel;

    machine = editor->selected_machine;

    if(machine != NULL){
      AgsAcceleration *current;
      GList *automation, *acceleration;
      gdouble width, height;

      automation = machine->audio->automation;

      width = (gdouble) GTK_WIDGET(drawing_area)->allocation.width;
      height = (gdouble) GTK_WIDGET(drawing_area)->allocation.height;

      while(automation != NULL){
	if((AGS_AUDIO_NOTATION_DEFAULT & (machine->audio->flags)) == 0){
	  channel = ags_channel_nth(machine->audio->output,
				    AGS_AUTOMATION(automation->data)->line);
	}else{
	  channel = ags_channel_nth(machine->audio->input,
				    AGS_AUTOMATION(automation->data)->line);
	}

	ags_automation_edit_draw_strip(drawing_area,
				       channel, AGS_AUTOMATION(automation->data)->control_name,
				       cr);
	
	acceleration = AGS_ACCELERATION(AGS_AUTOMATION(automation->data)->acceleration);

	while(acceleration != NULL){

	  acceleration = acceleration->next;
	}

	automation = automation->next;
      }      
    }
  }
}

/**
 * ags_automation_edit_draw_scroll:
 * @automation_edit: the #AgsAutomationEdit
 * @cr: the #cairo_t surface
 * @position: the new position
 *
 * Change visible x-position of @automation_edit.
 *
 * Since: 0.4
 */
void
ags_automation_edit_draw_scroll(GtkDrawingArea *drawing_area, cairo_t *cr,
				gdouble position)
{
  AgsAutomationEdit *automation_edit;
  gdouble width, height;

  automation_edit = gtk_widget_get_ancestor(drawing_area,
					    AGS_TYPE_AUTOMATION_EDIT);

  width = (gdouble) GTK_WIDGET(drawing_area)->allocation.width;
  height = (gdouble) GTK_WIDGET(drawing_area)->allocation.height;

  //TODO:JK: implement me
}


GtkDrawingArea*
ags_automation_edit_add_drawing_area(AgsAutomationEdit *automation_edit,
				     AgsAutomation *automation)
{
  GtkDrawingArea *drawing_area;

  drawing_area = (GtkDrawingArea *) gtk_drawing_area_new();
  gtk_widget_set_style((GtkWidget *) automation_edit->drawing_area, automation_edit_style);
  gtk_widget_set_events(GTK_WIDGET (automation_edit->drawing_area), GDK_EXPOSURE_MASK
			| GDK_LEAVE_NOTIFY_MASK
			| GDK_BUTTON_PRESS_MASK
			| GDK_BUTTON_RELEASE_MASK
			| GDK_POINTER_MOTION_MASK
			| GDK_POINTER_MOTION_HINT_MASK
			);
  gtk_box_pack_start(automation_edit->drawing_area,
		     drawing_area,
		     TRUE, FALSE,
		     0);

  return(drawing_area);
}

/**
 * ags_automation_edit_new:
 *
 * Create a new #AgsAutomationEdit.
 *
 * Since: 0.4
 */
AgsAutomationEdit*
ags_automation_edit_new()
{
  AgsAutomationEdit *automation_edit;

  automation_edit = (AgsAutomationEdit *) g_object_new(AGS_TYPE_AUTOMATION_EDIT, NULL);

  return(automation_edit);
}
