/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/object/ags_connectable.h>

#include <ags/i18n.h>

void ags_task_class_init(AgsTaskClass *task);
void ags_task_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_task_init(AgsTask *task);
void ags_task_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec);
void ags_task_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec);
void ags_task_connect(AgsConnectable *connectable);
void ags_task_disconnect(AgsConnectable *connectable);
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

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_task_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_task = g_type_register_static(G_TYPE_OBJECT,
					   "AgsTask",
					   &ags_task_info,
					   0);

    g_type_add_interface_static(ags_type_task,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_task);
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
   * Since: 1.0.0
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
   * Since: 1.0.0
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
   * Since: 1.0.0
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
ags_task_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_task_connect;
  connectable->disconnect = ags_task_disconnect;
}

void
ags_task_init(AgsTask *task)
{
  task->flags = 0;

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
ags_task_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_task_disconnect(AgsConnectable *connectable)
{
  /* empty */
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

  G_OBJECT_CLASS(ags_task_parent_class)->finalize(gobject);
}

/**
 * ags_task_launch:
 * @task: an #AgsTask
 *
 * Intercept task.
 *
 * Since: 1.0.0
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
 * Since: 1.0.0
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
 * Since: 1.0.0
 */
AgsTask*
ags_task_new()
{
  AgsTask *task;

  task = (AgsTask *) g_object_new(AGS_TYPE_TASK,
				  NULL);

  return(task);
}
