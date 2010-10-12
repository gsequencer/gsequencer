#ifndef __AGS_RECALL_VOLUME_H__
#define __AGS_RECALL_VOLUME_H__

#include <glib.h>
#include <glib-object.h>

#include "../ags_recall.h"

#include "../ags_channel.h"

#define AGS_TYPE_RECALL_VOLUME            (ags_recall_volume_get_type())
#define AGS_RECALL_VOLUME(obj)            (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_RECALL_VOLUME, AgsRecallVolume))
#define AGS_RECALL_VOLUME_CLASS(class)    (G_TYPE_CHECK_INSTANCE_CAST(class, AGS_TYPE_RECALL_VOLUME, AgsRecallVolumeClass))

typedef struct _AgsRecallVolume AgsRecallVolume;
typedef struct _AgsRecallVolumeClass AgsRecallVolumeClass;

struct _AgsRecallVolume{
  AgsRecall recall;

  AgsChannel *channel;
  gdouble *volume;
};

struct _AgsRecallVolumeClass{
  AgsRecallClass recall;
};

GType ags_recall_volume_get_type();

AgsRecallVolume* ags_recall_volume_new(AgsChannel *channel);

#endif /*__AGS_RECALL_VOLUME_H__*/
