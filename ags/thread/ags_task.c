/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ags/thread/ags_task.h>

#include <ags/thread/ags_task_launcher.h>

#include <ags/i18n.h>

void ags_task_class_init(AgsTaskClass *task);
void ags_task_init(AgsTask *task);
void ags_task_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec);
void ags_task_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec);
void ags_task_dispose(GObject *gobject);
void ags_task_finalize(GObject *gobject);

/**
 * SECTION:ags_task
 * @short_description: thread safe task
 * @title: AgsTask
 * @section_id: 
 * @include: ags/thread/ags_task.h
 *
 * #AgsTask object acts an interceptor in a thread safe context.
 */

enum{
  PROP_0,
  PROP_TASK_LAUNCHER,
};

enum{
  LAUNCH,
  FAILURE,
  LAST_SIGNAL,
};

static gpointer ags_task_parent_class = NULL;
static guint task_signals[LAST_SIGNAL];

GType
ags_task_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_task = 0;

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

    ags_type_task = g_type_register_static(G_TYPE_OBJECT,
					   "AgsTask",
					   &ags_task_info,
					   0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_task);
  }

  return g_define_type_id__volatile;
}

void
ags_task_class_init(AgsTaskClass *task)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_task_parent_class = g_type_class_peek_parent(task);

  /* GObjectClass */
  gobject = (GObjectClass *) task;

  gobject->set_property = ags_task_set_property;
  gobject->get_property = ags_task_get_property;

  gobject->dispose = ags_task_dispose;
  gobject->finalize = ags_task_finalize;

  /* properties */
  /**
   * AgsTask:task-launcher:
   *
   * The assigned #AgsTaskLauncher
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("task-launcher",
				   i18n_pspec("the task launcher object"),
				   i18n_pspec("The task launcher object"),
				   AGS_TYPE_TASK_LAUNCHER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TASK_LAUNCHER,
				  param_spec);

  /* AgsTaskClass */
  task->launch = NULL;
  task->failure = NULL;

  /* signals */
  /**
   * AgsTask::launch:
   * @task: the #AgsTask to launch
   *
   * The ::launch signal is emited in a thread safe context
   *
   * Since: 3.0.0
   */
  task_signals[LAUNCH] =
    g_signal_new("launch",
		 G_TYPE_FROM_CLASS (task),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsTaskClass, launch),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsTask::failure:
   * @task: the #AgsTask failed to do its work.
   * @error: the #GError-struct
   *
   * The ::failure signal is emited if ::launch fails
   *
   * Since: 3.0.0
   */
  task_signals[FAILURE] =
    g_signal_new("failure",
		 G_TYPE_FROM_CLASS (task),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsTaskClass, failure),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__POINTER,
		 G_TYPE_NONE, 1,
		 G_TYPE_POINTER);
}

void
ags_task_init(AgsTask *task)
{
  int err;
  
  task->flags = 0;

  /* task mutex */
  g_rec_mutex_init(&(task->obj_mutex));
  
  task->task_name = NULL;

  task->task_launcher = NULL;
}

