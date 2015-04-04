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

#include <ags/audio/task/ags_start_devout.h>

#include <ags/object/ags_application_context.h>
#include <ags-lib/object/ags_connectable.h>

#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_devout_thread.h>

void ags_start_devout_class_init(AgsStartDevoutClass *start_devout);
void ags_start_devout_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_start_devout_init(AgsStartDevout *start_devout);
void ags_start_devout_connect(AgsConnectable *connectable);
void ags_start_devout_disconnect(AgsConnectable *connectable);
void ags_start_devout_finalize(GObject *gobject);

void ags_start_devout_launch(AgsTask *task);

/**
 * SECTION:ags_start_devout
 * @short_description: start devout object
 * @title: AgsStartDevout
 * @section_id:
 * @include: ags/audio/task/ags_start_devout.h
 *
 * The #AgsStartDevout task starts devout.
 */

static gpointer ags_start_devout_parent_class = NULL;
static AgsConnectableInterface *ags_start_devout_parent_connectable_interface;

GType
ags_start_devout_get_type()
{
  static GType ags_type_start_devout = 0;

  if(!ags_type_start_devout){
    static const GTypeInfo ags_start_devout_info = {
      sizeof (AgsStartDevoutClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_start_devout_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsStartDevout),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_start_devout_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_start_devout_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_start_devout = g_type_register_static(AGS_TYPE_TASK,
						   "AgsStartDevout\0",
						   &ags_start_devout_info,
						   0);

    g_type_add_interface_static(ags_type_start_devout,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_start_devout);
}

void
ags_start_devout_class_init(AgsStartDevoutClass *start_devout)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_start_devout_parent_class = g_type_class_peek_parent(start_devout);

  /* gobject */
  gobject = (GObjectClass *) start_devout;

  gobject->finalize = ags_start_devout_finalize;

  /* task */
  task = (AgsTaskClass *) start_devout;

  task->launch = ags_start_devout_launch;
}

void
ags_start_devout_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_start_devout_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_start_devout_connect;
  connectable->disconnect = ags_start_devout_disconnect;
}

void
ags_start_devout_init(AgsStartDevout *start_devout)
{
  start_devout->devout = NULL;
}

void
ags_start_devout_connect(AgsConnectable *connectable)
{
  ags_start_devout_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_start_devout_disconnect(AgsConnectable *connectable)
{
  ags_start_devout_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_start_devout_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_start_devout_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_start_devout_launch(AgsTask *task)
{
  AgsStartDevout *start_devout;
  AgsDevout *devout;
  AgsAudioLoop *audio_loop;
  AgsDevoutThread *devout_thread;
  guint val;
  GError *error;

  start_devout = AGS_START_DEVOUT(task);

  devout = start_devout->devout;

  if((AGS_DEVOUT_PLAY & (devout->flags)) != 0){
    return;
  }

  audio_loop = AGS_AUDIO_LOOP(AGS_APPLICATION_CONTEXT(devout->application_context)->main_loop);
  devout_thread = AGS_DEVOUT_THREAD(audio_loop->devout_thread);

  /* append to AgsDevout */
  audio_loop->flags |= (AGS_AUDIO_LOOP_PLAY_AUDIO |
			AGS_AUDIO_LOOP_PLAY_CHANNEL |
			AGS_AUDIO_LOOP_PLAY_RECALL);
  devout->flags |= (AGS_DEVOUT_START_PLAY |
		    AGS_DEVOUT_PLAY);

  error = devout_thread->error;
  devout_thread->error = NULL;

  ags_thread_start(AGS_THREAD(devout_thread));
  
  if((AGS_THREAD_SINGLE_LOOP & (AGS_THREAD(devout_thread)->flags)) == 0){
    if(devout_thread->error != NULL &&
       error == NULL){
      ags_task_failure(AGS_TASK(start_devout), devout_thread->error);
    }else{
      pthread_mutex_lock(&(AGS_THREAD(devout_thread)->start_mutex));

      val = g_atomic_int_get(&(AGS_THREAD(devout_thread)->flags));

      if((AGS_THREAD_INITIAL_RUN & val) != 0){
	while((AGS_THREAD_INITIAL_RUN & val) != 0){
	  pthread_cond_wait(&(AGS_THREAD(devout_thread)->start_cond),
			    &(AGS_THREAD(devout_thread)->start_mutex));

	  val = g_atomic_int_get(&(AGS_THREAD(devout_thread)->flags));
	}
      }
    
      //    ags_thread_unlock(AGS_THREAD(devout_thread));
      pthread_mutex_unlock(&(AGS_THREAD(devout_thread)->start_mutex));
    }
  }
}

/**
 * ags_start_devout_new:
 * @devout: the #AgsDevout
 *
 * Creates an #AgsStartDevout.
 *
 * Returns: an new #AgsStartDevout.
 *
 * Since: 0.4
 */
AgsStartDevout*
ags_start_devout_new(AgsDevout *devout)
{
  AgsStartDevout *start_devout;

  start_devout = (AgsStartDevout *) g_object_new(AGS_TYPE_START_DEVOUT,
						 NULL);

  start_devout->devout = devout;

  return(start_devout);
}
