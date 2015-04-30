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

#include <ags/thread/ags_iterator_thread.h>

#include <ags/object/ags_marshal.h>
#include <ags/object/ags_connectable.h>

#include <ags/thread/ags_recycling_thread.h>

void ags_iterator_thread_class_init(AgsIteratorThreadClass *iterator_thread);
void ags_iterator_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_iterator_thread_init(AgsIteratorThread *iterator_thread);
void ags_iterator_thread_connect(AgsConnectable *connectable);
void ags_iterator_thread_disconnect(AgsConnectable *connectable);
void ags_iterator_thread_finalize(GObject *gobject);

void ags_iterator_thread_start(AgsThread *thread);
void ags_iterator_thread_run(AgsThread *thread);

void ags_iterator_thread_real_children_ready(AgsIteratorThread *iterator_thread,
					     AgsThread *current);

enum{
  CHILDREN_READY,
  LAST_SIGNAL,
};

static gpointer ags_iterator_thread_parent_class = NULL;
static AgsConnectableInterface *ags_iterator_thread_parent_connectable_interface;

static guint iterator_thread_signals[LAST_SIGNAL];

GType
ags_iterator_thread_get_type()
{
  static GType ags_type_iterator_thread = 0;

  if(!ags_type_iterator_thread){
    static const GTypeInfo ags_iterator_thread_info = {
      sizeof (AgsIteratorThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_iterator_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsIteratorThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_iterator_thread_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_iterator_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_iterator_thread = g_type_register_static(AGS_TYPE_THREAD,
						      "AgsIteratorThread\0",
						      &ags_iterator_thread_info,
						      0);
    
    g_type_add_interface_static(ags_type_iterator_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_iterator_thread);
}

void
ags_iterator_thread_class_init(AgsIteratorThreadClass *iterator_thread)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;

  ags_iterator_thread_parent_class = g_type_class_peek_parent(iterator_thread);

  /* GObject */
  gobject = (GObjectClass *) iterator_thread;

  gobject->finalize = ags_iterator_thread_finalize;

  /* AgsThread */
  thread = (AgsThreadClass *) iterator_thread;

  thread->start = ags_iterator_thread_start;

  /* AgsIteratorThread */
  iterator_thread->children_ready = ags_iterator_thread_real_children_ready;

  /* signals */
  iterator_thread_signals[CHILDREN_READY] = 
    g_signal_new("children_ready\0",
		 G_TYPE_FROM_CLASS(iterator_thread),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsIteratorThreadClass, children_ready),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__OBJECT_OBJECT,
		 G_TYPE_BOOLEAN, 2,
		 G_TYPE_OBJECT,
		 G_TYPE_OBJECT);
}

void
ags_iterator_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_iterator_thread_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_iterator_thread_connect;
  connectable->disconnect = ags_iterator_thread_disconnect;
}

void
ags_iterator_thread_init(AgsIteratorThread *iterator_thread)
{
  iterator_thread->flags = 0;

  pthread_mutex_init(&(iterator_thread->tic_mutex), NULL);
  pthread_cond_init(&(iterator_thread->tic_cond), NULL);

  iterator_thread->recycling_thread = NULL;

  iterator_thread->channel = NULL;
  iterator_thread->recall_id = 0;
  iterator_thread->stage = 0;
}

void
ags_iterator_thread_connect(AgsConnectable *connectable)
{
  AgsThread *thread;

  thread = AGS_THREAD(connectable);

  ags_iterator_thread_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_iterator_thread_disconnect(AgsConnectable *connectable)
{
  ags_iterator_thread_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_iterator_thread_finalize(GObject *gobject)
{
  AgsIteratorThread *iterator_thread;

  iterator_thread = AGS_ITERATOR_THREAD(gobject);

  /*  */
  G_OBJECT_CLASS(ags_iterator_thread_parent_class)->finalize(gobject);
}

void
ags_iterator_thread_start(AgsThread *thread)
{
  AGS_THREAD_CLASS(ags_iterator_thread_parent_class)->start(thread);
}

void
ags_iterator_thread_run(AgsThread *thread)
{
  AgsIteratorThread *iterator_thread;

  iterator_thread = AGS_ITERATOR_THREAD(thread);

  /*  */
  pthread_mutex_lock(&(iterator_thread->tic_mutex));

  iterator_thread->flags &= (~AGS_ITERATOR_THREAD_DONE);

  if((AGS_ITERATOR_THREAD_WAIT & (iterator_thread->flags)) != 0 &&
     (AGS_ITERATOR_THREAD_DONE & (iterator_thread->flags)) == 0){
    while((AGS_ITERATOR_THREAD_WAIT & (iterator_thread->flags)) != 0 &&
	  (AGS_ITERATOR_THREAD_DONE & (iterator_thread->flags)) == 0){
      pthread_cond_wait(&(iterator_thread->tic_cond),
			&(iterator_thread->tic_mutex));
    }
  }

  iterator_thread->flags |= AGS_ITERATOR_THREAD_WAIT;

  pthread_mutex_unlock(&(iterator_thread->tic_mutex));

  /*  */
  AGS_THREAD_CLASS(ags_iterator_thread_parent_class)->run(thread);

  ags_channel_recursive_play_threaded(iterator_thread->channel,
				      iterator_thread->recall_id,
				      iterator_thread->stage);
}

void
ags_iterator_thread_real_children_ready(AgsIteratorThread *iterator_thread,
					AgsThread *current)
{
  AgsRecyclingThread *recycling_thread;

  recycling_thread = AGS_RECYCLING_THREAD(current);

  pthread_mutex_lock(&(recycling_thread->iteration_mutex));

  recycling_thread->flags &= (~AGS_RECYCLING_THREAD_WAIT);
  pthread_cond_signal(&(recycling_thread->iteration_cond));

  pthread_mutex_unlock(&(recycling_thread->iteration_mutex));
}

void
ags_iterator_thread_children_ready(AgsIteratorThread *iterator_thread,
				   AgsThread *current)
{
  g_return_if_fail(AGS_IS_ITERATOR_THREAD(iterator_thread));

  g_object_ref((GObject *) iterator_thread);
  g_signal_emit(G_OBJECT(iterator_thread),
		iterator_thread_signals[CHILDREN_READY], 0,
		current);
  g_object_unref((GObject *) iterator_thread);
}

AgsIteratorThread*
ags_iterator_thread_new(AgsChannel *channel,
			AgsRecallID *recall_id,
			gint stage)
{
  AgsIteratorThread *iterator_thread;
  
  iterator_thread = (AgsIteratorThread *) g_object_new(AGS_TYPE_ITERATOR_THREAD,
						       NULL);

  iterator_thread->channel = channel;
  iterator_thread->recall_id = recall_id;
  iterator_thread->stage = stage;

  return(iterator_thread);
}
