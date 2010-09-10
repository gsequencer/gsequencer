#include "ags_drum_output_pad.h"
#include "ags_drum_output_pad_callbacks.h"

#include "ags_drum.h"

#include "../ags_line.h"

#include "../../audio/ags_audio.h"
#include "../../audio/ags_channel.h"
#include "../../audio/ags_recycling.h"
#include "../../audio/ags_audio_signal.h"

#include "../../audio/recall/ags_delay.h"
#include "../../audio/recall/ags_play_channel.h"
#include "../../audio/recall/ags_play_volume.h"

GType ags_drum_output_pad_get_type();
void ags_drum_output_pad_class_init(AgsDrumOutputPadClass *drum_output_pad);
void ags_drum_output_pad_init(AgsDrumOutputPad *drum_output_pad);
void ags_drum_output_pad_destroy(GtkObject *object);
void ags_drum_output_pad_connect(AgsDrumOutputPad *drum_output_pad);

void ags_drum_output_pad_resize_lines(AgsPad *pad, GType line_type,
				      guint audio_channels, guint audio_channels_old);

GType
ags_drum_output_pad_get_type()
{
  static GType drum_output_pad_type = 0;

  if (!drum_output_pad_type){
    static const GtkTypeInfo drum_output_pad_info = {
      "AgsDrumOutputPad\0",
      sizeof(AgsDrumOutputPad), /* base_init */
      sizeof(AgsDrumOutputPadClass), /* base_finalize */
      (GtkClassInitFunc) ags_drum_output_pad_class_init,
      (GtkObjectInitFunc) ags_drum_output_pad_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      (GtkClassInitFunc) NULL,
    };
    drum_output_pad_type = gtk_type_unique (AGS_TYPE_PAD, &drum_output_pad_info);
  }

  return (drum_output_pad_type);
}

void
ags_drum_output_pad_class_init(AgsDrumOutputPadClass *drum_output_pad)
{
  AgsPadClass *pad;

  pad = (AgsPadClass *) drum_output_pad;

  pad->resize_lines = ags_drum_output_pad_resize_lines;
}

void
ags_drum_output_pad_init(AgsDrumOutputPad *drum_output_pad)
{
  drum_output_pad->flags = 0;
}

void
ags_drum_output_pad_destroy(GtkObject *object)
{
}

void
ags_drum_output_pad_connect(AgsDrumOutputPad *drum_output_pad)
{
}

void
ags_drum_output_pad_resize_lines(AgsPad *pad, GType line_type,
				 guint audio_channels, guint audio_channels_old)
{
  AgsDrum *drum;
  AgsDrumOutputPad *drum_output_pad;
  AgsLine *line;
  AgsChannel *channel;
  AgsAudioSignal *audio_signal;
  AgsDelaySharedAudio *delay_shared_audio;
  AgsPlayChannel *play_channel;
  GList *list_line, *recall_shared;
  guint stop;

  ags_pad_real_resize_lines(pad, audio_channels, audio_channels_old);

  if(audio_channels > audio_channels_old){
    drum = (AgsDrum *) gtk_widget_get_ancestor((GtkWidget *) pad, AGS_TYPE_DRUM);
    drum_output_pad = (AgsDrumOutputPad *) pad;

    channel = pad->channel;
    
    list_line = gtk_container_get_children((GtkContainer *) pad->option->menu);
    list_line = g_list_nth(list_line, audio_channels_old);

    recall_shared = ags_recall_shared_find_type(drum->machine.audio->recall_shared,
						AGS_TYPE_DELAY_SHARED_AUDIO);

    if(recall_shared != NULL){
      delay_shared_audio = (AgsDelaySharedAudio *) recall_shared->data;
      stop = ((guint) drum->length_spin->adjustment->value) * (delay_shared_audio->delay + 1);
    }else{
      stop = 1;
    }

    while(list_line != NULL){
      line = AGS_LINE(list_line->data);

      audio_signal = ags_audio_signal_get_template(channel->first_recycling->audio_signal);
      ags_audio_signal_stream_resize(audio_signal, stop);

      /* AgsPlayChannel */
      play_channel = ags_play_channel_new(channel,
					  AGS_DEVOUT(AGS_AUDIO(channel->audio)->devout));

      play_channel->recall.flags |= AGS_RECALL_TEMPLATE;

      ags_play_channel_connect(play_channel);

      g_signal_connect((GObject *) play_channel, "done\0",
		       G_CALLBACK(ags_drum_output_pad_play_done), drum_output_pad);

      g_signal_connect((GObject *) play_channel, "cancel\0",
		       G_CALLBACK(ags_drum_output_pad_play_cancel), drum_output_pad);

      channel->play = g_list_append(channel->play, (gpointer) play_channel);

      channel = channel->next;
      list_line = list_line->next;
    }
  }
}

AgsDrumOutputPad*
ags_drum_output_pad_new(AgsChannel *channel)
{
  AgsDrumOutputPad *drum_output_pad;

  drum_output_pad = (AgsDrumOutputPad *) g_object_new(AGS_TYPE_DRUM_OUTPUT_PAD, NULL);

  drum_output_pad->pad.channel = channel;

  return(drum_output_pad);
}
