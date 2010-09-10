#include "ags_stream_audio_signal.h"

GType ags_stream_audio_signal_get_type();
void ags_stream_audio_signal_class_init(AgsStreamAudioSignalClass *stream_audio_signal);
void ags_stream_audio_signal_init(AgsStreamAudioSignal *stream_audio_signal);
void ags_stream_audio_signal_finalize(GObject *gobject);

void ags_stream_audio_signal_run_inter(AgsRecall *recall, gpointer data);

AgsRecall* ags_stream_audio_signal_duplicate(AgsRecall *recall, AgsRecallID *recall_id);

static gpointer ags_stream_audio_signal_parent_class = NULL;

GType
ags_stream_audio_signal_get_type()
{
  static GType ags_type_stream_audio_signal = 0;

  if(!ags_type_stream_audio_signal){
    static const GTypeInfo ags_stream_audio_signal_info = {
      sizeof (AgsStreamAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_stream_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsStreamAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_stream_audio_signal_init,
    };
    ags_type_stream_audio_signal = g_type_register_static(AGS_TYPE_RECALL,
							  "AgsStreamAudioSignal\0",
							  &ags_stream_audio_signal_info,
							  0);
  }
  return (ags_type_stream_audio_signal);
}

void
ags_stream_audio_signal_class_init(AgsStreamAudioSignalClass *stream_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_stream_audio_signal_parent_class = g_type_class_peek_parent(stream_audio_signal);

  gobject = (GObjectClass *) stream_audio_signal;

  gobject->finalize = ags_stream_audio_signal_finalize;

  recall = (AgsRecallClass *) stream_audio_signal;

  recall->duplicate = ags_stream_audio_signal_duplicate;
}

void
ags_stream_audio_signal_init(AgsStreamAudioSignal *stream_audio_signal)
{
  stream_audio_signal->audio_signal = NULL;
}

void
ags_stream_audio_signal_finalize(GObject *gobject)
{
  AgsStreamAudioSignal *stream_audio_signal;
}

void
ags_stream_audio_signal_connect(AgsStreamAudioSignal *stream_audio_signal)
{
  //  ags_recall_connect(AGS_RECALL(stream_audio_signal));

  g_signal_connect((GObject *) stream_audio_signal, "run_inter\0",
		   G_CALLBACK(ags_stream_audio_signal_run_inter), NULL);
}

void
ags_stream_audio_signal_run_inter(AgsRecall *recall, gpointer data)
{
  AgsStreamAudioSignal *stream_audio_signal;

  stream_audio_signal = AGS_STREAM_AUDIO_SIGNAL(recall);

  if(stream_audio_signal->audio_signal->stream_current != NULL){
    stream_audio_signal->audio_signal->stream_current = stream_audio_signal->audio_signal->stream_current->next;
  }else{
    ags_recall_done(recall);
  }
}

AgsRecall*
ags_stream_audio_signal_duplicate(AgsRecall *recall, AgsRecallID *recall_id)
{
  AgsStreamAudioSignal *stream_audio_signal, *copy;

  stream_audio_signal = (AgsStreamAudioSignal *) recall;
  copy = (AgsStreamAudioSignal *) AGS_RECALL_CLASS(ags_stream_audio_signal_parent_class)->duplicate(recall, recall_id);

  copy->audio_signal = stream_audio_signal->audio_signal;

  return((AgsRecall *) copy);
}

AgsStreamAudioSignal*
ags_stream_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsStreamAudioSignal *stream_audio_signal;

  stream_audio_signal = (AgsStreamAudioSignal *) g_object_new(AGS_TYPE_STREAM_AUDIO_SIGNAL, NULL);

  stream_audio_signal->audio_signal = audio_signal;

  return(stream_audio_signal);
}
