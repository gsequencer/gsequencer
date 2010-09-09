#include "ags_machine_link_editor.h"
#include "ags_machine_link_editor_callbacks.h"

#include "ags_machine.h"
#include "ags_machine_editor.h"

#include "../audio/ags_channel.h"
#include "../audio/ags_input.h"
#include "../audio/ags_output.h"

int
ags_machine_link_editor_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsMachineLinkEditor *machine_link_editor)
{
  AgsMachineEditor *machine_editor;

  if(old_parent != NULL)
    return(0);

  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor((GtkWidget *) machine_link_editor, AGS_TYPE_MACHINE_EDITOR);

  g_signal_connect_after((GObject *) machine_editor, "set_machine\0",
			 G_CALLBACK(ags_machine_link_editor_set_machine_callback), machine_link_editor);

  return(0);
}

void
ags_machine_link_editor_set_machine_callback(AgsMachineEditor *machine_editor, AgsMachine *machine, AgsMachineLinkEditor *machine_link_editor)
{
  GtkMenu *menu_input, *menu_output;
  GtkMenuItem *item;
  GList *list;
  gboolean found_own;

  gtk_spin_button_set_range(machine_link_editor->input_first_line, 0.0, (gdouble) machine->audio->input_lines - 1);
  gtk_spin_button_set_range(machine_link_editor->input_n, 0.0, (gdouble) machine->audio->input_lines - 1);

  gtk_spin_button_set_range(machine_link_editor->output_first_line, 0.0, (gdouble) machine->audio->output_lines - 1);
  gtk_spin_button_set_range(machine_link_editor->output_n, 0.0, (gdouble) machine->audio->output_lines - 1);

  list = gtk_container_get_children((GtkContainer *) GTK_WIDGET(machine)->parent);

  menu_input = (GtkMenu *) machine_link_editor->input_link->menu;
  menu_output = (GtkMenu *) machine_link_editor->output_link->menu;
  found_own = FALSE;

  while(list != NULL){
    if(!found_own && list->data == (gpointer) (machine_editor->machine)){
      found_own = TRUE;
      list = list->next;
      continue;
    }

    item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strconcat(G_OBJECT_TYPE_NAME(G_OBJECT(list->data)), ": \0", AGS_MACHINE(list->data)->name, NULL));
    g_object_set_data((GObject *) item, (char *) g_type_name(AGS_TYPE_MACHINE), list->data);
    gtk_menu_shell_append((GtkMenuShell *) menu_input, (GtkWidget *) item);

    item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strconcat(G_OBJECT_TYPE_NAME(G_OBJECT(list->data)), ": \0", AGS_MACHINE(list->data)->name, NULL));
    g_object_set_data((GObject *) item, (char *) g_type_name(AGS_TYPE_MACHINE), list->data);
    gtk_menu_shell_append((GtkMenuShell *) menu_output, (GtkWidget *) item);

    list = list->next;
  }
}

int
ags_machine_link_editor_input_first_line_callback(GtkSpinButton *spin_button, AgsMachineLinkEditor *machine_link_editor)
{
  AgsMachine *machine_link;
  AgsMachineEditor *machine_editor;
  guint line_n, link_n;

  machine_link = (AgsMachine *) g_object_get_data((GObject *) machine_link_editor->input_link->menu_item, (char *) g_type_name(AGS_TYPE_MACHINE));
  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor((GtkWidget *) machine_link_editor, AGS_TYPE_MACHINE_EDITOR);

  line_n = machine_editor->machine->audio->input_lines - (guint) spin_button->adjustment->value - 1;

  if(machine_link == NULL){
    gtk_spin_button_set_range(machine_link_editor->input_n, 0.0, (gdouble) line_n);
  }else{
    link_n = machine_link->audio->output_lines - (guint) machine_link_editor->input_first_link->adjustment->value - 1;
    gtk_spin_button_set_range(machine_link_editor->input_n, 0.0, (gdouble) ((line_n < link_n) ? line_n: link_n));
  }

  return(0);
}

int
ags_machine_link_editor_input_link_callback(GtkOptionMenu *option, AgsMachineLinkEditor *machine_link_editor)
{
  AgsMachine *machine_link;
  AgsMachineEditor *machine_editor;
  guint line_n, link_n;

  machine_link = (AgsMachine *) g_object_get_data((GObject *) option->menu_item, (char *) g_type_name(AGS_TYPE_MACHINE));
  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor((GtkWidget *) machine_link_editor, AGS_TYPE_MACHINE_EDITOR);

  line_n = machine_editor->machine->audio->input_lines - (guint) machine_link_editor->input_first_line->adjustment->value - 1;

  if(machine_link == NULL){
    gtk_spin_button_set_range(machine_link_editor->input_first_link, 0.0, 0.0);

    gtk_spin_button_set_range(machine_link_editor->input_n, 0.0, (gdouble) line_n);
  }else{
    gtk_spin_button_set_range(machine_link_editor->input_first_link, 0.0, (gdouble) machine_link->audio->output_lines);

    link_n = machine_link->audio->output_lines - (guint) machine_link_editor->input_first_link->adjustment->value - 1;
    gtk_spin_button_set_range(machine_link_editor->input_n, 0.0, (gdouble) ((line_n < link_n) ? line_n: link_n));
  }

  return(0);
}

