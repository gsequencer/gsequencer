/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <ags/X/editor/ags_machine_selector_callbacks.h>

#include <ags/X/editor/ags_machine_selection.h>
#include <ags/X/editor/ags_machine_radio_button.h>

#include <ags/X/machine/ags_drum.h>
#include <ags/X/machine/ags_matrix.h>
#include <ags/X/machine/ags_synth.h>
#include <ags/X/machine/ags_ffplayer.h>

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
  gtk_widget_show_all((GtkWidget *) machine_radio_button);
}

void
ags_machine_selector_popup_remove_index_callback(GtkWidget *menu_item, AgsMachineSelector *machine_selector)
{
  AgsEditor *editor;
  AgsMachineRadioButton *machine_radio_button;
  GList *list, *list_start;

  editor = gtk_widget_get_ancestor(machine_selector,
				   AGS_TYPE_EDITOR);

  /* emit changed */
  list_start = 
    list = gtk_container_get_children(GTK_CONTAINER(machine_selector));
  list = list->next;
  
  while(list != NULL){
    machine_radio_button = AGS_MACHINE_RADIO_BUTTON(list->data);

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(machine_radio_button))){
      ags_machine_selector_changed(machine_selector,
				   NULL);

      break;
    }

    list = list->next;
  }

  g_list_free(list_start);
  
  /* destroy edit widgets */
  if(machine_radio_button->machine != NULL){
    list = editor->editor_child;

    while(list != NULL){
      if(AGS_EDITOR_CHILD(list->data) == machine_radio_button->machine){
	gtk_widget_destroy(AGS_EDITOR_CHILD(list->data)->notebook);
	gtk_widget_destroy(AGS_EDITOR_CHILD(list->data)->meter);
	gtk_widget_destroy(AGS_EDITOR_CHILD(list->data)->edit_widget);
	
	break;
      }
    
      list = list->next;
    }
  }
  
  /**/
  gtk_widget_destroy(GTK_WIDGET(machine_radio_button));
}

void
ags_machine_selector_popup_link_index_callback(GtkWidget *menu_item, AgsMachineSelector *machine_selector)
{
  AgsWindow *window;
  AgsMachine *machine;
  AgsMachineSelection *machine_selection;
  AgsMachineRadioButton *machine_radio_button;
  GList *list;

  list = gtk_window_list_toplevels();

  while(list != NULL && !AGS_IS_WINDOW(list->data)) list = list->next;

  window = list->data;

  machine_selection = (AgsMachineSelection *) ags_machine_selection_new(window);
  ags_machine_selection_load_defaults(machine_selection);
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
  AgsMachineRadioButton *machine_radio_button, *existing_radio_button;
  GtkVBox *vbox;
  GtkContainer *content_area;
  GList *list, *list_start, *index, *index_start;;

  if(response == GTK_RESPONSE_ACCEPT){
    /* retrieve machine */
    machine = NULL;
    vbox = GTK_DIALOG(machine_selection)->vbox;

    if(response == GTK_RESPONSE_ACCEPT){
      list_start =
	list = gtk_container_get_children(GTK_CONTAINER(AGS_MACHINE_SELECTION(machine_selection)->window->machines));
      index_start =
	index = gtk_container_get_children(vbox);

      while(list != NULL){
	if(AGS_IS_FFPLAYER(list->data) || AGS_IS_DRUM(list->data) || AGS_IS_MATRIX(list->data)){
	  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(index->data))){
	    machine = AGS_MACHINE(list->data);

	    break;
	  }

	  index = index->next;
	}
	
	list = list->next;
      }

      g_list_free(list_start);
      g_list_free(index_start);
    }

    /* retrieve selected */    
    machine_radio_button = NULL;
    existing_radio_button = NULL;
    
    list_start =
      list = gtk_container_get_children(GTK_CONTAINER(machine_selector));
    list = list->next;

    while(list != NULL){
      if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(list->data))){
	machine_radio_button = AGS_MACHINE_RADIO_BUTTON(list->data);
      }

      if(AGS_MACHINE_RADIO_BUTTON(list->data)->machine == machine){
	existing_radio_button = AGS_MACHINE_RADIO_BUTTON(list->data);
      }

      list = list->next;
    }

    g_list_free(list_start);


    /* decide if link to editor or change index */
    if(existing_radio_button != NULL){
      gtk_button_clicked(existing_radio_button);
    }else{
      g_object_set(G_OBJECT(machine_radio_button),
		   "machine\0", machine,
		   NULL);
    }
  }

  gtk_widget_destroy(machine_selection);
}

void
ags_machine_selector_radio_changed(GtkWidget *radio_button, AgsMachineSelector *machine_selector)
{
  ags_machine_selector_changed(machine_selector, AGS_MACHINE_RADIO_BUTTON(radio_button)->machine);
}

void
ags_machine_selector_popup_reverse_mapping_callback(GtkWidget *menu_item, AgsMachineSelector *machine_selector)
{
  AgsEditor *editor;

  editor = gtk_widget_get_ancestor(machine_selector,
				   AGS_TYPE_EDITOR);

  if(editor->selected_machine != NULL){
    if(gtk_check_menu_item_get_active(menu_item)){
      editor->selected_machine->audio->flags |= AGS_AUDIO_REVERSE_MAPPING;
    }else{
      editor->selected_machine->audio->flags &= (~AGS_AUDIO_REVERSE_MAPPING);
    }
  }
}

void
ags_machine_selector_popup_shift_piano_callback(GtkWidget *menu_item, AgsMachineSelector *machine_selector)
{
  AgsEditor *editor;

  editor = gtk_widget_get_ancestor(machine_selector,
				   AGS_TYPE_EDITOR);

  if(editor->selected_machine != NULL){
    GList *notation;

    notation = editor->selected_machine->audio->notation;

    while(notation != NULL){
      g_free(AGS_NOTATION(notation->data)->base_note);
      AGS_NOTATION(notation->data)->base_note = g_strdup(gtk_menu_item_get_label(menu_item));
      
      notation = notation->next;
    }

    ags_meter_paint(editor->current_meter);
  }
}
