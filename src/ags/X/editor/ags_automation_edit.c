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

#include <ags/X/ags_automation_editor.h>

#include <ags/X/editor/ags_automation_edit.h>
#include <ags/X/editor/ags_automation_area.h>

void ags_automation_edit_class_init(AgsAutomationEditClass *automation_edit);
void ags_automation_edit_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_automation_edit_init(AgsAutomationEdit *automation_edit);
void ags_automation_edit_connect(AgsConnectable *connectable);
void ags_automation_edit_disconnect(AgsConnectable *connectable);

void ags_automation_edit_paint(AgsAutomationEdit *automation_edit);

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
 * ags_automation_edit_draw_position:
 * @drawing_area: the #GtkVBox containing drawing area
 *
 * Draws the cursor.
 *
 * Since: 0.4
 */
void
ags_automation_edit_draw_position(GtkVBox *drawing_area)
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
ags_automation_edit_draw_scroll(GtkVBox *drawing_area,
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

void
ags_automation_edit_paint(AgsAutomationEdit *automation_edit)
{
  AgsAutomationEditor *automation_editor;

  automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor(automation_edit,
								      AGS_TYPE_AUTOMATION_EDITOR);

  if(automation_editor->selected_machine != NULL){
    AgsMachine *machine;
    AgsAutomationArea *automation_area;
    AgsChannel *channel;
    cairo_t *cr;
    GList *list;
    guint width, height;
    gdouble x0, x1;

    machine = automation_editor->selected_machine;
    
    list = gtk_container_get_children(automation_edit->drawing_area);

    while(list != NULL){
      automation_area = AGS_AUTOMATION_AREA(list->data);

      if((AGS_AUDIO_NOTATION_DEFAULT & (machine->audio->flags)) == 0){
	channel = ags_channel_nth(machine->audio->output,
				  automation_area->automation->line);
      }else{
	channel = ags_channel_nth(machine->audio->input,
				  automation_area->automation->line);
      }

      cr = gdk_cairo_create(GTK_WIDGET(list->data)->window);

      width = GTK_WIDGET(list->data)->allocation.width;
      height = GTK_WIDGET(list->data)->allocation.height;

      x0 = GTK_RANGE(automation_edit->vscrollbar)->adjustment->value;
      x1 = x0 + width;

      ags_automation_area_draw_strip(automation_area, cr,
				     channel, automation_area->automation->control_name);

      ags_automation_area_draw_automation(automation_area, cr,
					  x0, x1);

      ags_automation_area_draw_scale(automation_area, cr,
				     automation_area->automation->lower, automation_area->automation->upper);

      list = list->next;
    }

    g_list_free(list);
  }
}

GtkDrawingArea*
ags_automation_edit_add_drawing_area(AgsAutomationEdit *automation_edit)
{
  GtkDrawingArea *drawing_area;

  drawing_area = (GtkDrawingArea *) ags_automation_area_new();
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

  //TODO:JK: implement me

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
