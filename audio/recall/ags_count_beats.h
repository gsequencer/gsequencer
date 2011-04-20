#ifndef __AGS_COUNT_BEATS_H__
#define __AGS_COUNT_BEATS_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_audio.h>

#define AGS_TYPE_COUNT_BEATS                (ags_count_beats_get_type())
#define AGS_COUNT_BEATS(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_COUNT_BEATS, AgsCountBeats))
#define AGS_COUNT_BEATS_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_COUNT_BEATS, AgsCountBeats))
#define AGS_IS_COUNT_BEATS(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_COUNT_BEATS))
#define AGS_IS_COUNT_BEATS_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_COUNT_BEATS))
#define AGS_COUNT_BEATS_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_COUNT_BEATS, AgsCountBeatsClass))

typedef struct _AgsCountBeats AgsCountBeats;
typedef struct _AgsCountBeatsClass AgsCountBeatsClass;

struct _AgsCountBeats
{
  AgsRecallAudio recall_audio;

  double bpm;
};

struct _AgsCountBeatsClass
{
  AgsRecallAudioClass recall_audio;
};

GType ags_count_beats_get_type();

AgsCountBeats* ags_count_beats_new(AgsAudio *audio, double bpm);

#endif /*__AGS_COUNT_BEATS_H__*/
