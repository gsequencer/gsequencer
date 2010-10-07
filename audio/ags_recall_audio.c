#include "ags_recall_audio.h"

GType ags_recall_audio_get_type();
void ags_recall_audio_class_init(AgsRecallAudioClass *recall_audio);
void ags_recall_audio_init(AgsRecallAudio *recall_audio);
void ags_recall_audio_finalize(GObject *gobject);

static gpointer ags_recall_audio_parent_class = NULL;

GType
ags_recall_audio_get_type()
{
  static GType ags_type_recall_audio = 0;

  if(!ags_type_recall_audio){
    static const GTypeInfo ags_recall_audio_info = {
      sizeof (AgsRecallAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_audio_init,
    };

    ags_type_recall_audio = g_type_register_static(AGS_TYPE_RECALL,
						   "AgsRecallAudio\0",
						   &ags_recall_audio_info,
						   0);
  }

  return(ags_type_recall_audio);
}

void
ags_recall_audio_class_init(AgsRecallAudioClass *recall_audio)
{
  GObjectClass *gobject;

  ags_recall_audio_parent_class = g_type_class_peek_parent(recall_audio);

  gobject = (GObjectClass *) recall_audio;

  gobject->finalize = ags_recall_audio_finalize;
}

void
ags_recall_audio_init(AgsRecallAudio *recall_audio)
{
}

void
ags_recall_audio_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_recall_audio_parent_class)->finalize(gobject);
}

AgsRecallAudio*
ags_recall_audio_new()
{
  AgsRecallAudio *recall_audio;

  recall_audio = (AgsRecallAudio *) g_object_new(AGS_TYPE_RECALL_AUDIO, NULL);

  return(recall_audio);
}
