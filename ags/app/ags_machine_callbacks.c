/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/app/ags_machine_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_pad.h>
#include <ags/app/ags_machine_editor.h>
#include <ags/app/ags_connection_editor.h>
#include <ags/app/ags_midi_dialog.h>

#include <ags/app/export/ags_wave_export_dialog.h>

#include <ags/app/editor/ags_envelope_dialog.h>
#include <ags/app/editor/ags_machine_radio_button.h>

#include <ags/i18n.h>

void ags_machine_recall_set_loop(AgsMachine *machine,
				 AgsRecall *recall);

int ags_machine_popup_rename_response_callback(GtkWidget *widget, gint response, AgsMachine *machine);
int ags_machine_popup_rename_audio_response_callback(GtkWidget *widget, gint response, AgsMachine *machine);
int ags_machine_popup_reposition_audio_response_callback(GtkWidget *widget, gint response, AgsMachine *machine);
int ags_machine_popup_properties_destroy_callback(GtkWidget *widget, AgsMachine *machine);

//FIXME:JK: remove me
#if 0
void
ags_machine_show_callback(GtkWidget *widget, AgsMachine *machine)
{
  AgsAudio *audio;
  AgsPlaybackDomain *playback_domain;
  
  guint i;

  static const guint staging_program[] = {
    (AGS_SOUND_STAGING_AUTOMATE | AGS_SOUND_STAGING_RUN_INTER | AGS_SOUND_STAGING_FX),
  };

  audio = machine->audio;

  g_object_get(audio,
	       "playback-domain", &playback_domain,
	       NULL);

  if(playback_domain != NULL){
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      AgsThread *audio_thread;
	  
      audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							  i);

      if(audio_thread != NULL){
	ags_audio_thread_set_do_fx_staging((AgsAudioThread *) audio_thread,
					   TRUE);
	ags_audio_thread_set_staging_program((AgsAudioThread *) audio_thread,
					     staging_program,
					     1);
	    
	g_object_unref(audio_thread);
      }
    }
    
    g_object_unref(playback_domain);
  }
}
#endif

void
ags_machine_check_message_callback(GObject *application_context, AgsMachine *machine)
{
  ags_machine_check_message(machine);
}

void
ags_machine_recall_set_loop(AgsMachine *machine,
			    AgsRecall *recall)
{
  AgsNavigation *navigation;
  AgsPort *port;

  AgsApplicationContext *application_context;

  GValue value = G_VALUE_INIT;

  application_context = ags_application_context_get_instance();

  navigation = ags_ui_provider_get_navigation(AGS_UI_PROVIDER(application_context));

  /* loop */
  port = NULL;
    
  g_object_get(recall,
	       "loop", &port,
	       NULL);

  g_value_init(&value,
	       G_TYPE_BOOLEAN);

  g_value_set_boolean(&value,
		      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(navigation->loop)));

  ags_port_safe_write(port,
		      &value);

  if(port != NULL){
    g_object_unref(port);
  }
    
  /* loop start */
  port = NULL;
    
  g_object_get(recall,
	       "loop-start", &port,
	       NULL);

  g_value_unset(&value);
  g_value_init(&value,
	       G_TYPE_UINT64);

  g_value_set_uint64(&value,
		     16 * gtk_spin_button_get_value_as_int(navigation->loop_left_tact));

  ags_port_safe_write(port,
		      &value);

  if(port != NULL){
    g_object_unref(port);
  }
    
  /* loop end */
  port = NULL;
    
  g_object_get(recall,
	       "loop-end", &port,
	       NULL);

  g_value_unset(&value);
  g_value_init(&value,
	       G_TYPE_UINT64);

  g_value_set_uint64(&value,
		     16 * gtk_spin_button_get_value_as_int(navigation->loop_right_tact));

  ags_port_safe_write(port,
		      &value);

  if(port != NULL){
    g_object_unref(port);
  }
}

