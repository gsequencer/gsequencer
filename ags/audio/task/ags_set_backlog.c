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

#include <ags/audio/task/ags_set_backlog.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_audio_signal.h>

#include <ags/i18n.h>

void ags_set_backlog_class_init(AgsSetBacklogClass *set_backlog);
void ags_set_backlog_init(AgsSetBacklog *set_backlog);
void ags_set_backlog_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_set_backlog_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_set_backlog_dispose(GObject *gobject);
void ags_set_backlog_finalize(GObject *gobject);

void ags_set_backlog_launch(AgsTask *task);

void ags_set_backlog_audio_signal(AgsSetBacklog *set_backlog, AgsAudioSignal *audio_signal);

/**
 * SECTION:ags_set_backlog
 * @short_description: reset backlog
 * @title: AgsSetBacklog
 * @section_id:
 * @include: ags/audio/task/ags_set_backlog.h
 *
 * The #AgsSetBacklog task resets backlog of scope.
 */

static gpointer ags_set_backlog_parent_class = NULL;
AgsSetBacklog *ags_set_backlog = NULL;

enum{
  PROP_0,
  PROP_SCOPE,
  PROP_HAS_BACKLOG,
};

GType
ags_set_backlog_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_set_backlog = 0;

    static const GTypeInfo ags_set_backlog_info = {
      sizeof(AgsSetBacklogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_set_backlog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSetBacklog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_set_backlog_init,
    };

    ags_type_set_backlog = g_type_register_static(AGS_TYPE_TASK,
						  "AgsSetBacklog",
						  &ags_set_backlog_info,
						  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_set_backlog);
  }

  return g_define_type_id__volatile;
}

void
ags_set_backlog_class_init(AgsSetBacklogClass *set_backlog)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;

  ags_set_backlog_parent_class = g_type_class_peek_parent(set_backlog);

  /* gobject */
  gobject = (GObjectClass *) set_backlog;

  gobject->set_property = ags_set_backlog_set_property;
  gobject->get_property = ags_set_backlog_get_property;

  gobject->dispose = ags_set_backlog_dispose;
  gobject->finalize = ags_set_backlog_finalize;

  /* properties */
  /**
   * AgsSetBacklog:scope:
   *
   * The assigned #GObject as scope.
   * 
   * Since: 6.2.0
   */
  param_spec = g_param_spec_pointer("scope",
				   i18n_pspec("scope of set backlog"),
				   i18n_pspec("The scope of set backlog"),
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SCOPE,
				  param_spec);

  /**
   * AgsSetBacklog:has-backlog:
   *
   * The backlog to apply to scope.
   * 
   * Since: 6.2.0
   */
  param_spec = g_param_spec_boolean("has-backlog",
				    i18n_pspec("has backlog"),
				    i18n_pspec("The backlog to apply"),
				    TRUE,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_HAS_BACKLOG,
				  param_spec);

  /* task */
  task = (AgsTaskClass *) set_backlog;

  task->launch = ags_set_backlog_launch;
}

void
ags_set_backlog_init(AgsSetBacklog *set_backlog)
{
  AGS_TASK(set_backlog)->flags |= AGS_TASK_CYCLIC;

  set_backlog->scope = NULL;
  set_backlog->has_backlog = TRUE;
}

