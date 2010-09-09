#ifndef __AGS_DELAY_SHARED_AUDIO_H__
#define __AGS_DELAY_SHARED_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include "../ags_recall_shared_audio.h"

#define AGS_TYPE_DELAY_SHARED_AUDIO                (ags_delay_shared_audio_get_type())
#define AGS_DELAY_SHARED_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DELAY_SHARED_AUDIO, AgsDelaySharedAudio))
#define AGS_DELAY_SHARED_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_DELAY_SHARED_AUDIO, AgsDelaySharedAudio))
#define AGS_IS_DELAY_SHARED_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_DELAY_SHARED_AUDIO))
#define AGS_IS_DELAY_SHARED_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_DELAY_SHARED_AUDIO))
#define AGS_DELAY_SHARED_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_DELAY_SHARED_AUDIO, AgsDelaySharedAudioClass))

typedef struct _AgsDelaySharedAudio AgsDelaySharedAudio;
typedef struct _AgsDelaySharedAudioClass AgsDelaySharedAudioClass;

struct _AgsDelaySharedAudio
{
  AgsRecallSharedAudio shared_audio;

  guint delay;
};

struct _AgsDelaySharedAudioClass
{
  AgsRecallSharedAudioClass shared_audio;
};

AgsDelaySharedAudio* ags_delay_shared_audio_new(guint delay);

#endif /*__AGS_DELAY_SHARED_AUDIO_H__*/
