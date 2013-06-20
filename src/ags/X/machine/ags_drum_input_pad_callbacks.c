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

#include <ags/X/machine/ags_drum_input_pad_callbacks.h>
#include <ags/X/machine/ags_drum.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_pattern.h>
#include <ags/audio/ags_recall.h>

#include <ags/audio/task/ags_append_channel.h>
#include <ags/audio/task/ags_add_audio_signal.h>
#include <ags/audio/task/ags_cancel_channel.h>

#include <ags/audio/recall/ags_play_audio_file.h>

#include <ags/audio/file/ags_audio_file.h>

#include <math.h>

void ags_drum_input_pad_open_play_callback(GtkToggleButton *toggle_button, AgsDrumInputPad *pad);
void ags_drum_input_pad_open_play_done(AgsRecall *recall, AgsDrumInputPad *drum_input_pad);
void ags_drum_input_pad_open_play_stop(AgsRecall *recall, AgsDrumInputPad *drum_input_pad);
void ags_drum_input_pad_open_play_cancel(AgsRecall *recall, AgsDrumInputPad *drum_input_pad);
void ags_drum_input_pad_open_response_callback(GtkWidget *widget, gint response, AgsDrumInputPad *pad);

#define AGS_DRUM_INPUT_PAD_OPEN_AUDIO_FILE_NAME "AgsDrumInputPadOpenAudioFileName"
#define AGS_DRUM_INPUT_PAD_OPEN_SPIN_BUTTON "AgsDrumInputPadOpenSpinButton"

void
ags_drum_input_pad_option_callback(GtkOptionMenu *option, AgsDrumInputPad *drum_input_pad)
{
  AgsDrum *drum;
  GList *list;

  drum = (AgsDrum *) gtk_widget_get_ancestor((GtkWidget *) drum_input_pad, AGS_TYPE_DRUM);

  list = gtk_container_get_children((GtkContainer *) option->menu);

  while(list != NULL && list->data != option->menu_item)
    list = list->next;

  drum_input_pad->pad.selected_line = (list != NULL) ? AGS_LINE(list->data): NULL;

  if(drum->selected_pad == drum_input_pad)
    ags_drum_set_pattern(drum);
}

void
ags_drum_input_pad_open_callback(GtkWidget *widget, AgsDrumInputPad *drum_input_pad)
{
  GtkFileChooserDialog *file_chooser;
  GtkHBox *hbox;
  GtkLabel *label;
  GtkSpinButton *spin_button;
  GtkToggleButton *play;

  if(drum_input_pad->file_chooser != NULL)
    return;

  drum_input_pad->file_chooser =
    file_chooser = (GtkFileChooserDialog *) gtk_file_chooser_dialog_new (g_strdup("Open File\0"),
									 (GtkWindow *) gtk_widget_get_toplevel((GtkWidget *) drum_input_pad),
									 GTK_FILE_CHOOSER_ACTION_OPEN,
									 GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
									 GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
									 NULL);
  gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(file_chooser), FALSE);
  g_object_set_data((GObject *) file_chooser, (char *) g_type_name(AGS_TYPE_AUDIO_FILE), NULL);
  g_object_set_data((GObject *) file_chooser, AGS_DRUM_INPUT_PAD_OPEN_AUDIO_FILE_NAME, NULL);

  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_file_chooser_set_extra_widget((GtkFileChooser *) file_chooser, (GtkWidget *) hbox);
  
  label = (GtkLabel *) gtk_label_new(g_strdup("channel: \0"));
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) label, FALSE, FALSE, 0);

  spin_button = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 16.0, 1.0);
  g_object_set_data((GObject *) file_chooser, AGS_DRUM_INPUT_PAD_OPEN_SPIN_BUTTON, spin_button);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) spin_button, FALSE, FALSE, 0);

  if(drum_input_pad->pad.group->active)
    gtk_widget_set_sensitive((GtkWidget *) spin_button, FALSE);

  play = (GtkToggleButton *) g_object_new(GTK_TYPE_TOGGLE_BUTTON,
					  "label\0", GTK_STOCK_MEDIA_PLAY,
					  "use-stock\0", TRUE,
					  "use-underline\0", TRUE,
					  NULL);
  gtk_box_pack_start((GtkBox *) GTK_DIALOG(file_chooser)->action_area, (GtkWidget *) play, FALSE, FALSE, 0);
  gtk_box_reorder_child((GtkBox *) GTK_DIALOG(file_chooser)->action_area, (GtkWidget *) play, 0);

  gtk_widget_show_all((GtkWidget *) file_chooser);

  g_signal_connect((GObject *) file_chooser, "response\0",
		   G_CALLBACK(ags_drum_input_pad_open_response_callback), (gpointer) drum_input_pad);
  g_signal_connect((GObject *) play, "toggled\0",
		   G_CALLBACK(ags_drum_input_pad_open_play_callback), (gpointer) drum_input_pad);
}

