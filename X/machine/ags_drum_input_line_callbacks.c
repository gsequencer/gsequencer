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
#include "../../audio/recall/ags_copy_pattern_channel.h"
#include "../../audio/recall/ags_copy_pattern_channel_run.h"

void
ags_drum_input_line_audio_set_pads_callback(AgsAudio *audio, GType type,
					    guint pads, guint pads_old,
					    AgsDrumInputLine *drum_input_line)
{
  if(type == AGS_TYPE_OUTPUT){
    if(pads > pads_old){
      ags_drum_input_line_map_recall(drum_input_line, pads_old);
    }else{
      AgsChannel *destination, *channel;
      AgsRecall *recall;
      GList *recall_list;

      channel = drum_input_line->line.channel;

      /* AgsCopyPattern */
      recall_list = channel->recall;

      while((recall_list = ags_recall_find_type(recall_list, AGS_TYPE_COPY_PATTERN_CHANNEL_RUN)) != NULL){
	recall = AGS_RECALL(recall_list->data);
	destination = AGS_COPY_PATTERN_CHANNEL(recall->recall_channel)->destination;

	if(destination->pad >= pads){
	  channel->recall = g_list_delete_link(channel->recall, recall_list);
	  
	  g_object_unref(recall);
	  g_object_unref(destination);
	}

	recall_list = recall_list->next;
      }
    }
  }
}

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
  AgsCopyPatternChannelRun *copy_pattern_channel_run;

  fprintf(stdout, "ags_drum_input_line_copy_pattern_done\n\0");

  copy_pattern_channel_run = AGS_COPY_PATTERN_CHANNEL_RUN(recall);
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
