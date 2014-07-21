#include <ags/X/editor/ags_machine_selector_callbacks.h>

#include <ags/X/editor/ags_machine_selection.h>
#include <ags/X/editor/ags_machine_radio_button.h>

void
ags_machine_selector_popup_add_tab_callback(GtkWidget *menu_item, AgsMachineSelector *machine_selector)
{
  //TODO:JK: implement me
}

void
ags_machine_selector_popup_remove_tab_callback(GtkWidget *menu_item, AgsMachineSelector *machine_selector)
{
  //TODO:JK: implement me
}

void
ags_machine_selector_popup_add_index_callback(GtkWidget *menu_item, AgsMachineSelector *machine_selector)
{
  AgsMachineRadioButton *machine_radio_button, *group;
  GList *list;

  list = gtk_container_get_children(GTK_CONTAINER(machine_selector));
  list = list->next;

  if(list == NULL){
    group = NULL;
  }else{
    group = AGS_MACHINE_RADIO_BUTTON(list->data);
  }

  machine_radio_button = (AgsMachineRadioButton *) g_object_new(AGS_TYPE_MACHINE_RADIO_BUTTON,
								"group\0", group,
								NULL);
  gtk_box_pack_start(GTK_BOX(machine_selector),
		     GTK_WIDGET(machine_radio_button),
		     FALSE, FALSE,
		     0);
  gtk_widget_show_all(machine_radio_button);
}

void
ags_machine_selector_popup_remove_index_callback(GtkWidget *menu_item, AgsMachineSelector *machine_selector)
{
  AgsMachineRadioButton *machine_radio_button;
  GList *list;

  list = gtk_container_get_children(GTK_CONTAINER(machine_selector));
  list = list->next;
  
  while(list != NULL){
    machine_radio_button = AGS_MACHINE_RADIO_BUTTON(list->data);

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(machine_radio_button))){
      gtk_widget_destroy(GTK_WIDGET(machine_radio_button));

      ags_machine_selector_changed(machine_selector,
				   NULL);

      break;
    }

    list = list->next;
  }
}

void
ags_machine_selector_popup_link_index_callback(GtkWidget *menu_item, AgsMachineSelector *machine_selector)
{
  AgsMachine *machine;
  AgsMachineSelection *machine_selection;
  AgsMachineRadioButton *machine_radio_button;
  GList *list;

  list = gtk_container_get_children(GTK_CONTAINER(machine_selector));
  list = list->next;
  
  while(list != NULL){
    machine_radio_button = AGS_MACHINE_RADIO_BUTTON(list->data);

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(machine_radio_button))){
      break;
    }

    list = list->next;
  }

  machine_selection = (AgsMachineSelection *) ags_machine_selection_new(gtk_widget_get_toplevel(machine_selector));
  machine = ags_machine_selection_run(machine_selection);
  g_object_set(G_OBJECT(machine_radio_button),
	       "machine\0", machine,
	       NULL);
}