void
ags_drum_input_pad_open_play_callback(GtkToggleButton *toggle_button, AgsDrumInputPad *drum_input_pad)
{
  /*
  GtkFileChooserDialog *file_chooser;
  AgsDevout *devout;
  AgsAudioFile *audio_file;
  AgsPlayAudioFile *play_audio_file;
  GList *list;
  gchar *name0, *name1;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  file_chooser = drum_input_pad->file_chooser;
  name0 = (gchar *) gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));
  name1 = g_object_get_data((GObject *) file_chooser, AGS_DRUM_INPUT_PAD_OPEN_AUDIO_FILE_NAME);

  devout = AGS_DEVOUT(AGS_AUDIO(drum_input_pad->pad.channel->audio)->devout);

  if(toggle_button->active){
    /* AgsPlayAudioFile * /
    play_audio_file = ags_play_audio_file_new();
    play_audio_file->devout = devout;
    ags_play_audio_file_connect(play_audio_file);

    g_object_set_data((GObject *) file_chooser, (char *) g_type_name(AGS_TYPE_PLAY_AUDIO_FILE), play_audio_file);

    g_signal_connect((GObject *) play_audio_file, "done\0",
		     G_CALLBACK(ags_drum_input_pad_open_play_done), drum_input_pad);
    g_signal_connect((GObject *) play_audio_file, "cancel\0",
		     G_CALLBACK(ags_drum_input_pad_open_play_cancel), drum_input_pad);

    /* AgsAudioFile * /
    if(!g_strcmp0(name0, name1)){
      audio_file = (AgsAudioFile *) g_object_get_data((GObject *) file_chooser, g_type_name(AGS_TYPE_AUDIO_FILE));
    }else{
      if(name1 != NULL){
	audio_file = g_object_get_data((GObject *) file_chooser, (char *) g_type_name(AGS_TYPE_AUDIO_FILE));
	g_object_unref(G_OBJECT(audio_file));
      }

      audio_file = ags_audio_file_new();
      g_object_set_data((GObject *) file_chooser, (char *) g_type_name(AGS_TYPE_AUDIO_FILE), audio_file);

      audio_file->flags |= AGS_AUDIO_FILE_ALL_CHANNELS;
      audio_file->name = (gchar *) name0;

      ags_audio_file_set_devout(audio_file, devout);

      ags_audio_file_open(audio_file);
      fprintf(stdout, "ags_drum_input_pad_open_play: \n\0");
      
      AGS_AUDIO_FILE_GET_CLASS(audio_file)->read_buffer(audio_file);
      // ags_audio_file_read_audio_signal will be called later
    }

    play_audio_file->audio_file = audio_file;

    /* AgsDevout * /
    g_static_mutex_lock(&mutex);
    devout->play_recall = g_list_append(devout->play_recall, (gpointer) play_audio_file);
    devout->flags |= AGS_DEVOUT_PLAY_RECALL;
    devout->play_recall_ref++;
    AGS_DEVOUT_GET_CLASS(devout)->run((void *) devout);
    g_static_mutex_unlock(&mutex);
  }else{
    fprintf(stdout, "ags_drum_input_pad_open_file_play_done\n\0");

    if((AGS_DRUM_INPUT_PAD_OPEN_PLAY_DONE & (drum_input_pad->flags)) == 0){
      play_audio_file = (AgsPlayAudioFile *) g_object_get_data((GObject *) file_chooser, (char *) g_type_name(AGS_TYPE_PLAY_AUDIO_FILE));
      play_audio_file->recall.flags |= AGS_RECALL_CANCEL;
    }else
      drum_input_pad->flags &= (~AGS_DRUM_INPUT_PAD_OPEN_PLAY_DONE);
  }
*/
}

