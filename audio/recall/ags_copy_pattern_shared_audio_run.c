#include "ags_copy_pattern_shared_audio_run.h"

GType ags_copy_pattern_shared_audio_run_get_type();
void ags_copy_pattern_shared_audio_run_class_init(AgsCopyPatternSharedAudioRunClass *copy_pattern_shared_audio_run);
void ags_copy_pattern_shared_audio_run_init(AgsCopyPatternSharedAudioRun *copy_pattern_shared_audio_run);
void ags_copy_pattern_shared_audio_run_finalize(GObject *gobject);

static gpointer ags_copy_pattern_shared_audio_run_parent_class = NULL;

GType
ags_copy_pattern_shared_audio_run_get_type()
{
  static GType ags_type_copy_pattern_shared_audio_run = 0;

  if(!ags_type_copy_pattern_shared_audio_run){
    static const GTypeInfo ags_copy_pattern_shared_audio_run_info = {
      sizeof (AgsCopyPatternSharedAudioRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_pattern_shared_audio_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCopyPatternSharedAudioRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_pattern_shared_audio_run_init,
    };

    ags_type_copy_pattern_shared_audio_run = g_type_register_static(AGS_TYPE_RECALL_SHARED_AUDIO,
								    "AgsCopyPatternSharedAudioRun\0",
								    &ags_copy_pattern_shared_audio_run_info,
								    0);
  }

  return(ags_type_copy_pattern_shared_audio_run);
}

void
ags_copy_pattern_shared_audio_run_class_init(AgsCopyPatternSharedAudioRunClass *copy_pattern_shared_audio_run)
{
  GObjectClass *gobject;

  ags_copy_pattern_shared_audio_run_parent_class = g_type_class_peek_parent(copy_pattern_shared_audio_run);

  gobject = (GObjectClass *) copy_pattern_shared_audio_run;

  gobject->finalize = ags_copy_pattern_shared_audio_run_finalize;
}

void
ags_copy_pattern_shared_audio_run_init(AgsCopyPatternSharedAudioRun *copy_pattern_shared_audio_run)
{
  copy_pattern_shared_audio_run->delay = NULL;
  copy_pattern_shared_audio_run->bit = 0;
}

void
ags_copy_pattern_shared_audio_run_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_copy_pattern_shared_audio_run_parent_class)->finalize(gobject);
}

AgsCopyPatternSharedAudioRun*
ags_copy_pattern_shared_audio_run_new(AgsDelay *delay, guint bit)
{
  AgsCopyPatternSharedAudioRun *copy_pattern_shared_audio_run;

  copy_pattern_shared_audio_run = (AgsCopyPatternSharedAudioRun *) g_object_new(AGS_TYPE_COPY_PATTERN_SHARED_AUDIO_RUN, NULL);

  copy_pattern_shared_audio_run->delay = delay;
  copy_pattern_shared_audio_run->bit = bit;

  return(copy_pattern_shared_audio_run);
}
