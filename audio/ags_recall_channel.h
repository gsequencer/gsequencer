#ifndef __AGS_RECALL_CHANNEL_H__
#define __AGS_RECALL_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include "ags_recall.h"
#include "ags_channel.h"

#define AGS_TYPE_RECALL_CHANNEL                (ags_recall_channel_get_type())
#define AGS_RECALL_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RECALL_CHANNEL, AgsRecallChannel))
#define AGS_RECALL_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RECALL_CHANNEL, AgsRecallChannelClass))
#define AGS_IS_RECALL_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_RECALL_CHANNEL))
#define AGS_IS_RECALL_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_RECALL_CHANNEL))
#define AGS_RECALL_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_RECALL_CHANNEL, AgsRecallChannelClass))

typedef struct _AgsRecallChannel AgsRecallChannel;
typedef struct _AgsRecallChannelClass AgsRecallChannelClass;

struct _AgsRecallChannel
{
  AgsRecall recall;

  AgsChannel *channel;
};

struct _AgsRecallChannelClass
{
  AgsRecallClass recall;
};

AgsRecallChannel* ags_recall_channel_new(AgsChannel *channel);

#endif /*__AGS_RECALL_CHANNEL_H__*/
