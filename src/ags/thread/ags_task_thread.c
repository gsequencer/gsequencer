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

#include <ags/thread/ags_task_thread.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_devout.h>

#include <math.h>

void ags_task_thread_class_init(AgsTaskThreadClass *task_thread);
void ags_task_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_task_thread_init(AgsTaskThread *task_thread);
void ags_task_thread_connect(AgsConnectable *connectable);
void ags_task_thread_disconnect(AgsConnectable *connectable);
void ags_task_thread_finalize(GObject *gobject);

void ags_task_thread_start(AgsThread *thread);
void ags_task_thread_run(AgsThread *thread);

void* ags_task_thread_append_task_thread(void *ptr);
void* ags_task_thread_append_tasks_thread(void *ptr);

static gpointer ags_task_thread_parent_class = NULL;
static AgsConnectableInterface *ags_task_thread_parent_connectable_interface;

static gboolean DEBUG;

GType
ags_task_thread_get_type()
{
  static GType ags_type_task_thread = 0;

  if(!ags_type_task_thread){
    static const GTypeInfo ags_task_thread_info = {
      sizeof (AgsTaskThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_task_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsTaskThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_task_thread_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_task_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_task_thread = g_type_register_static(AGS_TYPE_THREAD,
						    "AgsTaskThread\0",
						    &ags_task_thread_info,
						    0);
    
    g_type_add_interface_static(ags_type_task_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_task_thread);
}

void
ags_task_thread_class_init(AgsTaskThreadClass *task_thread)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;

  ags_task_thread_parent_class = g_type_class_peek_parent(task_thread);

  /* GObject */
  gobject = (GObjectClass *) task_thread;

  gobject->finalize = ags_task_thread_finalize;

  /* AgsThread */
  thread = (AgsThreadClass *) task_thread;

  thread->start = ags_task_thread_start;
  thread->run = ags_task_thread_run;
}

void
ags_task_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_task_thread_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_task_thread_connect;
  connectable->disconnect = ags_task_thread_disconnect;
}

void
ags_task_thread_init(AgsTaskThread *task_thread)
{
  AgsThread *thread;

  thread = AGS_THREAD(task_thread);

  //  g_atomic_int_or(&thread->flags,
  //		  AGS_THREAD_LOCK_GREEDY_RUN_MUTEX);

  pthread_mutex_init(&(task_thread->read_mutex), NULL);
  pthread_mutex_init(&(task_thread->launch_mutex), NULL);

  task_thread->queued = 0;
  task_thread->pending = 0;

  task_thread->exec = NULL;
  task_thread->queue = NULL;
}

void
ags_task_thread_connect(AgsConnectable *connectable)
{
  ags_task_thread_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_task_thread_disconnect(AgsConnectable *connectable)
{
  ags_task_thread_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_task_thread_finalize(GObject *gobject)
{
  AgsTaskThread *task_thread;

  task_thread = AGS_TASK_THREAD(gobject);

  /* free AgsTask lists */
  ags_list_free_and_unref_link(task_thread->exec);
  ags_list_free_and_unref_link(task_thread->queue);

  /*  */
  G_OBJECT_CLASS(ags_task_thread_parent_class)->finalize(gobject);
}

void
ags_task_thread_start(AgsThread *thread)
{
  if((AGS_THREAD_SINGLE_LOOP & (thread->flags)) == 0){
    AGS_THREAD_CLASS(ags_task_thread_parent_class)->start(thread);
  }
}

void
ags_task_thread_run(AgsThread *thread)
{
  AgsDevout *devout;
  AgsTaskThread *task_thread;
  GList *list;
  static struct timespec play_idle;
  static useconds_t idle;
  guint prev_pending;
  static gboolean initialized = FALSE;

  task_thread = AGS_TASK_THREAD(thread);
  devout = AGS_DEVOUT(thread->devout);

  if(!initialized){
    play_idle.tv_sec = 0;
    play_idle.tv_nsec = 10 * round(sysconf(_SC_CLK_TCK) * (double) AGS_DEVOUT_DEFAULT_BUFFER_SIZE  / (double) AGS_DEVOUT_DEFAULT_SAMPLERATE);
    //    idle = sysconf(_SC_CLK_TCK) * round(sysconf(_SC_CLK_TCK) * (double) AGS_DEVOUT_DEFAULT_BUFFER_SIZE  / (double) AGS_DEVOUT_DEFAULT_SAMPLERATE / 8.0);

    initialized = TRUE;
  }

  /*  */
  if((AGS_THREAD_INITIAL_RUN & (thread->flags)) != 0){
    pthread_mutex_lock(&(thread->start_mutex));

    thread->flags &= (~AGS_THREAD_INITIAL_RUN);
    pthread_cond_broadcast(&(thread->start_cond));

    pthread_mutex_unlock(&(thread->start_mutex));
  }

  /*  */
  pthread_mutex_lock(&(task_thread->read_mutex));

  g_list_free(task_thread->exec);
  list = 
    task_thread->exec = task_thread->queue;
  task_thread->queue = NULL;

  prev_pending = task_thread->pending;
  task_thread->pending = g_list_length(list);
  task_thread->queued -= prev_pending;

  pthread_mutex_unlock(&(task_thread->read_mutex));

  /* launch tasks */
  if(list != NULL){
    AgsTask *task;
    int i;

    pthread_mutex_lock(&(task_thread->launch_mutex));
    pthread_mutex_lock(&(AGS_AUDIO_LOOP(thread->parent)->recall_mutex));

    for(i = 0; i < task_thread->pending; i++){
      task = AGS_TASK(list->data);

      //      g_message("ags_devout_task_thread - launching task: %s\n\0", G_OBJECT_TYPE_NAME(task));

      ags_task_launch(task);

      list = list->next;
    }

    pthread_mutex_unlock(&(AGS_AUDIO_LOOP(thread->parent)->recall_mutex));
    pthread_mutex_unlock(&(task_thread->launch_mutex));
  }
}

void*
ags_task_thread_append_task_thread(void *ptr)
{
  AgsTask *task;
  AgsTaskThread *task_thread;
  AgsTaskThreadAppend *append;
  gboolean initial_wait;
  int ret;

  append = (AgsTaskThreadAppend *) ptr;

  task_thread = append->task_thread;
  task = AGS_TASK(append->data);

  free(append);

  /* lock */
  pthread_mutex_lock(&(task_thread->read_mutex));

  /* append to queue */
  task_thread->queued += 1;

  task_thread->queue = g_list_append(task_thread->queue, task);

  /*  */
  pthread_mutex_unlock(&(task_thread->read_mutex));

  /*  */
  //  g_message("ags_task_thread_append_task_thread ------------------------- %d\0", devout->append_task_suspend);

  /*  */
  pthread_exit(NULL);
}

/**
 * ags_task_thread_append_task:
 * @task_thread an #AgsTaskThread
 * @task an #AgsTask
 *
 * Adds the task to @task_thread.
 */
void
ags_task_thread_append_task(AgsTaskThread *task_thread, AgsTask *task)
{
  AgsTaskThreadAppend *append;
  pthread_t thread;

  append = (AgsTaskThreadAppend *) malloc(sizeof(AgsTaskThreadAppend));

  append->task_thread = task_thread;
  append->data = task;

  pthread_create(&thread, NULL,
		 &ags_task_thread_append_task_thread, append);
}

void*
ags_task_thread_append_tasks_thread(void *ptr)
{
  AgsTask *task;
  AgsTaskThread *task_thread;
  AgsTaskThreadAppend *append;
  GList *list;
  gboolean initial_wait;
  int ret;

  append = (AgsTaskThreadAppend *) ptr;

  task_thread = append->task_thread;
  list = (GList *) append->data;

  free(append);

  /* lock */
  pthread_mutex_lock(&(task_thread->read_mutex));

  /* append to queue */
  task_thread->queued += g_list_length(list);

  task_thread->queue = g_list_concat(task_thread->queue, list);

  /*  */
  pthread_mutex_unlock(&(task_thread->read_mutex));

  /*  */
  pthread_exit(NULL);
}

/**
 * ags_task_thread_append_tasks:
 * @task_thread an #AgsTaskThread
 * @list a GList with #AgsTask as data
 *
 * Concats the list with @task_thread's internal task list. Don't
 * free the list you pass. It will be freed for you.
 */
void
ags_task_thread_append_tasks(AgsTaskThread *task_thread, GList *list)
{
  AgsTaskThreadAppend *append;
  pthread_t thread;

  append = (AgsTaskThreadAppend *) malloc(sizeof(AgsTaskThreadAppend));

  append->task_thread = task_thread;
  append->data = list;

  pthread_create(&thread, NULL,
		 &ags_task_thread_append_tasks_thread, append);
}

AgsTaskThread*
ags_task_thread_new(GObject *devout)
{
  AgsTaskThread *task_thread;

  task_thread = (AgsTaskThread *) g_object_new(AGS_TYPE_TASK_THREAD,
					       "devout\0", devout,
					       NULL);


  return(task_thread);
}
