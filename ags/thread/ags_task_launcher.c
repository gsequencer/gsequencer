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

#include <ags/thread/ags_task_launcher.h>

#include <ags/object/ags_connectable.h>

#include <ags/i18n.h>

void ags_task_launcher_class_init(AgsTaskLauncherClass *task_launcher);
void ags_task_launcher_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_task_launcher_init(AgsTaskLauncher *task_launcher);
void ags_task_launcher_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_task_launcher_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_task_launcher_dispose(GObject *gobject);
void ags_task_launcher_finalize(GObject *gobject);

AgsUUID* ags_task_launcher_get_uuid(AgsConnectable *connectable);
gboolean ags_task_launcher_has_resource(AgsConnectable *connectable);
gboolean ags_task_launcher_is_ready(AgsConnectable *connectable);
void ags_task_launcher_add_to_registry(AgsConnectable *connectable);
void ags_task_launcher_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_task_launcher_list_resource(AgsConnectable *connectable);
xmlNode* ags_task_launcher_xml_compose(AgsConnectable *connectable);
void ags_task_launcher_xml_parse(AgsConnectable *connectable,
				 xmlNode *node);
gboolean ags_task_launcher_is_connected(AgsConnectable *connectable);
void ags_task_launcher_connect(AgsConnectable *connectable);
void ags_task_launcher_disconnect(AgsConnectable *connectable);

void ags_task_launcher_real_run(AgsTaskLauncher *task_launcher);

gboolean ags_task_launcher_source_func(AgsTaskLauncher *task_launcher);

/**
 * SECTION:ags_task_launcher
 * @short_description: task launcher
 * @title: AgsTaskLauncher
 * @section_id:
 * @include: ags/thread/ags_task_launcher.h
 *
 * The #AgsTaskLauncher acts as task launcher.
 */

enum{
  RUN,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_TASK,
  PROP_CYCLIC_TASK,
};

static gpointer ags_task_launcher_parent_class = NULL;
static guint task_launcher_signals[LAST_SIGNAL];

GType
ags_task_launcher_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_task_launcher = 0;

    static const GTypeInfo ags_task_launcher_info = {
      sizeof (AgsTaskLauncherClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_task_launcher_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsTaskLauncher),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_task_launcher_init,
    };
    
    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_task_launcher_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_task_launcher = g_type_register_static(G_TYPE_OBJECT,
						    "AgsTaskLauncher",
						    &ags_task_launcher_info,
						    0);
    
    g_type_add_interface_static(ags_type_task_launcher,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_task_launcher);
  }

  return g_define_type_id__volatile;
}

