#ifndef __AGS_CANCEL_AUDIO_H__
#define __AGS_CANCEL_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_task.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>

#define AGS_TYPE_CANCEL_AUDIO                (ags_cancel_audio_get_type())
#define AGS_CANCEL_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CANCEL_AUDIO, AgsCancelAudio))
#define AGS_CANCEL_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CANCEL_AUDIO, AgsCancelAudioClass))
#define AGS_IS_CANCEL_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_CANCEL_AUDIO))
#define AGS_IS_CANCEL_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_CANCEL_AUDIO))
#define AGS_CANCEL_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_CANCEL_AUDIO, AgsCancelAudioClass))

typedef struct _AgsCancelAudio AgsCancelAudio;
typedef struct _AgsCancelAudioClass AgsCancelAudioClass;

struct _AgsCancelAudio
{
  AgsTask task;

  AgsAudio *audio;
  guint group_id;

  AgsDevoutPlay *play;
};

struct _AgsCancelAudioClass
{
  AgsTaskClass task;
};

GType ags_cancel_audio_get_type();

AgsCancelAudio* ags_cancel_audio_new(AgsAudio *audio, guint group_id,
				     AgsDevoutPlay *play);

#endif /*__AGS_CANCEL_AUDIO_H__*/
