#include "ags_copy_pattern_shared_audio.h"

GType ags_copy_pattern_shared_audio_get_type();
void ags_copy_pattern_shared_audio_class_init(AgsCopyPatternSharedAudioClass *copy_pattern_shared_audio);
void ags_copy_pattern_shared_audio_init(AgsCopyPatternSharedAudio *copy_pattern_shared_audio);
void ags_copy_pattern_shared_audio_finalize(GObject *gobject);

static gpointer ags_copy_pattern_shared_audio_parent_class = NULL;

GType
ags_copy_pattern_shared_audio_get_type()
{
  static GType ags_type_copy_pattern_shared_audio = 0;

  if(!ags_type_copy_pattern_shared_audio){
    static const GTypeInfo ags_copy_pattern_shared_audio_info = {
      sizeof (AgsCopyPatternSharedAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_pattern_shared_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCopyPatternSharedAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_pattern_shared_audio_init,
    };

    ags_type_copy_pattern_shared_audio = g_type_register_static(AGS_TYPE_RECALL_SHARED_AUDIO,
							       "AgsCopyPatternSharedAudio\0",
							       &ags_copy_pattern_shared_audio_info,
							       0);
  }

  return(ags_type_copy_pattern_shared_audio);
}

void
ags_copy_pattern_shared_audio_class_init(AgsCopyPatternSharedAudioClass *copy_pattern_shared_audio)
{
  GObjectClass *gobject;

  ags_copy_pattern_shared_audio_parent_class = g_type_class_peek_parent(copy_pattern_shared_audio);

  gobject = (GObjectClass *) copy_pattern_shared_audio;

  gobject->finalize = ags_copy_pattern_shared_audio_finalize;
}

void
ags_copy_pattern_shared_audio_init(AgsCopyPatternSharedAudio *copy_pattern_shared_audio)
{
  copy_pattern_shared_audio->devout = NULL;

  copy_pattern_shared_audio->i = 0;
  copy_pattern_shared_audio->j = 0;

  copy_pattern_shared_audio->length = 0;
  copy_pattern_shared_audio->loop = FALSE;

  copy_pattern_shared_audio->stream_length = 0;
}

void
ags_copy_pattern_shared_audio_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_copy_pattern_shared_audio_parent_class)->finalize(gobject);
}

AgsCopyPatternSharedAudio*
ags_copy_pattern_shared_audio_new(AgsDevout *devout,
				  guint i, guint j,
				  guint length, gboolean loop,
				  guint stream_length)
{
  AgsCopyPatternSharedAudio *copy_pattern_shared_audio;

  copy_pattern_shared_audio = (AgsCopyPatternSharedAudio *) g_object_new(AGS_TYPE_COPY_PATTERN_SHARED_AUDIO, NULL);

  copy_pattern_shared_audio->devout = devout;

  copy_pattern_shared_audio->i = i;
  copy_pattern_shared_audio->j = j;

  copy_pattern_shared_audio->length = length;
  copy_pattern_shared_audio->loop = loop;

  copy_pattern_shared_audio->stream_length = stream_length;

  return(copy_pattern_shared_audio);
}
