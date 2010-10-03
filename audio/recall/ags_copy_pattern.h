#ifndef __AGS_COPY_PATTERN_H__
#define __AGS_COPY_PATTERN_H__

#include <glib.h>
#include <glib-object.h>

#include "../ags_recall.h"

#include "ags_copy_pattern_shared_audio_run.h"
#include "ags_copy_pattern_shared_channel.h"

#define AGS_TYPE_COPY_PATTERN                (ags_copy_pattern_get_type())
#define AGS_COPY_PATTERN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_COPY_PATTERN, AgsCopyPattern))
#define AGS_COPY_PATTERN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_COPY_PATTERN, AgsCopyPatternClass))
#define AGS_IS_COPY_PATTERN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_COPY_PATTERN))
#define AGS_IS_COPY_PATTERN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_COPY_PATTERN))
#define AGS_COPY_PATTERN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_COPY_PATTERN, AgsCopyPatternClass))

typedef struct _AgsCopyPattern AgsCopyPattern;
typedef struct _AgsCopyPatternClass AgsCopyPatternClass;

struct _AgsCopyPattern
{
  AgsRecall recall;

  gulong tic_handler;

  AgsCopyPatternSharedAudioRun *shared_audio_run;
  AgsCopyPatternSharedChannel *shared_channel;
};

struct _AgsCopyPatternClass
{
  AgsRecallClass recall;
};

AgsCopyPattern* ags_copy_pattern_new(AgsCopyPatternSharedAudioRun *shared_audio_run,
				     AgsCopyPatternSharedChannel *shared_channel);

#endif /*__AGS_COPY_PATTERN_H__*/
