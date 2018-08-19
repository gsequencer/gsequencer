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

#include <ags/audio/task/ags_start_soundcard.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_soundcard_thread.h>

#include <ags/i18n.h>

void ags_start_soundcard_class_init(AgsStartSoundcardClass *start_soundcard);
void ags_start_soundcard_init(AgsStartSoundcard *start_soundcard);
void ags_start_soundcard_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_start_soundcard_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_start_soundcard_dispose(GObject *gobject);
void ags_start_soundcard_finalize(GObject *gobject);

void ags_start_soundcard_launch(AgsTask *task);

/**
 * SECTION:ags_start_soundcard
 * @short_description: start soundcard object
 * @title: AgsStartSoundcard
 * @section_id:
 * @include: ags/audio/task/ags_start_soundcard.h
 *
 * The #AgsStartSoundcard task starts soundcard.
 */

static gpointer ags_start_soundcard_parent_class = NULL;

enum{
  PROP_0,
  PROP_APPLICATION_CONTEXT,
};

GType
ags_start_soundcard_get_type()
{
  static GType ags_type_start_soundcard = 0;

  if(!ags_type_start_soundcard){
    static const GTypeInfo ags_start_soundcard_info = {
      sizeof(AgsStartSoundcardClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_start_soundcard_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsStartSoundcard),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_start_soundcard_init,
    };

    ags_type_start_soundcard = g_type_register_static(AGS_TYPE_TASK,
						      "AgsStartSoundcard",
						      &ags_start_soundcard_info,
						      0);
  }
  
  return(ags_type_start_soundcard);
}

void
ags_start_soundcard_class_init(AgsStartSoundcardClass *start_soundcard)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;

  ags_start_soundcard_parent_class = g_type_class_peek_parent(start_soundcard);

  /* gobject */
  gobject = (GObjectClass *) start_soundcard;

  gobject->set_property = ags_start_soundcard_set_property;
  gobject->get_property = ags_start_soundcard_get_property;

  gobject->dispose = ags_start_soundcard_dispose;
  gobject->finalize = ags_start_soundcard_finalize;

  /* properties */
  /**
   * AgsStartSoundcard:application-context:
   *
   * The assigned #AgsApplicationContext
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("application-context",
				   i18n_pspec("application context of start soundcard"),
				   i18n_pspec("The application context of start soundcard task"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_CONTEXT,
				  param_spec);

  /* task */
  task = (AgsTaskClass *) start_soundcard;

  task->launch = ags_start_soundcard_launch;
}

void
ags_start_soundcard_init(AgsStartSoundcard *start_soundcard)
{
  start_soundcard->application_context = NULL;
}

void
ags_start_soundcard_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsStartSoundcard *start_soundcard;

  start_soundcard = AGS_START_SOUNDCARD(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      GObject *application_context;

      application_context = (GObject *) g_value_get_object(value);

      if(start_soundcard->application_context == (GObject *) application_context){
	return;
      }

      if(start_soundcard->application_context != NULL){
	g_object_unref(start_soundcard->application_context);
      }

      if(application_context != NULL){
	g_object_ref(application_context);
      }

      start_soundcard->application_context = (GObject *) application_context;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_start_soundcard_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsStartSoundcard *start_soundcard;

  start_soundcard = AGS_START_SOUNDCARD(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, start_soundcard->application_context);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_start_soundcard_dispose(GObject *gobject)
{
  AgsStartSoundcard *start_soundcard;

  start_soundcard = AGS_START_SOUNDCARD(gobject);

  if(start_soundcard->application_context != NULL){
    g_object_unref(start_soundcard->application_context);

    start_soundcard->application_context = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_start_soundcard_parent_class)->dispose(gobject);
}

void
ags_start_soundcard_finalize(GObject *gobject)
{
  AgsAudioLoop *audio_loop;
  AgsSoundcardThread *soundcard_thread;

  AgsApplicationContext *application_context;
  AgsSoundcard *soundcard;

  application_context = AGS_START_SOUNDCARD(gobject)->application_context;

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
  G_OBJECT_CLASS(ags_start_soundcard_parent_class)->finalize(gobject);
}

void
ags_start_soundcard_launch(AgsTask *task)
{
  AgsStartSoundcard *start_soundcard;

  AgsThread *audio_loop;
  AgsThread *soundcard_thread;

  AgsApplicationContext *application_context;

  start_soundcard = AGS_START_SOUNDCARD(task);

  application_context = start_soundcard->application_context;

  /* get main loop */
  g_object_get(application_context,
	       "main-loop", &audio_loop,
	       NULL);

  soundcard_thread = audio_loop;
  
  while((soundcard_thread = ags_thread_find_type(soundcard_thread,
						 AGS_TYPE_SOUNDCARD_THREAD)) != NULL){
    GObject *soundcard;
    
    g_message("start soundcard");

    g_object_get(soundcard_thread,
		 "soundcard", &soundcard,
		 NULL);
    
    AGS_SOUNDCARD_THREAD(soundcard_thread)->error = NULL;
    ags_soundcard_play_init(AGS_SOUNDCARD(soundcard),
			    &(AGS_SOUNDCARD_THREAD(soundcard_thread)->error));
    
    /* append soundcard thread */
    ags_thread_add_start_queue(audio_loop,
			       soundcard_thread);

    soundcard_thread = g_atomic_pointer_get(&(soundcard_thread->next));
  }
}

/**
 * ags_start_soundcard_new:
 * @application_context: the #AgsApplicationContext
 *
 * Create a new instance of #AgsStartSoundcard.
 *
 * Returns: the new #AgsStartSoundcard.
 *
 * Since: 2.0.0
 */
AgsStartSoundcard*
ags_start_soundcard_new(AgsApplicationContext *application_context)
{
  AgsStartSoundcard *start_soundcard;

  start_soundcard = (AgsStartSoundcard *) g_object_new(AGS_TYPE_START_SOUNDCARD,
						       "application-context", application_context,
						       NULL);

  return(start_soundcard);
}
