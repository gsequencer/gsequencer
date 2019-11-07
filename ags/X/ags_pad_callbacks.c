/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/X/ags_pad_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_line_callbacks.h>

#include <ags/X/thread/ags_gui_thread.h>

void
ags_pad_group_clicked_callback(GtkWidget *widget, AgsPad *pad)
{
  AgsLine *line;
  GtkContainer *container;
  GList *list, *list_start;

  if(gtk_toggle_button_get_active(pad->group)){
    container = (GtkContainer *) pad->expander_set;

    list_start = 
      list = gtk_container_get_children(container);
    
    while(list != NULL){
      line = AGS_LINE(list->data);

      if(!gtk_toggle_button_get_active(line->group)){
	gtk_toggle_button_set_active(line->group, TRUE);
      }

      list = list->next;
    }

    g_list_free(list_start);
  }else{
    container = (GtkContainer *) pad->expander_set;

    list_start = 
      list = gtk_container_get_children(container);
    
    while(list != NULL){
      line = AGS_LINE(list->data);

      if(!gtk_toggle_button_get_active(line->group)){
	g_list_free(list_start);
	
	return;
      }

      list = list->next;
    }

    g_list_free(list_start);
    gtk_toggle_button_set_active(pad->group, TRUE);
  }
}

void
ags_pad_mute_clicked_callback(GtkWidget *widget, AgsPad *pad)
{
  AgsMachine *machine;
  GtkContainer *container;

  AgsChannel *current, *next_pad, *next_current;

  AgsSetMuted *set_muted;

  AgsApplicationContext *application_context;

  GList *list, *list_start;
  GList *start_task;

  gboolean is_output;

  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) pad,
						   AGS_TYPE_MACHINE);
  
  application_context = ags_application_context_get_instance();

  /*  */
  start_task = NULL;

  if(gtk_toggle_button_get_active(pad->mute)){
    if(gtk_toggle_button_get_active(pad->solo)){
      gtk_toggle_button_set_active(pad->solo, FALSE);
    }
    
    /* mute */
    current = pad->channel;

    if(current != NULL){
      g_object_ref(current);
    }
    
    next_pad = ags_channel_next_pad(pad->channel);

    next_current = NULL;
    
    while(current != next_pad){
      /* instantiate set muted task */
      set_muted = ags_set_muted_new((GObject *) current,
				    TRUE);
      start_task = g_list_prepend(start_task,
				  set_muted);

      /* iterate */
      next_current = ags_channel_next(current);

      g_object_unref(current);

      current = next_current;
    }

    if(next_pad != NULL){
      g_object_unref(next_pad);
    }

    if(next_current != NULL){
      g_object_unref(next_current);
    }
  }else{
    if((AGS_MACHINE_SOLO & (machine->flags)) != 0){
      is_output = (AGS_IS_OUTPUT(pad->channel))? TRUE: FALSE;

      container = (GtkContainer *) (is_output ? machine->output: machine->input);

      list_start = 
	list = gtk_container_get_children(container);

      while(!gtk_toggle_button_get_active(AGS_PAD(list->data)->solo)){
	list = list->next;
      }

      g_list_free(list_start);

      gtk_toggle_button_set_active(AGS_PAD(list->data)->solo, FALSE);

      machine->flags &= ~(AGS_MACHINE_SOLO);
    }

    /* unmute */
    current = pad->channel;

    if(current != NULL){
      g_object_ref(current);
    }
    
    next_pad = ags_channel_next_pad(pad->channel);

    next_current = NULL;
    
    while(current != next_pad){
      /* instantiate set muted task */
      set_muted = ags_set_muted_new((GObject *) current,
				    FALSE);
      start_task = g_list_prepend(start_task,
				  set_muted);
      
      /* iterate */
      next_current = ags_channel_next(current);

      g_object_unref(current);

      current = next_current;
    }

    if(next_pad != NULL){
      g_object_unref(next_pad);
    }

    if(next_current != NULL){
      g_object_unref(next_current);
    }
  }

  ags_xorg_application_context_schedule_task_list(application_context,
						  start_task);
}

void
ags_pad_solo_clicked_callback(GtkWidget *widget, AgsPad *pad)
{
  AgsMachine *machine;
  GtkContainer *container;

  GList *list, *list_start;

  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) pad, AGS_TYPE_MACHINE);

  if(gtk_toggle_button_get_active(pad->solo)){
    container = (GtkContainer *) (AGS_IS_OUTPUT(pad->channel) ? machine->output: machine->input);

    if(gtk_toggle_button_get_active(pad->mute))
      gtk_toggle_button_set_active(pad->mute, FALSE);

    list_start = 
      list = gtk_container_get_children(container);

    while(list != NULL){
      if(list->data == pad){
	list = list->next;
	continue;
      }

      gtk_toggle_button_set_active(AGS_PAD(list->data)->mute, TRUE);

      list = list->next;
    }

    g_list_free(list_start);
    machine->flags |= (AGS_MACHINE_SOLO);
  }else{
    machine->flags &= ~(AGS_MACHINE_SOLO);
  }
}
