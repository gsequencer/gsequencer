/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/audio/task/ags_reset_recall_staging.h>

#include <math.h>

#include <ags/i18n.h>

void ags_reset_recall_staging_class_init(AgsResetRecallStagingClass *reset_recall_staging);
void ags_reset_recall_staging_init(AgsResetRecallStaging *reset_recall_staging);
void ags_reset_recall_staging_connect(AgsConnectable *connectable);
void ags_reset_recall_staging_dispose(GObject *gobject);
void ags_reset_recall_staging_finalize(GObject *gobject);

void ags_reset_recall_staging_launch(AgsTask *task);

/**
 * SECTION:ags_reset_recall_staging
 * @short_description: reset fx peak
 * @title: AgsResetRecallStaging
 * @section_id:
 * @include: ags/audio/task/ags_reset_recall_staging.h
 *
 * The #AgsResetRecallStaging task resets ags-fx-peak to recompute the peak during next run.
 */

static gpointer ags_reset_recall_staging_parent_class = NULL;

AgsResetRecallStaging *ags_reset_recall_staging = NULL;

GType
ags_reset_recall_staging_get_type()
{
  static gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_reset_recall_staging = 0;

    static const GTypeInfo ags_reset_recall_staging_info = {
      sizeof(AgsResetRecallStagingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_reset_recall_staging_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsResetRecallStaging),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_reset_recall_staging_init,
    };

    ags_type_reset_recall_staging = g_type_register_static(AGS_TYPE_TASK,
						    "AgsResetRecallStaging",
						    &ags_reset_recall_staging_info,
						    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_reset_recall_staging);
  }

  return g_define_type_id__volatile;
}

void
ags_reset_recall_staging_class_init(AgsResetRecallStagingClass *reset_recall_staging)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_reset_recall_staging_parent_class = g_type_class_peek_parent(reset_recall_staging);

  /* GObjectClass */
  gobject = (GObjectClass *) reset_recall_staging;

  gobject->dispose = ags_reset_recall_staging_dispose;
  gobject->finalize = ags_reset_recall_staging_finalize;

  /* AgsTaskClass */
  task = (AgsTaskClass *) reset_recall_staging;
  
  task->launch = ags_reset_recall_staging_launch;
}

void
ags_reset_recall_staging_init(AgsResetRecallStaging *reset_recall_staging)
{
  AGS_TASK(reset_recall_staging)->flags |= AGS_TASK_CYCLIC;
  
  reset_recall_staging->staging_flags = (//NOTE:JK: let it be
					// AGS_SOUND_STAGING_RUN_INIT_PRE |
					// AGS_SOUND_STAGING_RUN_INIT_INTER |
					// AGS_SOUND_STAGING_RUN_INIT_POST |
					AGS_SOUND_STAGING_FEED_INPUT_QUEUE |
					AGS_SOUND_STAGING_AUTOMATE |
					AGS_SOUND_STAGING_RUN_PRE |
					AGS_SOUND_STAGING_RUN_INTER |
					AGS_SOUND_STAGING_RUN_POST |
					AGS_SOUND_STAGING_DO_FEEDBACK |
					AGS_SOUND_STAGING_FEED_OUTPUT_QUEUE |
					AGS_SOUND_STAGING_FINI);
  
  reset_recall_staging->recall = NULL;
}

void
ags_reset_recall_staging_dispose(GObject *gobject)
{
  AgsResetRecallStaging *reset_recall_staging;

  reset_recall_staging = AGS_RESET_RECALL_STAGING(gobject);

  if(reset_recall_staging->recall != NULL){
    g_list_free_full(reset_recall_staging->recall,
		     g_object_unref);

    reset_recall_staging->recall = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_reset_recall_staging_parent_class)->dispose(gobject);
}

void
ags_reset_recall_staging_finalize(GObject *gobject)
{
  AgsResetRecallStaging *reset_recall_staging;

  reset_recall_staging = AGS_RESET_RECALL_STAGING(gobject);

  if(reset_recall_staging->recall != NULL){
    g_list_free_full(reset_recall_staging->recall,
		     g_object_unref);
  }

  if(reset_recall_staging == ags_reset_recall_staging){
    ags_reset_recall_staging = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_reset_recall_staging_parent_class)->finalize(gobject);
}

