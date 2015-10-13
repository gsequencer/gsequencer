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

#include <ags/X/ags_pad_callbacks.h>

#include <ags/main.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_pattern.h>
#include <ags/audio/ags_recall.h>

#include <ags/audio/recall/ags_play_channel_run.h>

#include <ags/audio/task/ags_start_devout.h>
#include <ags/audio/task/ags_add_audio_signal.h>

#include <ags/audio/task/recall/ags_set_muted.h>

#include <ags/X/ags_machine.h>
#include <ags/X/ags_line_callbacks.h>

extern pthread_mutex_t ags_application_mutex;

void ags_pad_start_complete_response(GtkWidget *dialog,
				     gint response,
				     AgsPad *pad);

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
  AgsWindow *window;
  AgsMachine *machine;
  GtkContainer *container;

  AgsChannel *current, *next_pad;

  AgsSetMuted *set_muted;

  AgsMutexManager *mutex_manager;
  AgsAudioLoop *audio_loop;
  AgsTaskThread *task_thread;

  AgsMain *application_context;

  GList *list, *list_start, *tasks;

  gboolean is_output;
  
  pthread_mutex_t *current_mutex;

  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) pad,
						   AGS_TYPE_MACHINE);
  window = (AgsWindow *) gtk_widget_get_toplevel(machine);
  
  application_context = window->application_context;

  /* get audio loop */
  pthread_mutex_lock(&(ags_application_mutex));

  audio_loop = application_context->main_loop;

  pthread_mutex_unlock(&(ags_application_mutex));

  /* get task thread */
  task_thread = (AgsTaskThread *) ags_thread_find_type(audio_loop,
						       AGS_TYPE_TASK_THREAD);

  /*  */
  current = pad->channel;
  tasks = NULL;

  /* lookup current mutex */
  pthread_mutex_lock(&(ags_application_mutex));

  current_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) current);
  
  pthread_mutex_unlock(&(ags_application_mutex));

  if(gtk_toggle_button_get_active(pad->mute)){
    if(gtk_toggle_button_get_active(pad->solo)){
      gtk_toggle_button_set_active(pad->solo, FALSE);
    }
    
    /* mute */
    pthread_mutex_lock(current_mutex);

    next_pad = pad->channel->next_pad;

    pthread_mutex_unlock(current_mutex);

    while(current != next_pad){
      /* lookup current mutex */
      pthread_mutex_lock(&(ags_application_mutex));

      current_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) current);
  
      pthread_mutex_unlock(&(ags_application_mutex));

      /* instantiate set muted task */
      set_muted = ags_set_muted_new((GObject *) current,
				    TRUE);
      tasks = g_list_prepend(tasks,
			     set_muted);

      /* iterate */
      pthread_mutex_lock(current_mutex);
      
      current = current->next;
      
      pthread_mutex_unlock(current_mutex);
    }
  }else{
    if((AGS_MACHINE_SOLO & (machine->flags)) != 0){
      pthread_mutex_lock(current_mutex);

      is_output = (AGS_IS_OUTPUT(pad->channel))? TRUE: FALSE;
      
      pthread_mutex_unlock(current_mutex);

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
    pthread_mutex_lock(current_mutex);

    next_pad = pad->channel->next_pad;

    pthread_mutex_unlock(current_mutex);
    
    while(current != next_pad){
      /* lookup current mutex */
      pthread_mutex_lock(&(ags_application_mutex));

      current_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) current);
  
      pthread_mutex_unlock(&(ags_application_mutex));

      /* instantiate set muted task */
      set_muted = ags_set_muted_new((GObject *) current,
				    FALSE);
      tasks = g_list_prepend(tasks,
			     set_muted);

      /* iterate */
      pthread_mutex_lock(current_mutex);
      
      current = current->next;
      
      pthread_mutex_unlock(current_mutex);
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

void
ags_pad_start_complete_callback(AgsTaskCompletion *task_completion,
				AgsPad *pad)
{
  AgsWindow *window;
  GtkMessageDialog *dialog;
  
  AgsDevoutThread *devout_thread;
  AgsTask *task;

  task = (AgsTask *) task_completion->task;
  window = AGS_MAIN(AGS_START_DEVOUT(task)->devout->application_context)->window;
  devout_thread = (AgsDevoutThread *) ags_thread_find_type(AGS_MAIN(window->application_context)->main_loop,
							   AGS_TYPE_DEVOUT_THREAD);

  if(devout_thread->error != NULL){
    /* show error message */
    dialog = (GtkMessageDialog *) gtk_message_dialog_new(GTK_WINDOW(window),
							 GTK_DIALOG_DESTROY_WITH_PARENT,
							 GTK_MESSAGE_ERROR,
							 GTK_BUTTONS_CLOSE,
							 "Error: %s\0", devout_thread->error->message);
    g_signal_connect(dialog, "response\0",
		     G_CALLBACK(ags_pad_start_complete_response), pad);
    gtk_widget_show_all((GtkWidget *) dialog);
  }
}

void
ags_pad_start_complete_response(GtkWidget *dialog,
				gint response,
				AgsPad *pad)
{
  gtk_widget_destroy(dialog);
}

void
ags_pad_init_channel_launch_callback(AgsTask *task, AgsPad *input_pad)
{ 
  AgsWindow *window;
  AgsDevout *devout;
  AgsChannel *channel, *next_pad;
  AgsRecycling *recycling;

  AgsAddAudioSignal *add_audio_signal;

  AgsAudioLoop *audio_loop;
  AgsTaskThread *task_thread;

  AgsMutexManager *mutex_manager;

  AgsMain *application_context;
  
  GList *recall, *tmp;
  GList *list, *list_start;

  pthread_mutex_t *audio_mutex;

  window = gtk_widget_get_ancestor(input_pad,
				   AGS_TYPE_WINDOW);
  application_context = window->application_context;
  
  pthread_mutex_lock(&(ags_application_mutex));
  
  mutex_manager = ags_mutex_manager_get_instance();

  audio_loop = application_context->main_loop;
  
  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) input_pad->channel->audio);

  pthread_mutex_unlock(&(ags_application_mutex));

  pthread_mutex_lock(audio_mutex);
  
  devout = AGS_DEVOUT(AGS_AUDIO(input_pad->channel->audio)->devout);

  task_thread = ags_thread_find_type(audio_loop,
				     AGS_TYPE_TASK_THREAD);

  list_start = 
    list = gtk_container_get_children((GtkContainer *) input_pad->expander_set);
  
  channel = input_pad->channel;
  next_pad = channel->next_pad;

