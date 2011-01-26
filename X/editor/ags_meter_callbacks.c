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

#include <ags/X/editor/ags_meter_callbacks.h>

#include <ags/X/ags_editor.h>

#include <math.h>

gboolean
ags_meter_destroy_callback(GtkObject *object, AgsMeter *meter)
{
  ags_meter_destroy(object);

  return(FALSE);
}

void
ags_meter_show_callback(GtkWidget *widget, AgsMeter *meter)
{
  ags_meter_show(widget);
}

gboolean
ags_meter_expose_event(GtkWidget *widget, GdkEventExpose *event, AgsMeter *meter)
{
  /*
  AgsEditor *editor;
  AgsMachine *machine;
  guint rows, nth_y, y0;

  editor = (AgsEditor *) gtk_widget_get_ancestor(widget, AGS_TYPE_EDITOR);
  nth_y = ceil(GTK_RANGE(editor->vscrollbar)->adjustment->value / (double)editor->control_height);
  y0 = editor->control_height - (guint)(GTK_RANGE(editor->vscrollbar)->adjustment->value) % editor->control_height;

  if(editor->selected != NULL){
    machine = (AgsMachine *) g_object_get_data((GObject *) editor->selected, g_type_name(AGS_TYPE_MACHINE));
    rows = (machine != NULL) ? machine->audio->input_pads: 0;
  }else
    rows = 0;

  ags_meter_paint(meter, rows, nth_y, y0);
  */

  ags_meter_paint(meter);

  return(TRUE);
}

gboolean
ags_meter_configure_event(GtkWidget *widget, GdkEventConfigure *event, AgsMeter *meter)
{
  /*
  AgsEditor *editor;
  AgsMachine *machine;
  guint rows, nth_y, y0;

  editor = (AgsEditor *) gtk_widget_get_ancestor(widget, AGS_TYPE_EDITOR);
  nth_y = ceil(GTK_RANGE(editor->vscrollbar)->adjustment->value / (double)editor->control_height);
  y0 = editor->control_height - (guint)(GTK_RANGE(editor->vscrollbar)->adjustment->value) % editor->control_height;

  if(editor->selected != NULL){
    machine = (AgsMachine *) g_object_get_data((GObject *) editor->selected, g_type_name(AGS_TYPE_MACHINE));
    rows = (machine != NULL) ? machine->audio->input_pads: 0;
  }else
    rows = 0;

  ags_meter_paint(meter, rows, nth_y, y0);
  */

  ags_meter_paint(meter);

  return(FALSE);
}
