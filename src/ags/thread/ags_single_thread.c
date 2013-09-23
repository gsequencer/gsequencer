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

#include <ags/thread/ags_single_thread.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_devout.h>

void ags_single_thread_class_init(AgsSingleThreadClass *single_thread);
void ags_single_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_single_thread_init(AgsSingleThread *single_thread);
void ags_single_thread_connect(AgsConnectable *connectable);
void ags_single_thread_disconnect(AgsConnectable *connectable);
void ags_single_thread_finalize(GObject *gobject);

void ags_single_thread_start(AgsThread *thread);
void ags_single_thread_run(AgsThread *thread);
void ags_single_thread_stop(AgsThread *thread);

static gpointer ags_single_thread_parent_class = NULL;
static AgsConnectableInterface *ags_single_thread_parent_connectable_interface;

GType
ags_single_thread_get_type()
{
  static GType ags_type_single_thread = 0;

  if(!ags_type_single_thread){
    static const GTypeInfo ags_single_thread_info = {
      sizeof (AgsSingleThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_single_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSingleThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_single_thread_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_single_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_single_thread = g_type_register_static(AGS_TYPE_THREAD,
						    "AgsSingleThread\0",
						    &ags_single_thread_info,
						    0);
    
    g_type_add_interface_static(ags_type_single_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_single_thread);
}

void
ags_single_thread_class_init(AgsSingleThreadClass *single_thread)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;

  ags_single_thread_parent_class = g_type_class_peek_parent(single_thread);

  /* GObject */
  gobject = (GObjectClass *) single_thread;

  gobject->finalize = ags_single_thread_finalize;

  /* AgsThread */
  thread = (AgsThreadClass *) single_thread;

  thread->start = ags_single_thread_start;
  thread->run = ags_single_thread_run;
  thread->stop = ags_single_thread_stop;
}

void
ags_single_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_single_thread_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_single_thread_connect;
  connectable->disconnect = ags_single_thread_disconnect;
}

void
ags_single_thread_init(AgsSingleThread *single_thread)
{
  AgsThread *thread;
  AgsDevout *devout;

  thread = AGS_THREAD(single_thread);

  devout = AGS_DEVOUT(thread->devout);
  devout->flags |= AGS_DEVOUT_NONBLOCKING;

  single_thread->audio_loop = ags_audio_loop_new(G_OBJECT(devout));
  AGS_THREAD(single_thread->audio_loop)->flags |= AGS_THREAD_SINGLE_LOOP;

  single_thread->task_thread = AGS_TASK_THREAD(devout->audio_loop->task_thread);
  AGS_THREAD(single_thread->task_thread)->flags |= AGS_THREAD_SINGLE_LOOP;

  single_thread->devout_thread = AGS_DEVOUT_THREAD(devout->audio_loop->devout_thread);
  AGS_THREAD(single_thread->task_thread)->flags |= AGS_THREAD_SINGLE_LOOP;

  single_thread->gui_thread = AGS_GUI_THREAD(devout->audio_loop->gui_thread);
  AGS_THREAD(single_thread->gui_thread)->flags |= AGS_THREAD_SINGLE_LOOP;
  single_thread->gui_thread->frequency = 1.0 / (double) AGS_SINGLE_THREAD_DEFAULT_GUI_JIFFIE;
}

void
ags_single_thread_connect(AgsConnectable *connectable)
{
  ags_single_thread_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_single_thread_disconnect(AgsConnectable *connectable)
{
  ags_single_thread_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_single_thread_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_single_thread_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_single_thread_start(AgsThread *thread)
{
  AgsSingleThread *single_thread;

  single_thread = AGS_SINGLE_THREAD(thread);

  ags_thread_start((AgsThread *) single_thread->audio_loop);

  ags_thread_start((AgsThread *) single_thread->task_thread);

  ags_thread_start((AgsThread *) single_thread->devout_thread);

  ags_thread_start((AgsThread *) single_thread->gui_thread);
}

void
ags_single_thread_run(AgsThread *thread)
{
  AgsSingleThread *single_thread;
  struct timespec play_start, play_exceeded, play_idle, current;

  single_thread = AGS_SINGLE_THREAD(thread);

  play_idle.tv_sec = 0;
  play_idle.tv_nsec = (double) NSEC_PER_SEC / (double) AGS_DEVOUT_DEFAULT_SAMPLERATE * (double) AGS_DEVOUT_DEFAULT_BUFFER_SIZE;

  while((AGS_THREAD_RUNNING & (g_atomic_int_get(&thread->flags))) != 0){
    /* initial value to calculate timing */
    clock_gettime(CLOCK_MONOTONIC, &play_start);

    /*  */
    ags_thread_run((AgsThread *) single_thread->audio_loop);

    ags_thread_run((AgsThread *) single_thread->task_thread);

    ags_thread_run((AgsThread *) single_thread->devout_thread);

    ags_thread_run((AgsThread *) single_thread->gui_thread);

    /* do timing */
    clock_gettime(CLOCK_MONOTONIC, &play_exceeded);

    if(play_start.tv_sec < play_exceeded.tv_sec){
      play_exceeded.tv_nsec += NSEC_PER_SEC;
      play_exceeded.tv_sec--;
    }

    if(play_start.tv_sec < play_exceeded.tv_sec){
      continue;
    }
    
    /* calculate timing */
    current.tv_sec = 0;
    current.tv_nsec = play_idle.tv_nsec - (play_exceeded.tv_nsec + play_start.tv_nsec);

    nanosleep(&current, NULL);
  }
}

void
ags_single_thread_stop(AgsThread *thread)
{
  AgsSingleThread *single_thread;

  single_thread = AGS_SINGLE_THREAD(thread);

  ags_thread_stop((AgsThread *) single_thread->audio_loop);

  ags_thread_stop((AgsThread *) single_thread->task_thread);

  ags_thread_stop((AgsThread *) single_thread->devout_thread);

  ags_thread_stop((AgsThread *) single_thread->gui_thread);
}

AgsSingleThread*
ags_single_thread_new(GObject *single)
{
  AgsSingleThread *single_thread;

  single_thread = (AgsSingleThread *) g_object_new(AGS_TYPE_SINGLE_THREAD,
						   NULL);


  return(single_thread);
}


