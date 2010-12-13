#ifndef __AGS_LINK_CHANNEL_H__
#define __AGS_LINK_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_task.h>

#include <ags/audio/ags_channel.h>

#define AGS_TYPE_LINK_CHANNEL                (ags_link_channel_get_type())
#define AGS_LINK_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LINK_CHANNEL, AgsLinkChannel))
#define AGS_LINK_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LINK_CHANNEL, AgsLinkChannelClass))
#define AGS_IS_LINK_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_LINK_CHANNEL))
#define AGS_IS_LINK_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_LINK_CHANNEL))
#define AGS_LINK_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_LINK_CHANNEL, AgsLinkChannelClass))

typedef struct _AgsLinkChannel AgsLinkChannel;
typedef struct _AgsLinkChannelClass AgsLinkChannelClass;

struct _AgsLinkChannel
{
  AgsTask task;

  AgsChannel *channel;
  AgsChannel *link;

  GError *error;
};

struct _AgsLinkChannelClass
{
  AgsTaskClass task;
};

GType ags_link_channel_get_type();

AgsLinkChannel* ags_link_channel_new(AgsChannel *channel, AgsChannel *link);

#endif /*__AGS_LINK_CHANNEL_H__*/
