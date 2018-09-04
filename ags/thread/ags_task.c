/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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
#include <ags/thread/ags_task_thread.h>

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
void ags_task_finalize(GObject *gobject);

/**
 * SECTION:ags_task
 * @short_description: Perform operations in a thread safe context.
 * @title: AgsTask
 * @section_id: 
 * @include: ags/thread/ags_task.h
 *
 * #AgsTask object acts an interceptor in a thread safe context.
 */

enum{
  PROP_0,
  PROP_TASK_THREAD,
};

enum{
  LAUNCH,
  FAILURE,
  LAST_SIGNAL,
};

static gpointer ags_task_parent_class = NULL;
static guint task_signals[LAST_SIGNAL];

static pthread_mutex_t ags_task_class_mutex = PTHREAD_MUTEX_INITIALIZER;

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

  gobject->finalize = ags_task_finalize;

  /* properties */
  /**
   * AgsTask:task-thread:
   *
   * The assigned #AgsTaskThread
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("task-thread",
				   i18n_pspec("the task thread object"),
				   i18n_pspec("The task thread object"),
				   AGS_TYPE_TASK_THREAD,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TASK_THREAD,
				  param_spec);

  /* AgsTaskClass */
  task->launch = NULL;
  task->failure = NULL;

  /* signals */
  /**
   * AgsTask::launch:
   * @task: the #AgsTask to launch.
   *
   * The ::launch signal is emited in a thread safe context
   *
   * Since: 2.0.0
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
   * Since: 2.0.0
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
  task->obj_mutexattr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));

  pthread_mutexattr_init(task->obj_mutexattr);
  pthread_mutexattr_settype(task->obj_mutexattr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  err = pthread_mutexattr_setprotocol(task->obj_mutexattr,
				      PTHREAD_PRIO_INHERIT);

  if(err != 0){
    g_warning("no priority inheritance");
  }
#endif
  
  task->obj_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(task->obj_mutex,
		     task->obj_mutexattr);
  
  task->name = NULL;

  task->delay = 0;

  pthread_cond_init(&(task->wait_sync_task_cond), NULL);

  task->task_thread = NULL;
}

void
ags_task_set_property(GObject *gobject,
		      guint prop_id,
		      const GValue *value,
		      GParamSpec *param_spec)
{
  AgsTask *task;

  task = AGS_TASK(gobject);

  switch(prop_id){
  case PROP_TASK_THREAD:
    {
      AgsTaskThread *task_thread;

      task_thread = (AgsTaskThread *) g_value_get_object(value);

      if(task->task_thread == (GObject *) task_thread){
	return;
      }

      if(task->task_thread != NULL){
	g_object_unref(G_OBJECT(task->task_thread));
      }

      if(task_thread != NULL){
	g_object_ref(G_OBJECT(task_thread));
      }
      
      task->task_thread = (GObject *) task_thread;
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

  task = AGS_TASK(gobject);

  switch(prop_id){
  case PROP_TASK_THREAD:
    {
      g_value_set_object(value, task->task_thread);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_task_finalize(GObject *gobject)
{
  AgsTask *task;

  task = AGS_TASK(gobject);

  if(task->name != NULL){
    g_free(task->name);
  }

  pthread_cond_destroy(&(task->wait_sync_task_cond));
  
  /* task mutex */
  pthread_mutexattr_destroy(task->obj_mutexattr);
  free(task->obj_mutexattr);

  pthread_mutex_destroy(task->obj_mutex);
  free(task->obj_mutex);

  /* call parent */
  G_OBJECT_CLASS(ags_task_parent_class)->finalize(gobject);
}

/**
 * ags_task_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_task_get_class_mutex()
{
  return(&ags_task_class_mutex);
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
 * Since: 2.0.0
 */
gboolean
ags_task_test_flags(AgsTask *task, guint flags)
{
  gboolean retval;  
  
  pthread_mutex_t *task_mutex;

  if(!AGS_IS_TASK(task)){
    return(FALSE);
  }

  /* get task mutex */
  pthread_mutex_lock(ags_task_get_class_mutex());
  
  task_mutex = task->obj_mutex;
  
  pthread_mutex_unlock(ags_task_get_class_mutex());

  /* test */
  pthread_mutex_lock(task_mutex);

  retval = (flags & (task->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(task_mutex);

  return(retval);
}

/**
 * ags_task_set_flags:
 * @task: the #AgsTask
 * @flags: see enum AgsTaskFlags
 *
 * Enable a feature of #AgsTask.
 *
 * Since: 2.0.0
 */
void
ags_task_set_flags(AgsTask *task, guint flags)
{
  guint task_flags;
  
  pthread_mutex_t *task_mutex;

  if(!AGS_IS_TASK(task)){
    return;
  }

  /* get task mutex */
  pthread_mutex_lock(ags_task_get_class_mutex());
  
  task_mutex = task->obj_mutex;
  
  pthread_mutex_unlock(ags_task_get_class_mutex());

  /* set flags */
  pthread_mutex_lock(task_mutex);

  task->flags |= flags;
  
  pthread_mutex_unlock(task_mutex);
}
    
/**
 * ags_task_unset_flags:
 * @task: the #AgsTask
 * @flags: see enum AgsTaskFlags
 *
 * Disable a feature of AgsTask.
 *
 * Since: 2.0.0
 */
void
ags_task_unset_flags(AgsTask *task, guint flags)
{
  guint task_flags;
  
  pthread_mutex_t *task_mutex;

  if(!AGS_IS_TASK(task)){
    return;
  }

  /* get task mutex */
  pthread_mutex_lock(ags_task_get_class_mutex());
  
  task_mutex = task->obj_mutex;
  
  pthread_mutex_unlock(ags_task_get_class_mutex());

  /* unset flags */
  pthread_mutex_lock(task_mutex);

  task->flags &= (~flags);
  
  pthread_mutex_unlock(task_mutex);
}

/**
 * ags_task_launch:
 * @task: an #AgsTask
 *
 * Intercept task.
 *
 * Since: 2.0.0
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
 * Since: 2.0.0
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
 * Creates a #AgsTask
 *
 * Returns: a new #AgsTask
 *
 * Since: 2.0.0
 */
AgsTask*
ags_task_new()
{
  AgsTask *task;

  task = (AgsTask *) g_object_new(AGS_TYPE_TASK,
				  NULL);

  return(task);
}
