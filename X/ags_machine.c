#include "ags_machine.h"
#include "ags_machine_callbacks.h"

#include "../object/ags_connectable.h"

#include "ags_window.h"

void ags_machine_class_init(AgsMachineClass *machine);
void ags_machine_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_machine_init(AgsMachine *machine);
void ags_machine_connect(AgsConnectable *connectable);
void ags_machine_disconnect(AgsConnectable *connectable);
void ags_machine_destroy(GtkObject *object);
void ags_machine_show(GtkWidget *widget);

GtkMenu* ags_machine_popup_new(AgsMachine *machine);

extern void ags_file_read_machine(AgsFile *file, AgsMachine *machine);
extern void ags_file_write_machine(AgsFile *file, AgsMachine *machine);

GType
ags_machine_get_type(void)
{
  static GType ags_type_machine = 0;

  if(!ags_type_machine){
    static const GTypeInfo ags_machine_info = {
      sizeof (AgsMachineClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_machine_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMachine),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_machine_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_machine_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_machine = g_type_register_static(GTK_TYPE_HANDLE_BOX,
					      "AgsMachine\0", &ags_machine_info,
					      0);
    
    g_type_add_interface_static(ags_type_machine,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_machine);
}

void
ags_machine_class_init(AgsMachineClass *machine)
{
}

void
ags_machine_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_machine_connect;
  connectable->disconnect = ags_machine_disconnect;
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
ags_machine_connect(AgsConnectable *connectable)
{
  AgsMachine *machine;
  GList *pad_list;

  /* AgsMachine */
  machine = AGS_MACHINE(connectable);

  /* AgsAudio */
  ags_connectable_connect(AGS_CONNECTABLE(machine->audio));

  /* GtkObject */
  g_signal_connect(G_OBJECT (machine), "destroy\0",
		   G_CALLBACK(ags_machine_destroy_callback), (gpointer) machine);

  /* GtkWidget */
  g_signal_connect(G_OBJECT (machine), "button_press_event\0",
		   G_CALLBACK(ags_machine_button_press_callback), (gpointer) machine);

  /* AgsPad - input */
  if(machine->input != NULL){
    pad_list = gtk_container_get_children(GTK_CONTAINER(machine->input));

    while(pad_list != NULL){
      ags_connectable_connect(AGS_CONNECTABLE(pad_list->data));
      
      pad_list = pad_list->next;
    }
  }

  /* AgsPad - output */
  if(machine->output != NULL){
    pad_list = gtk_container_get_children(GTK_CONTAINER(machine->output));
    
    while(pad_list != NULL){
      ags_connectable_connect(AGS_CONNECTABLE(pad_list->data));
      
      pad_list = pad_list->next;
    }
  }
}

void
ags_machine_disconnect(AgsConnectable *connectable)
{
  /* empty */
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

GtkListStore*
ags_machine_get_possible_links(AgsMachine *machine)
{
  GtkListStore *model;
  GtkTreeIter iter;
  GList *list;

  model = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
		    
  gtk_list_store_append(model, &iter);
  gtk_list_store_set(model, &iter,
		     0, "NULL\0",
		     1, NULL,
		     -1);

  if(GTK_WIDGET(machine)->parent != NULL){
    list = gtk_container_get_children(GTK_CONTAINER(GTK_WIDGET(machine)->parent));

    while(list != NULL){
      if(list->data != machine){
	gtk_list_store_append(model, &iter);
	gtk_list_store_set(model, &iter,
			   0, g_strdup_printf("%s: %s\0", 
					      G_OBJECT_TYPE_NAME(G_OBJECT(list->data)),
					      AGS_MACHINE(list->data)->name),
			   1, list->data,
			   -1);
      }

      list = list->next;
    }
  }

  return(model);
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
