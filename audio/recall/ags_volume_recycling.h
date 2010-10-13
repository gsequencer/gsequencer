#ifndef __AGS_VOLUME_RECYCLING_H__
#define __AGS_VOLUME_RECYCLING_H__

#include <glib.h>
#include <glib-object.h>

#include "../ags_recall.h"

#include "../ags_recycling.h"

#define AGS_TYPE_VOLUME_RECYCLING                (ags_volume_recycling_get_type())
#define AGS_VOLUME_RECYCLING(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_VOLUME_RECYCLING, AgsVolumeRecycling))
#define AGS_VOLUME_RECYCLING_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_VOLUME_RECYCLING, AgsVolumeRecyclingClass))
#define AGS_IS_VOLUME_RECYCLING(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_VOLUME_RECYCLING))
#define AGS_IS_VOLUME_RECYCLING_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_VOLUME_RECYCLING))
#define AGS_VOLUME_RECYCLING_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_VOLUME_RECYCLING, AgsVolumeRecyclingClass))

typedef struct _AgsVolumeRecycling AgsVolumeRecycling;
typedef struct _AgsVolumeRecyclingClass AgsVolumeRecyclingClass;

struct _AgsVolumeRecycling
{
  AgsRecall recall;

  AgsRecycling *recycling;

  gulong add_audio_signal_handler;
  gulong add_audio_signal_with_frame_count_handler;

  gdouble *volume;
};

struct _AgsVolumeRecyclingClass
{
  AgsRecallClass recall;
};

GType ags_volume_recycling_get_type();

AgsVolumeRecycling* ags_volume_recycling_new(AgsRecycling *recycling, gdouble *volume);

#endif /*__AGS_VOLUME_RECYCLING_H__*/
