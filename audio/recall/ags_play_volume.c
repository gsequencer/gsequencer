#include "ags_play_volume.h"

#include "../ags_devout.h"
#include "../ags_audio.h"
#include "../ags_output.h"
#include "../ags_input.h"
#include "../ags_recycling.h"
#include "../ags_audio_signal.h"
#include "../ags_recall_id.h"

#include <stdlib.h>
#include <string.h>

GType ags_play_volume_get_type();
void ags_play_volume_class_init(AgsPlayVolumeClass *volume);
void ags_play_volume_init(AgsPlayVolume *volume);
void ags_play_volume_connect(AgsPlayVolume *volume);

void ags_play_volume(AgsRecall *recall, AgsRecallID *recall_id, gpointer data);
void ags_play_volume_stop(AgsRecall *recall, AgsRecallID *recall_id, gpointer data);
void ags_play_volume_cancel(AgsRecall *recall, AgsRecallID *recall_id, gpointer data);

GType
ags_play_volume_get_type()
{
  static GType ags_type_play_volume = 0;

  if(!ags_type_play_volume){
    static const GTypeInfo ags_play_volume_info = {
      sizeof (AgsPlayVolumeClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_volume_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayVolume),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_volume_init,
    };
    ags_type_play_volume = g_type_register_static(AGS_TYPE_RECALL, "AgsPlayVolume\0", &ags_play_volume_info, 0);
  }
  return (ags_type_play_volume);
}

void
ags_play_volume_class_init(AgsPlayVolumeClass *volume)
{
  AgsRecallClass *recall;
}

void
ags_play_volume_init(AgsPlayVolume *volume)
{
  volume->channel = NULL;
  volume->audio_signal = NULL;
  volume->volume = NULL;
}

void
ags_play_volume_connect(AgsPlayVolume *volume)
{
  g_signal_connect((GObject *) volume, "run_init_inter\0",
		   G_CALLBACK(ags_play_volume_map_audio_signal), NULL);

  g_signal_connect((GObject *) volume, "run_inter\0",
		   G_CALLBACK(ags_play_volume), NULL);

  g_signal_connect((GObject *) volume, "stop\0",
		   G_CALLBACK(ags_play_volume_stop), NULL);

  g_signal_connect((GObject *) volume, "cancel\0",
		   G_CALLBACK(ags_play_volume_cancel), NULL);
}

void
ags_play_volume(AgsRecall *recall, AgsRecallID *recall_id, gpointer data)
{
  AgsPlayVolume *volume;
  AgsAudioSignal *audio_signal;
  GList *list_audio_signal;
  short *buffer;
  guint i;

  /*
  volume = (AgsPlayVolume *) recall;
  list_audio_signal = volume->audio_signal;

  while(list_audio_signal != NULL){
    audio_signal = (AgsAudioSignal *) list_audio_signal->data;

    if(audio_signal->current_stream != NULL){
      buffer = (short *) audio_signal->current_stream->data;
    }else{
      volume->audio_signal = g_list_remove(volume->audio_signal, audio_signal);
      return;
    }
  
    for(i = audio_signal->attack[0]; i < AGS_DEVOUT(audio_signal->devout)->buffer_size; i++)
      buffer[i] = (short) ((0xffff) & (int)((double)buffer[i] * (double)volume->volume[0]));
    
    if(audio_signal->attack[0] != 0){
      if(audio_signal->current_stream->next != NULL){
	buffer = (short *) audio_signal->current_stream->next->data;
      }else{
	volume->audio_signal = g_list_remove(volume->audio_signal, audio_signal);
	return;
      }
      
      for(i = 0; i < audio_signal->attack[0]; i++)
	buffer[i] = (short) (0xffff & (int)((double)buffer[i] * (double)volume->volume[0]));
    }

    list_audio_signal = list_audio_signal->next;
  }

  if(volume->audio_signal == NULL)
    g_signal_emit_by_name((GObject *) volume, "done\0", recall_id);
  */
}

void
ags_play_volume_stop(AgsRecall *recall, AgsRecallID *recall_id, gpointer data)
{
}

void
ags_play_volume_cancel(AgsRecall *recall, AgsRecallID *recall_id, gpointer data)
{
}

int
ags_play_volume_map_audio_signal(AgsRecall *recall, AgsRecallID *recall_id, gpointer data)
{
  AgsPlayVolume *play_volume;
  AgsRecycling *recycling;
  GList *list_audio_signal;
  /*
  play_volume = AGS_PLAY_VOLUME(recall);
  recycling = play_volume->channel->first_recycling;

  list_audio_signal = NULL;

  while(recycling != play_volume->channel->last_recycling->next){
    list_audio_signal = g_list_prepend(list_audio_signal, ags_recycling_get_audio_signal_play(recycling));

    recycling = recycling->next;
  }

  play_volume->audio_signal = list_audio_signal;
  */
  return(0);
}

AgsPlayVolume*
ags_play_volume_new(AgsChannel *channel)
{
  AgsPlayVolume *volume;

  volume = (AgsPlayVolume *) g_object_new(AGS_TYPE_PLAY_VOLUME, NULL);

  volume->channel = channel;

  return(volume);
}
