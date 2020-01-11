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

#include <ags/audio/task/ags_start_soundcard.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_soundcard_thread.h>
#include <ags/audio/thread/ags_export_thread.h>

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
};

GType
ags_start_soundcard_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_start_soundcard = 0;

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

    g_once_init_leave(&g_define_type_id__volatile, ags_type_start_soundcard);
  }

  return g_define_type_id__volatile;
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

  /* task */
  task = (AgsTaskClass *) start_soundcard;

  task->launch = ags_start_soundcard_launch;
}

void
ags_start_soundcard_init(AgsStartSoundcard *start_soundcard)
{
  //empty
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

  /* call parent */
  G_OBJECT_CLASS(ags_start_soundcard_parent_class)->dispose(gobject);
}

void
ags_start_soundcard_finalize(GObject *gobject)
{
  AgsStartSoundcard *start_soundcard;

  start_soundcard = AGS_START_SOUNDCARD(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_start_soundcard_parent_class)->finalize(gobject);
}

void
ags_start_soundcard_launch(AgsTask *task)
{
  AgsStartSoundcard *start_soundcard;

  AgsThread *audio_loop;
  AgsThread *soundcard_thread;
  AgsThread *export_thread;
  AgsThread *next_thread;

  AgsApplicationContext *application_context;

  start_soundcard = AGS_START_SOUNDCARD(task);

  application_context = ags_application_context_get_instance();

  g_return_if_fail(AGS_IS_CONCURRENCY_PROVIDER(application_context));

  /* get main loop */
  audio_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  soundcard_thread = ags_thread_find_type(audio_loop,
					  AGS_TYPE_SOUNDCARD_THREAD);
  
  while(soundcard_thread != NULL){
    if(AGS_IS_SOUNDCARD_THREAD(soundcard_thread)){
      GObject *soundcard;
    
      guint soundcard_capability;

      g_message("start soundcard");

      g_object_get(soundcard_thread,
		   "soundcard", &soundcard,
		   NULL);
    
      soundcard_capability = ags_soundcard_get_capability(AGS_SOUNDCARD(soundcard));

      AGS_SOUNDCARD_THREAD(soundcard_thread)->error = NULL;

      if(soundcard_capability == AGS_SOUNDCARD_CAPABILITY_PLAYBACK){
	ags_soundcard_play_init(AGS_SOUNDCARD(soundcard),
				&(AGS_SOUNDCARD_THREAD(soundcard_thread)->error));
      }else if(soundcard_capability == AGS_SOUNDCARD_CAPABILITY_CAPTURE){
	ags_soundcard_record_init(AGS_SOUNDCARD(soundcard),
				  &(AGS_SOUNDCARD_THREAD(soundcard_thread)->error));
      }
    
      /* append soundcard thread */
      ags_thread_add_start_queue(audio_loop,
				 soundcard_thread);

      g_object_unref(soundcard);
    }

    /* iterate */
    next_thread = ags_thread_next(soundcard_thread);

    g_object_unref(soundcard_thread);
    
    soundcard_thread = next_thread;
  }

  export_thread = ags_thread_find_type(audio_loop,
				       AGS_TYPE_EXPORT_THREAD);
  
  while(export_thread != NULL){
    if(AGS_IS_EXPORT_THREAD(export_thread)){
      GObject *export;
    
      guint export_capability;

      g_message("start export");

      /* append export thread */
      ags_thread_add_start_queue(audio_loop,
				 export_thread);
    }
    
    /* iterate */
    next_thread = ags_thread_next(export_thread);

    g_object_unref(export_thread);
    
    export_thread = next_thread;
  }

  /* unref */
  g_object_unref(audio_loop);
}

/**
 * ags_start_soundcard_new:
 *
 * Create a new instance of #AgsStartSoundcard.
 *
 * Returns: the new #AgsStartSoundcard.
 *
 * Since: 3.0.0
 */
AgsStartSoundcard*
ags_start_soundcard_new()
{
  AgsStartSoundcard *start_soundcard;

  start_soundcard = (AgsStartSoundcard *) g_object_new(AGS_TYPE_START_SOUNDCARD,
						       NULL);

  return(start_soundcard);
}
