/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/audio/task/ags_reset_fx_analyse.h>

#include <math.h>

#include <ags/i18n.h>

void ags_reset_fx_analyse_class_init(AgsResetFxAnalyseClass *reset_fx_analyse);
void ags_reset_fx_analyse_init(AgsResetFxAnalyse *reset_fx_analyse);
void ags_reset_fx_analyse_connect(AgsConnectable *connectable);
void ags_reset_fx_analyse_dispose(GObject *gobject);
void ags_reset_fx_analyse_finalize(GObject *gobject);

void ags_reset_fx_analyse_launch(AgsTask *task);

/**
 * SECTION:ags_reset_fx_analyse
 * @short_description: reset fx analyse
 * @title: AgsResetFxAnalyse
 * @section_id:
 * @include: ags/audio/task/ags_reset_fx_analyse.h
 *
 * The #AgsResetFxAnalyse task resets ags-fx-analyse to recompute the analyse during next run.
 */

static gpointer ags_reset_fx_analyse_parent_class = NULL;

AgsResetFxAnalyse *ags_reset_fx_analyse = NULL;

GType
ags_reset_fx_analyse_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_reset_fx_analyse = 0;

    static const GTypeInfo ags_reset_fx_analyse_info = {
      sizeof(AgsResetFxAnalyseClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_reset_fx_analyse_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsResetFxAnalyse),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_reset_fx_analyse_init,
    };

    ags_type_reset_fx_analyse = g_type_register_static(AGS_TYPE_TASK,
						       "AgsResetFxAnalyse",
						       &ags_reset_fx_analyse_info,
						       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_reset_fx_analyse);
  }

  return g_define_type_id__volatile;
}

void
ags_reset_fx_analyse_class_init(AgsResetFxAnalyseClass *reset_fx_analyse)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_reset_fx_analyse_parent_class = g_type_class_peek_parent(reset_fx_analyse);

  /* GObjectClass */
  gobject = (GObjectClass *) reset_fx_analyse;

  gobject->dispose = ags_reset_fx_analyse_dispose;
  gobject->finalize = ags_reset_fx_analyse_finalize;

  /* AgsTaskClass */
  task = (AgsTaskClass *) reset_fx_analyse;
  
  task->launch = ags_reset_fx_analyse_launch;
}

void
ags_reset_fx_analyse_init(AgsResetFxAnalyse *reset_fx_analyse)
{
  AGS_TASK(reset_fx_analyse)->flags |= AGS_TASK_CYCLIC;
  
  reset_fx_analyse->fx_analyse_channel = NULL;
}

void
ags_reset_fx_analyse_dispose(GObject *gobject)
{
  AgsResetFxAnalyse *reset_fx_analyse;

  reset_fx_analyse = AGS_RESET_FX_ANALYSE(gobject);

  if(reset_fx_analyse->fx_analyse_channel != NULL){
    g_list_free_full(reset_fx_analyse->fx_analyse_channel,
		     g_object_unref);

    reset_fx_analyse->fx_analyse_channel = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_reset_fx_analyse_parent_class)->dispose(gobject);
}

void
ags_reset_fx_analyse_finalize(GObject *gobject)
{
  AgsResetFxAnalyse *reset_fx_analyse;

  reset_fx_analyse = AGS_RESET_FX_ANALYSE(gobject);

  if(reset_fx_analyse->fx_analyse_channel != NULL){
    g_list_free_full(reset_fx_analyse->fx_analyse_channel,
		     g_object_unref);
  }

  if(reset_fx_analyse == ags_reset_fx_analyse){
    ags_reset_fx_analyse = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_reset_fx_analyse_parent_class)->finalize(gobject);
}

void
ags_reset_fx_analyse_launch(AgsTask *task)
{
  AgsResetFxAnalyse *reset_fx_analyse;

  GList *fx_analyse_channel;

  GRecMutex *task_mutex;
  GRecMutex *fx_analyse_channel_mutex;
  
  reset_fx_analyse = AGS_RESET_FX_ANALYSE(task);

  task_mutex = AGS_TASK_GET_OBJ_MUTEX(reset_fx_analyse);

  g_rec_mutex_lock(task_mutex);

  fx_analyse_channel = reset_fx_analyse->fx_analyse_channel;
  
  while(fx_analyse_channel != NULL){
    fx_analyse_channel_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_analyse_channel->data);

    /* reset */
    g_rec_mutex_lock(fx_analyse_channel_mutex);
    
    AGS_FX_ANALYSE_CHANNEL(fx_analyse_channel->data)->magnitude_cleared = FALSE;

    g_rec_mutex_unlock(fx_analyse_channel_mutex);

    /* iterate */
    fx_analyse_channel = fx_analyse_channel->next;
  }

  g_rec_mutex_unlock(task_mutex);
}

