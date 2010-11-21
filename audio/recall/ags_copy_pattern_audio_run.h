#ifndef __AGS_COPY_PATTERN_AUDIO_RUN_H__
#define __AGS_COPY_PATTERN_AUDIO_RUN_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_audio_run.h>

#include <ags/audio/recall/ags_delay_audio_run.h>

#define AGS_TYPE_COPY_PATTERN_AUDIO_RUN                (ags_copy_pattern_audio_run_get_type())
#define AGS_COPY_PATTERN_AUDIO_RUN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_COPY_PATTERN_AUDIO_RUN, AgsCopyPatternAudioRun))
#define AGS_COPY_PATTERN_AUDIO_RUN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_COPY_PATTERN_AUDIO_RUN, AgsCopyPatternAudioRun))
#define AGS_IS_COPY_PATTERN_AUDIO_RUN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_COPY_PATTERN_AUDIO_RUN))
#define AGS_IS_COPY_PATTERN_AUDIO_RUN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_COPY_PATTERN_AUDIO_RUN))
#define AGS_COPY_PATTERN_AUDIO_RUN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_COPY_PATTERN_AUDIO_RUN, AgsCopyPatternAudioRunClass))

typedef struct _AgsCopyPatternAudioRun AgsCopyPatternAudioRun;
typedef struct _AgsCopyPatternAudioRunClass AgsCopyPatternAudioRunClass;

typedef enum{
  AGS_COPY_PATTERN_AUDIO_RUN_RUN_CONNECTED  = 1,
}AgsCopyPatternAudioRunFlags;

struct _AgsCopyPatternAudioRun
{
  AgsRecallAudioRun recall_audio_run;

  guint flags;

  //  AgsCopyPatternAudio *copy_pattern_audio;

  guint recall_ref;

  guint hide_ref;
  guint hide_ref_counter;

  gulong tic_alloc_handler;
  gulong tic_count_handler;

  AgsDelayAudioRun *delay_audio_run;
  guint bit;
};

struct _AgsCopyPatternAudioRunClass
{
  AgsRecallAudioRunClass recall_audio_run;
};

GType ags_copy_pattern_audio_run_get_type();

AgsCopyPatternAudioRun* ags_copy_pattern_audio_run_new(AgsRecallAudio *recall_audio,
						       AgsDelayAudioRun *delay_audio_run, guint bit);

#endif /*__AGS_COPY_PATTERN_AUDIO_RUN_H__*/
