#include <ags/X/editor/ags_machine_selector_callbacks.h>

#include <ags/X/editor/ags_machine_selection.h>
#include <ags/X/editor/ags_machine_radio_button.h>

void ags_machine_selector_selection_response(GtkWidget *machine_selection,
					     gint response,
					     AgsMachineSelector *machine_selector);

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
  GList *list, *list_start;

  list_start = 
    list = gtk_container_get_children(GTK_CONTAINER(machine_selector));
  list = list->next;

  if(list == NULL){
    group = NULL;
  }else{
    group = AGS_MACHINE_RADIO_BUTTON(list->data);
  }

  g_list_free(list_start);

  machine_radio_button = (AgsMachineRadioButton *) g_object_new(AGS_TYPE_MACHINE_RADIO_BUTTON,
								"group\0", group,
								NULL);
  gtk_box_pack_start(GTK_BOX(machine_selector),
		     GTK_WIDGET(machine_radio_button),
		     FALSE, FALSE,
		     0);
  g_signal_connect_after(G_OBJECT(machine_radio_button), "clicked\0",
			 G_CALLBACK(ags_machine_selector_radio_changed), machine_selector);
  gtk_widget_show_all(machine_radio_button);
}

void
ags_machine_selector_popup_remove_index_callback(GtkWidget *menu_item, AgsMachineSelector *machine_selector)
{
  AgsMachineRadioButton *machine_radio_button;
  GList *list, *list_start;

  list_start = 
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

  g_list_free(list_start);
}

void
ags_machine_selector_popup_link_index_callback(GtkWidget *menu_item, AgsMachineSelector *machine_selector)
{
  AgsMachine *machine;
  AgsMachineSelection *machine_selection;
  AgsMachineRadioButton *machine_radio_button;

  machine_selection = (AgsMachineSelection *) ags_machine_selection_new(gtk_widget_get_toplevel(machine_selector));
  ags_machine_selection_load_default(machine_selection);
  g_signal_connect(G_OBJECT(machine_selection), "response\0",
		   G_CALLBACK(ags_machine_selector_selection_response), machine_selector);
  gtk_widget_show_all(machine_selection);
}

void
ags_machine_selector_selection_response(GtkWidget *machine_selection,
					gint response,
					AgsMachineSelector *machine_selector)
{
  AgsMachine *machine;
  AgsMachineRadioButton *machine_radio_button;
  GtkVBox *vbox;
  GtkContainer *content_area;
  GList *list, *list_start, *index, *index_start;;

  if(response == GTK_RESPONSE_ACCEPT){

    list_start = 
      list = gtk_container_get_children(GTK_CONTAINER(machine_selector));
    list = list->next;
  
    while(list != NULL){
      machine_radio_button = AGS_MACHINE_RADIO_BUTTON(list->data);

      if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(machine_radio_button))){
	break;
      }

      list = list->next;
    }

    g_list_free(list_start);

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(machine_selection));
    machine = NULL;

    vbox = gtk_container_get_children(content_area)->data;

    if(response == GTK_RESPONSE_ACCEPT){
      list_start = 
	list = gtk_container_get_children(GTK_CONTAINER(AGS_MACHINE_SELECTION(machine_selection)->window->machines));
      index_start = 
	index = gtk_container_get_children(vbox);

      while(index != NULL){
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(index->data))){
	  machine = AGS_MACHINE(list->data);
	
	  break;
	}

	index = index->next;
	list = list->next;
      }

      g_list_free(list_start);
      g_list_free(index_start);
    }

    g_object_set(G_OBJECT(machine_radio_button),
		 "machine\0", machine,
		 NULL);
  }

  gtk_widget_destroy(machine_selection);
}

void
ags_machine_selector_radio_changed(GtkWidget *radio_button, AgsMachineSelector *machine_selector)
{
  ags_machine_selector_changed(machine_selector, AGS_MACHINE_RADIO_BUTTON(radio_button)->machine);
}
