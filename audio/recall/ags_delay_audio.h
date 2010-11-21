#ifndef __AGS_DELAY_AUDIO_H__
#define __AGS_DELAY_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_audio.h>

#define AGS_TYPE_DELAY_AUDIO                (ags_delay_audio_get_type())
#define AGS_DELAY_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DELAY_AUDIO, AgsDelayAudio))
#define AGS_DELAY_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_DELAY_AUDIO, AgsDelayAudio))
#define AGS_IS_DELAY_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_DELAY_AUDIO))
#define AGS_IS_DELAY_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_DELAY_AUDIO))
#define AGS_DELAY_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_DELAY_AUDIO, AgsDelayAudioClass))

typedef struct _AgsDelayAudio AgsDelayAudio;
typedef struct _AgsDelayAudioClass AgsDelayAudioClass;

struct _AgsDelayAudio
{
  AgsRecallAudio recall_audio;

  guint delay;
};

struct _AgsDelayAudioClass
{
  AgsRecallAudioClass recall_audio;
};

GType ags_delay_audio_get_type();

AgsDelayAudio* ags_delay_audio_new(AgsAudio *audio,
				   guint delay);

#endif /*__AGS_DELAY_AUDIO_H__*/