void
ags_machine_map_recall_callback(AgsMachine *machine,
				gpointer user_data)
{
  GList *start_play;
  GList *start_recall;
  GList *list;
  
  start_play = NULL;
  start_recall = NULL;
  
  g_object_get(machine->audio,
	       "play", &start_play,
	       "recall", &start_recall,
	       NULL);

  list = start_play;
  
  while((list = ags_recall_template_find_type(list, AGS_TYPE_FX_NOTATION_AUDIO)) != NULL){
    ags_machine_recall_set_loop(machine,
				list->data);

    /* iterate */
    list = list->next;
  }

  list = start_play;
  
  while((list = ags_recall_template_find_type(list, AGS_TYPE_FX_PLAYBACK_AUDIO)) != NULL){
    ags_machine_recall_set_loop(machine,
				list->data);

    /* iterate */
    list = list->next;
  }

  list = start_recall;
  
  while((list = ags_recall_template_find_type(list, AGS_TYPE_FX_NOTATION_AUDIO)) != NULL){
    ags_machine_recall_set_loop(machine,
				list->data);

    /* iterate */
    list = list->next;
  }
  
  list = start_recall;
  
  while((list = ags_recall_template_find_type(list, AGS_TYPE_FX_PLAYBACK_AUDIO)) != NULL){
    ags_machine_recall_set_loop(machine,
				list->data);

    /* iterate */
    list = list->next;
  }
  
  g_list_free_full(start_play,
		   (GDestroyNotify) g_object_unref);
  
  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_machine_move_up_callback(GAction *action, GVariant *parameter,
			     AgsMachine *machine)
{
  AgsWindow *window;

  GList *start_list, *list;
  
  window = gtk_widget_get_ancestor(machine,
				   AGS_TYPE_WINDOW);

  start_list = ags_window_get_machine(window);

  list = g_list_find(start_list,
		     machine);

  if(list->prev != NULL && list->prev->prev != NULL){
    gtk_box_reorder_child_after(window->machine_box,
				machine,
				list->prev->prev->data);
  }

  g_list_free(start_list);
}

void
ags_machine_move_down_callback(GAction *action, GVariant *parameter,
			       AgsMachine *machine)
{
  AgsWindow *window;

  GList *start_list, *list;
  
  window = gtk_widget_get_ancestor(machine,
				   AGS_TYPE_WINDOW);

  start_list = ags_window_get_machine(window);

  list = g_list_find(start_list,
		     machine);

  if(list->next != NULL){
    gtk_box_reorder_child_after(window->machine_box,
				machine,
				list->next->data);
  }

  g_list_free(start_list);
}

void
ags_machine_hide_callback(GAction *action, GVariant *parameter,
			  AgsMachine *machine)
{
  gtk_widget_hide(machine->frame);
}

void
ags_machine_show_callback(GAction *action, GVariant *parameter,
			  AgsMachine *machine)
{
  gtk_widget_show(machine->frame);
}

void
ags_machine_destroy_callback(GAction *action, GVariant *parameter,
			     AgsMachine *machine)
{
  AgsWindow *window;

  AgsApplicationContext *application_context;

  AgsAudio *audio;  

  AgsRemoveAudio *remove_audio;

  GList *start_list, *list;
  
  application_context = ags_application_context_get_instance();

  window = gtk_widget_get_ancestor(machine,
				   AGS_TYPE_WINDOW);

  ags_machine_set_run(machine,
		      FALSE);

  list = 
    start_list = ags_machine_selector_get_machine_radio_button(window->composite_editor->machine_selector);

  while(list != NULL){
    if(AGS_MACHINE_RADIO_BUTTON(list->data)->machine == machine){
      ags_machine_selector_remove_machine_radio_button(window->composite_editor->machine_selector,
						       list->data);
      
      g_object_run_dispose(list->data);
      g_object_unref(list->data);
      
      break;
    }
    
    list = list->next;
  }

  g_list_free(start_list);
  
  /* destroy machine */
  audio = machine->audio;
  g_object_ref(audio);

  ags_connectable_disconnect(AGS_CONNECTABLE(machine));

  ags_window_remove_machine(window,
			    machine);
  
  g_object_run_dispose(machine);
  g_object_unref(machine);

  /* get task thread */
  remove_audio = ags_remove_audio_new(audio);
  
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) remove_audio);
}

