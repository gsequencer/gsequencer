#include "ags_delay_audio.h"

#include "ags_delay_audio_run.h"

GType ags_delay_audio_get_type();
void ags_delay_audio_class_init(AgsDelayAudioClass *delay_audio);
void ags_delay_audio_init(AgsDelayAudio *delay_audio);
void ags_delay_audio_finalize(GObject *gobject);

static gpointer ags_delay_audio_parent_class = NULL;

GType
ags_delay_audio_get_type()
{
  static GType ags_type_delay_audio = 0;

  if(!ags_type_delay_audio){
    static const GTypeInfo ags_delay_audio_info = {
      sizeof (AgsDelayAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_delay_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsDelayAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_delay_audio_init,
    };

    ags_type_delay_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
						  "AgsDelayAudio\0",
						  &ags_delay_audio_info,
						  0);
  }

  return(ags_type_delay_audio);
}

void
ags_delay_audio_class_init(AgsDelayAudioClass *delay_audio)
{
  GObjectClass *gobject;

  ags_delay_audio_parent_class = g_type_class_peek_parent(delay_audio);

  gobject = (GObjectClass *) delay_audio;

  gobject->finalize = ags_delay_audio_finalize;
}

void
ags_delay_audio_init(AgsDelayAudio *delay_audio)
{
  delay_audio->delay = 0;
}

void
ags_delay_audio_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_delay_audio_parent_class)->finalize(gobject);
}

AgsDelayAudio*
ags_delay_audio_new(AgsAudio *audio,
		    guint delay)
{
  AgsDelayAudio *delay_audio;

  delay_audio = (AgsDelayAudio *) g_object_new(AGS_TYPE_DELAY_AUDIO,
					       "recall_audio_type\0", AGS_TYPE_DELAY_AUDIO,
					       "recall_audio_run_type\0", AGS_TYPE_DELAY_AUDIO_RUN,
					       "audio", audio,
					       NULL);

  delay_audio->delay = delay;

  return(delay_audio);
}
