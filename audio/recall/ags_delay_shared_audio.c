#include "ags_delay_shared_audio.h"

GType ags_delay_shared_audio_get_type();
void ags_delay_shared_audio_class_init(AgsDelaySharedAudioClass *delay_shared_audio);
void ags_delay_shared_audio_init(AgsDelaySharedAudio *delay_shared_audio);
void ags_delay_shared_audio_finalize(GObject *gobject);

static gpointer ags_delay_shared_audio_parent_class = NULL;

GType
ags_delay_shared_audio_get_type()
{
  static GType ags_type_delay_shared_audio = 0;

  if(!ags_type_delay_shared_audio){
    static const GTypeInfo ags_delay_shared_audio_info = {
      sizeof (AgsDelaySharedAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_delay_shared_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsDelaySharedAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_delay_shared_audio_init,
    };

    ags_type_delay_shared_audio = g_type_register_static(AGS_TYPE_RECALL_SHARED_AUDIO,
							 "AgsDelaySharedAudio\0",
							 &ags_delay_shared_audio_info,
							 0);
  }

  return(ags_type_delay_shared_audio);
}

void
ags_delay_shared_audio_class_init(AgsDelaySharedAudioClass *delay_shared_audio)
{
  GObjectClass *gobject;

  ags_delay_shared_audio_parent_class = g_type_class_peek_parent(delay_shared_audio);

  gobject = (GObjectClass *) delay_shared_audio;

  gobject->finalize = ags_delay_shared_audio_finalize;
}

void
ags_delay_shared_audio_init(AgsDelaySharedAudio *delay_shared_audio)
{
  delay_shared_audio->delay = 0;
}

void
ags_delay_shared_audio_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_delay_shared_audio_parent_class)->finalize(gobject);
}

AgsDelaySharedAudio*
ags_delay_shared_audio_new(guint delay)
{
  AgsDelaySharedAudio *delay_shared_audio;

  delay_shared_audio = (AgsDelaySharedAudio *) g_object_new(AGS_TYPE_DELAY_SHARED_AUDIO, NULL);

  delay_shared_audio->delay = delay;

  return(delay_shared_audio);
}
