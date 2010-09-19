#ifndef __AGS_PLAY_RECYCLING_H__
#define __AGS_PLAY_RECYCLING_H__

#include <glib.h>
#include <glib-object.h>

#include "../ags_recall.h"

#include "../ags_devout.h"
#include "../ags_recycling.h"
#include "../ags_audio_signal.h"

#define AGS_TYPE_PLAY_RECYCLING                (ags_play_recycling_get_type())
#define AGS_PLAY_RECYCLING(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLAY_RECYCLING, AgsPlayRecycling))
#define AGS_PLAY_RECYCLING_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PLAY_RECYCLING, AgsPlayRecyclingClass))
#define AGS_IS_PLAY_RECYCLING(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PLAY_RECYCLING))
#define AGS_IS_PLAY_RECYCLING_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PLAY_RECYCLING))
#define AGS_PLAY_RECYCLING_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_PLAY_RECYCLING, AgsPlayRecyclingClass))

typedef struct _AgsPlayRecycling AgsPlayRecycling;
typedef struct _AgsPlayRecyclingClass AgsPlayRecyclingClass;

struct _AgsPlayRecycling
{
  AgsRecall recall;

  AgsDevout *devout;

  AgsRecycling *source;
  gulong source_add_audio_signal_handler;
  gulong source_add_audio_signal_with_length_handler;
  gulong source_remove_audio_signal_handler;

  guint audio_channel;
};

struct _AgsPlayRecyclingClass
{
  AgsRecallClass recall;
};

AgsPlayRecycling* ags_play_recycling_new(AgsRecycling *source, guint audio_channel,
					 AgsDevout *devout);

#endif /*__AGS_PLAY_RECYCLING_H__*/
