#include "ags_meter_callbacks.h"

#include "../ags_editor.h"

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
