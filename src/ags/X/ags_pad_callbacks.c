/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/ags_pad_callbacks.h>

#include <ags/main.h>

#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>

#include <ags/audio/task/recall/ags_set_muted.h>

#include <ags/X/ags_machine.h>

int
ags_pad_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsPad *pad)
{
  if(old_parent != NULL)
    return;
}

int
ags_pad_destroy_callback(GtkObject *object, AgsPad *pad)
{
  ags_pad_destroy(object);
}

int
ags_pad_show_callback(GtkWidget *widget, AgsPad *pad)
{
  ags_pad_show(widget);
}

int
ags_pad_option_changed_callback(GtkWidget *widget, AgsPad *pad)
{

  return(0);
}

int
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
	return(0);
      }

      list = list->next;
    }

    g_list_free(list_start);
    gtk_toggle_button_set_active(pad->group, TRUE);
  }

  return(0);
}

int
ags_pad_mute_clicked_callback(GtkWidget *widget, AgsPad *pad)
{
  AgsMachine *machine;
  GtkContainer *container;
  AgsTaskThread *task_thread;
  AgsChannel *current;
  AgsSetMuted *set_muted;
  GList *list, *list_start, *tasks;

  task_thread = AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(AGS_DEVOUT(AGS_AUDIO(pad->channel->audio)->devout)->ags_main)->main_loop)->task_thread);

  current = pad->channel;
  tasks = NULL;

  if(gtk_toggle_button_get_active(pad->mute)){
    if(gtk_toggle_button_get_active(pad->solo))
      gtk_toggle_button_set_active(pad->solo, FALSE);

    /* mute */
    while(current != pad->channel->next_pad){
      set_muted = ags_set_muted_new(G_OBJECT(current),
				    TRUE);
      tasks = g_list_prepend(tasks, set_muted);

      current = current->next;
    }
  }else{
    machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) pad, AGS_TYPE_MACHINE);

    if((AGS_MACHINE_SOLO & (machine->flags)) != 0){
      container = (GtkContainer *) (AGS_IS_OUTPUT(pad->channel) ? machine->output: machine->input);
      list_start = 
	list = gtk_container_get_children(container);

      while(!gtk_toggle_button_get_active(AGS_PAD(list->data)->solo))
	list = list->next;

      g_list_free(list_start);

      gtk_toggle_button_set_active(AGS_PAD(list->data)->solo, FALSE);

      machine->flags &= ~(AGS_MACHINE_SOLO);
    }

    /* unmute */
    while(current != pad->channel->next_pad){
      set_muted = ags_set_muted_new(G_OBJECT(current),
				    FALSE);
      tasks = g_list_prepend(tasks, set_muted);

      current = current->next;
    }
  }

  ags_task_thread_append_tasks(task_thread,
			       tasks);

  return(0);
}

int
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
  }else
    machine->flags &= ~(AGS_MACHINE_SOLO);

  return(0);
}
