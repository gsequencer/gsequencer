#ifndef __AGS_COPY_AUDIO_SIGNAL_H__
#define __AGS_COPY_AUDIO_SIGNAL_H__
 
#include <glib.h>
#include <glib-object.h>

#include "../ags_recall.h"

#include "../ags_devout.h"
#include "../ags_audio_signal.h"

#define AGS_TYPE_COPY_AUDIO_SIGNAL                (ags_copy_audio_signal_get_type())
#define AGS_COPY_AUDIO_SIGNAL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_COPY_AUDIO_SIGNAL, AgsCopyAudioSignal))
#define AGS_COPY_AUDIO_SIGNAL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_COPY_AUDIO_SIGNAL, AgsCopyAudioSignalClass))
#define AGS_IS_COPY_AUDIO_SIGNAL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_COPY_AUDIO_SIGNAL))
#define AGS_IS_COPY_AUDIO_SIGNAL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_COPY_AUDIO_SIGNAL))
#define AGS_COPY_AUDIO_SIGNAL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_COPY_AUDIO_SIGNAL, AgsCopyAudioSignalClass))

typedef struct _AgsCopyAudioSignal AgsCopyAudioSignal;
typedef struct _AgsCopyAudioSignalClass AgsCopyAudioSignalClass;

struct _AgsCopyAudioSignal
{
  AgsRecall recall;

  AgsDevout *devout;

  AgsAudioSignal *destination;
  AgsAttack *attack;

  AgsAudioSignal *source;
};

struct _AgsCopyAudioSignalClass
{
  AgsRecallClass recall;
};

AgsCopyAudioSignal* ags_copy_audio_signal_new(AgsAudioSignal *destination,
					      AgsAudioSignal *source,
					      AgsDevout *devout);

#endif /*__AGS_COPY_AUDIO_SIGNAL_H__*/
