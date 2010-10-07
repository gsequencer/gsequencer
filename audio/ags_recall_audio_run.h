#ifndef __AGS_RECALL_AUDIO_RUN_H__
#define __AGS_RECALL_AUDIO_RUN_H__

#include <glib.h>
#include <glib-object.h>

#include "ags_recall.h"

#define AGS_TYPE_RECALL_AUDIO_RUN                (ags_recall_audio_run_get_type())
#define AGS_RECALL_AUDIO_RUN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RECALL_AUDIO_RUN, AgsRecallAudioRun))
#define AGS_RECALL_AUDIO_RUN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RECALL_AUDIO_RUN, AgsRecallAudioRunClass))
#define AGS_IS_RECALL_AUDIO_RUN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_RECALL_AUDIO_RUN))
#define AGS_IS_RECALL_AUDIO_RUN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_RECALL_AUDIO_RUN))
#define AGS_RECALL_AUDIO_RUN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_RECALL_AUDIO_RUN, AgsRecallAudioRunClass))

typedef struct _AgsRecallAudioRun AgsRecallAudioRun;
typedef struct _AgsRecallAudioRunClass AgsRecallAudioRunClass;

struct _AgsRecallAudioRun
{
  AgsRecall recall;
};

struct _AgsRecallAudioRunClass
{
  AgsRecallClass recall;
};

AgsRecallAudioRun* ags_recall_audio_run_new();

#endif /*__AGS_RECALL_AUDIO_RUN_H__*/