int
ags_machine_link_editor_input_first_link_callback(GtkSpinButton *spin_button, AgsMachineLinkEditor *machine_link_editor)
{
  AgsMachine *machine_link;
  AgsMachineEditor *machine_editor;
  guint line_n, link_n;

  machine_link = (AgsMachine *) g_object_get_data((GObject *) machine_link_editor->input_link->menu_item, (char *) g_type_name(AGS_TYPE_MACHINE));
  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor((GtkWidget *) machine_link_editor, AGS_TYPE_MACHINE_EDITOR);
  
  line_n = machine_editor->machine->audio->input_lines - (guint) machine_link_editor->input_first_line->adjustment->value - 1;

  if(machine_link == NULL){
    gtk_spin_button_set_range(machine_link_editor->input_n, 0.0, (gdouble) line_n);
  }else{
    link_n = machine_link->audio->output_lines - (guint) spin_button->adjustment->value - 1;
    gtk_spin_button_set_range(machine_link_editor->input_n, 0.0, (gdouble) ((line_n < link_n) ? line_n: link_n));
  }

  return(0);
}

int
ags_machine_link_editor_output_first_line_callback(GtkSpinButton *spin_button, AgsMachineLinkEditor *machine_link_editor)
{
  AgsMachine *machine_link;
  AgsMachineEditor *machine_editor;
  guint line_n, link_n;

  machine_link = (AgsMachine *) g_object_get_data((GObject *) machine_link_editor->output_link->menu_item, (char *) g_type_name(AGS_TYPE_MACHINE));
  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor((GtkWidget *) machine_link_editor, AGS_TYPE_MACHINE_EDITOR);

  line_n = machine_editor->machine->audio->output_lines - (guint) spin_button->adjustment->value - 1;

  if(machine_link == NULL){
    gtk_spin_button_set_range(machine_link_editor->output_n, 0.0, (gdouble) line_n);
  }else{
    link_n = machine_link->audio->input_lines - (guint) machine_link_editor->output_first_link->adjustment->value - 1;
    gtk_spin_button_set_range(machine_link_editor->output_n, 0.0, (gdouble) ((line_n < link_n) ? line_n: link_n));
  }

  return(0);
}

int
ags_machine_link_editor_output_link_callback(GtkOptionMenu *option, AgsMachineLinkEditor *machine_link_editor)
{
  AgsMachine *machine_link;
  AgsMachineEditor *machine_editor;
  guint line_n, link_n;

  machine_link = (AgsMachine *) g_object_get_data((GObject *) option->menu_item, (char *) g_type_name(AGS_TYPE_MACHINE));
  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor((GtkWidget *) machine_link_editor, AGS_TYPE_MACHINE_EDITOR);

  line_n = machine_editor->machine->audio->output_lines - (guint) machine_link_editor->output_first_line->adjustment->value - 1;

  if(machine_link == NULL){
    gtk_spin_button_set_range(machine_link_editor->output_first_link, 0.0, 0.0);

    gtk_spin_button_set_range(machine_link_editor->output_n, 0.0, (gdouble) line_n);
  }else{
    gtk_spin_button_set_range(machine_link_editor->output_first_link, 0.0, (gdouble) machine_link->audio->input_lines);

    link_n = machine_link->audio->input_lines - (guint) machine_link_editor->output_first_link->adjustment->value - 1;
    gtk_spin_button_set_range(machine_link_editor->output_n, 0.0, (gdouble) ((line_n < link_n) ? line_n: link_n));
  }

  return(0);
}

int
ags_machine_link_editor_output_first_link_callback(GtkSpinButton *spin_button, AgsMachineLinkEditor *machine_link_editor)
{
  AgsMachine *machine_link;
  AgsMachineEditor *machine_editor;
  guint line_n, link_n;

  machine_link = (AgsMachine *) g_object_get_data((GObject *) machine_link_editor->output_link->menu_item, (char *) g_type_name(AGS_TYPE_MACHINE));
  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor((GtkWidget *) machine_link_editor, AGS_TYPE_MACHINE_EDITOR);
  
  line_n = machine_editor->machine->audio->output_lines - (guint) machine_link_editor->output_first_line->adjustment->value - 1;

  if(machine_link == NULL){
    gtk_spin_button_set_range(machine_link_editor->output_n, 0.0, (gdouble) line_n);
  }else{
    link_n = machine_link->audio->input_lines - (guint) spin_button->adjustment->value - 1;
    gtk_spin_button_set_range(machine_link_editor->output_n, 0.0, (gdouble) ((line_n < link_n) ? line_n: link_n));
  }

  return(0);
}

