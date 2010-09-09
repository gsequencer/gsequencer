#ifndef __AGS_COPY_PATTERN_SHARED_AUDIO_RUN_H__
#define __AGS_COPY_PATTERN_SHARED_AUDIO_RUN_H__

#include <glib.h>
#include <glib-object.h>

#include "../ags_recall_shared_audio.h"

#include "ags_delay.h"

#define AGS_TYPE_COPY_PATTERN_SHARED_AUDIO_RUN                (ags_copy_pattern_shared_audio_run_get_type())
#define AGS_COPY_PATTERN_SHARED_AUDIO_RUN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_COPY_PATTERN_SHARED_AUDIO_RUN, AgsCopyPatternSharedAudioRun))
#define AGS_COPY_PATTERN_SHARED_AUDIO_RUN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_COPY_PATTERN_SHARED_AUDIO_RUN, AgsCopyPatternSharedAudioRun))
#define AGS_IS_COPY_PATTERN_SHARED_AUDIO_RUN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_COPY_PATTERN_SHARED_AUDIO_RUN))
#define AGS_IS_COPY_PATTERN_SHARED_AUDIO_RUN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_COPY_PATTERN_SHARED_AUDIO_RUN))
#define AGS_COPY_PATTERN_SHARED_AUDIO_RUN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_COPY_PATTERN_SHARED_AUDIO_RUN, AgsCopyPatternSharedAudioRunClass))

typedef struct _AgsCopyPatternSharedAudioRun AgsCopyPatternSharedAudioRun;
typedef struct _AgsCopyPatternSharedAudioRunClass AgsCopyPatternSharedAudioRunClass;

struct _AgsCopyPatternSharedAudioRun
{
  AgsRecallSharedAudio shared_audio;

  AgsDelay *delay;
  guint bit;
};

struct _AgsCopyPatternSharedAudioRunClass
{
  AgsRecallSharedAudioClass shared_audio;
};

AgsCopyPatternSharedAudioRun* ags_copy_pattern_shared_audio_run_new(AgsDelay *delay, guint bit);

#endif /*__AGS_COPY_PATTERN_SHARED_AUDIO_RUN_H__*/
