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
#include <ags/X/machine/ags_dssi_bridge.h>
#include <ags/X/machine/ags_lv2_bridge.h>

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
  ags_machine_selector_add_index(machine_selector);
}

void
ags_machine_selector_popup_remove_index_callback(GtkWidget *menu_item, AgsMachineSelector *machine_selector)
{
  AgsMachineRadioButton *machine_radio_button;
  
  GList *list, *list_start;

  guint nth;
  
  /* find index */
  list_start = 
    list = gtk_container_get_children(GTK_CONTAINER(machine_selector));
  list = list->next;

  nth = 0;
  
  while(list != NULL){

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(list->data))){
      machine_radio_button = AGS_MACHINE_RADIO_BUTTON(list->data);
      ags_machine_selector_changed(machine_selector,
				   NULL);

      break;
    }

    list = list->next;
    nth++;
  }

  g_list_free(list_start);

  /* remove index */
  ags_machine_selector_remove_index(machine_selector,
				    nth);
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

  if((AGS_MACHINE_SELECTOR_NOTATION & (machine_selector->flags)) != 0){
    machine_selection->flags |= AGS_MACHINE_SELECTION_NOTATION;
  }else if((AGS_MACHINE_SELECTOR_AUTOMATION & (machine_selector->flags)) != 0){
    machine_selection->flags |= AGS_MACHINE_SELECTION_AUTOMATION;
  }
  
  ags_machine_selection_load_defaults(machine_selection);
  g_signal_connect(G_OBJECT(machine_selection), "response\0",
		   G_CALLBACK(ags_machine_selector_selection_response), machine_selector);
  gtk_widget_show_all((GtkWidget *) machine_selection);
}

void
ags_machine_selector_selection_response(GtkWidget *machine_selection,
					gint response,
					AgsMachineSelector *machine_selector)
{
  AgsMachine *machine;
  GtkVBox *vbox;
  GtkContainer *content_area;

  GList *list, *list_start;

  if(response == GTK_RESPONSE_ACCEPT){
    /* retrieve machine */
    machine = NULL;
    vbox = (GtkVBox *) GTK_DIALOG(machine_selection)->vbox;

    if(response == GTK_RESPONSE_ACCEPT){
      list_start =
	list = gtk_container_get_children((GtkContainer *) vbox);

      while(list != NULL){
	if(GTK_IS_TOGGLE_BUTTON(list->data) &&
	   gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(list->data))){
	  machine = g_object_get_data(list->data,
				      AGS_MACHINE_SELECTION_INDEX);
	  
	  break;
	}
	
	list = list->next;
      }

      g_list_free(list_start);
    }

    /* link index  */
    ags_machine_selector_link_index(machine_selector,
				    machine);
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

  editor = (AgsEditor *) gtk_widget_get_ancestor((GtkWidget *) machine_selector,
						 AGS_TYPE_EDITOR);
  
  if(editor->selected_machine != NULL){
    if(gtk_check_menu_item_get_active((GtkCheckMenuItem *) menu_item)){
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

  editor = (AgsEditor *) gtk_widget_get_ancestor((GtkWidget *) machine_selector,
						 AGS_TYPE_EDITOR);

  if(editor->selected_machine != NULL){
    GList *notation;

    notation = editor->selected_machine->audio->notation;

    while(notation != NULL){
      g_free(AGS_NOTATION(notation->data)->base_note);
      AGS_NOTATION(notation->data)->base_note = g_strdup(gtk_menu_item_get_label((GtkMenuItem *) menu_item));
      
      notation = notation->next;
    }

    ags_meter_paint(editor->current_meter);
  }
}
