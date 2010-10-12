#ifndef __AGS_PLAY_VOLUME_H__
#define __AGS_PLAY_VOLUME_H__

#include <glib.h>
#include <glib-object.h>

#include "../ags_recall.h"

#include "../ags_channel.h"

#define AGS_TYPE_PLAY_VOLUME            (ags_play_volume_get_type())
#define AGS_PLAY_VOLUME(obj)            (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_PLAY_VOLUME, AgsPlayVolume))
#define AGS_PLAY_VOLUME_CLASS(class)    (G_TYPE_CHECK_INSTANCE_CAST(class, AGS_TYPE_PLAY_VOLUME, AgsPlayVolumeClass))

typedef struct _AgsPlayVolume AgsPlayVolume;
typedef struct _AgsPlayVolumeClass AgsPlayVolumeClass;

struct _AgsPlayVolume{
  AgsRecall recall;

  AgsChannel *channel;
  GList *audio_signal;
  gdouble *volume;
};

struct _AgsPlayVolumeClass{
  AgsRecallClass recall;
};

GType ags_play_volume_get_type();

int ags_play_volume_map_audio_signal(AgsRecall *recall, AgsRecallID *recall_id, gpointer data);

AgsPlayVolume* ags_play_volume_new(AgsChannel *channel);

#endif /*__AGS_PLAY_VOLUME_H__*/
