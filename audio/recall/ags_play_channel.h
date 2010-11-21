#ifndef __AGS_PLAY_CHANNEL_H__
#define __AGS_PLAY_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_channel.h>

#define AGS_TYPE_PLAY_CHANNEL                (ags_play_channel_get_type())
#define AGS_PLAY_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLAY_CHANNEL, AgsPlayChannel))
#define AGS_PLAY_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PLAY_CHANNEL, AgsPlayChannelClass))
#define AGS_IS_PLAY_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PLAY_CHANNEL))
#define AGS_IS_PLAY_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_PLAY_CHANNEL))
#define AGS_PLAY_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_PLAY_CHANNEL, AgsPlayChannelClass))

typedef struct _AgsPlayChannel AgsPlayChannel;
typedef struct _AgsPlayChannelClass AgsPlayChannelClass;

struct _AgsPlayChannel
{
  AgsRecall recall;

  //  guint ref;

  AgsDevout *devout;

  AgsChannel *source;
  gulong source_recycling_changed_handler;
};

struct _AgsPlayChannelClass
{
  AgsRecallClass recall;
};

GType ags_play_channel_get_type();

AgsPlayChannel* ags_play_channel_new(AgsChannel *source,
				     AgsDevout *devout);

#endif /*__AGS_PLAY_CHANNEL_H__*/
