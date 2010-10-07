#include "ags_recall_audio_run.h"

GType ags_recall_audio_run_get_type();
void ags_recall_audio_run_class_init(AgsRecallAudioRunClass *recall_audio_run);
void ags_recall_audio_run_init(AgsRecallAudioRun *recall_audio_run);
void ags_recall_audio_run_finalize(GObject *gobject);

static gpointer ags_recall_audio_run_parent_class = NULL;

GType
ags_recall_audio_run_get_type()
{
  static GType ags_type_recall_audio_run = 0;

  if(!ags_type_recall_audio_run){
    static const GTypeInfo ags_recall_audio_run_info = {
      sizeof (AgsRecallAudioRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_audio_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallAudioRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_audio_run_init,
    };

    ags_type_recall_audio_run = g_type_register_static(AGS_TYPE_RECALL,
						       "AgsRecallAudioRun\0",
						       &ags_recall_audio_run_info,
						       0);
  }

  return(ags_type_recall_audio_run);
}

void
ags_recall_audio_run_class_init(AgsRecallAudioRunClass *recall_audio_run)
{
  GObjectClass *gobject;

  ags_recall_audio_run_parent_class = g_type_class_peek_parent(recall_audio_run);

  gobject = (GObjectClass *) recall_audio_run;

  gobject->finalize = ags_recall_audio_run_finalize;
}

void
ags_recall_audio_run_init(AgsRecallAudioRun *recall_audio_run)
{
}

void
ags_recall_audio_run_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_recall_audio_run_parent_class)->finalize(gobject);
}

AgsRecallAudioRun*
ags_recall_audio_run_new()
{
  AgsRecallAudioRun *recall_audio_run;

  recall_audio_run = (AgsRecallAudioRun *) g_object_new(AGS_TYPE_RECALL_AUDIO_RUN, NULL);

  return(recall_audio_run);
}
