#ifndef __AGS_APPEND_AUDIO_H__
#define __AGS_APPEND_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_task.h>
#include <ags/audio/ags_devout.h>

#define AGS_TYPE_APPEND_AUDIO                (ags_append_audio_get_type())
#define AGS_APPEND_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_APPEND_AUDIO, AgsAppendAudio))
#define AGS_APPEND_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_APPEND_AUDIO, AgsAppendAudioClass))
#define AGS_IS_APPEND_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_APPEND_AUDIO))
#define AGS_IS_APPEND_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_APPEND_AUDIO))
#define AGS_APPEND_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_APPEND_AUDIO, AgsAppendAudioClass))

typedef struct _AgsAppendAudio AgsAppendAudio;
typedef struct _AgsAppendAudioClass AgsAppendAudioClass;

struct _AgsAppendAudio
{
  AgsTask task;

  AgsDevout *devout;
  AgsDevoutPlay *devout_play;
};

struct _AgsAppendAudioClass
{
  AgsTaskClass task;
};

GType ags_append_audio_get_type();

AgsAppendAudio* ags_append_audio_new(AgsDevout *devout,
				     AgsDevoutPlay *play);

#endif /*__AGS_APPEND_AUDIO_H__*/