#ifdef AGS_DEBUG
  g_message("launch\0");
#endif
  
  while(channel != next_pad){
    if(AGS_DEVOUT_PLAY(channel->devout_play) == NULL ||
       AGS_DEVOUT_PLAY(channel->devout_play)->recall_id[0] == NULL){
      channel = channel->next;
      list = list->next;

      continue;
    }

    /* connect done */
    recall = ags_recall_find_provider_with_recycling_container(channel->play,
							       G_OBJECT(channel),
							       G_OBJECT(AGS_DEVOUT_PLAY(channel->devout_play)->recall_id[0]->recycling_container));

    tmp = recall;
    recall = ags_recall_find_type(recall,
				  AGS_TYPE_PLAY_CHANNEL_RUN);
    //TODO:JK: fix me
    //    g_list_free(tmp);

    if(recall != NULL){
      AgsAudioSignal *audio_signal;
      
      g_signal_connect_after(channel, "done\0",
			     G_CALLBACK(ags_line_channel_done_callback), AGS_LINE(list->data));
      
      /* add audio signal */
      recycling = channel->first_recycling;

      while(recycling != channel->last_recycling->next){
	audio_signal = ags_audio_signal_new((GObject *) devout,
					    (GObject *) recycling,
					    (GObject *) AGS_RECALL(recall->data)->recall_id);
	/* add audio signal */
	ags_recycling_create_audio_signal_with_defaults(recycling,
							audio_signal,
							0.0, 0);
	audio_signal->stream_current = audio_signal->stream_beginning;
	ags_audio_signal_connect(audio_signal);
  
	/*
	 * emit add_audio_signal on AgsRecycling
	 */
	ags_recycling_add_audio_signal(recycling,
				       audio_signal);

	recycling = recycling->next;
      }    
    }

    //TODO:JK: fix me
    //    g_list_free(recall);

    channel = channel->next;
    list = list->next;
  }

  g_list_free(list_start);

  pthread_mutex_unlock(audio_mutex);
}
