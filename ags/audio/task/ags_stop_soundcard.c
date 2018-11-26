/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/audio/task/ags_stop_soundcard.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_soundcard_thread.h>
#include <ags/audio/thread/ags_export_thread.h>

#include <ags/i18n.h>

void ags_stop_soundcard_class_init(AgsStopSoundcardClass *stop_soundcard);
void ags_stop_soundcard_init(AgsStopSoundcard *stop_soundcard);
void ags_stop_soundcard_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_stop_soundcard_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_stop_soundcard_dispose(GObject *gobject);
void ags_stop_soundcard_finalize(GObject *gobject);

void ags_stop_soundcard_launch(AgsTask *task);

/**
 * SECTION:ags_stop_soundcard
 * @short_description: stop soundcard object
 * @title: AgsStopSoundcard
 * @section_id:
 * @include: ags/audio/task/ags_stop_soundcard.h
 *
 * The #AgsStopSoundcard task stops soundcard.
 */

static gpointer ags_stop_soundcard_parent_class = NULL;

enum{
  PROP_0,
  PROP_APPLICATION_CONTEXT,
};

GType
ags_stop_soundcard_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_stop_soundcard = 0;

    static const GTypeInfo ags_stop_soundcard_info = {
      sizeof(AgsStopSoundcardClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_stop_soundcard_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsStopSoundcard),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_stop_soundcard_init,
    };

    ags_type_stop_soundcard = g_type_register_static(AGS_TYPE_TASK,
						     "AgsStopSoundcard",
						     &ags_stop_soundcard_info,
						     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_stop_soundcard);
  }

  return g_define_type_id__volatile;
}

void
ags_stop_soundcard_class_init(AgsStopSoundcardClass *stop_soundcard)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;

  ags_stop_soundcard_parent_class = g_type_class_peek_parent(stop_soundcard);

  /* gobject */
  gobject = (GObjectClass *) stop_soundcard;

  gobject->set_property = ags_stop_soundcard_set_property;
  gobject->get_property = ags_stop_soundcard_get_property;

  gobject->dispose = ags_stop_soundcard_dispose;
  gobject->finalize = ags_stop_soundcard_finalize;

  /* properties */
  /**
   * AgsStopSoundcard:application-context:
   *
   * The assigned #AgsApplicationContext
   * 
   * Since: 2.1.0
   */
  param_spec = g_param_spec_object("application-context",
				   i18n_pspec("application context of stop soundcard"),
				   i18n_pspec("The application context of stop soundcard task"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_CONTEXT,
				  param_spec);

  /* task */
  task = (AgsTaskClass *) stop_soundcard;

  task->launch = ags_stop_soundcard_launch;
}

void
ags_stop_soundcard_init(AgsStopSoundcard *stop_soundcard)
{
  stop_soundcard->application_context = NULL;
}

void
ags_stop_soundcard_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsStopSoundcard *stop_soundcard;

  stop_soundcard = AGS_STOP_SOUNDCARD(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      GObject *application_context;

      application_context = (GObject *) g_value_get_object(value);

      if(stop_soundcard->application_context == (GObject *) application_context){
	return;
      }

      if(stop_soundcard->application_context != NULL){
	g_object_unref(stop_soundcard->application_context);
      }

      if(application_context != NULL){
	g_object_ref(application_context);
      }

      stop_soundcard->application_context = (GObject *) application_context;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_stop_soundcard_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec)
{
  AgsStopSoundcard *stop_soundcard;

  stop_soundcard = AGS_STOP_SOUNDCARD(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, stop_soundcard->application_context);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_stop_soundcard_dispose(GObject *gobject)
{
  AgsStopSoundcard *stop_soundcard;

  stop_soundcard = AGS_STOP_SOUNDCARD(gobject);

  if(stop_soundcard->application_context != NULL){
    g_object_unref(stop_soundcard->application_context);

    stop_soundcard->application_context = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_stop_soundcard_parent_class)->dispose(gobject);
}

void
ags_stop_soundcard_finalize(GObject *gobject)
{
  AgsAudioLoop *audio_loop;
  AgsSoundcardThread *soundcard_thread;

  AgsApplicationContext *application_context;
  AgsSoundcard *soundcard;

  application_context = AGS_STOP_SOUNDCARD(gobject)->application_context;

  if(application_context != NULL){
    //FIXME:JK: wrong location of code
    audio_loop = AGS_AUDIO_LOOP(application_context->main_loop);

    soundcard_thread = (AgsSoundcardThread *) ags_thread_find_type((AgsThread *) audio_loop,
								   AGS_TYPE_SOUNDCARD_THREAD);

    if(soundcard_thread->error != NULL){
      g_error_free(soundcard_thread->error);

      soundcard_thread->error = NULL;
    }
    
    g_object_unref(application_context);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_stop_soundcard_parent_class)->finalize(gobject);
}

void
ags_stop_soundcard_launch(AgsTask *task)
{
  AgsStopSoundcard *stop_soundcard;

  AgsThread *audio_loop;
  AgsThread *soundcard_thread;
  AgsThread *export_thread;

  AgsApplicationContext *application_context;

  stop_soundcard = AGS_STOP_SOUNDCARD(task);

  application_context = stop_soundcard->application_context;

  /* get main loop */
  g_object_get(application_context,
	       "main-loop", &audio_loop,
	       NULL);

  soundcard_thread = ags_thread_find_type(audio_loop,
					  AGS_TYPE_SOUNDCARD_THREAD);
  
  while(soundcard_thread != NULL){
    if(AGS_IS_SOUNDCARD_THREAD(soundcard_thread)){    
      /* stop soundcard thread */
      ags_thread_stop(soundcard_thread);
    }
    
    soundcard_thread = g_atomic_pointer_get(&(soundcard_thread->next));
  }

  export_thread = ags_thread_find_type(audio_loop,
				       AGS_TYPE_EXPORT_THREAD);
  
  while(export_thread != NULL){
    if(AGS_IS_EXPORT_THREAD(export_thread)){
      /* stop export thread */
      ags_thread_stop(export_thread);
    }
    
    export_thread = g_atomic_pointer_get(&(export_thread->next));
  }
}

/**
 * ags_stop_soundcard_new:
 * @application_context: the #AgsApplicationContext
 *
 * Create a new instance of #AgsStopSoundcard.
 *
 * Returns: the new #AgsStopSoundcard.
 *
 * Since: 2.1.0
 */
AgsStopSoundcard*
ags_stop_soundcard_new(AgsApplicationContext *application_context)
{
  AgsStopSoundcard *stop_soundcard;

  stop_soundcard = (AgsStopSoundcard *) g_object_new(AGS_TYPE_STOP_SOUNDCARD,
						     "application-context", application_context,
						     NULL);

  return(stop_soundcard);
}
