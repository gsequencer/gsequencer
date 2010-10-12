#include "ags_play_audio_file.h"

#include "../ags_recall_id.h"

void ags_play_audio_file_class_init(AgsPlayAudioFileClass *play_audio_file);
void ags_play_audio_file_init(AgsPlayAudioFile *play_audio_file);
void ags_play_audio_file_finalize(GObject *gobject);
void ags_play_audio_file_connect(AgsPlayAudioFile *play_audio_file);

void ags_play_audio_file(AgsRecall *recall, AgsRecallID *recall_id, gpointer data);
void ags_play_audio_file_stop(AgsRecall *recall, AgsRecallID *recall_id, gpointer data);
void ags_play_audio_file_cancel(AgsRecall *recall, AgsRecallID *recall_id, gpointer data);

static gpointer ags_play_audio_file_parent_class = NULL;

GType
ags_play_audio_file_get_type()
{
  static GType ags_type_play_audio_file = 0;

  if(!ags_type_play_audio_file){
    static const GTypeInfo ags_play_audio_file_info = {
      sizeof (AgsPlayAudioFileClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_audio_file_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayAudioFile),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_audio_file_init,
    };
    ags_type_play_audio_file = g_type_register_static(AGS_TYPE_RECALL, "AgsPlayAudioFile\0", &ags_play_audio_file_info, 0);
  }
  return (ags_type_play_audio_file);
}

void
ags_play_audio_file_class_init(AgsPlayAudioFileClass *play_audio_file)
{
  GObjectClass *gobject;

  ags_play_audio_file_parent_class = g_type_class_peek_parent(play_audio_file);

  gobject = (GObjectClass *) play_audio_file;
  gobject->finalize = ags_play_audio_file_finalize;
}

void
ags_play_audio_file_init(AgsPlayAudioFile *play_audio_file)
{
  play_audio_file->audio_file = NULL;
  play_audio_file->current = 0;

  play_audio_file->devout = NULL;
}

void
ags_play_audio_file_finalize(GObject *gobject)
{
  AgsPlayAudioFile *play_audio_file;

  play_audio_file = AGS_PLAY_AUDIO_FILE(gobject);
  g_object_unref(G_OBJECT(play_audio_file->audio_file));

  G_OBJECT_CLASS(ags_play_audio_file_parent_class)->finalize(gobject);
}

void
ags_play_audio_file_connect(AgsPlayAudioFile *play_audio_file)
{
  //  ags_recall_connect(AGS_RECALL(play_audio_file));

  g_signal_connect((GObject *) play_audio_file, "run_inter\0",
		   G_CALLBACK(ags_play_audio_file), NULL);

  g_signal_connect((GObject *) play_audio_file, "stop\0",
		   G_CALLBACK(ags_play_audio_file_stop), NULL);

  g_signal_connect((GObject *) play_audio_file, "cancel\0",
		   G_CALLBACK(ags_play_audio_file_cancel), NULL);
}

void
ags_play_audio_file(AgsRecall *recall, AgsRecallID *recall_id, gpointer data)
{
  AgsPlayAudioFile *play_audio_file;
  short *buffer;
  guint i0, i1, j, stop;
  gboolean play_done;

  play_audio_file = (AgsPlayAudioFile *) recall;

  if((AGS_DEVOUT_BUFFER0 & play_audio_file->devout->flags) != 0){
    buffer = play_audio_file->devout->buffer[1];
  }else if((AGS_DEVOUT_BUFFER1 & play_audio_file->devout->flags) != 0){
    buffer = play_audio_file->devout->buffer[2];
  }else if((AGS_DEVOUT_BUFFER2 & play_audio_file->devout->flags) != 0){
    buffer = play_audio_file->devout->buffer[3];
  }else if((AGS_DEVOUT_BUFFER3 & play_audio_file->devout->flags) != 0){
    buffer = play_audio_file->devout->buffer[0];
  }

  i0 = play_audio_file->current;
  stop = i0 + play_audio_file->devout->buffer_size;

  if(stop < play_audio_file->audio_file->frames)
    play_done = FALSE;
  else{
    stop = play_audio_file->audio_file->frames;
    play_done = TRUE;
  }

  for(i1 = 0; i0 < stop; i0++, i1++){
    for(j = 0; j < play_audio_file->audio_file->channels || j < play_audio_file->devout->dsp_channels; j++)
      buffer[i1 * play_audio_file->devout->dsp_channels + j] = ((buffer[i1 * play_audio_file->devout->dsp_channels + j]) / 2) + ((play_audio_file->audio_file->buffer[i0 * play_audio_file->audio_file->channels + j]) / 2);
  }

  play_audio_file->current = i0;

  if(play_done)
    g_signal_emit_by_name((GObject *) recall, "done\0", recall_id);
}

void
ags_play_audio_file_stop(AgsRecall *recall, AgsRecallID *recall_id, gpointer data)
{
}

void
ags_play_audio_file_cancel(AgsRecall *recall, AgsRecallID *recall_id, gpointer data)
{
}

AgsPlayAudioFile*
ags_play_audio_file_new()
{
  AgsPlayAudioFile *play_audio_file;

  play_audio_file = (AgsPlayAudioFile *) g_object_new(AGS_TYPE_PLAY_AUDIO_FILE, NULL);

  return(play_audio_file);
}

