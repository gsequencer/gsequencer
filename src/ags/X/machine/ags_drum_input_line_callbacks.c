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

#include <ags/X/machine/ags_drum_input_line_callbacks.h>
#include <ags/X/machine/ags_drum.h>

#include <ags/main.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_pattern.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_volume_channel.h>
#include <ags/audio/recall/ags_play_channel_run.h>
#include <ags/audio/recall/ags_copy_pattern_channel.h>
#include <ags/audio/recall/ags_copy_pattern_channel_run.h>

#include <ags/audio/task/ags_change_indicator.h>

#include <ags/widget/ags_vindicator.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_line_member.h>

void
ags_drum_input_line_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsDrumInputLine *drum_input_line)
{
  /* set devout */
  //TODO:JK: implement me
}

void
ags_drum_input_line_audio_set_pads_callback(AgsAudio *audio, GType type,
					    guint pads, guint pads_old,
					    AgsDrumInputLine *drum_input_line)
{
  if(type == AGS_TYPE_OUTPUT){
    if(pads > pads_old){
      AgsChannel *current, *output;
      GList *recall;

      output = audio->output;

      while(output != NULL){
	current = ags_channel_nth(audio->input,
				  output->audio_channel);

	while(current != NULL){
	  recall = current->play;

	  while(recall != NULL){
	    if(AGS_IS_RECALL_CHANNEL_RUN(recall->data)){
	      g_object_set(G_OBJECT(recall->data),
			   "destination\0", output,
			   NULL);
	      ags_connectable_connect(AGS_CONNECTABLE(recall->data));
	    }

	    recall = recall->next;
	  }

	  recall = current->recall;

	  while(recall != NULL){
	    if(AGS_IS_RECALL_CHANNEL_RUN(recall->data)){
	      g_object_set(G_OBJECT(recall->data),
			   "destination\0", output,
			   NULL);

	      ags_connectable_connect(AGS_CONNECTABLE(recall->data));
	    }

	    recall = recall->next;
	  }

	  current = current->next_pad;
	}

	output = output->next;
      }
    }else{
      /* empty */
    }
  }
}

void
ags_drum_input_line_peak_run_post_callback(AgsRecall *peak_channel,
					   AgsDrumInputLine *drum_input_line)
{
  AgsTaskThread *task_thread;
  AgsChangeIndicator *change_indicator;
  AgsDrum *drum;
  GList *list;

  drum = (AgsDrum *) gtk_widget_get_ancestor(drum_input_line,
					     AGS_TYPE_DRUM);
  task_thread = AGS_AUDIO_LOOP(AGS_MAIN(AGS_DEVOUT(AGS_MACHINE(drum)->audio->devout)->ags_main)->main_loop)->task_thread;

  list = gtk_container_get_children(AGS_LINE(drum_input_line)->expander->table);

  while(list != NULL){
    if(AGS_IS_LINE_MEMBER(list->data) &&
       AGS_LINE_MEMBER(list->data)->widget_type == AGS_TYPE_VINDICATOR){
      GtkWidget *child;
      GValue value = {0,};

      child = gtk_bin_get_child(AGS_LINE_MEMBER(list->data));

      g_value_init(&value, G_TYPE_DOUBLE);
      ags_port_safe_read(AGS_LINE_MEMBER(list->data)->port,
			 &value);

      change_indicator = ags_change_indicator_new(child,
						  g_value_get_double(&value));

      ags_task_thread_append_task(task_thread,
				  change_indicator);

      break;
    }
    
    list = list->next;
  }
}

void
ags_drum_input_line_channel_done_callback(AgsChannel *source, AgsDrumInputLine *drum_input_line)
{
  AgsChannel *channel;
  AgsDevoutPlay *devout_play;
  AgsChannel *next_pad;
  GList *current_recall;
  gboolean all_done;

  g_message("ags_drum_input_line_channel_done\0");

  channel = AGS_PAD(AGS_LINE(drum_input_line)->pad)->channel;
  next_pad = channel->next_pad;

  all_done = TRUE;

  while(channel != next_pad){
    current_recall = channel->play;
    devout_play = AGS_DEVOUT_PLAY(channel->devout_play);
    
    if(devout_play->recall_id[0] != NULL){
      all_done = FALSE;
      break;
    }
    
    channel = channel->next;
  }

  if(all_done){
    AgsDrumInputPad *drum_input_pad;

    drum_input_pad = AGS_DRUM_INPUT_PAD(AGS_LINE(drum_input_line)->pad);

    gtk_toggle_button_set_active(drum_input_pad->play, FALSE);
  }
}

void
ags_drum_input_line_play_channel_run_cancel(AgsRecall *recall, AgsDrumInputLine *drum_input_line)
{
  /* empty */
}

void
ags_drum_input_line_play_volume_done(AgsRecall *recall, AgsDrumInputLine *drum_input_line)
{
  /* empty */
}

void
ags_drum_input_line_play_volume_cancel(AgsRecall *recall, AgsDrumInputLine *drum_input_line)
{
  /* empty */
}

void
ags_drum_input_line_copy_pattern_done(AgsRecall *recall, AgsDrumInputLine *drum_input_line)
{
  AgsCopyPatternChannelRun *copy_pattern_channel_run;

  fprintf(stdout, "ags_drum_input_line_copy_pattern_done\n\0");

  copy_pattern_channel_run = AGS_COPY_PATTERN_CHANNEL_RUN(recall);
  /*
  g_list_free(copy_pattern->destination);
  copy_pattern->destination = NULL;
  */
}

void
ags_drum_input_line_copy_pattern_cancel(AgsRecall *recall, AgsDrumInputLine *drum_input_line)
{
  /* empty */
}

void
ags_drum_input_line_recall_volume_done(AgsRecall *recall, AgsDrumInputLine *drum_input_line)
{
  /* empty */
}

void
ags_drum_input_line_recall_volume_cancel(AgsRecall *recall, AgsDrumInputLine *drum_input_line)
{
  /* empty */
}