void
ags_drum_input_pad_open_play_done(AgsRecall *recall, AgsDrumInputPad *drum_input_pad)
{
  GtkToggleButton *toggle_button;

  recall->flags |= AGS_RECALL_REMOVE;

  toggle_button = (GtkToggleButton *) gtk_container_get_children((GtkContainer *) GTK_DIALOG(drum_input_pad->file_chooser)->action_area)->data;

  drum_input_pad->flags |= AGS_DRUM_INPUT_PAD_OPEN_PLAY_DONE;
  gtk_toggle_button_set_active(toggle_button, FALSE);
}

void
ags_drum_input_pad_open_play_stop(AgsRecall *recall, AgsDrumInputPad *drum_input_pad)
{
  AgsPlayAudioFile *play_audio_file;

  play_audio_file = AGS_PLAY_AUDIO_FILE(recall);
  g_object_unref((GObject *) play_audio_file);
}

void
ags_drum_input_pad_open_play_cancel(AgsRecall *recall, AgsDrumInputPad *drum_input_pad)
{
  AgsPlayAudioFile *play_audio_file;

  play_audio_file = AGS_PLAY_AUDIO_FILE(recall);
  g_object_unref((GObject *) play_audio_file);
}

void
ags_drum_input_pad_open_response_callback(GtkWidget *widget, gint response, AgsDrumInputPad *drum_input_pad)
{
  /*
  GtkFileChooserDialog *file_chooser;
  GtkSpinButton *spin_button;
  AgsDevout *devout;
  AgsChannel *channel;
  AgsInput *input;
  AgsAudioFile *audio_file;
  AgsAudioSignal *audio_signal_source_old;
  char *name0, *name1;
  GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  file_chooser = drum_input_pad->file_chooser;

  if(response == GTK_RESPONSE_ACCEPT){
    name0 = gtk_file_chooser_get_filename((GtkFileChooser *) file_chooser);
    name1 = (char *) g_object_get_data((GObject *) file_chooser, AGS_DRUM_INPUT_PAD_OPEN_AUDIO_FILE_NAME);
    spin_button = (GtkSpinButton *) g_object_get_data((GObject *) file_chooser, AGS_DRUM_INPUT_PAD_OPEN_SPIN_BUTTON);

    if(!g_strcmp0(name0, name1)){
      audio_file = (AgsAudioFile *) g_object_get_data((GObject *) file_chooser, g_type_name(AGS_TYPE_AUDIO_FILE));
    }else{
      if(name1 != NULL){
	audio_file = (AgsAudioFile *) g_object_get_data((GObject *) file_chooser, g_type_name(AGS_TYPE_AUDIO_FILE));
	g_object_unref(G_OBJECT(audio_file));
      }

      audio_file = ags_audio_file_new();
      audio_file->name = name0;
      devout = AGS_DEVOUT(AGS_AUDIO(drum_input_pad->pad.channel->audio)->devout);
      ags_audio_file_set_devout(audio_file, devout);
      ags_audio_file_open(audio_file);
      AGS_AUDIO_FILE_GET_CLASS(audio_file)->read_buffer(audio_file);
      // ags_audio_file_read_audio_signal will be called later
    }

    channel = AGS_CHANNEL(drum_input_pad->pad.channel);

    if(drum_input_pad->pad.group->active){
      GList *list;
      guint i;

      audio_file->flags |= AGS_AUDIO_FILE_ALL_CHANNELS;
      ags_audio_file_read_audio_signal(audio_file);

      list = audio_file->audio_signal;

      for(i = 0; i < AGS_AUDIO(drum_input_pad->pad.channel->audio)->audio_channels && i < audio_file->channels; i++){
	if(channel->link != NULL)
	  ags_channel_set_link(channel, NULL);

	AGS_AUDIO_SIGNAL(list->data)->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	AGS_AUDIO_SIGNAL(list->data)->recycling = (GObject *) channel->first_recycling;

	audio_signal_source_old = ags_audio_signal_get_template(channel->first_recycling->audio_signal);

	g_static_mutex_lock(&mutex);
	channel->first_recycling->audio_signal = g_list_remove(channel->first_recycling->audio_signal, (gpointer) audio_signal_source_old);
	channel->first_recycling->audio_signal = g_list_prepend(channel->first_recycling->audio_signal, list->data);
	g_static_mutex_unlock(&mutex);

	g_object_unref(G_OBJECT(audio_signal_source_old));

	channel = channel->next;
	list = list->next;
      }
    }else{
      channel = ags_channel_nth(channel, (guint) gtk_option_menu_get_history(drum_input_pad->pad.option));
      audio_file->channel = (guint) spin_button->adjustment->value;
      ags_audio_file_read_audio_signal(audio_file);

      if(channel->link != NULL)
	ags_channel_set_link(channel, NULL);

      AGS_AUDIO_SIGNAL(audio_file->audio_signal->data)->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
      AGS_AUDIO_SIGNAL(audio_file->audio_signal->data)->recycling = (GObject *) channel->first_recycling;
      audio_signal_source_old = ags_audio_signal_get_template(channel->first_recycling->audio_signal);

      g_static_mutex_lock(&mutex);
      channel->first_recycling->audio_signal = g_list_remove(channel->first_recycling->audio_signal, (gpointer) audio_signal_source_old);
      channel->first_recycling->audio_signal = g_list_prepend(channel->first_recycling->audio_signal, audio_file->audio_signal->data);
      g_static_mutex_unlock(&mutex);

      g_object_unref(G_OBJECT(audio_signal_source_old));
    }

    gtk_widget_destroy((GtkWidget *) file_chooser);
    g_object_unref(G_OBJECT(audio_file));
  }else if(response == GTK_RESPONSE_CANCEL){
    audio_file = (AgsAudioFile *) g_object_get_data((GObject *) file_chooser, g_type_name(AGS_TYPE_AUDIO_FILE));

    if(audio_file != NULL)
      g_object_unref(G_OBJECT(audio_file));

    gtk_widget_destroy((GtkWidget *) file_chooser);
  }

  drum_input_pad->file_chooser = NULL;
  */
}

