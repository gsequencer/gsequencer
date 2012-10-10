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

  fprintf(stdout, "ags_drum_input_line_play_channel_run_done\n\0");

  channel = AGS_LINE(drum_input_line)->channel;

  if(channel->devout_play->group_id == recall->recall_id->group_id &&
     ags_recall_is_done(AGS_RECALL_CONTAINER(recall->container)->recall_channel_run,
			recall->recall_id->group_id)){
    AgsDrumInputPad *drum_input_pad;

    printf("ags_drum_input_line_play_channel_run_done\n\0");
    drum_input_pad = AGS_DRUM_INPUT_PAD(AGS_LINE(drum_input_line)->pad);

    gtk_toggle_button_set_active(drum_input_pad->play, FALSE);
  }

  /*
  AgsDevout *devout;
  AgsChannel *channel;
  AgsPlayChannel *play_channel;

  fprintf(stdout, "ags_drum_input_line_play_channel_done\n\0");

  play_channel = (AgsPlayChannel *) recall;
  devout = play_channel->devout;
  channel = drum_input_line->line.channel;

  if((AGS_DEVOUT_PLAY_PAD & (channel->devout_play->flags)) != 0){
    drum_input_pad->play_ref--;

    if(drum_input_pad->play_ref == 0){
      AgsChannel *next_pad;

      next_pad = channel->next_pad;

      while(channel != next_pad){
	channel->devout_play->flags |= AGS_DEVOUT_PLAY_DONE;

	channel = channel->next;
      }

      gtk_toggle_button_set_active(drum_input_pad->play, FALSE);
    }
  }else{
    channel = play_channel->source;
    channel->devout_play->flags |= AGS_DEVOUT_PLAY_DONE;
    gtk_toggle_button_set_active(drum_input_pad->play, FALSE);
  }
  */
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
