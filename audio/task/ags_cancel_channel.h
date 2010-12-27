#ifndef __AGS_CANCEL_CHANNEL_H__
#define __AGS_CANCEL_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_task.h>
#include <ags/audio/ags_channel.h>

#define AGS_TYPE_CANCEL_CHANNEL                (ags_cancel_channel_get_type())
#define AGS_CANCEL_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CANCEL_CHANNEL, AgsCancelChannel))
#define AGS_CANCEL_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CANCEL_CHANNEL, AgsCancelChannelClass))
#define AGS_IS_CANCEL_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_CANCEL_CHANNEL))
#define AGS_IS_CANCEL_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_CANCEL_CHANNEL))
#define AGS_CANCEL_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_CANCEL_CHANNEL, AgsCancelChannelClass))

typedef struct _AgsCancelChannel AgsCancelChannel;
typedef struct _AgsCancelChannelClass AgsCancelChannelClass;

struct _AgsCancelChannel
{
  AgsTask task;

  AgsChannel *channel;
  guint group_id;
};

struct _AgsCancelChannelClass
{
  AgsTaskClass task;
};

GType ags_cancel_channel_get_type();

AgsCancelChannel* ags_cancel_channel_new(AgsChannel *channel, guint group_id);

#endif /*__AGS_CANCEL_CHANNEL_H__*/
