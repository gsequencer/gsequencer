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

#include <ags/audio/task/ags_reset_fx_peak.h>

#include <math.h>

#include <ags/i18n.h>

void ags_reset_fx_peak_class_init(AgsResetFxPeakClass *reset_fx_peak);
void ags_reset_fx_peak_init(AgsResetFxPeak *reset_fx_peak);
void ags_reset_fx_peak_connect(AgsConnectable *connectable);
void ags_reset_fx_peak_dispose(GObject *gobject);
void ags_reset_fx_peak_finalize(GObject *gobject);

void ags_reset_fx_peak_launch(AgsTask *task);

/**
 * SECTION:ags_reset_fx_peak
 * @short_description: reset fx peak
 * @title: AgsResetFxPeak
 * @section_id:
 * @include: ags/audio/task/ags_reset_fx_peak.h
 *
 * The #AgsResetFxPeak task resets ags-fx-peak to recompute the peak during next run.
 */

static gpointer ags_reset_fx_peak_parent_class = NULL;

AgsResetFxPeak *ags_reset_fx_peak = NULL;

GType
ags_reset_fx_peak_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_reset_fx_peak = 0;

    static const GTypeInfo ags_reset_fx_peak_info = {
      sizeof(AgsResetFxPeakClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_reset_fx_peak_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsResetFxPeak),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_reset_fx_peak_init,
    };

    ags_type_reset_fx_peak = g_type_register_static(AGS_TYPE_TASK,
						    "AgsResetFxPeak",
						    &ags_reset_fx_peak_info,
						    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_reset_fx_peak);
  }

  return g_define_type_id__volatile;
}

void
ags_reset_fx_peak_class_init(AgsResetFxPeakClass *reset_fx_peak)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_reset_fx_peak_parent_class = g_type_class_peek_parent(reset_fx_peak);

  /* GObjectClass */
  gobject = (GObjectClass *) reset_fx_peak;

  gobject->dispose = ags_reset_fx_peak_dispose;
  gobject->finalize = ags_reset_fx_peak_finalize;

  /* AgsTaskClass */
  task = (AgsTaskClass *) reset_fx_peak;
  
  task->launch = ags_reset_fx_peak_launch;
}

void
ags_reset_fx_peak_init(AgsResetFxPeak *reset_fx_peak)
{
  AGS_TASK(reset_fx_peak)->flags |= AGS_TASK_CYCLIC;
  
  reset_fx_peak->fx_peak_channel = NULL;
}

void
ags_reset_fx_peak_dispose(GObject *gobject)
{
  AgsResetFxPeak *reset_fx_peak;

  reset_fx_peak = AGS_RESET_FX_PEAK(gobject);

  if(reset_fx_peak->fx_peak_channel != NULL){
    g_list_free_full(reset_fx_peak->fx_peak_channel,
		     g_object_unref);

    reset_fx_peak->fx_peak_channel = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_reset_fx_peak_parent_class)->dispose(gobject);
}

void
ags_reset_fx_peak_finalize(GObject *gobject)
{
  AgsResetFxPeak *reset_fx_peak;

  reset_fx_peak = AGS_RESET_FX_PEAK(gobject);

  if(reset_fx_peak->fx_peak_channel != NULL){
    g_list_free_full(reset_fx_peak->fx_peak_channel,
		     g_object_unref);
  }

  if(reset_fx_peak == ags_reset_fx_peak){
    ags_reset_fx_peak = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_reset_fx_peak_parent_class)->finalize(gobject);
}

void
ags_reset_fx_peak_launch(AgsTask *task)
{
  AgsResetFxPeak *reset_fx_peak;

  GList *fx_peak_channel;

  GRecMutex *task_mutex;
  GRecMutex *fx_peak_channel_mutex;
  
  reset_fx_peak = AGS_RESET_FX_PEAK(task);

  task_mutex = AGS_TASK_GET_OBJ_MUTEX(reset_fx_peak);

  g_rec_mutex_lock(task_mutex);

  fx_peak_channel = reset_fx_peak->fx_peak_channel;
  
  while(fx_peak_channel != NULL){
    fx_peak_channel_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_peak_channel->data);

    /* reset */
    g_rec_mutex_lock(fx_peak_channel_mutex);
    
    AGS_FX_PEAK_CHANNEL(fx_peak_channel->data)->peak_reseted = FALSE;

    g_rec_mutex_unlock(fx_peak_channel_mutex);

    /* iterate */
    fx_peak_channel = fx_peak_channel->next;
  }

  g_rec_mutex_unlock(task_mutex);
}

