#ifndef __AGS_COPY_STREAM_TO_STREAM_H__
#define __AGS_COPY_STREAM_TO_STREAM_H__

#include <glib.h>
#include <glib-object.h>

#include "../ags_recall.h"
#include "../ags_audio_signal.h"

#define AGS_TYPE_COPY_STREAM_TO_STREAM                (ags_copy_stream_to_stream_get_type())
#define AGS_COPY_STREAM_TO_STREAM(obj)                (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_COPY_STREAM_TO_STREAM, AgsCopyStreamToStream))
#define AGS_COPY_STREAM_TO_STREAM_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_COPY_STREAM_TO_STREAM, AgsCopyStreamToStream))

typedef struct _AgsCopyStreamToStream AgsCopyStreamToStream;
typedef struct _AgsCopyStreamToStreamClass AgsCopyStreamToStreamClass;

struct _AgsCopyStreamToStream{
  AgsRecall recall;

  AgsAudioSignal *destination;
  AgsAudioSignal *source;

  GList *stream0;
  GList *stream1;
};

struct _AgsCopyStreamToStreamClass{
  AgsRecallClass recall;
};

void ags_copy_stream_to_stream(AgsRecall *recall);

AgsCopyStreamToStream* ags_copy_stream_to_stream_new();

#endif /*__AGS_COPY_STREAM_TO_STREAM_H__*/
