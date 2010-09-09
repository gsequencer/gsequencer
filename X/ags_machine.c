#include "ags_machine.h"
#include "ags_machine_callbacks.h"

#include "ags_window.h"

GType ags_machine_get_type(void);
void ags_machine_class_init(AgsMachineClass *machine);
void ags_machine_init(AgsMachine *machine);
void ags_machine_connect(AgsMachine *machine);
void ags_machine_destroy(GtkObject *object);
void ags_machine_show(GtkWidget *widget);

GtkMenu* ags_machine_popup_new(AgsMachine *machine);

extern void ags_file_read_machine(AgsFile *file, AgsMachine *machine);
extern void ags_file_write_machine(AgsFile *file, AgsMachine *machine);

GType
ags_machine_get_type(void)
{
  static GType machine_type = 0;

  if (!machine_type){
    static const GtkTypeInfo machine_info = {
      "AgsMachine\0",
      sizeof(AgsMachine), /* base_init */
      sizeof(AgsMachineClass), /* base_finalize */
      (GtkClassInitFunc) ags_machine_class_init,
      (GtkObjectInitFunc) ags_machine_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      (GtkClassInitFunc) NULL,
    };

    machine_type = gtk_type_unique (GTK_TYPE_HANDLE_BOX, &machine_info);
  }

  return (machine_type);
}

void
ags_machine_class_init(AgsMachineClass *machine)
{
}

void
ags_machine_init(AgsMachine *machine)
{
  GtkFrame *frame;

  g_signal_connect_after((GObject *) machine, "parent_set\0",
			 G_CALLBACK(ags_machine_parent_set_callback), (gpointer) machine);

  machine->flags = 0;

  machine->name = NULL;

  frame = (GtkFrame *) gtk_frame_new(NULL);
  gtk_container_add((GtkContainer *) machine, (GtkWidget *) frame);

  machine->audio = ags_audio_new();
  machine->audio->machine = (GtkWidget *) machine;

  machine->output = NULL;
  machine->input = NULL;

  machine->popup = ags_machine_popup_new(machine);
  machine->properties = NULL;
  machine->rename = NULL;
}

void
ags_machine_connect(AgsMachine *machine)
{
  ags_audio_connect(machine->audio);

  g_signal_connect (G_OBJECT (machine), "destroy\0",
		    G_CALLBACK (ags_machine_destroy_callback), (gpointer) machine);

  g_signal_connect (G_OBJECT (machine), "button_press_event\0",
                    G_CALLBACK (ags_machine_button_press_callback), (gpointer) machine);
}

void
ags_machine_destroy(GtkObject *object)
{
  AgsMachine *machine;

  machine = (AgsMachine *) object;

  fprintf(stdout, "ags_machine_destroy\n\0");

  //  if(machine->name != NULL)
  //    free(machine->name);

  g_object_unref((GObject *) machine->audio);
}

void
ags_machine_show(GtkWidget *widget)
{
  AgsMachine *machine = (AgsMachine *) widget;
  AgsWindow *window;
  GtkFrame *frame;

  window = (AgsWindow *) gtk_widget_get_toplevel(widget);
  window->counter->everything++;

  frame = (GtkFrame *) gtk_container_get_children((GtkContainer *) machine)->data;
  gtk_widget_show_all((GtkWidget *) frame);
}

AgsMachine*
ags_machine_find_by_name(GList *list, char *name)
{
  while(list != NULL){
    if(!g_strcmp0(AGS_MACHINE(list->data)->name, name))
      return((AgsMachine *) list->data);

    list = list->next;
  }

  return(NULL);
}

AgsMachine*
ags_machine_new()
{
  AgsMachine *machine;

  machine = (AgsMachine *) g_object_new(AGS_TYPE_MACHINE, NULL);

  return(machine);
}

GtkMenu*
ags_machine_popup_new(AgsMachine *machine)
{
  GtkMenu *popup;
  GtkMenuItem *item;
  GList *list;

  popup = (GtkMenu *) gtk_menu_new();

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("move up\0"));
  gtk_menu_shell_append((GtkMenuShell *) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("move down\0"));
  gtk_menu_shell_append((GtkMenuShell *) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("hide\0"));
  gtk_menu_shell_append((GtkMenuShell *) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("show\0"));
  gtk_menu_shell_append((GtkMenuShell *) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("destroy\0"));
  gtk_menu_shell_append((GtkMenuShell *) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("rename\0"));
  gtk_menu_shell_append((GtkMenuShell *) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("properties\0"));
  gtk_menu_shell_append((GtkMenuShell *) popup, (GtkWidget*) item);

  list = gtk_container_get_children((GtkContainer *) popup);

  g_signal_connect((GObject*) list->data, "activate\0",
		   G_CALLBACK(ags_machine_popup_move_up_activate_callback), (gpointer) machine);
  list = list->next;

  g_signal_connect((GObject*) list->data, "activate\0",
		   G_CALLBACK(ags_machine_popup_move_down_activate_callback), (gpointer) machine);
  list = list->next;

  g_signal_connect((GObject*) list->data, "activate\0",
		   G_CALLBACK(ags_machine_popup_hide_activate_callback), (gpointer) machine);
  list = list->next;

  g_signal_connect((GObject*) list->data, "activate\0",
		   G_CALLBACK(ags_machine_popup_show_activate_callback), (gpointer) machine);
  list = list->next;

  g_signal_connect((GObject*) list->data, "activate\0",
		   G_CALLBACK(ags_machine_popup_destroy_activate_callback), (gpointer) machine);
  list = list->next;

  g_signal_connect((GObject*) list->data, "activate\0",
		   G_CALLBACK(ags_machine_popup_rename_activate_callback), (gpointer) machine);
  list = list->next;

  g_signal_connect((GObject*) list->data, "activate\0",
		   G_CALLBACK(ags_machine_popup_properties_activate_callback), (gpointer) machine);

  gtk_widget_show_all((GtkWidget*) popup);

  return(popup);
}
