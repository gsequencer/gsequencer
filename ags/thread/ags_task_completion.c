/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/thread/ags_task_completion.h>

#include <ags/object/ags_connectable.h>

#include <ags/thread/ags_task.h>

#include <ags/i18n.h>

void ags_task_completion_class_init(AgsTaskCompletionClass *task_completion);
void ags_task_completion_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_task_completion_init(AgsTaskCompletion *task_completion);
void ags_task_completion_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_task_completion_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_task_completion_finalize(GObject *gobject);

AgsUUID* ags_task_completion_get_uuid(AgsConnectable *connectable);
gboolean ags_task_completion_has_resource(AgsConnectable *connectable);
gboolean ags_task_completion_is_ready(AgsConnectable *connectable);
void ags_task_completion_add_to_registry(AgsConnectable *connectable);
void ags_task_completion_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_task_completion_list_resource(AgsConnectable *connectable);
xmlNode* ags_task_completion_xml_compose(AgsConnectable *connectable);
void ags_task_completion_xml_parse(AgsConnectable *connectable,
				   xmlNode *node);
gboolean ags_task_completion_is_connected(AgsConnectable *connectable);
void ags_task_completion_connect(AgsConnectable *connectable);
void ags_task_completion_disconnect(AgsConnectable *connectable);

void ags_task_completion_launch_callback(AgsTask *task,
					 AgsTaskCompletion *task_completion);

/**
 * SECTION:ags_task_completion
 * @short_description: task completion
 * @title: AgsTaskCompletion
 * @section_id:
 * @include: ags/thread/ags_task_completion.h
 *
 * The #AgsTaskCompletion should be used to complete tasks from caller thread.
 */

enum{
  PROP_0,
  PROP_TASK,
};

enum{
  COMPLETE,
  LAST_SIGNAL,
};

static gpointer ags_task_completion_parent_class = NULL;
static guint task_completion_signals[LAST_SIGNAL];

GType
ags_task_completion_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_task_completion = 0;

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
						      "AgsTaskCompletion",
						      &ags_task_completion_info,
						      0);
    
    g_type_add_interface_static(ags_type_task_completion,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_task_completion);
  }

  return(g_define_type_id__static);
}

