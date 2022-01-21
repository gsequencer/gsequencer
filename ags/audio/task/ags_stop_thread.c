/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/audio/task/ags_stop_thread.h>

#include <ags/i18n.h>

void ags_stop_thread_class_init(AgsStopThreadClass *stop_thread);
void ags_stop_thread_init(AgsStopThread *stop_thread);
void ags_stop_thread_dispose(GObject *gobject);
void ags_stop_thread_finalize(GObject *gobject);

void ags_stop_thread_recursive(AgsThread *thread);
void ags_stop_thread_launch(AgsTask *task);

/**
 * SECTION:ags_stop_thread
 * @short_description: stop sound config
 * @title: AgsStopThread
 * @section_id:
 * @include: ags/audio/task/ags_stop_thread.h
 *
 * The #AgsStopThread task stop the specified sound config.
 */

static gpointer ags_stop_thread_parent_class = NULL;

GType
ags_stop_thread_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_stop_thread = 0;

    static const GTypeInfo ags_stop_thread_info = {
      sizeof(AgsStopThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_stop_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsStopThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_stop_thread_init,
    };

    ags_type_stop_thread = g_type_register_static(AGS_TYPE_TASK,
						  "AgsStopThread",
						  &ags_stop_thread_info,
						  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_stop_thread);
  }

  return g_define_type_id__volatile;
}

void
ags_stop_thread_class_init(AgsStopThreadClass *stop_thread)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;
  
  ags_stop_thread_parent_class = g_type_class_peek_parent(stop_thread);

  /* GObjectClass */
  gobject = (GObjectClass *) stop_thread;

  gobject->dispose = ags_stop_thread_dispose;
  gobject->finalize = ags_stop_thread_finalize;

  /* properties */

  /* AgsTaskClass */
  task = (AgsTaskClass *) stop_thread;

  task->launch = ags_stop_thread_launch;
}

void
ags_stop_thread_init(AgsStopThread *stop_thread)
{
  //empty
}

void
ags_stop_thread_dispose(GObject *gobject)
{
  AgsStopThread *stop_thread;

  stop_thread = AGS_STOP_THREAD(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_stop_thread_parent_class)->dispose(gobject);
}

void
ags_stop_thread_finalize(GObject *gobject)
{
  AgsStopThread *stop_thread;

  stop_thread = AGS_STOP_THREAD(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_stop_thread_parent_class)->finalize(gobject);
}

void
ags_stop_thread_recursive(AgsThread *thread)
{
  AgsThread *child, *next;

  child = ags_thread_children(thread);

  while(child != NULL){
    ags_stop_thread_recursive(child);

    /* iterate */
    next = ags_thread_next(child);

    g_object_unref(child);

    child = next;
  }
  
  ags_thread_stop(thread);
}

void
ags_stop_thread_launch(AgsTask *task)
{
  AgsThread *audio_loop;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  audio_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  ags_stop_thread_recursive(audio_loop);
}

/**
 * ags_stop_thread_new:
 *
 * Creates a new instance of #AgsStopThread.
 *
 * Returns: the new #AgsStopThread.
 *
 * Since: 3.16.10
 */
AgsStopThread*
ags_stop_thread_new()
{
  AgsStopThread *stop_thread;

  stop_thread = (AgsStopThread *) g_object_new(AGS_TYPE_STOP_THREAD,
					       NULL);

  return(stop_thread);
}