void
ags_machine_rename_callback(GAction *action, GVariant *parameter,
			    AgsMachine *machine)
{
  //TODO:JK: implement me
}

void
ags_machine_rename_audio_callback(GAction *action, GVariant *parameter,
				  AgsMachine *machine)
{
  //TODO:JK: implement me
}

void
ags_machine_reposition_audio_callback(GAction *action, GVariant *parameter,
				      AgsMachine *machine)
{
  //TODO:JK: implement me
}

void
ags_machine_properties_callback(GAction *action, GVariant *parameter,
				AgsMachine *machine)
{
  //TODO:JK: implement me
}

void
ags_machine_sticky_controls_callback(GAction *action, GVariant *parameter,
				     AgsMachine *machine)
{
  //TODO:JK: implement me
}

void
ags_machine_copy_pattern_callback(GAction *action, GVariant *parameter,
				  AgsMachine *machine)
{
  ags_machine_copy_pattern(machine);
}

void
ags_machine_paste_pattern_callback(GAction *action, GVariant *parameter,
				   AgsMachine *machine)
{
  //TODO:JK: implement me
}

void
ags_machine_envelope_callback(GAction *action, GVariant *parameter,
			      AgsMachine *machine)
{
  //TODO:JK: implement me
}

void
ags_machine_audio_connection_callback(GAction *action, GVariant *parameter,
				      AgsMachine *machine)
{
  //TODO:JK: implement me
}

void
ags_machine_midi_connection_callback(GAction *action, GVariant *parameter,
				     AgsMachine *machine)
{
  //TODO:JK: implement me
}

void
ags_machine_audio_export_callback(GAction *action, GVariant *parameter,
				  AgsMachine *machine)
{
  //TODO:JK: implement me
}

void
ags_machine_midi_export_callback(GAction *action, GVariant *parameter,
				 AgsMachine *machine)
{
  //TODO:JK: implement me
}

void
ags_machine_audio_import_callback(GAction *action, GVariant *parameter,
				  AgsMachine *machine)
{
  //TODO:JK: implement me
}

void
ags_machine_midi_import_callback(GAction *action, GVariant *parameter,
				 AgsMachine *machine)
{
  //TODO:JK: implement me
}

void
ags_machine_open_response_callback(GtkDialog *dialog, gint response, AgsMachine *machine)
{
  AgsPCMFileChooserDialog *pcm_file_chooser_dialog;

  pcm_file_chooser_dialog = AGS_PCM_FILE_CHOOSER_DIALOG(dialog);

  if(response == GTK_RESPONSE_ACCEPT){
    GListModel *file;
    
    GSList *filename;

    guint i, i_stop;
    
    file = gtk_file_chooser_get_files(GTK_FILE_CHOOSER(pcm_file_chooser_dialog->file_chooser));

    i_stop = g_list_model_get_n_items(file);
    
    for(i = 0; i < i_stop; i++){
      GFile *current_file;
      
      current_file = g_list_model_get_item(file,
					   i);
      
      filename = g_slist_append(filename,
				g_file_get_path(file));
    }
    
    ags_machine_open_files(machine,
			   filename,
			   gtk_check_button_get_active(pcm_file_chooser_dialog->existing_channel),
			   gtk_check_button_get_active(pcm_file_chooser_dialog->new_channel));
  }

  gtk_window_destroy((GtkWindow *) pcm_file_chooser_dialog);
}

