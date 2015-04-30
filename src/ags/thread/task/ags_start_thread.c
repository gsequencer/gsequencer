/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
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

#include <ags/thread/task/ags_start_thread.h>

#include <ags/object/ags_connectable.h>

void ags_start_thread_class_init(AgsStartThreadClass *start_thread);
void ags_start_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_start_thread_init(AgsStartThread *start_thread);
void ags_start_thread_connect(AgsConnectable *connectable);
void ags_start_thread_disconnect(AgsConnectable *connectable);
void ags_start_thread_finalize(GObject *gobject);

void ags_start_thread_launch(AgsTask *task);

/**
 * SECTION:ags_start_thread
 * @short_description: add audio object to soundcard
 * @title: AgsStartThread
 * @section_id:
 * @include: ags/audio/task/ags_start_thread.h
 *
 * The #AgsStartThread task adds #AgsAudio to #AgsSoundcard.
 */

static gpointer ags_start_thread_parent_class = NULL;
static AgsConnectableInterface *ags_start_thread_parent_connectable_interface;

GType
ags_start_thread_get_type()
{
  static GType ags_type_start_thread = 0;

  if(!ags_type_start_thread){
    static const GTypeInfo ags_start_thread_info = {
      sizeof (AgsStartThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_start_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsStartThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_start_thread_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_start_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_start_thread = g_type_register_static(AGS_TYPE_TASK,
						"AgsStartThread\0",
						&ags_start_thread_info,
						0);
    
    g_type_add_interface_static(ags_type_start_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_start_thread);
}

void
ags_start_thread_class_init(AgsStartThreadClass *start_thread)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_start_thread_parent_class = g_type_class_peek_parent(start_thread);

  /* gobject */
  gobject = (GObjectClass *) start_thread;

  gobject->finalize = ags_start_thread_finalize;

  /* task */
  task = (AgsTaskClass *) start_thread;

  task->launch = ags_start_thread_launch;
}

void
ags_start_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_start_thread_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_start_thread_connect;
  connectable->disconnect = ags_start_thread_disconnect;
}

void
ags_start_thread_init(AgsStartThread *start_thread)
{
  start_thread->parent = NULL;
  start_thread->child = NULL;
}

void
ags_start_thread_connect(AgsConnectable *connectable)
{
  ags_start_thread_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_start_thread_disconnect(AgsConnectable *connectable)
{
  ags_start_thread_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_start_thread_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_start_thread_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_start_thread_launch(AgsTask *task)
{
  AgsStartThread *start_thread;

  guint val;

  g_message("start\0");
    
  start_thread = AGS_START_THREAD(task);

  if(start_thread->child->parent == NULL){
    ags_thread_add_child(start_thread->parent,
			 start_thread->child);
  }else{
    ags_thread_start(start_thread->child);
  }

  /* wait to complete */
  pthread_mutex_lock(&(start_thread->child->start_mutex));    

  val = g_atomic_int_get(&(AGS_THREAD(start_thread->child)->flags));
    
  if((AGS_THREAD_INITIAL_RUN & val) != 0){
    while((AGS_THREAD_INITIAL_RUN & val) != 0){
      pthread_cond_wait(&(AGS_THREAD(start_thread->child)->start_cond),
			&(AGS_THREAD(start_thread->child)->start_mutex));
      val = g_atomic_int_get(&(AGS_THREAD(start_thread->child)->flags));
    }
  }
    
  pthread_mutex_unlock(&(AGS_THREAD(start_thread->child)->start_mutex));
}

/**
 * ags_start_thread_new:
 * @soundcard: the #AgsSoundcard
 * @audio: the #AgsAudio to add
 *
 * Creates an #AgsStartThread.
 *
 * Returns: an new #AgsStartThread.
 *
 * Since: 0.4
 */
AgsStartThread*
ags_start_thread_new(AgsThread *parent,
		     AgsThread *child)
{
  AgsStartThread *start_thread;

  start_thread = (AgsStartThread *) g_object_new(AGS_TYPE_START_THREAD,
						 NULL);

  start_thread->parent = parent;
  start_thread->child = child;

  return(start_thread);
}
