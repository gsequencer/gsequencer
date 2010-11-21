#ifndef __AGS_RECALL_CHANNEL_RUN_H__
#define __AGS_RECALL_CHANNEL_RUN_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall.h>

#define AGS_TYPE_RECALL_CHANNEL_RUN                (ags_recall_channel_run_get_type())
#define AGS_RECALL_CHANNEL_RUN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RECALL_CHANNEL_RUN, AgsRecallChannelRun))
#define AGS_RECALL_CHANNEL_RUN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RECALL_CHANNEL_RUN, AgsRecallChannelRunClass))
#define AGS_IS_RECALL_CHANNEL_RUN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_RECALL_CHANNEL_RUN))
#define AGS_IS_RECALL_CHANNEL_RUN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_RECALL_CHANNEL_RUN))
#define AGS_RECALL_CHANNEL_RUN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_RECALL_CHANNEL_RUN, AgsRecallChannelRunClass))

typedef struct _AgsRecallChannelRun AgsRecallChannelRun;
typedef struct _AgsRecallChannelRunClass AgsRecallChannelRunClass;

struct _AgsRecallChannelRun
{
  AgsRecall recall;
};

struct _AgsRecallChannelRunClass
{
  AgsRecallClass recall;
};

GType ags_recall_channel_run_get_type();

AgsRecallChannelRun* ags_recall_channel_run_new();

#endif /*__AGS_RECALL_CHANNEL_RUN_H__*/