/**
 * ags_reset_fx_analyse_add:
 * @reset_fx_analyse: the #AgsResetFxAnalyse
 * @fx_analyse_channel: the #AgsFxAnalyseChannel
 *
 * Add @fx_analyse_channel.
 *
 * Since: 3.3.0
 */
void
ags_reset_fx_analyse_add(AgsResetFxAnalyse *reset_fx_analyse,
			 AgsFxAnalyseChannel *fx_analyse_channel)
{
  GRecMutex *task_mutex;

  if(!AGS_IS_RESET_FX_ANALYSE(reset_fx_analyse) ||
     !AGS_IS_FX_ANALYSE_CHANNEL(fx_analyse_channel)){
    return;
  }

  task_mutex = AGS_TASK_GET_OBJ_MUTEX(reset_fx_analyse);
  
  g_rec_mutex_lock(task_mutex);

  reset_fx_analyse->fx_analyse_channel = g_list_prepend(reset_fx_analyse->fx_analyse_channel,
							fx_analyse_channel);
  g_object_ref(fx_analyse_channel);
  
  g_rec_mutex_unlock(task_mutex);
}

/**
 * ags_reset_fx_analyse_remove:
 * @reset_fx_analyse: the #AgsResetFxAnalyse
 * @fx_analyse_channel: the #AgsFxAnalyseChannel
 *
 * Remove @fx_analyse_channel.
 *
 * Since: 3.3.0
 */
void
ags_reset_fx_analyse_remove(AgsResetFxAnalyse *reset_fx_analyse,
			    AgsFxAnalyseChannel *fx_analyse_channel)
{
  GRecMutex *task_mutex;

  if(!AGS_IS_RESET_FX_ANALYSE(reset_fx_analyse) ||
     !AGS_IS_FX_ANALYSE_CHANNEL(fx_analyse_channel)){
    return;
  }
  
  task_mutex = AGS_TASK_GET_OBJ_MUTEX(reset_fx_analyse);

  g_rec_mutex_lock(task_mutex);

  if(g_list_find(reset_fx_analyse->fx_analyse_channel,
		 fx_analyse_channel) != NULL){
    reset_fx_analyse->fx_analyse_channel = g_list_remove(reset_fx_analyse->fx_analyse_channel,
							 fx_analyse_channel);
    g_object_unref(fx_analyse_channel);
  }
  
  g_rec_mutex_unlock(task_mutex);
}

/**
 * ags_reset_fx_analyse_get_instance:
 *
 * Get instance.
 *
 * Returns: (transfer none): the #AgsResetFxAnalyse
 *
 * Since: 3.3.0
 */
AgsResetFxAnalyse*
ags_reset_fx_analyse_get_instance()
{
  static GMutex mutex;

  g_mutex_lock(&mutex);

  if(ags_reset_fx_analyse == NULL){
    AgsTaskLauncher *task_launcher;
    
    AgsApplicationContext *application_context;

    ags_reset_fx_analyse = ags_reset_fx_analyse_new();
    
    g_mutex_unlock(&mutex);

    /* add cyclic task */
    application_context = ags_application_context_get_instance();

    task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));
    ags_task_launcher_add_cyclic_task(task_launcher,
				      (AgsTask *) ags_reset_fx_analyse);
    
    /* unref */
    g_object_unref(task_launcher);
  }else{
    g_mutex_unlock(&mutex);
  }

  return(ags_reset_fx_analyse);
}

/**
 * ags_reset_fx_analyse_new:
 *
 * Creates an #AgsResetFxAnalyse.
 *
 * Returns: an new #AgsResetFxAnalyse.
 *
 * Since: 3.3.0
 */
AgsResetFxAnalyse*
ags_reset_fx_analyse_new()
{
  AgsResetFxAnalyse *reset_fx_analyse;
  
  reset_fx_analyse = (AgsResetFxAnalyse *) g_object_new(AGS_TYPE_RESET_FX_ANALYSE,
							NULL);
  
  return(reset_fx_analyse);
}
