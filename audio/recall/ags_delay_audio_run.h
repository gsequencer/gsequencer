#ifndef __AGS_DELAY_AUDIO_RUN_H__
#define __AGS_DELAY_AUDIO_RUN_H__

#include <glib.h>
#include <glib-object.h>

#include "../ags_recall_audio.h"
#include "../ags_recall_audio_run.h"

#define AGS_TYPE_DELAY_AUDIO_RUN                (ags_delay_audio_run_get_type())
#define AGS_DELAY_AUDIO_RUN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DELAY_AUDIO_RUN, AgsDelayAudioRun))
#define AGS_DELAY_AUDIO_RUN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_DELAY_AUDIO_RUN, AgsDelayAudioRun))
#define AGS_IS_DELAY_AUDIO_RUN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_DELAY_AUDIO_RUN))
#define AGS_IS_DELAY_AUDIO_RUN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_DELAY_AUDIO_RUN))
#define AGS_DELAY_AUDIO_RUN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_DELAY_AUDIO_RUN, AgsDelayAudioRunClass))

typedef struct _AgsDelayAudioRun AgsDelayAudioRun;
typedef struct _AgsDelayAudioRunClass AgsDelayAudioRunClass;

struct _AgsDelayAudioRun
{
  AgsRecallAudioRun recall_audio_run;

  guint recall_ref;

  guint hide_ref;
  guint hide_ref_counter;

  guint counter;
};

struct _AgsDelayAudioRunClass
{
  AgsRecallAudioRunClass recall_audio_run;

  void (*tic)(AgsDelayAudioRun *delay);
};

GType ags_delay_audio_run_get_type();

void ags_delay_audio_run_tic(AgsDelayAudioRun *delay, guint audio_channel);

AgsDelayAudioRun* ags_delay_audio_run_new(AgsRecallAudio *recall_audio);

#endif /*__AGS_DELAY_AUDIO_RUN_H__*/
