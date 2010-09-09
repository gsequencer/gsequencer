#include "ags_play_audio_signal.h"

#include "../ags_devout.h"
#include "../ags_audio_signal.h"
#include "../ags_recycling.h"
#include "../ags_channel.h"
#include "../ags_recall_id.h"

#include <stdlib.h>
#include <pthread.h>

GType ags_play_audio_signal_get_type();
void ags_play_audio_signal_class_init(AgsPlayAudioSignalClass *play_audio_signal);
void ags_play_audio_signal_init(AgsPlayAudioSignal *play_audio_signal);
void ags_play_audio_signal_finalize(GObject *gobject);

void ags_play_audio_signal_run_inter(AgsRecall *recall, gpointer data);

AgsRecall* ags_play_audio_signal_duplicate(AgsRecall *recall, AgsRecallID *recall_id);

static gpointer ags_play_audio_signal_parent_class = NULL;

GType
ags_play_audio_signal_get_type()
{
  static GType ags_type_play_audio_signal = 0;

  if(!ags_type_play_audio_signal){
    static const GTypeInfo ags_play_audio_signal_info = {
      sizeof (AgsPlayAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_audio_signal_init,
    };

    ags_type_play_audio_signal = g_type_register_static(AGS_TYPE_RECALL,
							"AgsPlayAudioSignal\0",
							&ags_play_audio_signal_info,
							0);
  }

  return(ags_type_play_audio_signal);
}

void
ags_play_audio_signal_class_init(AgsPlayAudioSignalClass *play_audio_signal)
{
  GObjectClass *gobject;
  
  ags_play_audio_signal_parent_class = g_type_class_peek_parent(play_audio_signal);

  gobject = (GObjectClass *) play_audio_signal;
  gobject->finalize = ags_play_audio_signal_finalize;
}

void
ags_play_audio_signal_init(AgsPlayAudioSignal *play_audio_signal)
{
  play_audio_signal->source = NULL;
  play_audio_signal->audio_channel = 0;

  play_audio_signal->devout = NULL;
  play_audio_signal->attack = NULL;
}

void
ags_play_audio_signal_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_play_audio_signal_parent_class)->finalize(gobject);
}

void
ags_play_audio_signal_connect(AgsPlayAudioSignal *play_audio_signal)
{
  ags_recall_connect((AgsRecall *) play_audio_signal);

  g_signal_connect((GObject *) play_audio_signal, "run_inter\0",
		   G_CALLBACK(ags_play_audio_signal_run_inter), NULL);
}

void
ags_play_audio_signal_run_inter(AgsRecall *recall, gpointer data)
{
  AgsPlayAudioSignal *play_audio_signal;
  AgsDevout *devout;
  AgsRecycling *recycling;
  AgsAudioSignal *source;
  GList *stream;
  short *buffer0, *buffer1;
  AgsAttack *attack;
  guint audio_channel;

  play_audio_signal = AGS_PLAY_AUDIO_SIGNAL(recall);

  devout = play_audio_signal->devout;
  source = AGS_AUDIO_SIGNAL(play_audio_signal->source);
  stream = source->stream_current;

  if(stream == NULL){
    //    ags_recall_done(recall, recall_id);
    return;
  }

  if((AGS_DEVOUT_BUFFER0 & devout->flags) != 0){
    buffer0 = devout->buffer[1];
    buffer1 = devout->buffer[2];
  }else if((AGS_DEVOUT_BUFFER1 & devout->flags) != 0){
    buffer0 = devout->buffer[2];
    buffer1 = devout->buffer[3];
  }else if((AGS_DEVOUT_BUFFER2 & devout->flags) != 0){
    buffer0 = devout->buffer[3];
    buffer1 = devout->buffer[0];
  }else if((AGS_DEVOUT_BUFFER3 & devout->flags) != 0){
    buffer0 = devout->buffer[0];
    buffer1 = devout->buffer[1];
  }

  attack = play_audio_signal->attack;
  audio_channel = play_audio_signal->audio_channel;

  ags_audio_signal_copy_buffer_to_buffer(&(buffer0[attack->first_start * devout->pcm_channels + audio_channel]), devout->pcm_channels,
					 (short *) stream->data, 1,
					 attack->first_length);

  if(attack->first_start != 0){
    ags_audio_signal_copy_buffer_to_buffer(&(buffer1[audio_channel]), devout->pcm_channels,
					   &(((short *) stream->data)[attack->first_length]), 1,
					   attack->first_start);
  }
  /*
  audio_signal->stream_current = audio_signal->stream_current->next;

  if(audio_signal->stream_current == NULL)
    ags_recall_done(recall, recall_id);
  */
}

AgsRecall*
ags_play_audio_signal_duplicate(AgsRecall *recall, AgsRecallID *recall_id)
{
  AgsPlayAudioSignal *play_audio_signal, *copy;

  play_audio_signal = (AgsPlayAudioSignal *) recall;
  copy = (AgsPlayAudioSignal *) AGS_RECALL_CLASS(ags_play_audio_signal_parent_class)->duplicate(recall, recall_id);

  copy->source = play_audio_signal->source;
  copy->audio_channel = play_audio_signal->audio_channel;
  copy->devout = play_audio_signal->devout;

  return((AgsRecall *) copy);
}

AgsPlayAudioSignal*
ags_play_audio_signal_new(AgsAudioSignal *source, guint audio_channel,
			  AgsDevout *devout)
{
  AgsPlayAudioSignal *play_audio_signal;

  play_audio_signal = (AgsPlayAudioSignal *) g_object_new(AGS_TYPE_PLAY_AUDIO_SIGNAL, NULL);

  play_audio_signal->devout = devout;

  if(devout != NULL)
    play_audio_signal->attack = ags_attack_get_from_devout((GObject *) devout);
  
  play_audio_signal->source = source;
  play_audio_signal->audio_channel = audio_channel;

  return(play_audio_signal);
}