void
ags_drum_input_pad_play_callback(GtkToggleButton *toggle_button, AgsDrumInputPad *drum_input_pad)
{
  AgsDrum *drum;
  AgsDevout *devout;
  AgsChannel *channel;

  drum = (AgsDrum *) gtk_widget_get_ancestor((GtkWidget *) drum_input_pad, AGS_TYPE_DRUM);
  devout = AGS_DEVOUT(drum->machine.audio->devout);

  if(toggle_button->active){
    AgsAppendChannel *append_channel;
    AgsAddAudioSignal *add_audio_signal;
    AgsRecycling *recycling;
    AgsRecallID *recall_id;
    GList *tasks;
    AgsGroupId group_id, child_group_id;
    guint flags;

    group_id = ags_recall_id_generate_group_id();
    child_group_id = ags_recall_id_generate_group_id();

    tasks = NULL;
    flags = AGS_AUDIO_SIGNAL_STANDALONE;

    if(drum_input_pad->pad.group->active){
      AgsChannel *next_pad;
      gint stage;
      gboolean arrange_group_id, duplicate_templates, resolve_dependencies;

      next_pad = drum_input_pad->pad.channel->next_pad;

      /* do init stuff here */
      for(stage = 0; stage < 3; stage++){
	channel = drum_input_pad->pad.channel;

	if(stage == 0){
	  arrange_group_id = TRUE;
	  duplicate_templates = TRUE;
	  resolve_dependencies = TRUE;
	}else{
	  arrange_group_id = FALSE;
	  duplicate_templates = FALSE;
	  resolve_dependencies = FALSE;
	}
	
	while(channel != next_pad){
	  if(stage == 0)
	    channel->devout_play->group_id = group_id;

	  ags_channel_recursive_play_init(channel, stage,
					  arrange_group_id, duplicate_templates, TRUE, FALSE, FALSE, resolve_dependencies,
					  group_id, child_group_id,
					  0);
	  
	  channel = channel->next;
	}
      }

      /* append to AgsDevout */
      channel = drum_input_pad->pad.channel;

      while(channel != next_pad){
	channel->devout_play->flags |= AGS_DEVOUT_PLAY_PAD;
	channel->devout_play->group_id = group_id;

	/* append channel for playback */
	append_channel = ags_append_channel_new(devout,
						channel->devout_play);
	tasks = g_list_prepend(tasks, append_channel);

	/* play an audio signal */
	recycling = channel->first_recycling;

	recall_id = ags_recall_id_find_group_id(channel->recall_id, group_id);

	while(recycling != channel->last_recycling->next){
	  add_audio_signal = ags_add_audio_signal_new(recycling,
						      devout,
						      recall_id,
						      flags);
	  tasks = g_list_prepend(tasks, add_audio_signal);

	  recycling = recycling->next;
	}

	channel = channel->next;
      }

      tasks = g_list_reverse(tasks);
      ags_devout_append_tasks(devout, tasks);
    }else{
      channel = ags_channel_nth(drum_input_pad->pad.channel,
				gtk_option_menu_get_history(drum_input_pad->pad.option));

      /* do init stuff here */
      channel->devout_play->group_id = group_id;

      ags_channel_recursive_play_init(channel, -1,
				      TRUE, TRUE, TRUE, FALSE, FALSE, TRUE,
				      group_id, child_group_id,
				      0);
      
      /* append to AgsDevout */
      channel->devout_play->flags &= (~AGS_DEVOUT_PLAY_PAD);

      append_channel = ags_append_channel_new(devout,
					      channel->devout_play);
      tasks = g_list_prepend(tasks, append_channel);

      /* play an audio signal */
      recycling = channel->first_recycling;
      
      recall_id = ags_recall_id_find_group_id(channel->recall_id, group_id);
      
      while(recycling != channel->last_recycling->next){
	add_audio_signal = ags_add_audio_signal_new(recycling,
						    devout,
						    recall_id,
						    flags);
	tasks = g_list_prepend(tasks, add_audio_signal);
	
	recycling = recycling->next;
      }

      tasks = g_list_reverse(tasks);      
      ags_devout_append_tasks(devout, tasks);
    }

    /* call run */
    if((AGS_AUDIO_LOOP_PLAY_CHANNEL & (devout->flags)) == 0){
      devout->audio_loop->flags |= AGS_AUDIO_LOOP_PLAY_CHANNEL;
    }

    ags_thread_start(devout->devout_thread);
  }else{
    AgsCancelChannel *cancel_channel;

    /* abort code */
    channel = drum_input_pad->pad.channel;

    if((AGS_DEVOUT_PLAY_PAD & (channel->devout_play->flags)) != 0){
      AgsChannel *next_pad;

      next_pad = channel->next_pad;

      if((AGS_DEVOUT_PLAY_DONE & (channel->devout_play->flags)) == 0){
	while(channel != next_pad){
	  cancel_channel = ags_cancel_channel_new(channel, channel->devout_play->group_id,
						  channel->devout_play);

	  ags_devout_append_task(devout, (AgsTask *) cancel_channel);

	  channel = channel->next;
	}
      }else{
	while(channel != next_pad){
	  channel->devout_play->flags |= AGS_DEVOUT_PLAY_REMOVE;
	  channel->devout_play->flags &= (~AGS_DEVOUT_PLAY_DONE);

	  channel = channel->next;
	}
      }
    }else{
      channel = ags_channel_nth(channel,
				gtk_option_menu_get_history(drum_input_pad->pad.option));

      if((AGS_DEVOUT_PLAY_DONE & (channel->devout_play->flags)) == 0){
	cancel_channel = ags_cancel_channel_new(channel, channel->devout_play->group_id,
						channel->devout_play);

	ags_devout_append_task(devout, (AgsTask *) cancel_channel);
      }else{
	channel->devout_play->flags |= AGS_DEVOUT_PLAY_REMOVE;
	channel->devout_play->flags &= (~AGS_DEVOUT_PLAY_DONE);
      }
    }
  }
}

void
ags_drum_input_pad_edit_callback(GtkWidget *toggle_button, AgsDrumInputPad *drum_input_pad)
{
  GtkToggleButton *toggle;
  AgsDrum *drum;

  drum = (AgsDrum *) gtk_widget_get_ancestor((GtkWidget *) drum_input_pad, AGS_TYPE_DRUM);

  if(drum->selected_edit_button != NULL){
    if(GTK_TOGGLE_BUTTON(toggle_button) != drum->selected_edit_button){
      toggle = drum->selected_edit_button;
      drum->selected_edit_button = NULL;
      gtk_toggle_button_set_active((GtkToggleButton *) toggle, FALSE);
      drum->selected_edit_button = (GtkToggleButton *) toggle_button;
      drum->selected_pad = (AgsDrumInputPad *) gtk_widget_get_ancestor((GtkWidget *) toggle_button, AGS_TYPE_DRUM_INPUT_PAD);
      ags_drum_set_pattern(drum);
    }else{
      toggle = drum->selected_edit_button;
      drum->selected_edit_button = NULL;
      gtk_toggle_button_set_active((GtkToggleButton *) toggle, TRUE);
      drum->selected_edit_button = toggle;
    }
  }
}
