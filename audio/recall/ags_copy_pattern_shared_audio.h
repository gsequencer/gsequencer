#ifndef __AGS_COPY_PATTERN_SHARED_AUDIO_H__
#define __AGS_COPY_PATTERN_SHARED_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include "../ags_recall_shared_audio.h"
#include "../ags_devout.h"

#define AGS_TYPE_COPY_PATTERN_SHARED_AUDIO                (ags_copy_pattern_shared_audio_get_type())
#define AGS_COPY_PATTERN_SHARED_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_COPY_PATTERN_SHARED_AUDIO, AgsCopyPatternSharedAudio))
#define AGS_COPY_PATTERN_SHARED_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_COPY_PATTERN_SHARED_AUDIO, AgsCopyPatternSharedAudio))
#define AGS_IS_COPY_PATTERN_SHARED_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_COPY_PATTERN_SHARED_AUDIO))
#define AGS_IS_COPY_PATTERN_SHARED_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_COPY_PATTERN_SHARED_AUDIO))
#define AGS_COPY_PATTERN_SHARED_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_COPY_PATTERN_SHARED_AUDIO, AgsCopyPatternSharedAudioClass))

typedef struct _AgsCopyPatternSharedAudio AgsCopyPatternSharedAudio;
typedef struct _AgsCopyPatternSharedAudioClass AgsCopyPatternSharedAudioClass;

struct _AgsCopyPatternSharedAudio
{
  AgsRecallSharedAudio shared_audio;

  AgsDevout *devout;

  guint i;
  guint j;

  guint length;
  gboolean loop;

  guint stream_length;
};

struct _AgsCopyPatternSharedAudioClass
{
  AgsRecallSharedAudioClass shared_audio;
};

AgsCopyPatternSharedAudio* ags_copy_pattern_shared_audio_new(AgsDevout *devout,
							     guint i, guint j,
							     guint length, gboolean loop,
							     guint stream_length);

#endif /*__AGS_COPY_PATTERN_SHARED_AUDIO_H__*/
