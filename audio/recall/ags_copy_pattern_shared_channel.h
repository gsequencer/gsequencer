#ifndef __AGS_COPY_PATTERN_SHARED_CHANNEL_H__
#define __AGS_COPY_PATTERN_SHARED_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include "../ags_recall_shared_channel.h"
#include "../ags_channel.h"
#include "../ags_pattern.h"

#define AGS_TYPE_COPY_PATTERN_SHARED_CHANNEL                (ags_copy_pattern_shared_channel_get_type())
#define AGS_COPY_PATTERN_SHARED_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_COPY_PATTERN_SHARED_CHANNEL, AgsCopyPatternSharedChannel))
#define AGS_COPY_PATTERN_SHARED_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_COPY_PATTERN_SHARED_CHANNEL, AgsCopyPatternSharedChannel))
#define AGS_IS_COPY_PATTERN_SHARED_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_COPY_PATTERN_SHARED_CHANNEL))
#define AGS_IS_COPY_PATTERN_SHARED_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_COPY_PATTERN_SHARED_CHANNEL))
#define AGS_COPY_PATTERN_SHARED_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_COPY_PATTERN_SHARED_CHANNEL, AgsCopyPatternSharedChannelClass))

typedef struct _AgsCopyPatternSharedChannel AgsCopyPatternSharedChannel;
typedef struct _AgsCopyPatternSharedChannelClass AgsCopyPatternSharedChannelClass;

struct _AgsCopyPatternSharedChannel
{
  AgsRecallSharedChannel shared_channel;

  AgsChannel *destination;

  AgsChannel *source;
  AgsPattern *pattern;
};

struct _AgsCopyPatternSharedChannelClass
{
  AgsRecallSharedChannelClass shared_channel;
};

AgsCopyPatternSharedChannel* ags_copy_pattern_shared_channel_new(AgsChannel *destination,
								 AgsChannel *source, AgsPattern *pattern);

#endif /*__AGS_COPY_PATTERN_SHARED_CHANNEL_H__*/
