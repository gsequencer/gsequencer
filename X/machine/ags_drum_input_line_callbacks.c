#include "ags_drum_input_line_callbacks.h"
#include "ags_drum.h"

#include "../../audio/ags_devout.h"
#include "../../audio/ags_audio.h"
#include "../../audio/ags_input.h"
#include "../../audio/ags_output.h"
#include "../../audio/ags_audio_signal.h"
#include "../../audio/ags_pattern.h"
#include "../../audio/ags_recall.h"

#include "../../audio/recall/ags_play_volume.h"
#include "../../audio/recall/ags_play_channel.h"
#include "../../audio/recall/ags_copy_pattern.h"

void
ags_drum_input_line_play_channel_done(AgsRecall *recall, AgsDrumInputLine *drum_input_line)
{
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
ags_drum_input_line_play_channel_cancel(AgsRecall *recall, AgsDrumInputLine *drum_input_line)
{
}

void
ags_drum_input_line_play_volume_done(AgsRecall *recall, AgsDrumInputLine *drum_input_line)
{
  AgsPlayVolume *play_volume;

  recall->flags |= AGS_RECALL_REMOVE;

  play_volume = AGS_PLAY_VOLUME(recall);
  g_list_free(play_volume->audio_signal);
  play_volume->audio_signal = NULL;
}

void
ags_drum_input_line_play_volume_cancel(AgsRecall *recall, AgsDrumInputLine *drum_input_line)
{
}

void
ags_drum_input_line_copy_pattern_done(AgsRecall *recall, AgsDrumInputLine *drum_input_line)
{
  AgsCopyPattern *copy_pattern;

  fprintf(stdout, "ags_drum_input_line_copy_pattern_done\n\0");

  copy_pattern = AGS_COPY_PATTERN(recall);
  recall->flags |= AGS_RECALL_HIDE;
  /*
  g_list_free(copy_pattern->destination);
  copy_pattern->destination = NULL;
  */
}

void
ags_drum_input_line_copy_pattern_cancel(AgsRecall *recall, AgsDrumInputLine *drum_input_line)
{
}

void
ags_drum_input_line_copy_pattern_loop(AgsRecall *recall, AgsDrumInputLine *drum_input_line)
{
  //AGS_COPY_PATTERN(recall)->bit = 0;
}

void
ags_drum_input_line_recall_volume_done(AgsRecall *recall, AgsDrumInputLine *drum_input_line)
{
}

void
ags_drum_input_line_recall_volume_cancel(AgsRecall *recall, AgsDrumInputLine *drum_input_line)
{
}
