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

#include <ags/audio/task/ags_reset_amplitude.h>

#include <math.h>

#include <ags/i18n.h>

void ags_reset_amplitude_class_init(AgsResetAmplitudeClass *reset_amplitude);
void ags_reset_amplitude_init(AgsResetAmplitude *reset_amplitude);
void ags_reset_amplitude_dispose(GObject *gobject);
void ags_reset_amplitude_finalize(GObject *gobject);

void ags_reset_amplitude_launch(AgsTask *task);

/**
 * SECTION:ags_reset_amplitude
 * @short_description: reset amplitude internal
 * @title: AgsResetAmplitude
 * @section_id:
 * @include: ags/audio/task/ags_reset_amplitude.h
 *
 * The #AgsResetAmplitude task resets amplitude to recompute the amplitude during next run.
 */

static gpointer ags_reset_amplitude_parent_class = NULL;

AgsResetAmplitude *ags_reset_amplitude = NULL;

GType
ags_reset_amplitude_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_reset_amplitude = 0;

    static const GTypeInfo ags_reset_amplitude_info = {
      sizeof (AgsResetAmplitudeClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_reset_amplitude_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsResetAmplitude),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_reset_amplitude_init,
    };

    ags_type_reset_amplitude = g_type_register_static(AGS_TYPE_TASK,
						      "AgsResetAmplitude",
						      &ags_reset_amplitude_info,
						      0);
    
    g_once_init_leave(&g_define_type_id__volatile, ags_type_reset_amplitude);
  }

  return g_define_type_id__volatile;
}

void
ags_reset_amplitude_class_init(AgsResetAmplitudeClass *reset_amplitude)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_reset_amplitude_parent_class = g_type_class_peek_parent(reset_amplitude);

  /* GObjectClass */
  gobject = (GObjectClass *) reset_amplitude;

  gobject->dispose = ags_reset_amplitude_dispose;
  gobject->finalize = ags_reset_amplitude_finalize;

  /* AgsTaskClass */
  task = (AgsTaskClass *) reset_amplitude;
  
  task->launch = ags_reset_amplitude_launch;
}

void
ags_reset_amplitude_init(AgsResetAmplitude *reset_amplitude)
{
  AGS_TASK(reset_amplitude)->flags |= AGS_TASK_CYCLIC;
  
  reset_amplitude->analyse_channel = NULL;
}

void
ags_reset_amplitude_dispose(GObject *gobject)
{
  AgsResetAmplitude *reset_amplitude;

  reset_amplitude = AGS_RESET_AMPLITUDE(gobject);

  if(reset_amplitude->analyse_channel != NULL){
    g_list_free_full(reset_amplitude->analyse_channel,
		     g_object_unref);

    reset_amplitude->analyse_channel = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_reset_amplitude_parent_class)->dispose(gobject);
}

void
ags_reset_amplitude_finalize(GObject *gobject)
{
  AgsResetAmplitude *reset_amplitude;

  reset_amplitude = AGS_RESET_AMPLITUDE(gobject);

  if(reset_amplitude->analyse_channel != NULL){
    g_list_free_full(reset_amplitude->analyse_channel,
		     g_object_unref);
  }

  if(reset_amplitude == ags_reset_amplitude){
    ags_reset_amplitude = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_reset_amplitude_parent_class)->finalize(gobject);
}

void
ags_reset_amplitude_launch(AgsTask *task)
{
  AgsResetAmplitude *reset_amplitude;

  GList *analyse_channel;

  GValue value = {0,};

  GRecMutex *task_mutex;
  
  reset_amplitude = AGS_RESET_AMPLITUDE(task);

  task_mutex = AGS_TASK_GET_OBJ_MUTEX(reset_amplitude);
  
  g_rec_mutex_lock(task_mutex);

  analyse_channel = reset_amplitude->analyse_channel;

  g_value_init(&value,
	       G_TYPE_BOOLEAN);
  g_value_set_boolean(&value,
		      FALSE);
  
  while(analyse_channel != NULL){
    ags_port_safe_write(AGS_ANALYSE_CHANNEL(analyse_channel->data)->buffer_computed,
			&value);
    
    analyse_channel = analyse_channel->next;
  }

  g_rec_mutex_unlock(task_mutex);

  g_value_unset(&value);
}