void
ags_set_backlog_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsSetBacklog *set_backlog;

  set_backlog = AGS_SET_BACKLOG(gobject);

  switch(prop_id){
  case PROP_SCOPE:
    {
      GObject *scope;

      scope = (GObject *) g_value_get_object(value);

      if(g_list_find(set_backlog->scope, scope) != NULL){
	return;
      }

      if(scope != NULL){
	g_object_ref(scope);

	set_backlog->scope = g_list_prepend(set_backlog->scope,
					    (GObject *) scope);
      }
    }
    break;
  case PROP_HAS_BACKLOG:
    {
      set_backlog->has_backlog = g_value_get_boolean(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_set_backlog_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsSetBacklog *set_backlog;

  set_backlog = AGS_SET_BACKLOG(gobject);

  switch(prop_id){
  case PROP_SCOPE:
    {
      g_value_set_pointer(value, g_list_copy_deep(set_backlog->scope,
						  (GCopyFunc) g_object_ref,
						  NULL));
    }
    break;
  case PROP_HAS_BACKLOG:
    {
      g_value_set_boolean(value, set_backlog->has_backlog);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_set_backlog_dispose(GObject *gobject)
{
  AgsSetBacklog *set_backlog;

  set_backlog = AGS_SET_BACKLOG(gobject);

  if(set_backlog->scope != NULL){
    g_list_free_full(set_backlog->scope,
		     g_object_unref);

    set_backlog->scope = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_set_backlog_parent_class)->dispose(gobject);
}

void
ags_set_backlog_finalize(GObject *gobject)
{
  AgsSetBacklog *set_backlog;

  set_backlog = AGS_SET_BACKLOG(gobject);

  if(set_backlog->scope != NULL){
    g_list_free_full(set_backlog->scope,
		     g_object_unref);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_set_backlog_parent_class)->finalize(gobject);
}

void
ags_set_backlog_launch(AgsTask *task)
{
  AgsSetBacklog *set_backlog;

  GList *start_scope, *scope;

  GRecMutex *task_mutex;

  set_backlog = AGS_SET_BACKLOG(task);

  task_mutex = AGS_TASK_GET_OBJ_MUTEX(task);

  g_rec_mutex_lock(task_mutex);

  scope = 
    start_scope = set_backlog->scope;

  while(scope != NULL){
    if(AGS_IS_AUDIO_SIGNAL(scope->data)){
      GRecMutex *backlog_mutex;

      backlog_mutex = AGS_AUDIO_SIGNAL_GET_BACKLOG_MUTEX(scope->data);

      g_rec_mutex_lock(backlog_mutex);
      
      AGS_AUDIO_SIGNAL(scope->data)->has_backlog = set_backlog->has_backlog;

      g_rec_mutex_unlock(backlog_mutex);
    }

    scope = scope->next;
  }

  g_rec_mutex_unlock(task_mutex);
}

/**
 * ags_set_backlog_add:
 * @set_backlog: the #AgsSetBacklog
 * @scope: the #AgsAudioSignal
 *
 * Add @scope to @set_backlog.
 * 
 * Since: 6.2.0
 */
void
ags_set_backlog_add(AgsSetBacklog *set_backlog,
		    AgsAudioSignal *scope)
{
  GRecMutex *task_mutex;

  g_return_if_fail(AGS_IS_SET_BACKLOG(set_backlog));
  g_return_if_fail(AGS_IS_AUDIO_SIGNAL(scope));
  
  task_mutex = AGS_TASK_GET_OBJ_MUTEX(set_backlog);

  g_rec_mutex_lock(task_mutex);

  if(g_list_find(set_backlog->scope, scope) == NULL){
    g_object_ref(scope);

    set_backlog->scope = g_list_prepend(set_backlog->scope,
					scope);
  }
  
  g_rec_mutex_unlock(task_mutex);
}

/**
 * ags_set_backlog_remove:
 * @set_backlog: the #AgsSetBacklog
 * @scope: the #AgsAudioSignal
 *
 * Remove @scope from @set_backlog.
 *
 * Since: 6.2.0
 */
void
ags_set_backlog_remove(AgsSetBacklog *set_backlog,
		       AgsAudioSignal *scope)
{
  GRecMutex *task_mutex;

  g_return_if_fail(AGS_IS_SET_BACKLOG(set_backlog));
  g_return_if_fail(AGS_IS_AUDIO_SIGNAL(scope));

  task_mutex = AGS_TASK_GET_OBJ_MUTEX(set_backlog);

  g_rec_mutex_lock(task_mutex);

  if(g_list_find(set_backlog->scope, scope) != NULL){
    set_backlog->scope = g_list_remove(set_backlog->scope,
				       scope);

    g_object_unref(scope);
  }  

  g_rec_mutex_unlock(task_mutex);
}

/**
 * ags_set_backlog_get_instance:
 *
 * Get instance of #AgsSetBacklog.
 *
 * Returns: the singleton of #AgsSetBacklog
 *
 * Since: 6.2.0
 */
AgsSetBacklog*
ags_set_backlog_get_instance()
{
  static GMutex mutex;
  
  g_mutex_lock(&mutex);
  
  if(ags_set_backlog == NULL){
    AgsTaskLauncher *task_launcher;
    
    AgsApplicationContext *application_context;

    ags_set_backlog = ags_set_backlog_new();

    /* add cyclic task */
    application_context = ags_application_context_get_instance();

    task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));
    ags_task_launcher_add_cyclic_task(task_launcher,
				      (AgsTask *) ags_set_backlog);
    
    /* unref */
    if(task_launcher != NULL){
      g_object_unref(task_launcher);
    }
  }
  
  g_mutex_unlock(&mutex);

  return(ags_set_backlog);
}

/**
 * ags_set_backlog_new:
 *
 * Create a new instance of #AgsSetBacklog.
 *
 * Returns: the new #AgsSetBacklog
 *
 * Since: 6.2.0
 */
AgsSetBacklog*
ags_set_backlog_new()
{
  AgsSetBacklog *set_backlog;

  set_backlog = (AgsSetBacklog *) g_object_new(AGS_TYPE_SET_BACKLOG,
					       NULL);

  return(set_backlog);
}
