#include "ags_recall_shared_audio.h"

GType ags_recall_shared_audio_get_type();
void ags_recall_shared_audio_class_init(AgsRecallSharedAudioClass *recall_shared_audio);
void ags_recall_shared_audio_init(AgsRecallSharedAudio *recall_shared_audio);
void ags_recall_shared_audio_finalize(GObject *gobject);

static gpointer ags_recall_shared_audio_parent_class = NULL;

GType
ags_recall_shared_audio_get_type()
{
  static GType ags_type_recall_shared_audio = 0;

  if(!ags_type_recall_shared_audio){
    static const GTypeInfo ags_recall_shared_audio_info = {
      sizeof (AgsRecallSharedAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_shared_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallSharedAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_shared_audio_init,
    };

    ags_type_recall_shared_audio = g_type_register_static(AGS_TYPE_RECALL_SHARED,
							  "AgsRecallSharedAudio\0",
							  &ags_recall_shared_audio_info,
							  0);
  }

  return(ags_type_recall_shared_audio);
}

void
ags_recall_shared_audio_class_init(AgsRecallSharedAudioClass *recall_shared_audio)
{
  GObjectClass *gobject;

  ags_recall_shared_audio_parent_class = g_type_class_peek_parent(recall_shared_audio);

  gobject = (GObjectClass *) recall_shared_audio;

  gobject->finalize = ags_recall_shared_audio_finalize;
}

void
ags_recall_shared_audio_init(AgsRecallSharedAudio *recall_shared_audio)
{
}

void
ags_recall_shared_audio_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_recall_shared_audio_parent_class)->finalize(gobject);
}

AgsRecallSharedAudio*
ags_recall_shared_audio_new()
{
  AgsRecallSharedAudio *recall_shared_audio;

  recall_shared_audio = (AgsRecallSharedAudio *) g_object_new(AGS_TYPE_RECALL_SHARED_AUDIO, NULL);

  return(recall_shared_audio);
}
