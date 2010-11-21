#ifndef __AGS_COPY_PATTERN_AUDIO_H__
#define __AGS_COPY_PATTERN_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_devout.h>

#define AGS_TYPE_COPY_PATTERN_AUDIO                (ags_copy_pattern_audio_get_type())
#define AGS_COPY_PATTERN_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_COPY_PATTERN_AUDIO, AgsCopyPatternAudio))
#define AGS_COPY_PATTERN_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_COPY_PATTERN_AUDIO, AgsCopyPatternAudio))
#define AGS_IS_COPY_PATTERN_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_COPY_PATTERN_AUDIO))
#define AGS_IS_COPY_PATTERN_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_COPY_PATTERN_AUDIO))
#define AGS_COPY_PATTERN_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_COPY_PATTERN_AUDIO, AgsCopyPatternAudioClass))

typedef struct _AgsCopyPatternAudio AgsCopyPatternAudio;
typedef struct _AgsCopyPatternAudioClass AgsCopyPatternAudioClass;

struct _AgsCopyPatternAudio
{
  AgsRecallAudio recall_audio;

  AgsDevout *devout;

  guint i;
  guint j;

  guint length;
  gboolean loop;

  guint stream_length;
};

struct _AgsCopyPatternAudioClass
{
  AgsRecallAudioClass recall_audio;
};

GType ags_copy_pattern_audio_get_type();

AgsCopyPatternAudio* ags_copy_pattern_audio_new(AgsDevout *devout, AgsAudio *audio,
						guint i, guint j,
						guint length, gboolean loop,
						guint stream_length);

#endif /*__AGS_COPY_PATTERN_AUDIO_H__*/
