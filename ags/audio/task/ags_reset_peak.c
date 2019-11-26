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

#include <ags/audio/task/ags_reset_peak.h>

#include <math.h>

#include <ags/i18n.h>

void ags_reset_peak_class_init(AgsResetPeakClass *reset_peak);
void ags_reset_peak_init(AgsResetPeak *reset_peak);
void ags_reset_peak_connect(AgsConnectable *connectable);
void ags_reset_peak_dispose(GObject *gobject);
void ags_reset_peak_finalize(GObject *gobject);

void ags_reset_peak_launch(AgsTask *task);

/**
 * SECTION:ags_reset_peak
 * @short_description: reset peak internal
 * @title: AgsResetPeak
 * @section_id:
 * @include: ags/audio/task/ags_reset_peak.h
 *
 * The #AgsResetPeak task resets peak to recompute the peak during next run.
 */

static gpointer ags_reset_peak_parent_class = NULL;

AgsResetPeak *ags_reset_peak = NULL;

GType
ags_reset_peak_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_reset_peak = 0;

    static const GTypeInfo ags_reset_peak_info = {
      sizeof(AgsResetPeakClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_reset_peak_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsResetPeak),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_reset_peak_init,
    };

    ags_type_reset_peak = g_type_register_static(AGS_TYPE_TASK,
						 "AgsResetPeak",
						 &ags_reset_peak_info,
						 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_reset_peak);
  }

  return g_define_type_id__volatile;
}

void
ags_reset_peak_class_init(AgsResetPeakClass *reset_peak)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_reset_peak_parent_class = g_type_class_peek_parent(reset_peak);

  /* GObjectClass */
  gobject = (GObjectClass *) reset_peak;

  gobject->dispose = ags_reset_peak_dispose;
  gobject->finalize = ags_reset_peak_finalize;

  /* AgsTaskClass */
  task = (AgsTaskClass *) reset_peak;
  
  task->launch = ags_reset_peak_launch;
}

void
ags_reset_peak_init(AgsResetPeak *reset_peak)
{
  AGS_TASK(reset_peak)->flags |= AGS_TASK_CYCLIC;
  
  reset_peak->peak_channel = NULL;
}

void
ags_reset_peak_dispose(GObject *gobject)
{
  AgsResetPeak *reset_peak;

  reset_peak = AGS_RESET_PEAK(gobject);

  if(reset_peak->peak_channel != NULL){
    g_list_free_full(reset_peak->peak_channel,
		     g_object_unref);

    reset_peak->peak_channel = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_reset_peak_parent_class)->dispose(gobject);
}

void
ags_reset_peak_finalize(GObject *gobject)
{
  AgsResetPeak *reset_peak;

  reset_peak = AGS_RESET_PEAK(gobject);

  if(reset_peak->peak_channel != NULL){
    g_list_free_full(reset_peak->peak_channel,
		     g_object_unref);
  }

  if(reset_peak == ags_reset_peak){
    ags_reset_peak = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_reset_peak_parent_class)->finalize(gobject);
}

void
ags_reset_peak_launch(AgsTask *task)
{
  AgsResetPeak *reset_peak;

  GList *peak_channel;

  GValue value = {0,};

  pthread_mutex_t *task_mutex;
  
  reset_peak = AGS_RESET_PEAK(task);

  task_mutex = AGS_TASK_GET_OBJ_MUTEX(reset_peak);

  pthread_mutex_lock(task_mutex);

  peak_channel = reset_peak->peak_channel;

  g_value_init(&value,
	       G_TYPE_BOOLEAN);
  g_value_set_boolean(&value,
		      FALSE);
  
  while(peak_channel != NULL){
    ags_port_safe_write(AGS_PEAK_CHANNEL(peak_channel->data)->buffer_computed,
			&value);
    
    peak_channel = peak_channel->next;
  }

  pthread_mutex_unlock(task_mutex);

  g_value_unset(&value);
}

/**
 * ags_reset_peak_add:
 * @reset_peak: the #AgsResetPeak
 * @peak_channel: the #AgsPeakChannel
 *
 * Add @peak_channel.
 *
 * Since: 2.0.0
 */
void
ags_reset_peak_add(AgsResetPeak *reset_peak,
		   AgsPeakChannel *peak_channel)
{
  pthread_mutex_t *task_mutex;

  if(!AGS_IS_RESET_PEAK(reset_peak) ||
     !AGS_IS_PEAK_CHANNEL(peak_channel)){
    return;
  }

  task_mutex = AGS_TASK_GET_OBJ_MUTEX(reset_peak);
  
  pthread_mutex_lock(task_mutex);

  reset_peak->peak_channel = g_list_prepend(reset_peak->peak_channel,
					    peak_channel);
  g_object_ref(peak_channel);
  
  pthread_mutex_unlock(task_mutex);
}

/**
 * ags_reset_peak_remove:
 * @reset_peak: the #AgsResetPeak
 * @peak_channel: the #AgsPeakChannel
 *
 * Remove @peak_channel.
 *
 * Since: 2.0.0
 */
void
ags_reset_peak_remove(AgsResetPeak *reset_peak,
		      AgsPeakChannel *peak_channel)
{
  pthread_mutex_t *task_mutex;

  if(!AGS_IS_RESET_PEAK(reset_peak) ||
     !AGS_IS_PEAK_CHANNEL(peak_channel)){
    return;
  }
  
  task_mutex = AGS_TASK_GET_OBJ_MUTEX(reset_peak);

  pthread_mutex_lock(task_mutex);

  if(g_list_find(reset_peak->peak_channel,
		 peak_channel) != NULL){
    reset_peak->peak_channel = g_list_remove(reset_peak->peak_channel,
					     peak_channel);
    g_object_unref(peak_channel);
  }
  
  pthread_mutex_unlock(task_mutex);
}

/**
 * ags_reset_peak_get_instance:
 *
 * Get instance.
 *
 * Returns: the #AgsResetPeak
 *
 * Since: 2.0.0
 */
AgsResetPeak*
ags_reset_peak_get_instance()
{
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&mutex);

  if(ags_reset_peak == NULL){
    AgsTaskThread *task_thread;
    
    AgsApplicationContext *application_context;

    ags_reset_peak = ags_reset_peak_new();
    
    pthread_mutex_unlock(&mutex);

    /* add cyclic task */
    application_context = ags_application_context_get_instance();

    task_thread = ags_concurrency_provider_get_task_thread(AGS_CONCURRENCY_PROVIDER(application_context));
    ags_task_thread_append_cyclic_task(task_thread,
				       (AgsTask *) ags_reset_peak);

    /* unref */
    g_object_unref(task_thread);
  }else{
    pthread_mutex_unlock(&mutex);
  }

  return(ags_reset_peak);
}

/**
 * ags_reset_peak_new:
 *
 * Creates an #AgsResetPeak.
 *
 * Returns: an new #AgsResetPeak.
 *
 * Since: 2.0.0
 */
AgsResetPeak*
ags_reset_peak_new()
{
  AgsResetPeak *reset_peak;
  
  reset_peak = (AgsResetPeak *) g_object_new(AGS_TYPE_RESET_PEAK,
					     NULL);
  
  return(reset_peak);
}
