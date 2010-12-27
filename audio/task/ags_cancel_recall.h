#ifndef __AGS_CANCEL_RECALL_H__
#define __AGS_CANCEL_RECALL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_task.h>
#include <ags/audio/ags_recall.h>

#define AGS_TYPE_CANCEL_RECALL                (ags_cancel_recall_get_type())
#define AGS_CANCEL_RECALL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CANCEL_RECALL, AgsCancelRecall))
#define AGS_CANCEL_RECALL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CANCEL_RECALL, AgsCancelRecallClass))
#define AGS_IS_CANCEL_RECALL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_CANCEL_RECALL))
#define AGS_IS_CANCEL_RECALL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_CANCEL_RECALL))
#define AGS_CANCEL_RECALL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_CANCEL_RECALL, AgsCancelRecallClass))

typedef struct _AgsCancelRecall AgsCancelRecall;
typedef struct _AgsCancelRecallClass AgsCancelRecallClass;

struct _AgsCancelRecall
{
  AgsTask task;

  AgsRecall *recall;
  guint audio_channel;
};

struct _AgsCancelRecallClass
{
  AgsTaskClass task;
};

GType ags_cancel_recall_get_type();

AgsCancelRecall* ags_cancel_recall_new(AgsRecall *recall, guint audio_channel);

#endif /*__AGS_CANCEL_RECALL_H__*/
