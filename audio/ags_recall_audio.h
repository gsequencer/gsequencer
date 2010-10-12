#ifndef __AGS_RECALL_AUDIO_H__
#define __AGS_RECALL_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include "ags_recall.h"
#include "ags_audio.h"

#define AGS_TYPE_RECALL_AUDIO                (ags_recall_audio_get_type())
#define AGS_RECALL_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RECALL_AUDIO, AgsRecallAudio))
#define AGS_RECALL_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RECALL_AUDIO, AgsRecallAudioClass))
#define AGS_IS_RECALL_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_RECALL_AUDIO))
#define AGS_IS_RECALL_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_RECALL_AUDIO))
#define AGS_RECALL_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_RECALL_AUDIO, AgsRecallAudioClass))

typedef struct _AgsRecallAudio AgsRecallAudio;
typedef struct _AgsRecallAudioClass AgsRecallAudioClass;

struct _AgsRecallAudio
{
  AgsRecall recall;

  AgsAudio *audio;
};

struct _AgsRecallAudioClass
{
  AgsRecallClass recall;
};

GType ags_recall_audio_get_type();

AgsRecallAudio* ags_recall_audio_new(AgsAudio *audio);

#endif /*__AGS_RECALL_AUDIO_H__*/