/**
 * ags_reset_fx_peak_add:
 * @reset_fx_peak: the #AgsResetFxPeak
 * @fx_peak_channel: the #AgsFxPeakChannel
 *
 * Add @fx_peak_channel.
 *
 * Since: 3.3.0
 */
void
ags_reset_fx_peak_add(AgsResetFxPeak *reset_fx_peak,
		      AgsFxPeakChannel *fx_peak_channel)
{
  GRecMutex *task_mutex;

  if(!AGS_IS_RESET_FX_PEAK(reset_fx_peak) ||
     !AGS_IS_FX_PEAK_CHANNEL(fx_peak_channel)){
    return;
  }

  task_mutex = AGS_TASK_GET_OBJ_MUTEX(reset_fx_peak);
  
  g_rec_mutex_lock(task_mutex);

  reset_fx_peak->fx_peak_channel = g_list_prepend(reset_fx_peak->fx_peak_channel,
						  fx_peak_channel);
  g_object_ref(fx_peak_channel);
  
  g_rec_mutex_unlock(task_mutex);
}

/**
 * ags_reset_fx_peak_remove:
 * @reset_fx_peak: the #AgsResetFxPeak
 * @fx_peak_channel: the #AgsFxPeakChannel
 *
 * Remove @fx_peak_channel.
 *
 * Since: 3.3.0
 */
void
ags_reset_fx_peak_remove(AgsResetFxPeak *reset_fx_peak,
			 AgsFxPeakChannel *fx_peak_channel)
{
  GRecMutex *task_mutex;

  if(!AGS_IS_RESET_FX_PEAK(reset_fx_peak) ||
     !AGS_IS_FX_PEAK_CHANNEL(fx_peak_channel)){
    return;
  }
  
  task_mutex = AGS_TASK_GET_OBJ_MUTEX(reset_fx_peak);

  g_rec_mutex_lock(task_mutex);

  if(g_list_find(reset_fx_peak->fx_peak_channel,
		 fx_peak_channel) != NULL){
    reset_fx_peak->fx_peak_channel = g_list_remove(reset_fx_peak->fx_peak_channel,
						   fx_peak_channel);
    g_object_unref(fx_peak_channel);
  }
  
  g_rec_mutex_unlock(task_mutex);
}

/**
 * ags_reset_fx_peak_get_instance:
 *
 * Get instance.
 *
 * Returns: (transfer none): the #AgsResetFxPeak
 *
 * Since: 3.3.0
 */
AgsResetFxPeak*
ags_reset_fx_peak_get_instance()
{
  static GMutex mutex;

  g_mutex_lock(&mutex);

  if(ags_reset_fx_peak == NULL){
    AgsTaskLauncher *task_launcher;
    
    AgsApplicationContext *application_context;

    ags_reset_fx_peak = ags_reset_fx_peak_new();
    
    g_mutex_unlock(&mutex);

    /* add cyclic task */
    application_context = ags_application_context_get_instance();

    task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));
    ags_task_launcher_add_cyclic_task(task_launcher,
				      (AgsTask *) ags_reset_fx_peak);
    
    /* unref */
    if(task_launcher != NULL){
      g_object_unref(task_launcher);
    }
  }else{
    g_mutex_unlock(&mutex);
  }

  return(ags_reset_fx_peak);
}

/**
 * ags_reset_fx_peak_new:
 *
 * Creates an #AgsResetFxPeak.
 *
 * Returns: an new #AgsResetFxPeak.
 *
 * Since: 3.3.0
 */
AgsResetFxPeak*
ags_reset_fx_peak_new()
{
  AgsResetFxPeak *reset_fx_peak;
  
  reset_fx_peak = (AgsResetFxPeak *) g_object_new(AGS_TYPE_RESET_FX_PEAK,
						  NULL);
  
  return(reset_fx_peak);
}