void
ags_machine_link_editor_add_input_callback(AgsMachineEditor *machine_editor, AgsMachineLinkEditor *machine_link_editor)
{
  AgsMachine *machine_link;
  guint line_n, link_n;

  machine_link = (AgsMachine *) g_object_get_data((GObject *) machine_link_editor->input_link->menu_item, (char *) g_type_name(AGS_TYPE_MACHINE));

  gtk_spin_button_set_range(machine_link_editor->input_first_line, 0.0, (gdouble) machine_editor->machine->audio->input_lines);

  line_n = machine_editor->machine->audio->input_lines - (guint) machine_link_editor->input_first_line->adjustment->value + 1;

  if(machine_link == NULL){
    gtk_spin_button_set_range(machine_link_editor->input_first_line, 0.0, (gdouble) line_n);
  }else{
    link_n = machine_link->audio->output_lines - (guint) machine_link_editor->input_first_link->adjustment->value + 1;
    gtk_spin_button_set_range(machine_link_editor->input_n, 0.0, (gdouble) ((line_n < link_n) ? line_n: link_n));
  }
}

void
ags_machine_link_editor_add_output_callback(AgsMachineEditor *machine_editor, AgsMachineLinkEditor *machine_link_editor)
{
  AgsMachine *machine_link;
  guint line_n, link_n;

  machine_link = (AgsMachine *) g_object_get_data((GObject *) machine_link_editor->input_link->menu_item, (char *) g_type_name(AGS_TYPE_MACHINE));

  gtk_spin_button_set_range(machine_link_editor->input_first_line, 0.0, (gdouble) machine_editor->machine->audio->output_lines);

  line_n = machine_editor->machine->audio->output_lines - (guint) machine_link_editor->output_first_line->adjustment->value + 1;

  if(machine_link == NULL){
    gtk_spin_button_set_range(machine_link_editor->output_first_line, 0.0, (gdouble) line_n);
  }else{
    link_n = machine_link->audio->input_lines - (guint) machine_link_editor->output_first_link->adjustment->value + 1;
    gtk_spin_button_set_range(machine_link_editor->output_n, 0.0, (gdouble) ((line_n < link_n) ? line_n: link_n));
  }
}

void
ags_machine_link_editor_remove_input_callback(AgsMachineEditor *machine_editor, AgsMachineLinkEditor *machine_link_editor)
{
  AgsMachine *machine_link;
  guint line_n, link_n;

  machine_link = (AgsMachine *) g_object_get_data((GObject *) machine_link_editor->input_link->menu_item, (char *) g_type_name(AGS_TYPE_MACHINE));

  gtk_spin_button_set_range(machine_link_editor->input_first_line, 0.0, (gdouble) machine_editor->machine->audio->input_lines);

  line_n = machine_editor->machine->audio->input_lines - (guint) machine_link_editor->input_first_line->adjustment->value + 1;

  if(machine_link == NULL){
    gtk_spin_button_set_range(machine_link_editor->input_first_line, 0.0, (gdouble) line_n);
  }else{
    link_n = machine_link->audio->output_lines - (guint) machine_link_editor->input_first_link->adjustment->value + 1;
    gtk_spin_button_set_range(machine_link_editor->input_n, 0.0, (gdouble) ((line_n < link_n) ? line_n: link_n));
  }
}

void
ags_machine_link_editor_remove_output_callback(AgsMachineEditor *machine_editor, AgsMachineLinkEditor *machine_link_editor)
{
  AgsMachine *machine_link;
  guint line_n, link_n;

  machine_link = (AgsMachine *) g_object_get_data((GObject *) machine_link_editor->input_link->menu_item, (char *) g_type_name(AGS_TYPE_MACHINE));

  gtk_spin_button_set_range(machine_link_editor->input_first_line, 0.0, (gdouble) machine_editor->machine->audio->output_lines);

  line_n = machine_editor->machine->audio->output_lines - (guint) machine_link_editor->output_first_line->adjustment->value + 1;

  if(machine_link == NULL){
    gtk_spin_button_set_range(machine_link_editor->output_first_line, 0.0, (gdouble) line_n);
  }else{
    link_n = machine_link->audio->input_lines - (guint) machine_link_editor->output_first_link->adjustment->value + 1;
    gtk_spin_button_set_range(machine_link_editor->output_n, 0.0, (gdouble) ((line_n < link_n) ? line_n: link_n));
  }
}

