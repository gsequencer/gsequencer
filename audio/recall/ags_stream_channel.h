#ifndef __AGS_STREAM_CHANNEL_H__
#define __AGS_STREAM_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include "../ags_recall.h"

#include "../ags_channel.h"

#define AGS_TYPE_STREAM_CHANNEL                (ags_stream_channel_get_type())
#define AGS_STREAM_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_STREAM_CHANNEL, AgsStreamChannel))
#define AGS_STREAM_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_STREAM_CHANNEL, AgsStreamChannelClass))
#define AGS_IS_STREAM_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_STREAM_CHANNEL))
#define AGS_IS_STREAM_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_STREAM_CHANNEL))
#define AGS_STREAM_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_STREAM_CHANNEL, AgsStreamChannelClass))

typedef struct _AgsStreamChannel AgsStreamChannel;
typedef struct _AgsStreamChannelClass AgsStreamChannelClass;

struct _AgsStreamChannel
{
  AgsRecall recall;

  AgsChannel *channel;
  gulong channel_recycling_changed_handler;
};

struct _AgsStreamChannelClass
{
  AgsRecallClass recall;
};

void ags_stream_channel_connect_run_handler(AgsStreamChannel *stream_channel);
void ags_stream_channel_disconnect_run_handler(AgsStreamChannel *stream_channel);

void ags_stream_channel_connect(AgsStreamChannel *stream_channel);

AgsStreamChannel* ags_stream_channel_new(AgsChannel *channel);

#endif /*__AGS_STREAM_CHANNEL_H__*/
