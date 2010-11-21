#ifndef __AGS_VOLUME_AUDIO_SIGNAL_H__
#define __AGS_VOLUME_AUDIO_SIGNAL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall.h>

#include <ags/audio/ags_audio_signal.h>

#define AGS_TYPE_VOLUME_AUDIO_SIGNAL                (ags_volume_audio_signal_get_type())
#define AGS_VOLUME_AUDIO_SIGNAL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_VOLUME_AUDIO_SIGNAL, AgsVolumeAudioSignal))
#define AGS_VOLUME_AUDIO_SIGNAL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_VOLUME_AUDIO_SIGNAL, AgsVolumeAudioSignalClass))
#define AGS_IS_VOLUME_AUDIO_SIGNAL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_VOLUME_AUDIO_SIGNAL))
#define AGS_IS_VOLUME_AUDIO_SIGNAL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_VOLUME_AUDIO_SIGNAL))
#define AGS_VOLUME_AUDIO_SIGNAL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_VOLUME_AUDIO_SIGNAL, AgsVolumeAudioSignalClass))

typedef struct _AgsVolumeAudioSignal AgsVolumeAudioSignal;
typedef struct _AgsVolumeAudioSignalClass AgsVolumeAudioSignalClass;

struct _AgsVolumeAudioSignal
{
  AgsRecall recall;

  AgsAudioSignal *audio_signal;

  gdouble *volume;
};

struct _AgsVolumeAudioSignalClass
{
  AgsRecallClass recall;
};

GType ags_volume_audio_signal_get_type();

AgsVolumeAudioSignal* ags_volume_audio_signal_new(AgsAudioSignal *audio_signal, gdouble *volume);

#endif /*__AGS_VOLUME_AUDIO_SIGNAL_H__*/