void
ags_reset_recall_staging_launch(AgsTask *task)
{
  AgsResetRecallStaging *reset_recall_staging;

  GList *start_recall, *recall;

  GRecMutex *task_mutex;
  
  reset_recall_staging = AGS_RESET_RECALL_STAGING(task);

  task_mutex = AGS_TASK_GET_OBJ_MUTEX(reset_recall_staging);

  g_rec_mutex_lock(task_mutex);

  recall =
    start_recall = g_list_copy_deep(reset_recall_staging->recall,
				    (GCopyFunc) g_object_ref,
				    NULL);

  g_rec_mutex_unlock(task_mutex);
  
  while(recall != NULL){
    /* reset */
    ags_recall_unset_staging_flags(AGS_RECALL(recall->data),
				   reset_recall_staging->staging_flags);

    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_reset_recall_staging_add:
 * @reset_recall_staging: the #AgsResetRecallStaging
 * @recall: the #AgsRecall
 *
 * Add @recall.
 *
 * Since: 6.3.0
 */
void
ags_reset_recall_staging_add(AgsResetRecallStaging *reset_recall_staging,
			     AgsRecall *recall)
{
  GRecMutex *task_mutex;

  if(!AGS_IS_RESET_RECALL_STAGING(reset_recall_staging) ||
     !AGS_IS_RECALL(recall)){
    return;
  }

  task_mutex = AGS_TASK_GET_OBJ_MUTEX(reset_recall_staging);
  
  g_rec_mutex_lock(task_mutex);

  if(g_list_find(reset_recall_staging->recall, recall) == NULL){
    reset_recall_staging->recall = g_list_prepend(reset_recall_staging->recall,
						  recall);
    g_object_ref(recall);
  }
  
  g_rec_mutex_unlock(task_mutex);
}

/**
 * ags_reset_recall_staging_remove:
 * @reset_recall_staging: the #AgsResetRecallStaging
 * @recall: the #AgsRecall
 *
 * Remove @recall.
 *
 * Since: 6.3.0
 */
void
ags_reset_recall_staging_remove(AgsResetRecallStaging *reset_recall_staging,
				AgsRecall *recall)
{
  GRecMutex *task_mutex;

  if(!AGS_IS_RESET_RECALL_STAGING(reset_recall_staging) ||
     !AGS_IS_RECALL(recall)){
    return;
  }
  
  task_mutex = AGS_TASK_GET_OBJ_MUTEX(reset_recall_staging);

  g_rec_mutex_lock(task_mutex);

  if(g_list_find(reset_recall_staging->recall, recall) != NULL){
    reset_recall_staging->recall = g_list_remove(reset_recall_staging->recall,
						 recall);
    g_object_unref(recall);
  }
  
  g_rec_mutex_unlock(task_mutex);
}

/**
 * ags_reset_recall_staging_get_instance:
 *
 * Get instance.
 *
 * Returns: (transfer none): the #AgsResetRecallStaging
 *
 * Since: 6.3.0
 */
AgsResetRecallStaging*
ags_reset_recall_staging_get_instance()
{
  static GMutex mutex;

  g_mutex_lock(&mutex);

  if(ags_reset_recall_staging == NULL){
    AgsTaskLauncher *task_launcher;
    
    AgsApplicationContext *application_context;

    ags_reset_recall_staging = ags_reset_recall_staging_new();
    
    g_mutex_unlock(&mutex);

    /* add cyclic task */
    application_context = ags_application_context_get_instance();

    task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));
    ags_task_launcher_add_cyclic_task(task_launcher,
				      (AgsTask *) ags_reset_recall_staging);
    
    /* unref */
    if(task_launcher != NULL){
      g_object_unref(task_launcher);
    }
  }else{
    g_mutex_unlock(&mutex);
  }

  return(ags_reset_recall_staging);
}

/**
 * ags_reset_recall_staging_new:
 *
 * Creates an #AgsResetRecallStaging.
 *
 * Returns: an new #AgsResetRecallStaging.
 *
 * Since: 6.3.0
 */
AgsResetRecallStaging*
ags_reset_recall_staging_new()
{
  AgsResetRecallStaging *reset_recall_staging;
  
  reset_recall_staging = (AgsResetRecallStaging *) g_object_new(AGS_TYPE_RESET_RECALL_STAGING,
								NULL);
  
  return(reset_recall_staging);
}
