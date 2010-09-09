#ifndef __AGS_PLAY_AUDIO_SIGNAL_H__
#define __AGS_PLAY_AUDIO_SIGNAL_H__
 
#include <glib.h>
#include <glib-object.h>

#include "../ags_recall.h"
#include "../ags_devout.h"
#include "../ags_audio_signal.h"

#define AGS_TYPE_PLAY_AUDIO_SIGNAL                (ags_play_audio_signal_get_type())
#define AGS_PLAY_AUDIO_SIGNAL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLAY_AUDIO_SIGNAL, AgsPlayAudioSignal))
#define AGS_PLAY_AUDIO_SIGNAL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PLAY_AUDIO_SIGNAL, AgsPlayAudioSignalClass))
#define AGS_IS_PLAY_AUDIO_SIGNAL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PLAY_AUDIO_SIGNAL))
#define AGS_IS_PLAY_AUDIO_SIGNAL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PLAY_AUDIO_SIGNAL))
#define AGS_PLAY_AUDIO_SIGNAL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_PLAY_AUDIO_SIGNAL, AgsPlayAudioSignalClass))

typedef struct _AgsPlayAudioSignal AgsPlayAudioSignal;
typedef struct _AgsPlayAudioSignalClass AgsPlayAudioSignalClass;

struct _AgsPlayAudioSignal
{
  AgsRecall recall;

  AgsAudioSignal *source;
  guint audio_channel;

  AgsDevout *devout;
  AgsAttack *attack;
};

struct _AgsPlayAudioSignalClass
{
  AgsRecallClass recall;
};

void ags_play_audio_signal_connect(AgsPlayAudioSignal *play_audio_signal);

AgsPlayAudioSignal* ags_play_audio_signal_new(AgsAudioSignal *source, guint audio_channel,
					      AgsDevout *devout);

#endif /*__AGS_PLAY_AUDIO_SIGNAL__H__*/
