#ifndef __AGS_COPY_PATTERN_CHANNEL_H__
#define __AGS_COPY_PATTERN_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_pattern.h>

#define AGS_TYPE_COPY_PATTERN_CHANNEL                (ags_copy_pattern_channel_get_type())
#define AGS_COPY_PATTERN_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_COPY_PATTERN_CHANNEL, AgsCopyPatternChannel))
#define AGS_COPY_PATTERN_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_COPY_PATTERN_CHANNEL, AgsCopyPatternChannel))
#define AGS_IS_COPY_PATTERN_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_COPY_PATTERN_CHANNEL))
#define AGS_IS_COPY_PATTERN_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_COPY_PATTERN_CHANNEL))
#define AGS_COPY_PATTERN_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_COPY_PATTERN_CHANNEL, AgsCopyPatternChannelClass))

typedef struct _AgsCopyPatternChannel AgsCopyPatternChannel;
typedef struct _AgsCopyPatternChannelClass AgsCopyPatternChannelClass;

struct _AgsCopyPatternChannel
{
  AgsRecallChannel recall_channel;

  AgsChannel *destination;

  AgsChannel *source;
  AgsPattern *pattern;
};

struct _AgsCopyPatternChannelClass
{
  AgsRecallChannelClass recall_channel;
};

GType ags_copy_pattern_channel_get_type();

AgsCopyPatternChannel* ags_copy_pattern_channel_new(AgsRecallAudio *recall_audio,
						    AgsChannel *destination,
						    AgsChannel *source, AgsPattern *pattern);

#endif /*__AGS_COPY_PATTERN_CHANNEL_H__*/