GType
ags_task_completion_flags_get_type()
{
  static gsize g_flags_type_id__static;

  if(g_once_init_enter(&g_flags_type_id__static)){
    static const GFlagsValue values[] = {
      { AGS_TASK_COMPLETION_QUEUED, "AGS_TASK_COMPLETION_QUEUED", "task-completion-queued" },
      { AGS_TASK_COMPLETION_BUSY, "AGS_TASK_COMPLETION_BUSY", "task-completion-busy" },
      { AGS_TASK_COMPLETION_READY, "AGS_TASK_COMPLETION_READY", "task-completion-ready" },
      { AGS_TASK_COMPLETION_COMPLETED, "AGS_TASK_COMPLETION_COMPLETED", "task-completion-completed" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsTaskCompletionFlags"), values);

    g_once_init_leave(&g_flags_type_id__static, g_flags_type_id);
  }
  
  return(g_flags_type_id__static);
}

void
ags_task_completion_class_init(AgsTaskCompletionClass *task_completion)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;
  
  ags_task_completion_parent_class = g_type_class_peek_parent(task_completion);

  /* GObject */
  gobject = (GObjectClass *) task_completion;

  gobject->set_property = ags_task_completion_set_property;
  gobject->get_property = ags_task_completion_get_property;

  gobject->finalize = ags_task_completion_finalize;

  /* properties */
  /**
   * AgsTaskCompletion:task:
   *
   * The assigned task.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("task",
				   i18n_pspec("assigned task"),
				   i18n_pspec("The task to complete"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TASK,
				  param_spec);

  /* AgsTaskClass */
  task_completion->complete = NULL;

  /* signals */
  /**
   * AgsTaskCompletion::complete:
   * @task_completion: the object to complete.
   *
   * The ::complete signal is emited as a task was finished
   * 
   * Since: 3.0.0
   */
  task_completion_signals[COMPLETE] =
    g_signal_new("complete",
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
  connectable->get_uuid = ags_task_completion_get_uuid;
  connectable->has_resource = ags_task_completion_has_resource;

  connectable->is_ready = ags_task_completion_is_ready;
  connectable->add_to_registry = ags_task_completion_add_to_registry;
  connectable->remove_from_registry = ags_task_completion_remove_from_registry;

  connectable->list_resource = ags_task_completion_list_resource;
  connectable->xml_compose = ags_task_completion_xml_compose;
  connectable->xml_parse = ags_task_completion_xml_parse;

  connectable->is_connected = ags_task_completion_is_connected;
  connectable->connect = ags_task_completion_connect;
  connectable->disconnect = ags_task_completion_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_task_completion_init(AgsTaskCompletion *task_completion)
{
  int err;
  
  task_completion->flags = 0;
  task_completion->connectable_flags = 0;

  /* task completion mutex */
  g_rec_mutex_init(&(task_completion->obj_mutex));

  /* uuid */
  task_completion->uuid = ags_uuid_alloc();
  ags_uuid_generate(task_completion->uuid);

  task_completion->task = NULL;
  task_completion->data = NULL;
}

void
ags_task_completion_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsTaskCompletion *task_completion;

  GRecMutex *task_completion_mutex;

  task_completion = AGS_TASK_COMPLETION(gobject);

  /* get task completion mutex */
  task_completion_mutex = AGS_TASK_COMPLETION_GET_OBJ_MUTEX(task_completion);

  switch(prop_id){
  case PROP_TASK:
    {
      AgsTask *task;
      
      task = g_value_get_object(value);
      
      g_rec_mutex_lock(task_completion_mutex);

      if(task == task_completion->task){      
	g_rec_mutex_unlock(task_completion_mutex);
      
	return;
      }

      if(task_completion->task != NULL){
	g_object_unref(task_completion->task);
      }

      if(task != NULL){
	g_object_ref(task);
      }

      task_completion->task = (GObject *) task;
      
      g_rec_mutex_unlock(task_completion_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_task_completion_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsTaskCompletion *task_completion;

  GRecMutex *task_completion_mutex;

  task_completion = AGS_TASK_COMPLETION(gobject);

  /* get task completion mutex */
  task_completion_mutex = AGS_TASK_COMPLETION_GET_OBJ_MUTEX(task_completion);

  switch(prop_id){
  case PROP_TASK:
    {
      g_rec_mutex_lock(task_completion_mutex);

      g_value_set_object(value, task_completion->task);

      g_rec_mutex_unlock(task_completion_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_task_completion_finalize(GObject *gobject)
{
  AgsTaskCompletion *task_completion;

  task_completion = (AgsTaskCompletion *) gobject;

  
  if(task_completion->task != NULL){
    g_object_unref(task_completion->task);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_task_completion_parent_class)->finalize(gobject);
}

AgsUUID*
ags_task_completion_get_uuid(AgsConnectable *connectable)
{
  AgsTaskCompletion *task_completion;
  
  AgsUUID *ptr;

  GRecMutex *task_completion_mutex;

  task_completion = AGS_TASK_COMPLETION(connectable);

  /* get task_completion mutex */
  task_completion_mutex = AGS_TASK_COMPLETION_GET_OBJ_MUTEX(task_completion);

  /* get UUID */
  g_rec_mutex_lock(task_completion_mutex);

  ptr = task_completion->uuid;

  g_rec_mutex_unlock(task_completion_mutex);
  
  return(ptr);
}

gboolean
ags_task_completion_has_resource(AgsConnectable *connectable)
{
  return(TRUE);
}

gboolean
ags_task_completion_is_ready(AgsConnectable *connectable)
{
  AgsTaskCompletion *task_completion;
  
  gboolean is_ready;

  GRecMutex *task_completion_mutex;

  task_completion = AGS_TASK_COMPLETION(connectable);

  /* get task_completion mutex */
  task_completion_mutex = AGS_TASK_COMPLETION_GET_OBJ_MUTEX(task_completion);

  /* check is added */
  g_rec_mutex_lock(task_completion_mutex);

  is_ready = (((AGS_CONNECTABLE_ADDED_TO_REGISTRY & (task_completion->connectable_flags)) != 0) ? TRUE: FALSE);

  g_rec_mutex_unlock(task_completion_mutex);
  
  return(is_ready);
}

void
ags_task_completion_add_to_registry(AgsConnectable *connectable)
{
  AgsTaskCompletion *task_completion;

  GRecMutex *task_completion_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  task_completion = AGS_TASK_COMPLETION(connectable);

  /* get task completion mutex */
  task_completion_mutex = AGS_TASK_COMPLETION_GET_OBJ_MUTEX(task_completion);

  g_rec_mutex_lock(task_completion_mutex);

  task_completion->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;
  
  g_rec_mutex_unlock(task_completion_mutex);
}

void
ags_task_completion_remove_from_registry(AgsConnectable *connectable)
{
  AgsTaskCompletion *task_completion;

  GRecMutex *task_completion_mutex;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  task_completion = AGS_TASK_COMPLETION(connectable);

  /* get task completion mutex */
  task_completion_mutex = AGS_TASK_COMPLETION_GET_OBJ_MUTEX(task_completion);

  g_rec_mutex_lock(task_completion_mutex);

  task_completion->connectable_flags &= (~AGS_CONNECTABLE_ADDED_TO_REGISTRY);
  
  g_rec_mutex_unlock(task_completion_mutex);
}

xmlNode*
ags_task_completion_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_task_completion_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_task_completion_xml_parse(AgsConnectable *connectable,
			      xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_task_completion_is_connected(AgsConnectable *connectable)
{
  AgsTaskCompletion *task_completion;
  
  gboolean is_connected;

  GRecMutex *task_completion_mutex;

  task_completion = AGS_TASK_COMPLETION(connectable);

  /* get task_completion mutex */
  task_completion_mutex = AGS_TASK_COMPLETION_GET_OBJ_MUTEX(task_completion);

  /* check is connected */
  g_rec_mutex_lock(task_completion_mutex);

  is_connected = (((AGS_CONNECTABLE_CONNECTED & (task_completion->connectable_flags)) != 0) ? TRUE: FALSE);
  
  g_rec_mutex_unlock(task_completion_mutex);
  
  return(is_connected);
}

void
ags_task_completion_connect(AgsConnectable *connectable)
{
  AgsTaskCompletion *task_completion;

  GRecMutex *task_completion_mutex;

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  task_completion = AGS_TASK_COMPLETION(connectable);

  /* get task completion mutex */
  task_completion_mutex = AGS_TASK_COMPLETION_GET_OBJ_MUTEX(task_completion);

  g_rec_mutex_lock(task_completion_mutex);

  task_completion->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_rec_mutex_unlock(task_completion_mutex);

  ags_task_completion_set_flags(task_completion,
				AGS_TASK_COMPLETION_QUEUED);
  
  g_signal_connect_after(task_completion->task, "launch",
			 G_CALLBACK(ags_task_completion_launch_callback), task_completion);
}

void
ags_task_completion_disconnect(AgsConnectable *connectable)
{
  AgsTaskCompletion *task_completion;

  GRecMutex *task_completion_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  task_completion = AGS_TASK_COMPLETION(connectable);

  /* get task completion mutex */
  task_completion_mutex = AGS_TASK_COMPLETION_GET_OBJ_MUTEX(task_completion);

  g_rec_mutex_lock(task_completion_mutex);

  task_completion->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_rec_mutex_unlock(task_completion_mutex);

  g_object_disconnect(task_completion->task,
		      "any_signal::launch",
		      G_CALLBACK(ags_task_completion_launch_callback),
		      task_completion,
		      NULL);
}

/**
 * ags_task_completion_test_flags:
 * @task_completion: the #AgsTaskCompletion
 * @flags: the flags
 *
 * Test @flags to be set on @task_completion.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_task_completion_test_flags(AgsTaskCompletion *task_completion,
			       AgsTaskCompletionFlags flags)
{
  gboolean retval;  
  
  GRecMutex *task_completion_mutex;

  if(!AGS_IS_TASK_COMPLETION(task_completion)){
    return(FALSE);
  }

  /* get task_completion mutex */
  task_completion_mutex = AGS_TASK_COMPLETION_GET_OBJ_MUTEX(task_completion);

  /* test */
  g_rec_mutex_lock(task_completion_mutex);

  retval = (flags & (task_completion->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(task_completion_mutex);

  return(retval);
}

/**
 * ags_task_completion_set_flags:
 * @task_completion: the #AgsTaskCompletion
 * @flags: see enum AgsTaskCompletionFlags
 *
 * Enable a feature of #AgsTaskCompletion.
 *
 * Since: 3.0.0
 */
void
ags_task_completion_set_flags(AgsTaskCompletion *task_completion,
			      AgsTaskCompletionFlags flags)
{
  guint task_completion_flags;
  
  GRecMutex *task_completion_mutex;

  if(!AGS_IS_TASK_COMPLETION(task_completion)){
    return;
  }

  /* get task_completion mutex */
  task_completion_mutex = AGS_TASK_COMPLETION_GET_OBJ_MUTEX(task_completion);

  /* set flags */
  g_rec_mutex_lock(task_completion_mutex);

  task_completion->flags |= flags;
  
  g_rec_mutex_unlock(task_completion_mutex);
}
    
/**
 * ags_task_completion_unset_flags:
 * @task_completion: the #AgsTaskCompletion
 * @flags: see enum AgsTaskCompletionFlags
 *
 * Disable a feature of AgsTaskCompletion.
 *
 * Since: 3.0.0
 */
void
ags_task_completion_unset_flags(AgsTaskCompletion *task_completion,
				AgsTaskCompletionFlags flags)
{
  guint task_completion_flags;
  
  GRecMutex *task_completion_mutex;

  if(!AGS_IS_TASK_COMPLETION(task_completion)){
    return;
  }

  /* get task_completion mutex */
  task_completion_mutex = AGS_TASK_COMPLETION_GET_OBJ_MUTEX(task_completion);

  /* unset flags */
  g_rec_mutex_lock(task_completion_mutex);

  task_completion->flags &= (~flags);
  
  g_rec_mutex_unlock(task_completion_mutex);
}

/**
 * ags_task_completion_complete:
 * @task_completion: the #AgsTaskCompletion
 * 
 * Emit ::complete signal
 * 
 * Since: 3.0.0
 */
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
  ags_task_completion_unset_flags(task_completion,
				  (AGS_TASK_COMPLETION_QUEUED |
				   AGS_TASK_COMPLETION_BUSY));

  ags_task_completion_set_flags(task_completion,
				AGS_TASK_COMPLETION_READY);
}

/**
 * ags_task_completion_new:
 * @task: the #AgsTask
 * @data: pointer to user data
 *
 * Create a new instance of #AgsTaskCompletion.
 *
 * Returns: the new #AgsTaskCompletion
 *
 * Since: 3.0.0
 */
AgsTaskCompletion*
ags_task_completion_new(GObject *task,
			gpointer data)
{
  AgsTaskCompletion *task_completion;
  
  task_completion = (AgsTaskCompletion *) g_object_new(AGS_TYPE_TASK_COMPLETION,
						       "task", task,
						       NULL);
  
  task_completion->data = data;
  
  return(task_completion);
}
