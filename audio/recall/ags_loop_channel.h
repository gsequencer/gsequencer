#ifndef __AGS_LOOP_CHANNEL_H__
#define __AGS_LOOP_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include "../ags_channel.h"
#include "../ags_recall_channel.h"
#include "../ags_recall_id.h"

#include "ags_delay_audio_run.h"

#define AGS_TYPE_LOOP_CHANNEL                (ags_loop_channel_get_type())
#define AGS_LOOP_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LOOP_CHANNEL, AgsLoopChannel))
#define AGS_LOOP_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LOOP_CHANNEL, AgsLoopChannelClass))
#define AGS_IS_LOOP_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LOOP_CHANNEL))
#define AGS_IS_LOOP_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LOOP_CHANNEL))
#define AGS_LOOP_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LOOP_CHANNEL, AgsLoopChannelClass))

typedef struct _AgsLoopChannel AgsLoopChannel;
typedef struct _AgsLoopChannelClass AgsLoopChannelClass;

struct _AgsLoopChannel
{
  AgsRecallChannel recall_channel;

  AgsDelayAudioRun *delay_audio_run;
  AgsChannel *channel;
};

struct _AgsLoopChannelClass
{
  AgsRecallChannelClass recall_channel;
};

GType ags_loop_channel_get_type();

void ags_loop_channel_connect(AgsLoopChannel *loop_channel);

AgsLoopChannel* ags_loop_channel_new(AgsChannel *channel,
				     AgsDelayAudioRun *delay_audio_run);

#endif /*__AGS_LOOP_CHANNEL_H__*/
