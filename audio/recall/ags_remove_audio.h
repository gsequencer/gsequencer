#ifndef __AGS_REMOVE_AUDIO_H__
#define __AGS_REMOVE_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_devout.h>

#define AGS_TYPE_REMOVE_AUDIO                (ags_remove_audio_get_type())
#define AGS_REMOVE_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_REMOVE_AUDIO, AgsRemoveAudio))
#define AGS_REMOVE_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_REMOVE_AUDIO, AgsRemoveAudio))
#define AGS_IS_REMOVE_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_REMOVE_AUDIO))
#define AGS_IS_REMOVE_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_REMOVE_AUDIO))
#define AGS_REMOVE_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_REMOVE_AUDIO, AgsRemoveAudioClass))

typedef struct _AgsRemoveAudio AgsRemoveAudio;
typedef struct _AgsRemoveAudioClass AgsRemoveAudioClass;

struct _AgsRemoveAudio
{
  AgsRecallAudio recall_audio;

  AgsDevout *devout;
};

struct _AgsRemoveAudioClass
{
  AgsRecallAudioClass recall_audio;
};

GType ags_remove_audio_get_type();

AgsRemoveAudio* ags_remove_audio_new(AgsDevout *devout, AgsAudio *audio);

#endif /*__AGS_REMOVE_AUDIO_H__*/
