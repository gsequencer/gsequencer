#ifndef __AGS_COPY_CHANNEL_H__
#define __AGS_COPY_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include "../ags_recall.h"

#include "../ags_devout.h"
#include "../ags_channel.h"

#define AGS_TYPE_COPY_CHANNEL                (ags_copy_channel_get_type())
#define AGS_COPY_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_COPY_CHANNEL, AgsCopyChannel))
#define AGS_COPY_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_COPY_CHANNEL, AgsCopyChannelClass))
#define AGS_IS_COPY_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_COPY_CHANNEL))
#define AGS_IS_COPY_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_COPY_CHANNEL))
#define AGS_COPY_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_COPY_CHANNEL, AgsCopyChannelClass))

typedef struct _AgsCopyChannel AgsCopyChannel;
typedef struct _AgsCopyChannelClass AgsCopyChannelClass;

typedef enum{
  AGS_COPY_CHANNEL_EXACT_LENGTH         = 1,
  AGS_COPY_CHANNEL_OMIT_FURTHER_ATTACK  = 1 << 1,
}AgsCopyChannelFlags;

struct _AgsCopyChannel
{
  AgsRecall recall;

  guint flags;

  AgsDevout *devout;

  AgsChannel *destination;
  gulong destination_recycling_changed_handler;

  AgsChannel *source;
  gulong source_recycling_changed_handler;
};

struct _AgsCopyChannelClass
{
  AgsRecallClass recall;
};

GType ags_copy_channel_get_type();

AgsCopyChannel* ags_copy_channel_new(AgsChannel *destination,
				     AgsChannel *source,
				     AgsDevout *devout);

#endif /*__AGS_COPY_CHANNEL_H__*/
