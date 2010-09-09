#ifndef __AGS_RECALL_SHARED_AUDIO_H__
#define __AGS_RECALL_SHARED_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include "ags_recall_shared.h"

#define AGS_TYPE_RECALL_SHARED_AUDIO                (ags_recall_shared_audio_get_type())
#define AGS_RECALL_SHARED_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RECALL_SHARED_AUDIO, AgsRecallSharedAudio))
#define AGS_RECALL_SHARED_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RECALL_SHARED_AUDIO, AgsRecallSharedAudioClass))
#define AGS_IS_RECALL_SHARED_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_RECALL_SHARED_AUDIO))
#define AGS_IS_RECALL_SHARED_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_RECALL_SHARED_AUDIO))
#define AGS_RECALL_SHARED_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_RECALL_SHARED_AUDIO, AgsRecallSharedAudioClass))

typedef struct _AgsRecallSharedAudio AgsRecallSharedAudio;
typedef struct _AgsRecallSharedAudioClass AgsRecallSharedAudioClass;

struct _AgsRecallSharedAudio
{
  AgsRecallShared shared;
};

struct _AgsRecallSharedAudioClass
{
  AgsRecallSharedClass shared;
};

AgsRecallSharedAudio* ags_recall_shared_audio_new();

#endif /*__AGS_RECALL_SHARED_AUDIO_H__*/