void
ags_task_launcher_class_init(AgsTaskLauncherClass *task_launcher)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_task_launcher_parent_class = g_type_class_peek_parent(task_launcher);

  /* GObject */
  gobject = (GObjectClass *) task_launcher;

  gobject->set_property = ags_task_launcher_set_property;
  gobject->get_property = ags_task_launcher_get_property;

  gobject->dispose = ags_task_launcher_dispose;
  gobject->finalize = ags_task_launcher_finalize;

  /* properties */
  /**
   * AgsTaskLauncher:task:
   *
   * The assigned #AgsTask to launch.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("task",
				    i18n_pspec("assigned tasks"),
				    i18n_pspec("The tasks assigned with"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TASK,
				  param_spec);

  /**
   * AgsTaskLauncher:cyclic-task:
   *
   * The assigned cyclic #AgsTask to launch.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("cyclic-task",
				    i18n_pspec("assigned cyclic tasks"),
				    i18n_pspec("The cyclic tasks assigned with"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CYCLIC_TASK,
				  param_spec);

  /* AgsTaskLauncher */
  task_launcher->run = ags_task_launcher_real_run;

  /* signals */
  /**
   * AgsTaskLauncher::run:
   * @task_launcher: the #AgsTaskLauncher
   *
   * The ::run signal is invoked to run #AgsTaskLauncher:task
   *
   * Since: 3.0.0
   */
  task_launcher_signals[RUN] =
    g_signal_new("run",
		 G_TYPE_FROM_CLASS (task_launcher),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsTaskLauncherClass, run),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_task_launcher_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_task_launcher_get_uuid;
  connectable->has_resource = ags_task_launcher_has_resource;

  connectable->is_ready = ags_task_launcher_is_ready;
  connectable->add_to_registry = ags_task_launcher_add_to_registry;
  connectable->remove_from_registry = ags_task_launcher_remove_from_registry;

  connectable->list_resource = ags_task_launcher_list_resource;
  connectable->xml_compose = ags_task_launcher_xml_compose;
  connectable->xml_parse = ags_task_launcher_xml_parse;

  connectable->is_connected = ags_task_launcher_is_connected;
  connectable->connect = ags_task_launcher_connect;
  connectable->disconnect = ags_task_launcher_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_task_launcher_init(AgsTaskLauncher *task_launcher)
{
  task_launcher->flags = 0;

  /* the obj mutex */
  g_rec_mutex_init(&(task_launcher->obj_mutex));

  /* uuid */
  task_launcher->uuid = ags_uuid_alloc();
  ags_uuid_generate(task_launcher->uuid);

  task_launcher->main_context = NULL;

  task_launcher->task = NULL;
  task_launcher->cyclic_task = NULL;

  /* wait */
  g_atomic_int_set(&(task_launcher->is_running),
		   FALSE);
  g_atomic_int_set(&(task_launcher->wait_count),
		   0);
  
  g_mutex_init(&(task_launcher->wait_mutex));
  g_cond_init(&(task_launcher->wait_cond));
}

