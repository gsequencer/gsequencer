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

#include <ags/X/ags_window.h>

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
      ags_drum_input_line_map_recall(drum_input_line, pads_old);
    }else{
      /* empty */
    }
  }
}

void
ags_drum_input_line_play_channel_run_done(AgsRecall *recall, AgsDrumInputLine *drum_input_line)
{
  AgsChannel *channel;
  AgsDevoutPlay *devout_play;
  AgsChannel *next_pad;
  GList *current_recall;
  gboolean all_done;

  g_message("ags_drum_input_line_play_channel_run_done\0");

  channel = AGS_LINE(drum_input_line)->channel;

  devout_play = AGS_DEVOUT_PLAY(channel->devout_play);

  channel = ags_channel_nth(AGS_AUDIO(channel->audio)->input, channel->line - channel->audio_channel);
  next_pad = channel->next_pad;

  all_done = TRUE;

  while(channel != next_pad){
    if(AGS_DEVOUT_PLAY(channel->devout_play)->group_id[0] != devout_play->group_id[0]){
      channel = channel->next;
      continue;
    }

    current_recall = channel->play;
    
    if(!ags_recall_is_done(current_recall,
			   devout_play->group_id[0])){
      all_done = FALSE;
      break;
    }
    
    channel = channel->next;
  }

  if(all_done){
    AgsDrumInputPad *drum_input_pad;

    g_message("ags_drum_input_line_play_channel_run_done\0");
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
