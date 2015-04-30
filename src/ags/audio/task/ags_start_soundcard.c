/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/audio/task/ags_start_soundcard.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_soundcard_thread.h>

void ags_start_soundcard_class_init(AgsStartSoundcardClass *start_soundcard);
void ags_start_soundcard_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_start_soundcard_init(AgsStartSoundcard *start_soundcard);
void ags_start_soundcard_connect(AgsConnectable *connectable);
void ags_start_soundcard_disconnect(AgsConnectable *connectable);
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
static AgsConnectableInterface *ags_start_soundcard_parent_connectable_interface;

GType
ags_start_soundcard_get_type()
{
  static GType ags_type_start_soundcard = 0;

  if(!ags_type_start_soundcard){
    static const GTypeInfo ags_start_soundcard_info = {
      sizeof (AgsStartSoundcardClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_start_soundcard_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsStartSoundcard),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_start_soundcard_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_start_soundcard_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_start_soundcard = g_type_register_static(AGS_TYPE_TASK,
						      "AgsStartSoundcard\0",
						      &ags_start_soundcard_info,
						      0);

    g_type_add_interface_static(ags_type_start_soundcard,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_start_soundcard);
}

void
ags_start_soundcard_class_init(AgsStartSoundcardClass *start_soundcard)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_start_soundcard_parent_class = g_type_class_peek_parent(start_soundcard);

  /* gobject */
  gobject = (GObjectClass *) start_soundcard;

  gobject->finalize = ags_start_soundcard_finalize;

  /* task */
  task = (AgsTaskClass *) start_soundcard;

  task->launch = ags_start_soundcard_launch;
}

void
ags_start_soundcard_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_start_soundcard_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_start_soundcard_connect;
  connectable->disconnect = ags_start_soundcard_disconnect;
}

void
ags_start_soundcard_init(AgsStartSoundcard *start_soundcard)
{
  start_soundcard->soundcard = NULL;
}

void
ags_start_soundcard_connect(AgsConnectable *connectable)
{
  ags_start_soundcard_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_start_soundcard_disconnect(AgsConnectable *connectable)
{
  ags_start_soundcard_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_start_soundcard_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_start_soundcard_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_start_soundcard_launch(AgsTask *task)
{
  AgsStartSoundcard *start_soundcard;
  AgsSoundcard *soundcard;
  AgsAudioLoop *audio_loop;
  AgsSoundcardThread *soundcard_thread;
  guint val;
  GError *error;

  start_soundcard = AGS_START_SOUNDCARD(task);

  soundcard = start_soundcard->soundcard;

  if((AGS_SOUNDCARD_PLAY & (soundcard->flags)) != 0){
    return;
  }

  audio_loop = AGS_AUDIO_LOOP(AGS_APPLICATION_CONTEXT(soundcard->application_context)->main_loop);
  soundcard_thread = ags_thread_find_type(audio_loop,
					  AGS_TYPE_SOUNDCARD_THREAD);

  /* append to AgsSoundcard */
  audio_loop->flags |= (AGS_AUDIO_LOOP_PLAY_AUDIO |
			AGS_AUDIO_LOOP_PLAY_CHANNEL |
			AGS_AUDIO_LOOP_PLAY_RECALL);
  soundcard->flags |= (AGS_SOUNDCARD_START_PLAY |
		       AGS_SOUNDCARD_PLAY);

  error = soundcard_thread->error;
  soundcard_thread->error = NULL;

  ags_thread_start(AGS_THREAD(soundcard_thread));
  
  if((AGS_THREAD_SINGLE_LOOP & (AGS_THREAD(soundcard_thread)->flags)) == 0){
    if(soundcard_thread->error != NULL &&
       error == NULL){
      ags_task_failure(AGS_TASK(start_soundcard), soundcard_thread->error);
    }else{
      pthread_mutex_lock(&(AGS_THREAD(soundcard_thread)->start_mutex));

      val = g_atomic_int_get(&(AGS_THREAD(soundcard_thread)->flags));

      if((AGS_THREAD_INITIAL_RUN & val) != 0){
	while((AGS_THREAD_INITIAL_RUN & val) != 0){
	  pthread_cond_wait(&(AGS_THREAD(soundcard_thread)->start_cond),
			    &(AGS_THREAD(soundcard_thread)->start_mutex));

	  val = g_atomic_int_get(&(AGS_THREAD(soundcard_thread)->flags));
	}
      }
    
      //    ags_thread_unlock(AGS_THREAD(soundcard_thread));
      pthread_mutex_unlock(&(AGS_THREAD(soundcard_thread)->start_mutex));
    }
  }
}

/**
 * ags_start_soundcard_new:
 * @soundcard: the #GObject implementing the #AgsSoundcard interface
 *
 * Creates an #AgsStartSoundcard.
 *
 * Returns: an new #AgsStartSoundcard.
 *
 * Since: 0.4
 */
AgsStartSoundcard*
ags_start_soundcard_new(AgsSoundcard *soundcard)
{
  AgsStartSoundcard *start_soundcard;

  start_soundcard = (AgsStartSoundcard *) g_object_new(AGS_TYPE_START_SOUNDCARD,
						       NULL);

  start_soundcard->soundcard = soundcard;

  return(start_soundcard);
}
