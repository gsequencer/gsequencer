#ifndef __AGS_RECALL_SHARED_CHANNEL_H__
#define __AGS_RECALL_SHARED_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include "ags_recall_shared.h"

#define AGS_TYPE_RECALL_SHARED_CHANNEL                (ags_recall_shared_channel_get_type())
#define AGS_RECALL_SHARED_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RECALL_SHARED_CHANNEL, AgsRecallSharedChannel))
#define AGS_RECALL_SHARED_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RECALL_SHARED_CHANNEL, AgsRecallSharedChannelClass))
#define AGS_IS_RECALL_SHARED_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_RECALL_SHARED_CHANNEL))
#define AGS_IS_RECALL_SHARED_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_RECALL_SHARED_CHANNEL))
#define AGS_RECALL_SHARED_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_RECALL_SHARED_CHANNEL, AgsRecallSharedChannelClass))

typedef struct _AgsRecallSharedChannel AgsRecallSharedChannel;
typedef struct _AgsRecallSharedChannelClass AgsRecallSharedChannelClass;

struct _AgsRecallSharedChannel
{
  AgsRecallShared shared;
};

struct _AgsRecallSharedChannelClass
{
  AgsRecallSharedClass shared;
};

AgsRecallSharedChannel* ags_recall_shared_channel_new();

#endif /*__AGS_RECALL_SHARED_CHANNEL_H__*/
