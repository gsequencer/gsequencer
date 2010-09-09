#ifndef __AGS_COPY_RECYCLING_H__
#define __AGS_COPY_RECYCLING_H__

#include <glib.h>
#include <glib-object.h>

#include "../ags_recall.h"

#include "../ags_devout.h"
#include "../ags_recycling.h"
#include "../ags_audio_signal.h"

#define AGS_TYPE_COPY_RECYCLING                (ags_copy_recycling_get_type())
#define AGS_COPY_RECYCLING(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_COPY_RECYCLING, AgsCopyRecycling))
#define AGS_COPY_RECYCLING_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_COPY_RECYCLING, AgsCopyRecyclingClass))
#define AGS_IS_COPY_RECYCLING(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_COPY_RECYCLING))
#define AGS_IS_COPY_RECYCLING_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_COPY_RECYCLING))
#define AGS_COPY_RECYCLING_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_COPY_RECYCLING, AgsCopyRecyclingClass))

typedef struct _AgsCopyRecycling AgsCopyRecycling;
typedef struct _AgsCopyRecyclingClass AgsCopyRecyclingClass;

struct _AgsCopyRecycling
{
  AgsRecall recall;

  AgsDevout *devout;

  AgsRecycling *destination;
  gulong destination_add_audio_signal_handler;
  gulong destination_add_audio_signal_with_length_handler;
  gulong destination_remove_audio_signal_handler;

  AgsRecycling *source;
  gulong source_add_audio_signal_handler;
  gulong source_add_audio_signal_with_length_handler;
  gulong source_remove_audio_signal_handler;

  AgsAudioSignal *child_destination;
};

struct _AgsCopyRecyclingClass
{
  AgsRecallClass recall;
};

void ags_copy_recycling_connect_run_handler(AgsCopyRecycling *copy_recycling);
void ags_copy_recycling_disconnect_run_handler(AgsCopyRecycling *copy_recycling);

void ags_copy_recycling_connect(AgsCopyRecycling *copy_recycling);

AgsCopyRecycling* ags_copy_recycling_new(AgsRecycling *destination,
					 AgsRecycling *source,
					 AgsDevout *devout);

#endif /*__AGS_COPY_RECYCLING_H__*/
