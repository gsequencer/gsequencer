#include "ags_loop_channel.h"

#include "../ags_devout.h"
#include "../ags_audio.h"
#include "../ags_audio_signal.h"
#include "../ags_recall_id.h"

void ags_loop_channel_class_init(AgsLoopChannelClass *loop_channel);
void ags_loop_channel_init(AgsLoopChannel *loop_channel);
void ags_loop_channel_finalize(GObject *gobject);

void ags_loop_channel_run_pre(AgsRecall *recall, AgsRecallID *recall_id,
			      gpointer data);

static gpointer ags_loop_channel_parent_class = NULL;

GType
ags_loop_channel_get_type()
{
  static GType ags_type_loop_channel = 0;

  if(!ags_type_loop_channel){
    static const GTypeInfo ags_loop_channel_info = {
      sizeof (AgsLoopChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_loop_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLoopChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_loop_channel_init,
    };

    ags_type_loop_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						   "AgsLoopChannel\0",
						   &ags_loop_channel_info,
						   0);
  }
  return (ags_type_loop_channel);
}

void
ags_loop_channel_class_init(AgsLoopChannelClass *loop_channel)
{
  GObjectClass *gobject;

  ags_loop_channel_parent_class = g_type_class_peek_parent(loop_channel);

  gobject = (GObjectClass *) loop_channel;
  gobject->finalize = ags_loop_channel_finalize;
}

void
ags_loop_channel_init(AgsLoopChannel *loop_channel)
{
  loop_channel->channel = NULL;
  loop_channel->delay_audio_run = NULL;
}

void
ags_loop_channel_finalize(GObject *gobject)
{
  AgsLoopChannel *loop_channel;
}

void
ags_loop_channel_connect(AgsLoopChannel *loop_channel)
{
  //  ags_recall_connect(AGS_RECALL(loop_channel));

  g_signal_connect((GObject *) loop_channel, "run_pre\0",
		   G_CALLBACK(ags_loop_channel_run_pre), NULL);
}

void
ags_loop_channel_run_pre(AgsRecall *recall, AgsRecallID *recall_id,
			 gpointer data)
{
  AgsLoopChannel *loop_channel;
  AgsDevout *devout;
  AgsRecycling *recycling;
  AgsAudioSignal *audio_signal;

  loop_channel = AGS_LOOP_CHANNEL(recall);

  devout = AGS_DEVOUT(AGS_AUDIO(loop_channel->channel->audio)->devout);


    recycling = loop_channel->channel->first_recycling;

    while(recycling != loop_channel->channel->last_recycling->next){
      audio_signal = ags_audio_signal_new((GObject *) recycling,
					  (GObject *) recall_id);
      audio_signal->devout = (GObject *) devout;
      ags_audio_signal_connect(audio_signal);

      ags_recycling_add_audio_signal(recycling,
				     audio_signal);
      audio_signal->stream_current = audio_signal->stream_beginning;
      
      
      recycling = recycling->next;
    }
}

AgsLoopChannel*
ags_loop_channel_new(AgsChannel *channel,
		     AgsDelayAudioRun *delay_audio_run)
{
  AgsLoopChannel *loop_channel;

  loop_channel = (AgsLoopChannel *) g_object_new(AGS_TYPE_LOOP_CHANNEL, NULL);

  loop_channel->channel = channel;
  loop_channel->delay_audio_run = delay_audio_run;

  return(loop_channel);
}
