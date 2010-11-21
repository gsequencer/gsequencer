#ifndef __AGS_STREAM_RECYCLING_H__
#define __AGS_STREAM_RECYCLING_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall.h>

#include <ags/audio/ags_recycling.h>

#define AGS_TYPE_STREAM_RECYCLING                (ags_stream_recycling_get_type())
#define AGS_STREAM_RECYCLING(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_STREAM_RECYCLING, AgsStreamRecycling))
#define AGS_STREAM_RECYCLING_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_STREAM_RECYCLING, AgsStreamRecyclingClass))
#define AGS_IS_STREAM_RECYCLING(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_STREAM_RECYCLING))
#define AGS_IS_STREAM_RECYCLING_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_STREAM_RECYCLING))
#define AGS_STREAM_RECYCLING_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_STREAM_RECYCLING, AgsStreamRecyclingClass))

typedef struct _AgsStreamRecycling AgsStreamRecycling;
typedef struct _AgsStreamRecyclingClass AgsStreamRecyclingClass;

struct _AgsStreamRecycling
{
  AgsRecall recall;

  AgsRecycling *recycling;

  gulong add_audio_signal_handler;
  gulong add_audio_signal_with_frame_count_handler;
};

struct _AgsStreamRecyclingClass
{
  AgsRecallClass recall;
};

GType ags_stream_recycling_get_type();

AgsStreamRecycling* ags_stream_recycling_new(AgsRecycling *recycling);

#endif /*__AGS_STREAM_RECYCLING_H__*/
