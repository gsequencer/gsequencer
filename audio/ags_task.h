#ifndef __AGS_TASK_H__
#define __AGS_TASK_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_TASK                (ags_task_get_type())
#define AGS_TASK(obj)                (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_TASK, AgsTask))
#define AGS_TASK_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_TASK, AgsTaskClass))
#define AGS_TASK_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_TASK, AgsTaskClass))

typedef struct _AgsTask AgsTask;
typedef struct _AgsTaskClass AgsTaskClass;

struct _AgsTask
{
  GObject object;

  char *name;
  gpointer data;

  guint start;
};

struct _AgsTaskClass
{
  GObjectClass object;

  void (*launch)(AgsTask *task);
  void (*passed)(AgsTask *task); // a counter step later
};

GType ags_task_get_type();

void ags_task_recall_delay(AgsTask *task);

AgsTask* ags_task_new();

#endif /*__AGS_TASK_H__*/
