#include "ags_recall_volume.h"

#include "../ags_devout.h"
#include "../ags_audio.h"
#include "../ags_output.h"
#include "../ags_input.h"
#include "../ags_recycling.h"
#include "../ags_audio_signal.h"
#include "../ags_recall_id.h"

#include <stdlib.h>
#include <string.h>

void ags_recall_volume_class_init(AgsRecallVolumeClass *recall_volume);
void ags_recall_volume_init(AgsRecallVolume *recall_volume);
void ags_recall_volume_connect(AgsRecallVolume *recall_volume);

void ags_recall_volume(AgsRecall *recall, AgsRecallID *recall_id, gpointer data);
void ags_recall_volume_stop(AgsRecall *recall, AgsRecallID *recall_id, gpointer data);
void ags_recall_volume_cancel(AgsRecall *recall, AgsRecallID *recall_id, gpointer data);

GType
ags_recall_volume_get_type()
{
  static GType ags_type_recall_volume = 0;

  if(!ags_type_recall_volume){
    static const GTypeInfo ags_recall_volume_info = {
      sizeof (AgsRecallVolumeClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_volume_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallVolume),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_volume_init,
    };
    ags_type_recall_volume = g_type_register_static(AGS_TYPE_RECALL, "AgsRecallVolume\0", &ags_recall_volume_info, 0);
  }
  return (ags_type_recall_volume);
}

void
ags_recall_volume_class_init(AgsRecallVolumeClass *recall_volume)
{
  AgsRecallClass *recall;
}

void
ags_recall_volume_init(AgsRecallVolume *recall_volume)
{
  recall_volume->channel = NULL;
  recall_volume->volume = NULL;
}

void
ags_recall_volume_connect(AgsRecallVolume *recall_volume)
{
  g_signal_connect((GObject *) recall_volume, "run_inter\0",
		   G_CALLBACK(ags_recall_volume), NULL);

  g_signal_connect((GObject *) recall_volume, "stop\0",
		   G_CALLBACK(ags_recall_volume_stop), NULL);

  g_signal_connect((GObject *) recall_volume, "cancel\0",
		   G_CALLBACK(ags_recall_volume_cancel), NULL);
}

void
ags_recall_volume(AgsRecall *recall, AgsRecallID *recall_id, gpointer data)
{
  AgsRecallVolume *recall_volume;
  AgsRecycling *recycling;
  AgsAudioSignal *audio_signal;
  GList *list_audio_signal, *list_audio_signal_next;
  short *buffer;
  guint i;

  /*
  recall_volume = (AgsRecallVolume *) recall;
  recycling = recall_volume->channel->first_recycling;

  //  fprintf(stdout, "ags_recall_volume: recall_volume = %f\n\0", recall_volume->volume[0]);

  while(recycling != recall_volume->channel->last_recycling->next){
    list_audio_signal = recycling->audio_signal;

    while(list_audio_signal != NULL){
      list_audio_signal_next = list_audio_signal->next;

      audio_signal = AGS_AUDIO_SIGNAL(list_audio_signal->data);

      if((AGS_AUDIO_SIGNAL_SOURCE & (audio_signal->flags)) != 0 || (AGS_AUDIO_SIGNAL_PLAY & (audio_signal->flags)) != 0){
	list_audio_signal = list_audio_signal_next;
	continue;
      }

      if(audio_signal->current_stream != NULL){
	buffer = (short *) audio_signal->current_stream->data;
      }else{
	g_signal_emit_by_name((GObject *) recall_volume, "done\0", recall_id);
	list_audio_signal = list_audio_signal_next;
	continue;
      }

      for(i = audio_signal->attack[0]; i < AGS_DEVOUT(audio_signal->devout)->buffer_size; i++)
	buffer[i] = (short) ((0xffff) & (int)((double)buffer[i] * (double)(*(recall_volume->volume))));

      if(audio_signal->attack[0] != 0){
	if(audio_signal->current_stream->next != NULL){
	  buffer = (short *) audio_signal->current_stream->next->data;
	}else{
	  g_signal_emit_by_name((GObject *) recall_volume, "done\0", recall_id);
	  list_audio_signal = list_audio_signal_next;
	  continue;
	}

	for(i = 0; i < audio_signal->attack[0]; i++)
	  buffer[i] = (short) (0xffff & (int)((double)buffer[i] * (double)(*(recall_volume->volume))));
      }

      list_audio_signal = list_audio_signal_next;
    }

    recycling = recycling->next;
  }
  */
}

void
ags_recall_volume_stop(AgsRecall *recall, AgsRecallID *recall_id, gpointer data)
{
}

void
ags_recall_volume_cancel(AgsRecall *recall, AgsRecallID *recall_id, gpointer data)
{
}

AgsRecallVolume*
ags_recall_volume_new(AgsChannel *channel)
{
  AgsRecallVolume *recall_volume;

  recall_volume = (AgsRecallVolume *) g_object_new(AGS_TYPE_RECALL_VOLUME, NULL);

  recall_volume->channel = channel;

  return(recall_volume);
}