void
ags_task_launcher_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsTaskLauncher *task_launcher;
  
  GRecMutex *task_launcher_mutex;

  task_launcher = AGS_TASK_LAUNCHER(gobject);

  /* get task launcher mutex */
  task_launcher_mutex = AGS_TASK_LAUNCHER_GET_OBJ_MUTEX(task_launcher);

  switch(prop_id){
  case PROP_TASK:
  {
    AgsTask *task;

    task = (AgsTask *) g_value_get_pointer(value);

    ags_task_launcher_add_task(task_launcher,
			       task);
  }
  break;
  case PROP_CYCLIC_TASK:
  {
    AgsTask *cyclic_task;

    cyclic_task = (AgsTask *) g_value_get_pointer(value);

    ags_task_launcher_add_cyclic_task(task_launcher,
				      cyclic_task);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_task_launcher_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsTaskLauncher *task_launcher;

  GRecMutex *task_launcher_mutex;

  task_launcher = AGS_TASK_LAUNCHER(gobject);

  /* get task launcher mutex */
  task_launcher_mutex = AGS_TASK_LAUNCHER_GET_OBJ_MUTEX(task_launcher);

  switch(prop_id){
  case PROP_TASK:
  {
    g_rec_mutex_lock(task_launcher_mutex);

    g_value_set_pointer(value, g_list_copy_deep(task_launcher->task,
						(GCopyFunc) g_object_ref,
						NULL));

    g_rec_mutex_unlock(task_launcher_mutex);
  }
  break;
  case PROP_CYCLIC_TASK:
  {
    g_rec_mutex_lock(task_launcher_mutex);

    g_value_set_pointer(value, g_list_copy_deep(task_launcher->cyclic_task,
						(GCopyFunc) g_object_ref,
						NULL));

    g_rec_mutex_unlock(task_launcher_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_task_launcher_dispose(GObject *gobject)
{
  AgsTaskLauncher *task_launcher;

  task_launcher = AGS_TASK_LAUNCHER(gobject);

  if(task_launcher->main_context != NULL){
    g_main_context_unref(task_launcher->main_context);
    
    task_launcher->main_context = NULL;
  }
  
  if(task_launcher->task != NULL){
    g_list_free_full(task_launcher->task,
		     g_object_unref);
    
    task_launcher->task = NULL;
  }
  
  if(task_launcher->cyclic_task != NULL){
    g_list_free_full(task_launcher->cyclic_task,
		     g_object_unref);
    
    task_launcher->cyclic_task = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_task_launcher_parent_class)->dispose(gobject);
}

void
ags_task_launcher_finalize(GObject *gobject)
{
  AgsTaskLauncher *task_launcher;

  task_launcher = AGS_TASK_LAUNCHER(gobject);
  
  /* UUID */
  ags_uuid_free(task_launcher->uuid);

  if(task_launcher->main_context != NULL){
    g_main_context_unref(task_launcher->main_context);
  }

  g_list_free_full(task_launcher->task,
		   g_object_unref);

  g_list_free_full(task_launcher->cyclic_task,
		   g_object_unref);
  
  /* call parent */
  G_OBJECT_CLASS(ags_task_launcher_parent_class)->finalize(gobject);
}

AgsUUID*
ags_task_launcher_get_uuid(AgsConnectable *connectable)
{
  AgsTaskLauncher *task_launcher;
  
  AgsUUID *ptr;

  GRecMutex *task_launcher_mutex;

  task_launcher = AGS_TASK_LAUNCHER(connectable);

  /* get task launcher mutex */
  task_launcher_mutex = AGS_TASK_LAUNCHER_GET_OBJ_MUTEX(task_launcher);

  /* get UUID */
  g_rec_mutex_lock(task_launcher_mutex);

  ptr = task_launcher->uuid;

  g_rec_mutex_unlock(task_launcher_mutex);
  
  return(ptr);
}

gboolean
ags_task_launcher_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_task_launcher_is_ready(AgsConnectable *connectable)
{
  AgsTaskLauncher *task_launcher;
  
  gboolean is_ready;

  task_launcher = AGS_TASK_LAUNCHER(connectable);

  /* check is added */
  is_ready = ags_task_launcher_test_flags(task_launcher, AGS_TASK_LAUNCHER_ADDED_TO_REGISTRY);
  
  return(is_ready);
}

void
ags_task_launcher_add_to_registry(AgsConnectable *connectable)
{
  AgsTaskLauncher *task_launcher;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  task_launcher = AGS_TASK_LAUNCHER(connectable);

  ags_task_launcher_set_flags(task_launcher, AGS_TASK_LAUNCHER_ADDED_TO_REGISTRY);
}

void
ags_task_launcher_remove_from_registry(AgsConnectable *connectable)
{
  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  //TODO:JK: implement me
}

xmlNode*
ags_task_launcher_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_task_launcher_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_task_launcher_xml_parse(AgsConnectable *connectable,
			    xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_task_launcher_is_connected(AgsConnectable *connectable)
{
  AgsTaskLauncher *task_launcher;
  
  gboolean is_connected;

  task_launcher = AGS_TASK_LAUNCHER(connectable);

  /* check is connected */
  is_connected = ags_task_launcher_test_flags(task_launcher, AGS_TASK_LAUNCHER_CONNECTED);
  
  return(is_connected);
}

void
ags_task_launcher_connect(AgsConnectable *connectable)
{
  AgsTaskLauncher *task_launcher;

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  task_launcher = AGS_TASK_LAUNCHER(connectable);

  ags_task_launcher_set_flags(task_launcher, AGS_TASK_LAUNCHER_CONNECTED);
}

void
ags_task_launcher_disconnect(AgsConnectable *connectable)
{
  AgsTaskLauncher *task_launcher;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  task_launcher = AGS_TASK_LAUNCHER(connectable);

  ags_task_launcher_unset_flags(task_launcher, AGS_TASK_LAUNCHER_CONNECTED);
}

/**
 * ags_task_launcher_test_flags:
 * @task_launcher: the #AgsTaskLauncher
 * @flags: the flags
 *
 * Test @flags to be set on @task_launcher.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_task_launcher_test_flags(AgsTaskLauncher *task_launcher, guint flags)
{
  gboolean retval;  
  
  GRecMutex *task_launcher_mutex;

  if(!AGS_IS_TASK_LAUNCHER(task_launcher)){
    return(FALSE);
  }

  /* get task_launcher mutex */
  task_launcher_mutex = AGS_TASK_LAUNCHER_GET_OBJ_MUTEX(task_launcher);

  /* test */
  g_rec_mutex_lock(task_launcher_mutex);

  retval = (flags & (task_launcher->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(task_launcher_mutex);

  return(retval);
}

/**
 * ags_task_launcher_set_flags:
 * @task_launcher: the #AgsTaskLauncher
 * @flags: see enum AgsTaskLauncherFlags
 *
 * Enable a feature of #AgsTaskLauncher.
 *
 * Since: 3.0.0
 */
void
ags_task_launcher_set_flags(AgsTaskLauncher *task_launcher, guint flags)
{
  guint task_launcher_flags;
  
  GRecMutex *task_launcher_mutex;

  if(!AGS_IS_TASK_LAUNCHER(task_launcher)){
    return;
  }

  /* get task_launcher mutex */
  task_launcher_mutex = AGS_TASK_LAUNCHER_GET_OBJ_MUTEX(task_launcher);

  /* set flags */
  g_rec_mutex_lock(task_launcher_mutex);

  task_launcher->flags |= flags;
  
  g_rec_mutex_unlock(task_launcher_mutex);
}
    
/**
 * ags_task_launcher_unset_flags:
 * @task_launcher: the #AgsTaskLauncher
 * @flags: see enum AgsTaskLauncherFlags
 *
 * Disable a feature of AgsTaskLauncher.
 *
 * Since: 3.0.0
 */
void
ags_task_launcher_unset_flags(AgsTaskLauncher *task_launcher, guint flags)
{
  guint task_launcher_flags;
  
  GRecMutex *task_launcher_mutex;

  if(!AGS_IS_TASK_LAUNCHER(task_launcher)){
    return;
  }

  /* get task_launcher mutex */
  task_launcher_mutex = AGS_TASK_LAUNCHER_GET_OBJ_MUTEX(task_launcher);

  /* unset flags */
  g_rec_mutex_lock(task_launcher_mutex);

  task_launcher->flags &= (~flags);
  
  g_rec_mutex_unlock(task_launcher_mutex);
}

/**
 * ags_task_launcher_attach:
 * @task_launcher: the #AgsTaskLauncher
 * @main_context: the #GMainContext-struct
 * 
 * Attach @task_launcher to @main_context.
 * 
 * Since: 3.0.0
 */
void
ags_task_launcher_attach(AgsTaskLauncher *task_launcher,
			 GMainContext *main_context)
{
  GRecMutex *task_launcher_mutex;

  if(!AGS_IS_TASK_LAUNCHER(task_launcher) ||
     main_context == NULL){
    return;
  }

  /* get task launcher mutex */
  task_launcher_mutex = AGS_TASK_LAUNCHER_GET_OBJ_MUTEX(task_launcher);

  g_rec_mutex_lock(task_launcher_mutex);

  task_launcher->main_context = main_context;
  g_main_context_ref(main_context);
  
  g_rec_mutex_unlock(task_launcher_mutex);  
}

/**
 * ags_task_launcher_add_task:
 * @task_launcher: the #AgsTaskLauncher
 * @task: the #AgsTask
 * 
 * Add @task to @task_launcher.
 * 
 * Since: 3.0.0
 */
void
ags_task_launcher_add_task(AgsTaskLauncher *task_launcher,
			   AgsTask *task)
{
  GRecMutex *task_launcher_mutex;

  if(!AGS_IS_TASK_LAUNCHER(task_launcher) ||
     !AGS_IS_TASK(task)){
    return;
  }

  /* get task launcher mutex */
  task_launcher_mutex = AGS_TASK_LAUNCHER_GET_OBJ_MUTEX(task_launcher);

  g_rec_mutex_lock(task_launcher_mutex);

  task_launcher->task = g_list_prepend(task_launcher->task,
				       task);
  g_object_ref(task);
  
  g_rec_mutex_unlock(task_launcher_mutex);
}

/**
 * ags_task_launcher_add_task_all:
 * @task_launcher: the #AgsTaskLauncher
 * @list: (element-type Ags.Task) (transfer none): the #GList-struct containing #AgsTask
 * 
 * Add all @list to @task_launcher.
 * 
 * Since: 3.0.0
 */
void
ags_task_launcher_add_task_all(AgsTaskLauncher *task_launcher,
			       GList *list)
{
  GRecMutex *task_launcher_mutex;

  if(!AGS_IS_TASK_LAUNCHER(task_launcher) ||
     list == NULL){
    return;
  }

  /* get task launcher mutex */
  task_launcher_mutex = AGS_TASK_LAUNCHER_GET_OBJ_MUTEX(task_launcher);

  g_rec_mutex_lock(task_launcher_mutex);

  if(task_launcher->task != NULL){
    task_launcher->task = g_list_concat(task_launcher->task,
					g_list_copy_deep(list,
							 (GCopyFunc) g_object_ref,
							 NULL));
  }else{
    task_launcher->task = g_list_copy_deep(list,
					   (GCopyFunc) g_object_ref,
					   NULL);
  }
  
  g_rec_mutex_unlock(task_launcher_mutex);
}

/**
 * ags_task_launcher_add_cyclic_task:
 * @task_launcher: the #AgsTaskLauncher
 * @cyclic_task: the cyclic #AgsTask
 * 
 * Add @cyclic_task to @task_launcher.
 * 
 * Since: 3.0.0
 */
void
ags_task_launcher_add_cyclic_task(AgsTaskLauncher *task_launcher,
				  AgsTask *cyclic_task)
{
  GRecMutex *task_launcher_mutex;

  if(!AGS_IS_TASK_LAUNCHER(task_launcher) ||
     !AGS_IS_TASK(cyclic_task)){
    return;
  }

  /* get task launcher mutex */
  task_launcher_mutex = AGS_TASK_LAUNCHER_GET_OBJ_MUTEX(task_launcher);

  g_rec_mutex_lock(task_launcher_mutex);

  task_launcher->cyclic_task = g_list_prepend(task_launcher->cyclic_task,
					      cyclic_task);
  g_object_ref(cyclic_task);
  
  g_rec_mutex_unlock(task_launcher_mutex);
}

/**
 * ags_task_launcher_remove_cyclic_task:
 * @task_launcher: the #AgsTaskLauncher
 * @cyclic_task: the cyclic #AgsTask
 * 
 * Remove @cyclic_task from @task_launcher.
 * 
 * Since: 3.0.0
 */
void
ags_task_launcher_remove_cyclic_task(AgsTaskLauncher *task_launcher,
				     AgsTask *cyclic_task)
{
  GRecMutex *task_launcher_mutex;

  if(!AGS_IS_TASK_LAUNCHER(task_launcher)){
    return;
  }

  /* get task launcher mutex */
  task_launcher_mutex = AGS_TASK_LAUNCHER_GET_OBJ_MUTEX(task_launcher);

  g_rec_mutex_lock(task_launcher_mutex);

  if(g_list_find(task_launcher->task, cyclic_task) != NULL){
    task_launcher->cyclic_task = g_list_remove(task_launcher->cyclic_task,
					       cyclic_task);
    g_object_unref(cyclic_task);
  }
  
  g_rec_mutex_unlock(task_launcher_mutex);
}

void
ags_task_launcher_real_run(AgsTaskLauncher *task_launcher)
{
  GList *start_task, *task;
  GList *start_cyclic_task, *cyclic_task;

  GRecMutex *task_launcher_mutex;

  /* get task launcher mutex */
  task_launcher_mutex = AGS_TASK_LAUNCHER_GET_OBJ_MUTEX(task_launcher);

  g_rec_mutex_lock(task_launcher_mutex);

  g_object_get(task_launcher,
	       "task", &start_task,
	       "cyclic-task", &start_cyclic_task,
	       NULL);

  g_list_free_full(task_launcher->task,
		   g_object_unref);
  
  task_launcher->task = NULL;
  
  g_rec_mutex_unlock(task_launcher_mutex);
  
  /* one shot task */
  task = start_task;

  while(task != NULL){
    ags_task_launch(task->data);
      
    /* iterate */
    task = task->next;
  }

  g_list_free_full(start_task,
		   g_object_unref);

  /* cyclic task */
  cyclic_task = start_cyclic_task;

  while(cyclic_task != NULL){
    ags_task_launch(cyclic_task->data);

    /* iterate */
    cyclic_task = cyclic_task->next;
  }

  g_list_free_full(start_cyclic_task,
		   g_object_unref);
}

/**
 * ags_task_launcher_run:
 * @task_launcher: the #AgsTaskLauncher
 *
 * Run tasks.
 *
 * Since: 3.0.0
 */
void
ags_task_launcher_run(AgsTaskLauncher *task_launcher)
{
  g_return_if_fail(AGS_IS_TASK_LAUNCHER(task_launcher));
  
  g_object_ref(task_launcher);
  g_signal_emit(task_launcher,
		task_launcher_signals[RUN], 0);
  g_object_unref(task_launcher);
}

gboolean
ags_task_launcher_source_func(AgsTaskLauncher *task_launcher)
{
  ags_task_launcher_run(task_launcher);

  return(FALSE);
}

/**
 * ags_task_launcher_sync_run:
 * @task_launcher: the #AgsTaskLauncher
 *
 * Sync run tasks.
 *
 * Since: 3.0.0
 */
void
ags_task_launcher_sync_run(AgsTaskLauncher *task_launcher)
{
  GMainContext *main_context;
  
  GRecMutex *task_launcher_mutex;
  
  if(!AGS_IS_TASK_LAUNCHER(task_launcher)){
    return;
  }

  g_mutex_lock(&(task_launcher->wait_mutex));

  g_atomic_int_set(&(task_launcher->is_running),
		   TRUE);

  g_mutex_unlock(&(task_launcher->wait_mutex));
  
  /* get task launcher mutex */
  task_launcher_mutex = AGS_TASK_LAUNCHER_GET_OBJ_MUTEX(task_launcher);

  /* get main context */
  g_rec_mutex_lock(task_launcher_mutex);

  main_context = task_launcher->main_context;
  g_main_context_ref(main_context);

  g_rec_mutex_unlock(task_launcher_mutex);

  /* invoke */
  g_object_ref(task_launcher);
  g_main_context_invoke_full(main_context,
			     G_PRIORITY_HIGH,
			     ags_task_launcher_source_func,
			     task_launcher,
			     (GDestroyNotify) g_object_unref);

  /* unref */
  g_main_context_unref(main_context);

  g_mutex_lock(&(task_launcher->wait_mutex));

  g_atomic_int_set(&(task_launcher->is_running),
		   FALSE);
  g_atomic_int_set(&(task_launcher->wait_count),
		   0);

  if(g_atomic_int_get(&(task_launcher->wait_count)) > 0){
    g_cond_broadcast(&(task_launcher->wait_cond));
  }
  
  g_mutex_unlock(&(task_launcher->wait_mutex));
}

/**
 * ags_task_launcher_new:
 *
 * Create a new #AgsTaskLauncher.
 *
 * Returns: the new #AgsTaskLauncher
 *
 * Since: 3.0.0
 */ 
AgsTaskLauncher*
ags_task_launcher_new()
{
  AgsTaskLauncher *task_launcher;

  task_launcher = (AgsTaskLauncher *) g_object_new(AGS_TYPE_TASK_LAUNCHER,
						   NULL);

  return(task_launcher);
}