/**
 * ags_reset_amplitude_add:
 * @reset_amplitude: the #AgsResetAmplitude
 * @analyse_channel: the #AgsAnalyseChannel
 *
 * Add @analyse_channel.
 *
 * Since: 3.0.0
 */
void
ags_reset_amplitude_add(AgsResetAmplitude *reset_amplitude,
			AgsAnalyseChannel *analyse_channel)
{
  GRecMutex *task_mutex;

  if(!AGS_IS_RESET_AMPLITUDE(reset_amplitude) ||
     !AGS_IS_ANALYSE_CHANNEL(analyse_channel)){
    return;
  }

  task_mutex = AGS_TASK_GET_OBJ_MUTEX(reset_amplitude);

  g_rec_mutex_lock(task_mutex);

  reset_amplitude->analyse_channel = g_list_prepend(reset_amplitude->analyse_channel,
						    analyse_channel);
  g_object_ref(analyse_channel);
  
  g_rec_mutex_unlock(task_mutex);
}

/**
 * ags_reset_amplitude_remove:
 * @reset_amplitude: the #AgsResetAmplitude
 * @analyse_channel: the #AgsAnalyseChannel
 *
 * Remove @analyse_channel.
 *
 * Since: 3.0.0
 */
void
ags_reset_amplitude_remove(AgsResetAmplitude *reset_amplitude,
			   AgsAnalyseChannel *analyse_channel)
{
  GRecMutex *task_mutex;

  if(!AGS_IS_RESET_AMPLITUDE(reset_amplitude) ||
     !AGS_IS_ANALYSE_CHANNEL(analyse_channel)){
    return;
  }

  task_mutex = AGS_TASK_GET_OBJ_MUTEX(reset_amplitude);

  g_rec_mutex_lock(task_mutex);

  if(g_list_find(reset_amplitude->analyse_channel,
		 analyse_channel) != NULL){
    reset_amplitude->analyse_channel = g_list_remove(reset_amplitude->analyse_channel,
						     analyse_channel);
    g_object_unref(analyse_channel);
  }
  
  g_rec_mutex_unlock(task_mutex);
}

/**
 * ags_reset_amplitude_get_instance:
 *
 * Get instance.
 *
 * Returns: (transfer none): the #AgsResetAmplitude
 *
 * Since: 3.0.0
 */
AgsResetAmplitude*
ags_reset_amplitude_get_instance()
{
  static GMutex mutex;

  g_mutex_lock(&mutex);

  if(ags_reset_amplitude == NULL){
    AgsTaskLauncher *task_launcher;
    
    AgsApplicationContext *application_context;

    /* reset amplitude */
    ags_reset_amplitude = ags_reset_amplitude_new();

    g_mutex_unlock(&mutex);

    /* add cyclic task */
    application_context = ags_application_context_get_instance();

    task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));
    ags_task_launcher_add_cyclic_task(task_launcher,
				      (AgsTask *) ags_reset_amplitude);
    
    /* unref */
    g_object_unref(task_launcher);
  }else{
    g_mutex_unlock(&mutex);
  }

  return(ags_reset_amplitude);
}

/**
 * ags_reset_amplitude_new:
 *
 * Creates an #AgsResetAmplitude.
 *
 * Returns: an new #AgsResetAmplitude.
 *
 * Since: 3.0.0
 */
AgsResetAmplitude*
ags_reset_amplitude_new()
{
  AgsResetAmplitude *reset_amplitude;
  
  reset_amplitude = (AgsResetAmplitude *) g_object_new(AGS_TYPE_RESET_AMPLITUDE,
						       NULL);
  
  return(reset_amplitude);
}
