#ifndef __AGS_STREAM_AUDIO_SIGNAL_H__
#define __AGS_STREAM_AUDIO_SIGNAL_H__

#include <glib.h>
#include <glib-object.h>

#include "../ags_recall.h"

#include "../ags_audio_signal.h"

#define AGS_TYPE_STREAM_AUDIO_SIGNAL                (ags_stream_audio_signal_get_type())
#define AGS_STREAM_AUDIO_SIGNAL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_STREAM_AUDIO_SIGNAL, AgsStreamAudioSignal))
#define AGS_STREAM_AUDIO_SIGNAL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_STREAM_AUDIO_SIGNAL, AgsStreamAudioSignalClass))
#define AGS_IS_STREAM_AUDIO_SIGNAL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_STREAM_AUDIO_SIGNAL))
#define AGS_IS_STREAM_AUDIO_SIGNAL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_STREAM_AUDIO_SIGNAL))
#define AGS_STREAM_AUDIO_SIGNAL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_STREAM_AUDIO_SIGNAL, AgsStreamAudioSignalClass))

typedef struct _AgsStreamAudioSignal AgsStreamAudioSignal;
typedef struct _AgsStreamAudioSignalClass AgsStreamAudioSignalClass;

struct _AgsStreamAudioSignal
{
  AgsRecall recall;

  AgsAudioSignal *audio_signal;
};

struct _AgsStreamAudioSignalClass
{
  AgsRecallClass recall;
};

AgsStreamAudioSignal* ags_stream_audio_signal_new(AgsAudioSignal *audio_signal);

#endif /*__AGS_STREAM_AUDIO_SIGNAL_H__*/
