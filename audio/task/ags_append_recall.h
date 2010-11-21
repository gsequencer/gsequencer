#ifndef __AGS_APPEND_RECALL_H__
#define __AGS_APPEND_RECALL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_task.h>

#include <ags/audio/ags_devout.h>

#define AGS_TYPE_APPEND_RECALL                (ags_append_recall_get_type())
#define AGS_APPEND_RECALL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_APPEND_RECALL, AgsAppendRecall))
#define AGS_APPEND_RECALL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_APPEND_RECALL, AgsAppendRecallClass))
#define AGS_IS_APPEND_RECALL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_APPEND_RECALL))
#define AGS_IS_APPEND_RECALL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_APPEND_RECALL))
#define AGS_APPEND_RECALL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_APPEND_RECALL, AgsAppendRecallClass))

typedef struct _AgsAppendRecall AgsAppendRecall;
typedef struct _AgsAppendRecallClass AgsAppendRecallClass;

struct _AgsAppendRecall
{
  AgsTask task;

  AgsDevout *devout;
  AgsDevoutPlay *devout_play;
};

struct _AgsAppendRecallClass
{
  AgsTaskClass task;
};

GType ags_append_recall_get_type();

AgsAppendRecall* ags_append_recall_new(AgsDevout *devout,
				       AgsDevoutPlay *devout_play);

#endif /*__AGS_APPEND_RECALL_H__*/
