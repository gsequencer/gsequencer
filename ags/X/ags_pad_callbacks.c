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
  AgsWindow *window;
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
  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) machine);
  
  application_context = (AgsApplicationContext *) window->application_context;

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

void
ags_pad_start_channel_launch_callback(AgsTask *task,
				      AgsPad *input_pad)
{ 
  AgsAudio *audio;
  AgsChannel *channel, *next, *next_channel;
  
  GObject *output_soundcard;

  GList *recall;

  /* get channel */
  channel = AGS_START_CHANNEL(task)->channel;

  /* get audio and its audio mutex */
  g_object_get(channel,
	       "audio", &audio,
	       "output-soundcard", &output_soundcard,
	       NULL);

  next = ags_channel_next(channel);

  next_channel = NULL;
  
#ifdef AGS_DEBUG
  g_message("launch");
#endif
  
  while(channel != next){
    AgsRecycling *first_recycling, *last_recycling;
    AgsRecycling *recycling, *next_recycling, *end_recycling;
    AgsAudioSignal *audio_signal;
    AgsNote *note;
    AgsPlayback *playback;
    AgsRecallID *recall_id;
    
    g_object_get(channel,
		 "playback", &playback,
		 NULL);

    if(playback != NULL){
      g_object_unref(playback);
    }
    
    recall_id = ags_playback_get_recall_id(playback,
					   AGS_SOUND_SCOPE_PLAYBACK);
    
    if(playback == NULL ||
       recall_id == NULL){
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
      
      continue;
    }
    
    g_object_get(playback,
		 "play-note", &note,
		 NULL);

    if(note != NULL){
      g_object_unref(note);
    }
    
    /* get some fields */
    g_object_get(channel,
		 "first-recycling", &first_recycling,
		 "last-recycling", &last_recycling,
		 NULL);

    end_recycling = ags_recycling_next(last_recycling);

    recycling = first_recycling;
    g_object_ref(recycling);
    
    while(recycling != end_recycling){      
      if(!ags_recall_global_get_rt_safe()){
	/* instantiate audio signal */
	audio_signal = ags_audio_signal_new((GObject *) output_soundcard,
					    (GObject *) recycling,
					    (GObject *) recall_id);
	g_object_set(audio_signal,
		     "note", note,
		     NULL);
	  
	/* add audio signal */
	ags_recycling_create_audio_signal_with_defaults(recycling,
							audio_signal,
							0.0, 0);
	audio_signal->stream_current = audio_signal->stream;
	ags_connectable_connect(AGS_CONNECTABLE(audio_signal));
	
	/*
	 * emit add_audio_signal on AgsRecycling
	 */
	ags_recycling_add_audio_signal(recycling,
				       audio_signal);
      }else{
	GList *start_list, *list;

	g_object_get(recycling,
		     "audio-signal", &start_list,
		     NULL);
      
	audio_signal = NULL;
	list = ags_audio_signal_find_by_recall_id(start_list,
						  (GObject *) recall_id);
	    
	if(list != NULL){
	  audio_signal = list->data;

	  g_object_set(audio_signal,
		       "note", note,
		       NULL);
	}

	g_list_free_full(start_list,
			 g_object_unref);

	g_object_set(note,
		     "rt-offset", 0,
		     NULL);
      }

      /* iterate */
      next_recycling = ags_recycling_next(recycling);

      g_object_unref(recycling);

      recycling = next_recycling;
    }

    if(first_recycling != NULL){
      g_object_unref(first_recycling);
      g_object_unref(last_recycling);
    }

    if(end_recycling != NULL){
      g_object_unref(end_recycling);
    }

    /* iterate */
    next_channel = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next_channel;
  }

  if(next != NULL){
    g_object_unref(next);
  }

  if(next_channel != NULL){
    g_object_unref(next_channel);
  }

  /* unref */
  g_object_unref(audio);
  
  g_object_unref(output_soundcard);
}
