#include "ags_machine_resize_editor_callbacks.h"

int
ags_machine_resize_editor_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsMachineResizeEditor *machine_resize_editor)
{
  AgsMachineEditor *machine_editor;

  if(old_parent != NULL)
    return(0);

  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor((GtkWidget *) machine_resize_editor, AGS_TYPE_MACHINE_EDITOR);

  g_signal_connect_after((GObject *) machine_editor, "set_machine\0",
			 G_CALLBACK(ags_machine_resize_editor_set_machine_callback), machine_resize_editor);

  return(0);
}

void
ags_machine_resize_editor_set_machine_callback(AgsMachineEditor *machine_editor, AgsMachine *machine, AgsMachineResizeEditor *machine_resize_editor)
{
  gtk_spin_button_set_value(machine_resize_editor->audio_channels, (gdouble) machine->audio->audio_channels);
  gtk_spin_button_set_value(machine_resize_editor->output_pads, (gdouble) machine->audio->output_pads);
  gtk_spin_button_set_value(machine_resize_editor->input_pads, (gdouble) machine->audio->input_pads);
}

void
ags_machine_resize_editor_add_input_callback(AgsMachineEditor *machine_editor, AgsMachineResizeEditor *machine_resize_editor)
{
  gtk_spin_button_set_value(machine_resize_editor->input_pads, (gdouble) machine_editor->machine->audio->input_pads);
}

void
ags_machine_resize_editor_add_output_callback(AgsMachineEditor *machine_editor, AgsMachineResizeEditor *machine_resize_editor)
{
  gtk_spin_button_set_value(machine_resize_editor->output_pads, (gdouble) machine_editor->machine->audio->output_pads);
}

void
ags_machine_resize_editor_remove_input_callback(AgsMachineEditor *machine_editor, AgsMachineResizeEditor *machine_resize_editor)
{
  gtk_spin_button_set_value(machine_resize_editor->input_pads, (gdouble) machine_editor->machine->audio->input_pads);
}

void
ags_machine_resize_editor_remove_output_callback(AgsMachineEditor *machine_editor, AgsMachineResizeEditor *machine_resize_editor)
{
  gtk_spin_button_set_value(machine_resize_editor->output_pads, (gdouble) machine_editor->machine->audio->output_pads);
}
