#ifndef __AGS_STREAM_H__
#define __AGS_STREAM_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_recall_id.h>

#define AGS_TYPE_STREAM                (ags_stream_get_type())
#define AGS_STREAM(obj)                (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_STREAM, AgsStream))
#define AGS_STREAM_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_STREAM, AgsStream))

typedef struct _AgsStream AgsStream;
typedef struct _AgsStreamClass AgsStreamClass;

struct _AgsStream{
  AgsRecall recall;

  AgsAudioSignal *audio_signal;
};

struct _AgsStreamClass{
  AgsRecallClass recall;
};

GType ags_stream_get_type();

void ags_stream(AgsRecall *recall, AgsRecallID *recall_id, gpointer data);

AgsStream* ags_stream_new();

#endif /*__AGS_STREAM_H__*/