void
ags_task_set_property(GObject *gobject,
		      guint prop_id,
		      const GValue *value,
		      GParamSpec *param_spec)
{
  AgsTask *task;

  GRecMutex *task_mutex;
  
  task = AGS_TASK(gobject);

  /* get task mutex */
  task_mutex = AGS_TASK_GET_OBJ_MUTEX(task);

  switch(prop_id){
  case PROP_TASK_LAUNCHER:
    {
      AgsTaskLauncher *task_launcher;

      task_launcher = (AgsTaskLauncher *) g_value_get_object(value);

      g_rec_mutex_lock(task_mutex);
      
      if(task->task_launcher == (GObject *) task_launcher){
	g_rec_mutex_unlock(task_mutex);

	return;
      }

      if(task->task_launcher != NULL){
	g_object_unref(G_OBJECT(task->task_launcher));
      }

      if(task_launcher != NULL){
	g_object_ref(G_OBJECT(task_launcher));
      }
      
      task->task_launcher = (GObject *) task_launcher;

      g_rec_mutex_unlock(task_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_task_get_property(GObject *gobject,
		      guint prop_id,
		      GValue *value,
		      GParamSpec *param_spec)
{
  AgsTask *task;

  GRecMutex *task_mutex;

  task = AGS_TASK(gobject);

  /* get task mutex */
  task_mutex = AGS_TASK_GET_OBJ_MUTEX(task);

  switch(prop_id){
  case PROP_TASK_LAUNCHER:
    {
      g_rec_mutex_lock(task_mutex);

      g_value_set_object(value, task->task_launcher);

      g_rec_mutex_unlock(task_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_task_dispose(GObject *gobject)
{
  AgsTask *task;

  task = AGS_TASK(gobject);

  if(task->task_launcher != NULL){
    g_object_unref(task->task_launcher);

    task->task_launcher = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_task_parent_class)->dispose(gobject);
}

void
ags_task_finalize(GObject *gobject)
{
  AgsTask *task;

  task = AGS_TASK(gobject);
  
  g_free(task->task_name);

  if(task->task_launcher != NULL){
    g_object_unref(task->task_launcher);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_task_parent_class)->finalize(gobject);
}

/**
 * ags_task_test_flags:
 * @task: the #AgsTask
 * @flags: the flags
 *
 * Test @flags to be set on @task.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_task_test_flags(AgsTask *task, guint flags)
{
  gboolean retval;  
  
  GRecMutex *task_mutex;

  if(!AGS_IS_TASK(task)){
    return(FALSE);
  }

  /* get task mutex */
  task_mutex = AGS_TASK_GET_OBJ_MUTEX(task);

  /* test */
  g_rec_mutex_lock(task_mutex);

  retval = (flags & (task->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(task_mutex);

  return(retval);
}

/**
 * ags_task_set_flags:
 * @task: the #AgsTask
 * @flags: see enum AgsTaskFlags
 *
 * Enable a feature of #AgsTask.
 *
 * Since: 3.0.0
 */
void
ags_task_set_flags(AgsTask *task, guint flags)
{
  guint task_flags;
  
  GRecMutex *task_mutex;

  if(!AGS_IS_TASK(task)){
    return;
  }

  /* get task mutex */
  task_mutex = AGS_TASK_GET_OBJ_MUTEX(task);

  /* set flags */
  g_rec_mutex_lock(task_mutex);

  task->flags |= flags;
  
  g_rec_mutex_unlock(task_mutex);
}
    
/**
 * ags_task_unset_flags:
 * @task: the #AgsTask
 * @flags: see enum AgsTaskFlags
 *
 * Disable a feature of AgsTask.
 *
 * Since: 3.0.0
 */
void
ags_task_unset_flags(AgsTask *task, guint flags)
{
  guint task_flags;
  
  GRecMutex *task_mutex;

  if(!AGS_IS_TASK(task)){
    return;
  }

  /* get task mutex */
  task_mutex = AGS_TASK_GET_OBJ_MUTEX(task);

  /* unset flags */
  g_rec_mutex_lock(task_mutex);

  task->flags &= (~flags);
  
  g_rec_mutex_unlock(task_mutex);
}

/**
 * ags_task_launch:
 * @task: an #AgsTask
 *
 * Intercept task.
 *
 * Since: 3.0.0
 */
void
ags_task_launch(AgsTask *task)
{
  g_return_if_fail(AGS_IS_TASK(task));

  g_object_ref(G_OBJECT(task));
  g_signal_emit(G_OBJECT(task),
		task_signals[LAUNCH], 0);
  g_object_unref(G_OBJECT(task));
}

/**
 * ags_task_failure:
 * @task: an #AgsTask
 * @error: is %NULL on success
 *
 * Signals failure of task.
 *
 * Since: 3.0.0
 */
void
ags_task_failure(AgsTask *task, GError *error)
{
  g_return_if_fail(AGS_IS_TASK(task));

  g_object_ref(G_OBJECT(task));
  g_signal_emit(G_OBJECT(task),
		task_signals[FAILURE], 0,
		error);
  g_object_unref(G_OBJECT(task));
}

/**
 * ags_task_new:
 *
 * Create a new #AgsTask.
 *
 * Returns: the new #AgsTask
 *
 * Since: 3.0.0
 */
AgsTask*
ags_task_new()
{
  AgsTask *task;

  task = (AgsTask *) g_object_new(AGS_TYPE_TASK,
				  NULL);

  return(task);
}
