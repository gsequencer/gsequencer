#include <ags/X/ags_machine_editor_callbacks.h>

#include <ags/object/ags_applicable.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>

#include <ags/X/ags_machine.h>
#include <ags/X/ags_pad_editor.h>
#include <ags/X/ags_line_editor.h>
#include <ags/X/ags_link_editor.h>

int
ags_machine_editor_destroy_callback(GtkObject *object, AgsMachineEditor *machine_editor)
{
  ags_machine_editor_destroy(object);
  return(0);
}

int
ags_machine_editor_show_callback(GtkWidget *widget, AgsMachineEditor *machine_editor)
{
  ags_machine_editor_show(widget);
  return(0);
}

int
ags_machine_editor_switch_page_callback(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, AgsMachineEditor *machine_editor)
{
  if(page_num == 0 || page_num == 1){
    gtk_widget_show((GtkWidget *) machine_editor->add);
    gtk_widget_show((GtkWidget *) machine_editor->remove);
  }else{
    gtk_widget_hide((GtkWidget *) machine_editor->add);
    gtk_widget_hide((GtkWidget *) machine_editor->remove);
  }
}

int
ags_machine_editor_add_callback(GtkWidget *button, AgsMachineEditor *machine_editor)
{
  /*
  if(gtk_notebook_get_current_page(machine_editor->notebook) == 0)
    g_signal_emit_by_name((GObject *) machine_editor, "add_output\0", NULL);
  else
    g_signal_emit_by_name((GObject *) machine_editor, "add_input\0", NULL);
  */

  return(0);
}

int
ags_machine_editor_remove_callback(GtkWidget *button, AgsMachineEditor *machine_editor)
{
  /*
  if(gtk_notebook_get_current_page(machine_editor->notebook) == 0)
    g_signal_emit_by_name((GObject *) machine_editor, "remove_output\0", NULL);
  else
    g_signal_emit_by_name((GObject *) machine_editor, "remove_input\0", NULL);
  */

  return(0);
}

int
ags_machine_editor_apply_callback(GtkWidget *widget, AgsMachineEditor *machine_editor)
{
  ags_applicable_apply(AGS_APPLICABLE(machine_editor));
  ags_applicable_reset(AGS_APPLICABLE(machine_editor));

  return(0);
}

int
ags_machine_editor_ok_callback(GtkWidget *widget, AgsMachineEditor *machine_editor)
{
  //  ags_applicable_set_update(AGS_APPLICABLE(machine_editor), FALSE);
  ags_applicable_apply(AGS_APPLICABLE(machine_editor));

  gtk_widget_destroy((GtkWidget *) machine_editor);

  return(0);
}

int
ags_machine_editor_cancel_callback(GtkWidget *widget, AgsMachineEditor *machine_editor)
{
  gtk_widget_destroy((GtkWidget *) machine_editor);

  return(0);
}
