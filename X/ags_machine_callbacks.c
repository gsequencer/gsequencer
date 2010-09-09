#include "ags_machine_callbacks.h"

#include "ags_window.h"
#include "ags_machine_editor.h"

int ags_machine_popup_rename_response_callback(GtkWidget *widget, gint response, AgsMachine *machine);

#define AGS_RENAME_ENTRY "AgsRenameEntry\0"

int
ags_machine_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsMachine *machine)
{
  AgsWindow *window;

  if(old_parent != NULL)
    return(0);

  window = AGS_WINDOW(gtk_widget_get_toplevel(widget));

  ags_audio_set_devout(machine->audio, G_OBJECT(window->navigation->devout));

  return(0);
}

int
ags_machine_destroy_callback(GtkObject *object, AgsMachine *machine)
{
  ags_machine_destroy(object);

  return(0);
}

int
ags_machine_button_press_callback(GtkWidget *handle_box, GdkEventButton *event, AgsMachine *machine)
{
  AgsWindow *window = AGS_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET(handle_box)));

  if(event->button == 3)
    gtk_menu_popup (GTK_MENU (machine->popup),
                    NULL, NULL, NULL, NULL,
                    event->button, event->time);
  else if(event->button == 1)
    window->selected = machine;

  return(0);
}

int
ags_machine_popup_move_up_activate_callback(GtkWidget *widget, AgsMachine *machine)
{
  GValue val={0,};

  g_value_init (&val, G_TYPE_INT);

  gtk_container_child_get_property(GTK_CONTAINER(GTK_WIDGET(machine)->parent),
				   GTK_WIDGET(machine),
				   "position", &val);

  if(g_value_get_int (&val) > 0){
    gtk_box_reorder_child(GTK_BOX(GTK_WIDGET(machine)->parent),
			  GTK_WIDGET(machine),
			  g_value_get_int (&val) - 1);
  }

  g_value_unset (&val);

  return(0);
}

int
ags_machine_popup_move_down_activate_callback(GtkWidget *widget, AgsMachine *machine)
{
  GValue val={0,};

  g_value_init (&val, G_TYPE_INT);

  gtk_container_child_get_property(GTK_CONTAINER(GTK_WIDGET(machine)->parent),
				   GTK_WIDGET(machine),
				   "position", &val);

  if(g_value_get_int (&val) < g_list_length(gtk_container_get_children((GtkContainer *) GTK_WIDGET(machine)->parent)) - 1){
    gtk_box_reorder_child(GTK_BOX(GTK_WIDGET(machine)->parent),
			  GTK_WIDGET(machine),
			  g_value_get_int (&val) + 1);
  }

  g_value_unset (&val);

  return(0);
}

int
ags_machine_popup_hide_activate_callback(GtkWidget *widget, AgsMachine *machine)
{
  gtk_widget_hide(GTK_BIN(gtk_container_get_children((GtkContainer *) GTK_WIDGET(machine))->data)->child);

  return(0);
}

int
ags_machine_popup_show_activate_callback(GtkWidget *widget, AgsMachine *machine)
{
  gtk_widget_show(GTK_BIN(gtk_container_get_children((GtkContainer *) GTK_WIDGET(machine))->data)->child);

  return(0);
}

int
ags_machine_popup_destroy_activate_callback(GtkWidget *widget, AgsMachine *machine)
{
  gtk_widget_destroy((GtkWidget *) machine);

  return(0);
}

int
ags_machine_popup_rename_activate_callback(GtkWidget *widget, AgsMachine *machine)
{
  GtkDialog *dialog;
  GtkEntry *entry;

  dialog = (GtkDialog *) gtk_dialog_new_with_buttons(g_strdup("rename\0"),
						     (GtkWindow *) gtk_widget_get_toplevel(GTK_WIDGET(machine)),
						     GTK_DIALOG_DESTROY_WITH_PARENT,
						     GTK_STOCK_OK,
						     GTK_RESPONSE_ACCEPT,
						     GTK_STOCK_CANCEL,
						     GTK_RESPONSE_REJECT,
						     NULL);

  entry = (GtkEntry *) gtk_entry_new();
  gtk_entry_set_text(entry, machine->name);
  gtk_box_pack_start((GtkBox *) dialog->vbox, (GtkWidget *) entry, FALSE, FALSE, 0);

  gtk_widget_show_all((GtkWidget *) dialog);

  g_signal_connect((GObject *) dialog, "response\0",
		   G_CALLBACK(ags_machine_popup_rename_response_callback), (gpointer) machine);

  return(0);
}

int
ags_machine_popup_rename_response_callback(GtkWidget *widget, gint response, AgsMachine *machine)
{
  gchar *text;

  if(response == GTK_RESPONSE_ACCEPT){
    if(machine->name != NULL)
      free(machine->name);

    text = gtk_editable_get_chars(GTK_EDITABLE(gtk_container_get_children((GtkContainer *) GTK_DIALOG(widget)->vbox)->data), 0, -1);
    machine->name = text;

    gtk_frame_set_label((GtkFrame *) gtk_container_get_children((GtkContainer *) machine)->data, g_strconcat(G_OBJECT_TYPE_NAME(machine), ": \0", text, NULL));
    g_free(text);
  }

  gtk_widget_destroy(widget);

  return(0);
}

int
ags_machine_popup_properties_activate_callback(GtkWidget *widget, AgsMachine *machine)
{
  machine->properties = (GtkDialog *) ags_machine_editor_new();
  gtk_window_set_default_size((GtkWindow *) machine->properties, -1, 400);
  g_signal_emit_by_name(G_OBJECT(machine->properties), "set_machine\0", machine);
  ags_machine_editor_connect((AgsMachineEditor *) machine->properties);
  gtk_widget_show((GtkWidget *) machine->properties);

  return(0);
}
