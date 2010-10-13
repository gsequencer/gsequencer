#ifndef __AGS_VOLUME_CHANNEL_H__
#define __AGS_VOLUME_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include "../ags_recall.h"

#include "../ags_channel.h"

#define AGS_TYPE_VOLUME_CHANNEL            (ags_volume_channel_get_type())
#define AGS_VOLUME_CHANNEL(obj)            (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_VOLUME_CHANNEL, AgsVolumeChannel))
#define AGS_VOLUME_CHANNEL_CLASS(class)    (G_TYPE_CHECK_INSTANCE_CAST(class, AGS_TYPE_VOLUME_CHANNEL, AgsVolumeChannelClass))

typedef struct _AgsVolumeChannel AgsVolumeChannel;
typedef struct _AgsVolumeChannelClass AgsVolumeChannelClass;

struct _AgsVolumeChannel{
  AgsRecall recall;

  AgsChannel *channel;
  gulong channel_recycling_changed_handler;

  gdouble *volume;
};

struct _AgsVolumeChannelClass{
  AgsRecallClass recall;
};

GType ags_volume_channel_get_type();

AgsVolumeChannel* ags_volume_channel_new(AgsChannel *channel, gdouble *volume);

#endif /*__AGS_VOLUME_CHANNEL_H__*/
