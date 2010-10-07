#ifndef __AGS_COPY_PATTERN_CHANNEL_RUN_H__
#define __AGS_COPY_PATTERN_CHANNEL_RUN_H__

#include <glib.h>
#include <glib-object.h>

#include "../ags_recall_audio.h"
#include "../ags_recall_audio_run.h"
#include "../ags_recall_channel.h"
#include "../ags_recall_channel_run.h"

#define AGS_TYPE_COPY_PATTERN_CHANNEL_RUN                (ags_copy_pattern_channel_run_get_type())
#define AGS_COPY_PATTERN_CHANNEL_RUN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_COPY_PATTERN_CHANNEL_RUN, AgsCopyPatternChannelRun))
#define AGS_COPY_PATTERN_CHANNEL_RUN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_COPY_PATTERN_CHANNEL_RUN, AgsCopyPatternChannelRunClass))
#define AGS_IS_COPY_PATTERN_CHANNEL_RUN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_COPY_PATTERN_CHANNEL_RUN))
#define AGS_IS_COPY_PATTERN_CHANNEL_RUN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_COPY_PATTERN_CHANNEL_RUN))
#define AGS_COPY_PATTERN_CHANNEL_RUN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_COPY_PATTERN_CHANNEL_RUN, AgsCopyPatternChannelRunClass))

typedef struct _AgsCopyPatternChannelRun AgsCopyPatternChannelRun;
typedef struct _AgsCopyPatternChannelRunClass AgsCopyPatternChannelRunClass;

struct _AgsCopyPatternChannelRun
{
  AgsRecallChannelRun recall_channel_run;

  gulong tic_handler;
};

struct _AgsCopyPatternChannelRunClass
{
  AgsRecallChannelRunClass recall_channel_run;
};

AgsCopyPatternChannelRun* ags_copy_pattern_channel_run_new(AgsRecallAudio *recall_audio,
							   AgsRecallAudioRun *recall_audio_run,
							   AgsRecallChannel *recall_channel);

#endif /*__AGS_COPY_PATTERN_CHANNEL_RUN_H__*/