void
ags_machine_play_callback(GtkWidget *toggle_button, AgsMachine *machine)
{
  if(machine == NULL){
    return;
  }

  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggle_button))){
    if((AGS_MACHINE_BLOCK_PLAY & (machine->flags)) != 0){      
      return;
    }

    g_message("machine: on");

    machine->flags |= AGS_MACHINE_BLOCK_PLAY;

    ags_machine_set_run_extended(machine,
				 TRUE,
				 TRUE, FALSE, FALSE, FALSE);

    machine->flags &= (~AGS_MACHINE_BLOCK_PLAY);
  }else{
    if((AGS_MACHINE_BLOCK_STOP & (machine->flags)) != 0){
      return;
    }

    g_message("machine: off");

    machine->flags |= AGS_MACHINE_BLOCK_STOP;

    ags_machine_set_run_extended(machine,
				 FALSE,
				 TRUE, FALSE, FALSE, FALSE);

    machine->flags &= (~AGS_MACHINE_BLOCK_STOP);
  }
}

void
ags_machine_resize_audio_channels_callback(AgsMachine *machine,
					   guint audio_channels, guint audio_channels_old,
					   gpointer data)
{
  AgsAudio *audio;
  AgsPlayback *playback;
  AgsChannel *start_output;
  AgsChannel *start_input;
  AgsChannel *channel, *next_pad, *next_channel;

  GList *start_pad, *pad;
  GList *start_line, *line;
  
  guint i;

  static const guint staging_program[] = {
    (AGS_SOUND_STAGING_AUTOMATE | AGS_SOUND_STAGING_RUN_INTER | AGS_SOUND_STAGING_FX),
  };

  audio = machine->audio;

  start_output = NULL;
  start_input = NULL;

  g_object_get(audio,
	       "output", &start_output,
	       "input", &start_input,
	       NULL);

  if(audio_channels > audio_channels_old){
    /* AgsOutput */
    channel = start_output;

    if(channel != NULL){
      g_object_ref(channel);
    }

    next_pad = NULL;
    
    while(channel != NULL){      
      /* get some fields */
      next_pad = ags_channel_next_pad(channel);
      
      next_channel = ags_channel_nth(channel,
				     audio_channels_old);

      if(channel != NULL){
	g_object_unref(channel);
      }
      
      channel = next_channel;
      
      while(channel != next_pad && channel != NULL){
	/* fx engine */
	g_object_get(channel,
		     "playback", &playback,
		     NULL);

	if(playback != NULL){
	  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
	    AgsThread *channel_thread;
	  
	    channel_thread = ags_playback_get_channel_thread(playback,
							     i);

	    if(channel_thread != NULL){
	      ags_channel_thread_set_do_fx_staging((AgsChannelThread *) channel_thread,
						   TRUE);
	      ags_channel_thread_set_staging_program((AgsChannelThread *) channel_thread,
						     staging_program,
						     1);
	    
	      g_object_unref(channel_thread);
	    }
	  }
	
	  g_object_unref(playback);
	}
	
	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      if(next_pad != NULL){
	g_object_unref(next_pad);
      }
    }

    if(channel != NULL){
      g_object_unref(channel);
    }

    /* AgsInput */
    channel = start_input;

    if(channel != NULL){
      g_object_ref(channel);
    }

    next_pad = NULL;
    
    while(channel != NULL){      
      /* get some fields */
      next_pad = ags_channel_next_pad(channel);

      next_channel = ags_channel_nth(channel,
				     audio_channels_old);
      
      if(channel != NULL){
	g_object_unref(channel);
      }
      
      channel = next_channel;
      
      while(channel != next_pad && channel != NULL){
	/* fx engine */
	g_object_get(channel,
		     "playback", &playback,
		     NULL);

	if(playback != NULL){
	  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
	    AgsThread *channel_thread;
	  
	    channel_thread = ags_playback_get_channel_thread(playback,
							     i);

	    if(channel_thread != NULL){
	      ags_channel_thread_set_do_fx_staging((AgsChannelThread *) channel_thread,
						   TRUE);
	      ags_channel_thread_set_staging_program((AgsChannelThread *) channel_thread,
						     staging_program,
						     1);
	    
	      g_object_unref(channel_thread);
	    }
	  }
	
	  g_object_unref(playback);
	}
	
	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      if(next_pad != NULL){
	g_object_unref(next_pad);
      }
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }
   
  /* unref */
  if(start_output != NULL){
    g_object_unref(start_output);
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }
  
  /* resize */
  if((AGS_CONNECTABLE_CONNECTED & (machine->connectable_flags)) != 0){
    if(audio_channels > audio_channels_old){
      if(machine->input_pad != NULL){
	pad =
	  start_pad = ags_machine_get_input_pad(machine);
      
	while(pad != NULL){
	  start_line = ags_pad_get_line(AGS_PAD(pad->data));
	  line = g_list_nth(start_line,
			    audio_channels_old);
	
	  for(i = 0; i < audio_channels - audio_channels_old; i++){
	    ags_connectable_connect(AGS_CONNECTABLE(line->data));

	    line = line->next;
	  }

	  g_list_free(start_line);
	
	  pad = pad->next;
	}

	g_list_free(start_pad);
      }

      if(machine->output_pad != NULL){
	pad =
	  start_pad = ags_machine_get_output_pad(machine);
      
	while(pad != NULL){
	  start_line = ags_pad_get_line(AGS_PAD(pad->data));
	  line = g_list_nth(start_line,
			    audio_channels_old);
	
	  for(i = 0; i < audio_channels - audio_channels_old; i++){
	    ags_connectable_connect(AGS_CONNECTABLE(line->data));

	    line = line->next;
	  }

	  g_list_free(start_line);
	  
	  pad = pad->next;
	}

	g_list_free(start_pad);
      }
    }
  }
}

void
ags_machine_resize_pads_callback(AgsMachine *machine,
				 GType channel_type,
				 guint pads, guint pads_old,
				 gpointer data)
{
  AgsAudio *audio;
  AgsPlayback *playback;
  AgsChannel *start_output;
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel;

  GList *start_pad, *pad;

  guint audio_channels;
  guint i;

  static const guint staging_program[] = {
    (AGS_SOUND_STAGING_AUTOMATE | AGS_SOUND_STAGING_RUN_INTER | AGS_SOUND_STAGING_FX),
  };

  audio = machine->audio;

  start_output = NULL;
  start_input = NULL;
  
  audio_channels = 0;

  if(g_type_is_a(channel_type, AGS_TYPE_INPUT)){
    if(pads > pads_old){
      /* get some fields */
      g_object_get(audio,
		   "input", &start_input,
		   "audio-channels", &audio_channels,
		   NULL);

      /* AgsOutput */
      channel = ags_channel_pad_nth(start_input,
				    pads_old);
      
      while(channel != NULL){
	/* fx engine */
	g_object_get(channel,
		     "playback", &playback,
		     NULL);

	if(playback != NULL){
	  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
	    AgsThread *channel_thread;
	  
	    channel_thread = ags_playback_get_channel_thread(playback,
							     i);

	    if(channel_thread != NULL){
	      ags_channel_thread_set_do_fx_staging((AgsChannelThread *) channel_thread,
						   TRUE);
	      ags_channel_thread_set_staging_program((AgsChannelThread *) channel_thread,
						     staging_program,
						     1);
	    
	      g_object_unref(channel_thread);
	    }
	  }
	
	  g_object_unref(playback);
	}
	
	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      if(start_input != NULL){
	g_object_unref(start_input);
      }

      if(channel != NULL){
	g_object_unref(channel);
      }
    }
  }else{
    if(pads > pads_old){
      /* get some fields */
      g_object_get(audio,
		   "output", &start_output,
		   "audio-channels", &audio_channels,
		   NULL);

      /* AgsOutput */
      channel = ags_channel_pad_nth(start_output,
				    pads_old);
      
      while(channel != NULL){
	/* fx engine */
	g_object_get(channel,
		     "playback", &playback,
		     NULL);

	if(playback != NULL){
	  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
	    AgsThread *channel_thread;
	  
	    channel_thread = ags_playback_get_channel_thread(playback,
							     i);

	    if(channel_thread != NULL){
	      ags_channel_thread_set_do_fx_staging((AgsChannelThread *) channel_thread,
						   TRUE);
	      ags_channel_thread_set_staging_program((AgsChannelThread *) channel_thread,
						     staging_program,
						     1);
	    
	      g_object_unref(channel_thread);
	    }
	  }
	
	  g_object_unref(playback);
	}
	
	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      if(start_output != NULL){
	g_object_unref(start_output);
      }

      if(channel != NULL){
	g_object_unref(channel);
      }
    }
  }
  
  /* resize */
  if((AGS_CONNECTABLE_CONNECTED & (machine->connectable_flags)) != 0){
    if(pads > pads_old){
      if(g_type_is_a(channel_type,
		     AGS_TYPE_INPUT)){
	if(machine->input_pad != NULL){
	  start_pad = ags_machine_get_input_pad(machine);
	  pad = g_list_nth(start_pad,
			   pads_old);
      
	  while(pad != NULL){
	    ags_connectable_connect(AGS_CONNECTABLE(pad->data));
	
	    pad = pad->next;
	  }
	}
      }

      if(g_type_is_a(channel_type,
		     AGS_TYPE_OUTPUT)){
	if(machine->output_pad != NULL){
	  start_pad = ags_machine_get_output_pad(machine);
	  pad = g_list_nth(pad,
			   pads_old);
      
	  while(pad != NULL){
	    ags_connectable_connect(AGS_CONNECTABLE(pad->data));
	
	    pad = pad->next;
	  }
	}
      }
    }
  }
}

void
ags_machine_stop_callback(AgsMachine *machine,
			  GList *recall_id, gint sound_scope,
			  gpointer data)
{
  gboolean reset_active;
  
  if((AGS_MACHINE_BLOCK_STOP_CALLBACK & (machine->flags)) != 0){
    return;
  }
  
  machine->flags |= AGS_MACHINE_BLOCK_STOP_CALLBACK;

  /* play button - check reset active */
  reset_active = (sound_scope == AGS_SOUND_SCOPE_SEQUENCER) ? TRUE: FALSE;
  
  if(reset_active){
    gtk_toggle_button_set_active(machine->play, FALSE);
  }

#if 0
  if(sound_scope == AGS_SOUND_SCOPE_SEQUENCER){
    ags_machine_set_run_extended(machine,
				 FALSE,
				 TRUE, FALSE, FALSE, FALSE);
  }

  if(sound_scope == AGS_SOUND_SCOPE_NOTATION){
    ags_machine_set_run_extended(machine,
				 FALSE,
				 FALSE, TRUE, FALSE, FALSE);
  }

  if(sound_scope == AGS_SOUND_SCOPE_WAVE){
    ags_machine_set_run_extended(machine,
				 FALSE,
				 FALSE, FALSE, TRUE, FALSE);
  }

  if(sound_scope == AGS_SOUND_SCOPE_MIDI){
    ags_machine_set_run_extended(machine,
				 FALSE,
				 FALSE, FALSE, FALSE, TRUE);
  }
#endif

  machine->flags &= (~AGS_MACHINE_BLOCK_STOP_CALLBACK);
}

void
ags_machine_active_playback_start_channel_launch_callback(AgsTask *task,
							  AgsPlayback *playback)
{
  AgsAudio *audio;
  AgsChannel *channel;
  AgsRecycling *first_recycling, *last_recycling;
  AgsRecycling *recycling, *next_recycling, *end_recycling;
  AgsAudioSignal *template, *audio_signal; 
  AgsRecallID *recall_id;
  AgsNote *play_note;
  AgsFxPlaybackAudio *fx_playback_audio;
  
  GObject *output_soundcard;

  GList *start_recall, *recall;
  
  GRecMutex *recycling_mutex;

  audio = NULL;
  
  channel = NULL;

  first_recycling = NULL;
  last_recycling = NULL;
  
  play_note = NULL;

  fx_playback_audio = NULL;
  
  start_recall = NULL;
  
  g_object_get(playback,
	       "channel", &channel,
	       "play-note", &play_note,
	       NULL);

  g_object_get(channel,
	       "audio", &audio,
	       NULL);

  g_object_get(audio,
	       "play", &start_recall,
	       NULL);

  recall = ags_recall_template_find_type(start_recall, AGS_TYPE_FX_PLAYBACK_AUDIO);

  if(recall != NULL){
    fx_playback_audio = recall->data;
  }
  
  recall_id = ags_playback_get_recall_id(playback,
					 AGS_SOUND_SCOPE_PLAYBACK);

  /* get some fields */
  g_object_get(channel,
	       "first-recycling", &first_recycling,
	       "last-recycling", &last_recycling,
	       NULL);

  end_recycling = ags_recycling_next(last_recycling);

  recycling = first_recycling;
  g_object_ref(recycling);

  next_recycling = NULL;
  
  while(recycling != end_recycling){
    output_soundcard = NULL;
    
    g_object_get(recycling,
		 "output-soundcard", &output_soundcard,
		 NULL);

    recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(recycling);

    g_rec_mutex_lock(recycling_mutex);

    template = ags_audio_signal_get_template(recycling->audio_signal);
    
    g_rec_mutex_unlock(recycling_mutex);

    /* instantiate audio signal */
    audio_signal = ags_audio_signal_new((GObject *) output_soundcard,
					(GObject *) recycling,
					(GObject *) recall_id);
    ags_audio_signal_set_flags(audio_signal, (AGS_AUDIO_SIGNAL_FEED |
					      AGS_AUDIO_SIGNAL_STREAM));
    g_object_set(audio_signal,
		 "template", template,
		 "note", play_note,
		 NULL);

    /* add audio signal */
    if(ags_audio_test_behaviour_flags(audio, AGS_SOUND_BEHAVIOUR_PATTERN_MODE)){
      ags_recycling_create_audio_signal_with_defaults(recycling,
						      audio_signal,
						      0.0, 0);
    }else{
      gdouble notation_delay;
      guint buffer_size;
      guint note_x0, note_x1;

      notation_delay = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(output_soundcard));

      g_object_get(recycling,
		   "buffer-size", &buffer_size,
		   NULL);
	
      g_object_get(play_note,
		   "x0", &note_x0,
		   "x1", &note_x1,
		   NULL);

      ags_recycling_create_audio_signal_with_frame_count(recycling,
							 audio_signal,
							 (guint) (((gdouble) buffer_size * notation_delay) * (gdouble) (note_x1 - note_x0)),
							 0.0, 0);
    }
      
    audio_signal->stream_current = audio_signal->stream;
    ags_connectable_connect(AGS_CONNECTABLE(audio_signal));

    ags_fx_playback_audio_add_feed_audio_signal(fx_playback_audio, audio_signal);

    /*
     * emit add_audio_signal on AgsRecycling
     */
    ags_recycling_add_audio_signal(recycling,
				   audio_signal);

    g_object_unref(output_soundcard);
    
    /* iterate */
    next_recycling = ags_recycling_next(recycling);

    g_object_unref(recycling);

    recycling = next_recycling;
  }

  if(next_recycling != NULL){
    g_object_unref(next_recycling);
  }
  
  /* unref */
  g_object_unref(audio);
  
  g_object_unref(channel);

  if(first_recycling != NULL){
    g_object_unref(first_recycling);
    g_object_unref(last_recycling);
  }
  
  if(end_recycling != NULL){
    g_object_unref(end_recycling);
  }
  
  g_object_unref(recall_id);

  g_object_unref(play_note);
}
