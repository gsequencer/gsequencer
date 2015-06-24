/* This file is part of GSequencer.
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

#include <ags/thread/ags_task_completion.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_task.h>

void ags_task_completion_class_init(AgsTaskCompletionClass *task_completion);
void ags_task_completion_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_task_completion_init(AgsTaskCompletion *task_completion);
void ags_task_completion_connect(AgsConnectable *connectable);
void ags_task_completion_disconnect(AgsConnectable *connectable);
void ags_task_completion_finalize(GObject *gobject);

void ags_task_completion_launch_callback(AgsTask *task,
					 AgsTaskCompletion *task_completion);

/**
 * SECTION:ags_task_completion
 * @short_description: gui thread
 * @title: AgsTaskCompletion
 * @section_id:
 * @include: ags/thread/ags_task_completion.h
 *
 * The #AgsTaskCompletion should be used to complete tasks from caller thread.
 */

enum{
  COMPLETE,
  LAST_SIGNAL,
};

static gpointer ags_task_completion_parent_class = NULL;
static guint task_completion_signals[LAST_SIGNAL];

GType
ags_task_completion_get_type()
{
  static GType ags_type_task_completion = 0;

  if(!ags_type_task_completion){
    static const GTypeInfo ags_task_completion_info = {
      sizeof (AgsTaskCompletionClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_task_completion_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsTaskCompletion),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_task_completion_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_task_completion_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_task_completion = g_type_register_static(G_TYPE_OBJECT,
						      "AgsTaskCompletion\0",
						      &ags_task_completion_info,
						      0);
    
    g_type_add_interface_static(ags_type_task_completion,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_task_completion);
}

void
ags_task_completion_class_init(AgsTaskCompletionClass *task_completion)
{
  GObjectClass *gobject;

  ags_task_completion_parent_class = g_type_class_peek_parent(task_completion);

  /* GObject */
  gobject = (GObjectClass *) task_completion;

  gobject->finalize = ags_task_completion_finalize;

  /* AgsTaskClass */
  task_completion->complete = NULL;

  /* signals */
  /**
   * AgsTaskCompletion::complete:
   * @task_completion: the object to complete.
   *
   * The ::complete signal is emited as a task was finished
   */
  task_completion_signals[COMPLETE] =
    g_signal_new("complete\0",
		 G_TYPE_FROM_CLASS(task_completion),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsTaskCompletionClass, complete),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_task_completion_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_task_completion_connect;
  connectable->disconnect = ags_task_completion_disconnect;
}

void
ags_task_completion_init(AgsTaskCompletion *task_completion)
{
  g_atomic_int_set(&(task_completion->flags),
		   0);
  
  task_completion->task = NULL;
  task_completion->data = NULL;
}

void
ags_task_completion_connect(AgsConnectable *connectable)
{
  AgsTaskCompletion *task_completion;

  task_completion = AGS_TASK_COMPLETION(connectable);

  g_atomic_int_or(&(task_completion->flags),
		  AGS_TASK_COMPLETION_QUEUED);

  g_signal_connect_after(task_completion->task, "launch\0",
			 G_CALLBACK(ags_task_completion_launch_callback), task_completion);
}

void
ags_task_completion_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_task_completion_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_task_completion_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_task_completion_complete(AgsTaskCompletion *task_completion)
{
  g_return_if_fail(AGS_IS_TASK_COMPLETION(task_completion));

  g_object_ref(G_OBJECT(task_completion));
  g_signal_emit(G_OBJECT(task_completion),
		task_completion_signals[COMPLETE], 0);
  g_object_unref(G_OBJECT(task_completion));
}

void
ags_task_completion_launch_callback(AgsTask *task,
				    AgsTaskCompletion *task_completion)
{
  g_atomic_int_and(&(task_completion->flags),
		   (~(AGS_TASK_COMPLETION_QUEUED | AGS_TASK_COMPLETION_BUSY)));

  g_atomic_int_or(&(task_completion->flags),
		  AGS_TASK_COMPLETION_READY);
}

/**
 * ags_task_completion_new:
 * @task: the #AgsTask
 * @data: pointer to user data
 *
 * Create a new #AgsTaskCompletion.
 *
 * Returns: the new #AgsTaskCompletion
 *
 * Since: 0.4
 */
AgsTaskCompletion*
ags_task_completion_new(GObject *task,
			gpointer data)
{
  AgsTaskCompletion *task_completion;
  
  task_completion = (AgsTaskCompletion *) g_object_new(AGS_TYPE_TASK_COMPLETION,
						       NULL);
  task_completion->task = task;
  g_object_ref(task);
  
  task_completion->data = data;
  
  return(task_completion);
}