void
ags_machine_link_editor_resize_audio_channels_callback(AgsMachineResizeEditor *machine_resize_editor, guint audio_channels, guint audio_channels_old, AgsMachineLinkEditor *machine_link_editor)
{
  AgsMachine *machine_link;
  AgsMachineEditor *machine_editor;
  guint line_n, link_n;

  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor((GtkWidget *) machine_link_editor, AGS_TYPE_MACHINE_EDITOR);

  /* AgsInput */
  machine_link = (AgsMachine *) g_object_get_data((GObject *) machine_link_editor->output_link->menu_item, (char *) g_type_name(AGS_TYPE_MACHINE));

  gtk_spin_button_set_range(machine_link_editor->input_first_line, 0.0, machine_editor->machine->audio->input_lines);

  line_n = machine_editor->machine->audio->output_lines - (guint) machine_link_editor->output_first_line->adjustment->value + 1;

  if(machine_link == NULL){
    gtk_spin_button_set_range(machine_link_editor->input_n, 0.0, (gdouble) line_n);
  }else{
    link_n = machine_link->audio->output_lines - (guint) machine_link_editor->input_first_link->adjustment->value + 1;
    gtk_spin_button_set_range(machine_link_editor->input_n, 0.0, (gdouble) ((line_n < link_n) ? line_n: link_n));
  }

  /* AgsOutput */
  machine_link = (AgsMachine *) g_object_get_data((GObject *) machine_link_editor->input_link->menu_item, (char *) g_type_name(AGS_TYPE_MACHINE));

  gtk_spin_button_set_range(machine_link_editor->output_first_line, 0.0, machine_editor->machine->audio->output_lines);

  line_n = machine_editor->machine->audio->output_lines - (guint) machine_link_editor->output_first_line->adjustment->value + 1;

  if(machine_link == NULL){
    gtk_spin_button_set_range(machine_link_editor->output_n, 0.0, (gdouble) line_n);
  }else{
    link_n = machine_link->audio->input_lines - (guint) machine_link_editor->output_first_link->adjustment->value + 1;
    gtk_spin_button_set_range(machine_link_editor->output_n, 0.0, (gdouble) ((line_n < link_n) ? line_n: link_n));
  }
}

void
ags_machine_link_editor_resize_input_pads_callback(AgsMachineResizeEditor *machine_resize_editor, guint pads, guint pads_old, AgsMachineLinkEditor *machine_link_editor)
{
  AgsMachine *machine_link;
  AgsMachineEditor *machine_editor;
  guint line_n, link_n;

  machine_link = (AgsMachine *) g_object_get_data((GObject *) machine_link_editor->input_link->menu_item, (char *) g_type_name(AGS_TYPE_MACHINE));
  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor((GtkWidget *) machine_link_editor, AGS_TYPE_MACHINE_EDITOR);

  gtk_spin_button_set_range(machine_link_editor->input_first_line, 0.0, machine_editor->machine->audio->input_lines);
  
  line_n = machine_editor->machine->audio->input_lines - (guint) machine_link_editor->input_first_line->adjustment->value + 1;

  if(machine_link == NULL){
    gtk_spin_button_set_range(machine_link_editor->input_n, 0.0, (gdouble) line_n);
  }else{
    link_n = machine_link->audio->output_lines - (guint) machine_link_editor->input_first_link->adjustment->value + 1;
    gtk_spin_button_set_range(machine_link_editor->input_n, 0.0, (gdouble) ((line_n < link_n) ? line_n: link_n));
  }
}

void
ags_machine_link_editor_resize_output_pads_callback(AgsMachineResizeEditor *machine_resize_editor, guint pads, guint pads_old, AgsMachineLinkEditor *machine_link_editor)
{
  AgsMachine *machine_link;
  AgsMachineEditor *machine_editor;
  guint line_n, link_n;

  machine_link = (AgsMachine *) g_object_get_data((GObject *) machine_link_editor->output_link->menu_item, (char *) g_type_name(AGS_TYPE_MACHINE));
  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor((GtkWidget *) machine_link_editor, AGS_TYPE_MACHINE_EDITOR);
  
  gtk_spin_button_set_range(machine_link_editor->output_first_line, 0.0, machine_editor->machine->audio->output_lines);

  line_n = machine_editor->machine->audio->output_lines - (guint) machine_link_editor->output_first_line->adjustment->value + 1;

  if(machine_link == NULL){
    gtk_spin_button_set_range(machine_link_editor->output_n, 0.0, (gdouble) line_n);
  }else{
    link_n = machine_link->audio->input_lines - (guint) machine_link_editor->output_first_link->adjustment->value + 1;
    gtk_spin_button_set_range(machine_link_editor->output_n, 0.0, (gdouble) ((line_n < link_n) ? line_n: link_n));
  }
}
