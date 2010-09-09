#include "ags_task.h"

GType ags_task_get_type();
void ags_task_class_init(AgsTaskClass *task);
void ags_task_init(AgsTask *task);
void ags_task_destroy(GObject *task);

void ags_task_launch(AgsTask *task);
void ags_task_passed(AgsTask *task);

GType
ags_task_get_type()
{
  static GType ags_type_task = 0;

  if(!ags_type_task){
    static const GTypeInfo ags_task_info = {
      sizeof (AgsTaskClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_task_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsTask),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_task_init,
    };
    ags_type_task = g_type_register_static(G_TYPE_OBJECT, "AgsTask\0", &ags_task_info, 0);
  }
  return (ags_type_task);
}

void
ags_task_class_init(AgsTaskClass *task)
{
  task->launch = ags_task_launch;
  task->passed = ags_task_passed;
}

void
ags_task_init(AgsTask *task)
{
  task->name = NULL;
  task->data = NULL;

  task->start = 0;
}

void
ags_task_destroy(GObject *task)
{
  g_object_unref(task);
}

void
ags_task_launch(AgsTask *task)
{
}

void
ags_task_passed(AgsTask *task)
{
}

void
ags_task_recall_delay(AgsTask *task)
{
}

void
ags_task_copy_input_to_output(AgsTask *task)
{
}

void
ags_task_copy_pattern(AgsTask *task)
{
}

AgsTask*
ags_task_new()
{
  AgsTask *task;

  task = (AgsTask *) g_object_new(AGS_TYPE_TASK, NULL);

  return(task);
}
