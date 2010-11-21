#include <ags/audio/recall/ags_copy_pattern_audio.h>

#include <ags/audio/recall/ags_copy_pattern_audio_run.h>
#include <ags/audio/recall/ags_copy_pattern_channel.h>
#include <ags/audio/recall/ags_copy_pattern_channel_run.h>

void ags_copy_pattern_audio_class_init(AgsCopyPatternAudioClass *copy_pattern_audio);
void ags_copy_pattern_audio_init(AgsCopyPatternAudio *copy_pattern_audio);
void ags_copy_pattern_audio_finalize(GObject *gobject);

static gpointer ags_copy_pattern_audio_parent_class = NULL;

GType
ags_copy_pattern_audio_get_type()
{
  static GType ags_type_copy_pattern_audio = 0;

  if(!ags_type_copy_pattern_audio){
    static const GTypeInfo ags_copy_pattern_audio_info = {
      sizeof (AgsCopyPatternAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_pattern_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCopyPatternAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_pattern_audio_init,
    };

    ags_type_copy_pattern_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
							 "AgsCopyPatternAudio\0",
							 &ags_copy_pattern_audio_info,
							 0);
  }

  return(ags_type_copy_pattern_audio);
}

void
ags_copy_pattern_audio_class_init(AgsCopyPatternAudioClass *copy_pattern_audio)
{
  GObjectClass *gobject;

  ags_copy_pattern_audio_parent_class = g_type_class_peek_parent(copy_pattern_audio);

  gobject = (GObjectClass *) copy_pattern_audio;

  gobject->finalize = ags_copy_pattern_audio_finalize;
}

void
ags_copy_pattern_audio_init(AgsCopyPatternAudio *copy_pattern_audio)
{
  copy_pattern_audio->devout = NULL;

  copy_pattern_audio->i = 0;
  copy_pattern_audio->j = 0;

  copy_pattern_audio->length = 0;
  copy_pattern_audio->loop = FALSE;

  copy_pattern_audio->stream_length = 0;
}

void
ags_copy_pattern_audio_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_copy_pattern_audio_parent_class)->finalize(gobject);
}

AgsCopyPatternAudio*
ags_copy_pattern_audio_new(AgsDevout *devout, AgsAudio *audio,
			   guint i, guint j,
			   guint length, gboolean loop,
			   guint stream_length)
{
  AgsCopyPatternAudio *copy_pattern_audio;

  copy_pattern_audio = (AgsCopyPatternAudio *) g_object_new(AGS_TYPE_COPY_PATTERN_AUDIO,
							    "recall_audio_type\0", AGS_TYPE_COPY_PATTERN_AUDIO,
							    "recall_audio_run_type\0", AGS_TYPE_COPY_PATTERN_AUDIO_RUN,
							    "recall_channel_type\0", AGS_TYPE_COPY_PATTERN_CHANNEL,
							    "recall_channel_run_type\0", AGS_TYPE_COPY_PATTERN_CHANNEL_RUN,
							    "audio\0", audio,
							    NULL);

  copy_pattern_audio->devout = devout;

  copy_pattern_audio->i = i;
  copy_pattern_audio->j = j;

  copy_pattern_audio->length = length;
  copy_pattern_audio->loop = loop;

  copy_pattern_audio->stream_length = stream_length;

  return(copy_pattern_audio);
}
