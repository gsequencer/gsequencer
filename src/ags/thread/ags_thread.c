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

#include <ags/thread/ags_thread.h>

#include <ags/object/ags_tree_iterator.h>
#include <ags-lib/object/ags_connectable.h>
#include <ags/object/ags_main_loop.h>

#include <ags/audio/ags_devout.h>

#include <stdio.h>
#include <math.h>

void ags_thread_class_init(AgsThreadClass *thread);
void ags_thread_tree_iterator_interface_init(AgsTreeIteratorInterface *tree);
void ags_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_thread_init(AgsThread *thread);
void ags_thread_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec);
void ags_thread_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec);
void ags_thread_iterate_nested(AgsTreeIterator *tree,
			       gpointer node_id);
void ags_thread_connect(AgsConnectable *connectable);
void ags_thread_disconnect(AgsConnectable *connectable);
void ags_thread_finalize(GObject *gobject);

void ags_thread_resume_handler(int sig);
void ags_thread_suspend_handler(int sig);

void ags_thread_set_devout(AgsThread *thread, GObject *devout);

void ags_thread_real_start(AgsThread *thread);
void* ags_thread_loop(void *ptr);
void ags_thread_real_timelock(AgsThread *thread);
void* ags_thread_timelock_loop(void *ptr);
void ags_thread_real_stop(AgsThread *thread);

enum{
  PROP_0,
  PROP_DEVOUT,
};

enum{
  START,
  RUN,
  SUSPEND,
  RESUME,
  TIMELOCK,
  STOP,
  LAST_SIGNAL,
};

static gpointer ags_thread_parent_class = NULL;
static guint thread_signals[LAST_SIGNAL];

static __thread AgsThread *ags_thread_self = NULL;

GType
ags_thread_get_type()
{
  static GType ags_type_thread = 0;

  if(!ags_type_thread){
    const GTypeInfo ags_thread_info = {
      sizeof (AgsThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_thread_init,
    };

    const GInterfaceInfo ags_tree_iterator_interface_info = {
      (GInterfaceInitFunc) ags_thread_tree_iterator_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_thread = g_type_register_static(G_TYPE_OBJECT,
					     "AgsThread\0",
					     &ags_thread_info,
					     0);
    
    g_type_add_interface_static(ags_type_thread,
				AGS_TYPE_TREE_ITERATOR,
				&ags_tree_iterator_interface_info);
    
    g_type_add_interface_static(ags_type_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return(ags_type_thread);
}

void
ags_thread_class_init(AgsThreadClass *thread)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_thread_parent_class = g_type_class_peek_parent(thread);

  /* GObject */
  gobject = (GObjectClass *) thread;

  gobject->set_property = ags_thread_set_property;
  gobject->get_property = ags_thread_get_property;

  gobject->finalize = ags_thread_finalize;

  /* properties */
  param_spec = g_param_spec_object("devout\0",
				   "devout assigned to\0",
				   "The AgsDevout it is assigned to.\0",
				   AGS_TYPE_DEVOUT,
				   G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVOUT,
				  param_spec);

  /* AgsThread */
  thread->start = ags_thread_real_start;
  thread->run = NULL;
  thread->suspend = NULL;
  thread->resume = NULL;
  thread->timelock = ags_thread_real_timelock;
  thread->stop = ags_thread_real_stop;

  /* signals */
  thread_signals[START] =
    g_signal_new("start\0",
		 G_TYPE_FROM_CLASS (thread),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsThreadClass, start),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  thread_signals[RUN] =
    g_signal_new("run\0",
		 G_TYPE_FROM_CLASS (thread),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsThreadClass, run),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  thread_signals[SUSPEND] =
    g_signal_new("suspend\0",
		 G_TYPE_FROM_CLASS (thread),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsThreadClass, suspend),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  thread_signals[RESUME] =
    g_signal_new("resume\0",
		 G_TYPE_FROM_CLASS (thread),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsThreadClass, resume),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  thread_signals[TIMELOCK] =
    g_signal_new("timelock\0",
		 G_TYPE_FROM_CLASS (thread),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsThreadClass, timelock),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  thread_signals[STOP] =
    g_signal_new("stop\0",
		 G_TYPE_FROM_CLASS (thread),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsThreadClass, stop),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_thread_tree_iterator_interface_init(AgsTreeIteratorInterface *tree)
{
  tree->iterate_nested = ags_thread_iterate_nested;
}

void
ags_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_thread_connect;
  connectable->disconnect = ags_thread_disconnect;
}

void
ags_thread_init(AgsThread *thread)
{
  struct sigaction sa;

  g_atomic_int_set(&(thread->flags),
		   0);

  sigfillset(&thread->wait_mask);
  sigdelset(&thread->wait_mask, AGS_THREAD_SUSPEND_SIG);
  sigdelset(&thread->wait_mask, AGS_THREAD_RESUME_SIG);

  sigfillset(&sa.sa_mask);
  sa.sa_flags = 0;

  sa.sa_handler = ags_thread_resume_handler;
  sigaction(AGS_THREAD_RESUME_SIG, &sa, NULL);

  sa.sa_handler = ags_thread_suspend_handler;
  sigaction(AGS_THREAD_SUSPEND_SIG, &sa, NULL);

  pthread_attr_init(&(thread->thread_attr));

  pthread_mutexattr_init(&(thread->mutexattr));
  pthread_mutexattr_settype(&(thread->mutexattr), PTHREAD_MUTEX_RECURSIVE);

  pthread_mutex_init(&(thread->mutex), &(thread->mutexattr));
  pthread_cond_init(&(thread->cond), NULL);

  pthread_mutex_init(&(thread->start_mutex), NULL);
  pthread_cond_init(&(thread->start_cond), NULL);

  thread->first_barrier = TRUE;
  thread->wait_count[0] = 1;
  thread->wait_count[1] = 1;

  pthread_mutex_init(&(thread->timelock_mutex), NULL);
  pthread_cond_init(&(thread->timelock_cond), NULL);

  pthread_mutex_init(&(thread->greedy_mutex), NULL);
  pthread_cond_init(&(thread->greedy_cond), NULL);
  thread->locked_greedy = 0;

  thread->timelock.tv_sec = 0;
  thread->timelock.tv_nsec = floor(NSEC_PER_SEC /
				   (AGS_AUDIO_LOOP_DEFAULT_JIFFIE + 1));

  thread->greedy_locks = NULL;

  pthread_mutex_init(&(thread->suspend_mutex), NULL);

  thread->devout = NULL;

  thread->parent = NULL;
  thread->next = NULL;
  thread->prev = NULL;
  thread->children = NULL;

  thread->data = NULL;
}

void
ags_thread_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsThread *thread;

  thread = AGS_THREAD(gobject);

  switch(prop_id){
  case PROP_DEVOUT:
    {
      AgsDevout *devout;
      AgsThread *current;

      devout = (AgsDevout *) g_value_get_object(value);

      if(thread->devout != NULL){
	g_object_unref(G_OBJECT(thread->devout));
      }

      if(devout != NULL){
	g_object_ref(G_OBJECT(devout));
      }

      thread->devout = G_OBJECT(devout);

      current = thread->children;

      while(current != NULL){
	g_object_set(G_OBJECT(current),
		     "devout\0", devout,
		     NULL);

	current = current->next;
      }
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_thread_get_property(GObject *gobject,
			guint prop_id,
			GValue *value,
			GParamSpec *param_spec)
{
  AgsThread *thread;

  thread = AGS_THREAD(gobject);

  switch(prop_id){
  case PROP_DEVOUT:
    {
      g_value_set_object(value, G_OBJECT(thread->devout));
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_thread_iterate_nested(AgsTreeIterator *tree,
			  gpointer node_id)
{
  //TODO:JK: implement me
}

void
ags_thread_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_thread_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_thread_finalize(GObject *gobject)
{
  AgsThread *thread;

  thread = AGS_THREAD(gobject);

  pthread_mutex_destroy(&(thread->mutex));
  pthread_cond_destroy(&(thread->cond));

  pthread_mutex_destroy(&(thread->start_mutex));
  pthread_cond_destroy(&(thread->start_cond));

  g_object_unref(G_OBJECT(thread->devout));

  /* call parent */
  G_OBJECT_CLASS(ags_thread_parent_class)->finalize(gobject);
}

void
ags_thread_resume_handler(int sig)
{
  g_atomic_int_and(&ags_thread_self->flags,
		   (~AGS_THREAD_SUSPENDED));

  ags_thread_resume(ags_thread_self);
}

void
ags_thread_suspend_handler(int sig)
{
  if ((AGS_THREAD_SUSPENDED & (g_atomic_int_get(&ags_thread_self->flags))) != 0) return;

  g_atomic_int_or(&ags_thread_self->flags,
		  AGS_THREAD_SUSPENDED);

  ags_thread_suspend(ags_thread_self);

  do sigsuspend(&ags_thread_self->wait_mask); while ((AGS_THREAD_SUSPENDED & (g_atomic_int_get(&ags_thread_self->flags))) != 0);
}

void
ags_thread_set_devout(AgsThread *thread, GObject *devout)
{
  //TODO:JK: implement me
}

/**
 * ags_thread_lock:
 * @thread an #AgsThread
 * 
 * Locks the threads own mutex and sets the appropriate flag.
 */
void
ags_thread_lock(AgsThread *thread)
{
  AgsThread *main_loop;

  if(thread == NULL){
    return;
  }
  
  main_loop = ags_thread_get_toplevel(thread);

  if(main_loop == thread){
    pthread_mutex_lock(&(thread->mutex));
    g_atomic_int_or(&(thread->flags),
		     (AGS_THREAD_LOCKED));
  }else{
    pthread_mutex_lock(&(main_loop->mutex));
    pthread_mutex_lock(&(thread->mutex));
    g_atomic_int_or(&(thread->flags),
		     (AGS_THREAD_LOCKED));
    pthread_mutex_unlock(&(main_loop->mutex));
  }
}

gboolean
ags_thread_trylock(AgsThread *thread)
{
  AgsThread *main_loop;
  guint val;

  if(thread == NULL){
    return(FALSE);
  }
    
  main_loop = ags_thread_get_toplevel(thread);

  if(main_loop == thread){
    if(pthread_mutex_trylock(&(thread->mutex)) != 0){
      return(FALSE);
    }

    g_atomic_int_or(&(thread->flags),
		     (AGS_THREAD_LOCKED));
  }else{
    if(pthread_mutex_trylock(&(main_loop->mutex)) != 0){
      return(FALSE);
    }

    if(pthread_mutex_trylock(&(thread->mutex)) != 0){
      pthread_mutex_unlock(&(main_loop->mutex));
      return(FALSE);
    }

    g_atomic_int_or(&(thread->flags),
		     (AGS_THREAD_LOCKED));
    pthread_mutex_unlock(&(main_loop->mutex));
  }

  return(TRUE);
}

/**
 * ags_thread_unlock:
 * @thread an #AgsThread
 *
 * Unlocks the threads own mutex and unsets the appropriate flag.
 */
void
ags_thread_unlock(AgsThread *thread)
{
  if(thread == NULL){
    return;
  }

  g_atomic_int_and(&(thread->flags),
		   (~AGS_THREAD_LOCKED));

  pthread_mutex_unlock(&(thread->mutex));
}

/**
 * ags_thread_get_toplevel:
 * @thread an #AgsThread
 * Returns: the toplevevel #AgsThread
 *
 * Retrieve toplevel thread.
 */
AgsThread*
ags_thread_get_toplevel(AgsThread *thread)
{
  if(thread == NULL){
    return(NULL);
  }

  while(thread->parent != NULL){
    thread = thread->parent;
  }

  return(thread);
}

/**
 * ags_thread_first:
 * @thread an #AgsThread
 * Returns: the very first #AgsThread within same tree level
 *
 * Retrieve first sibling.
 */
AgsThread*
ags_thread_first(AgsThread *thread)
{
  if(thread == NULL){
    return(NULL);
  }

  while(thread->prev != NULL){
    thread = thread->prev;
  }

  return(thread);
}

/**
 * ags_thread_last:
 * @thread an #AgsThread
 * Returns: the very last @AgsThread within same tree level
 * 
 * Retrieve last sibling.
 */
AgsThread*
ags_thread_last(AgsThread *thread)
{
  if(thread == NULL){
    return(NULL);
  }

  while(thread->next != NULL){
    thread = thread->next;
  }

  return(thread);
}

void
ags_thread_remove_child(AgsThread *thread, AgsThread *child)
{
  if(thread == NULL || child == NULL){
    return;
  }

  if(child->prev != NULL){
    child->prev->next = child->next;
  }

  if(child->next != NULL){
    child->next->prev = child->prev;
  }

  child->parent = NULL;
  child->prev = NULL;
  child->next = NULL;
}

void
ags_thread_add_child(AgsThread *thread, AgsThread *child)
{
  if(thread == NULL || child == NULL){
    return;
  }
  
  if(child->parent != NULL){
    ags_thread_remove_child(child->parent, child);
  }

  /*  */
  if(thread->children == NULL){
    thread->children = child;
    child->parent = thread;
  }else{
    AgsThread *sibling;

    sibling = ags_thread_last(thread->children);

    sibling->next = child;
    child->prev = sibling;
    child->parent = thread;
  }

  if((AGS_THREAD_RUNNING & (g_atomic_int_get(&thread->flags))) != 0){
    ags_thread_start(child);
  }
}

/**
 * ags_thread_parental_is_locked:
 * @thread an #AgsThread
 * @parent where to stop iteration
 * Returns: TRUE if locked otherwise FALSE
 *
 * Check the AGS_THREAD_LOCKED flag in parental levels.
 */
gboolean
ags_thread_parental_is_locked(AgsThread *thread, AgsThread *parent)
{
  AgsThread *current;

  if(thread == NULL){
    return(FALSE);
  }

  current = thread->parent;

  while(current != parent){
    if((AGS_THREAD_LOCKED & (g_atomic_int_get(&current->flags))) != 0){

      return(TRUE);
    }

    current = current->parent;
  }

  return(FALSE);
}

/**
 * ags_thread_sibling_is_locked:
 * @thread an #AgsThread
 * Returns: TRUE if locked otherwise FALSE
 *
 * Check the AGS_THREAD_LOCKED flag within sibling.
 */
gboolean
ags_thread_sibling_is_locked(AgsThread *thread)
{
  if(thread == NULL){
    return(FALSE);
  }

  thread = ags_thread_first(thread);

  while(thread->next != NULL){
    if((AGS_THREAD_LOCKED & (g_atomic_int_get(&thread->flags))) != 0){
      return(TRUE);
    }

    thread = thread->next;
  }

  return(FALSE);
}


/**
 * ags_thread_sibling_is_locked:
 * @thread an #AgsThread
 * Returns: TRUE if locked otherwise FALSE
 *
 * Check the AGS_THREAD_LOCKED flag within sibling.
 */
gboolean
ags_thread_children_is_locked(AgsThread *thread)
{
  auto gboolean ags_thread_children_is_locked_recursive(AgsThread *thread);

  gboolean ags_thread_children_is_locked_recursive(AgsThread *thread){
    AgsThread *current;

    if(thread == NULL){
      return(FALSE);
    }

    current = thread;

    while(current != NULL){
      if((AGS_THREAD_LOCKED & (g_atomic_int_get(&thread->flags))) != 0){
	return(TRUE);
      }

      if(ags_thread_children_is_locked_recursive(current->children)){
	return(TRUE);
      }

      current = current->next;
    }

    return(FALSE);
  }

  if(thread == NULL){
    return(FALSE);
  }

  return(ags_thread_children_is_locked_recursive(thread->children));
}

gboolean
ags_thread_is_current_ready(AgsThread *current)
{
  gint val;

  val = g_atomic_int_get(&(current->flags));

  if((AGS_THREAD_RUNNING & (val)) == 0){
    return(TRUE);
  }

  if((AGS_THREAD_WAIT_0 & (val)) != 0){
    return(TRUE);
  }

  return(FALSE);
}

gboolean
ags_thread_is_tree_ready(AgsThread *thread)
{
  AgsThread *main_loop;

  auto gboolean ags_thread_is_tree_ready_recursive(AgsThread *current);

  gboolean ags_thread_is_tree_ready_recursive(AgsThread *current){

    while(current != NULL){
      if(current == thread){
	return(!ags_thread_is_tree_ready_recursive(current->children));
      }

      if(thread != current && !ags_thread_is_current_ready(current)){
	return(FALSE);
      }

      if(!ags_thread_is_tree_ready_recursive(current->children)){
	return(FALSE);
      }

      current = current->next;
    }

    return(TRUE);
  }

  main_loop = ags_thread_get_toplevel(thread);

  return(ags_thread_is_tree_ready_recursive(main_loop));
}

void
ags_thread_main_loop_unlock_children(AgsThread *thread)
{
  AgsThread *main_loop;

  auto void ags_thread_main_loop_unlock_children_recursive(AgsThread *child);

  void ags_thread_main_loop_unlock_children_recursive(AgsThread *child){
    if(child == NULL){
      return;
    }

    while(child != NULL){
      g_atomic_int_and(&(child->flags),
		       (~AGS_THREAD_WAIT_0));

      ags_thread_main_loop_unlock_children_recursive(child->children);

      if((AGS_THREAD_BROADCAST_PARENT & (g_atomic_int_get(&thread->flags))) == 0){
	pthread_cond_signal(&(child->cond));
      }else{
	pthread_cond_broadcast(&(child->cond));
      }

      child = child->next;
    }
  }

  main_loop = ags_thread_get_toplevel(thread);

  ags_thread_main_loop_unlock_children_recursive(main_loop);
}

/**
 * ags_thread_next_parent_locked:
 * @thread an #AgsThread
 * @parent the parent #AgsThread where to stop.
 * 
 * Retrieve next locked thread above @thread.
 */
AgsThread*
ags_thread_next_parent_locked(AgsThread *thread, AgsThread *parent)
{
  AgsThread *current;

  current = thread->parent;

  while(current != parent){
    if((AGS_THREAD_WAITING_FOR_CHILDREN & (g_atomic_int_get(&current->flags))) != 0){
      return(current);
    }

    current = current->parent;
  }

  return(NULL);
}

/**
 * ags_thread_next_sibling_locked:
 * @thread an #AgsThread
 *
 * Retrieve next locked thread neighbooring @thread
 */
AgsThread*
ags_thread_next_sibling_locked(AgsThread *thread)
{
  AgsThread *current;

  current = ags_thread_first(thread);

  while(current != NULL){
    if(current == thread){
      current = current->next;
      
      continue;
    }

    if((AGS_THREAD_WAITING_FOR_SIBLING & (g_atomic_int_get(&thread->flags))) != 0){
      return(current);
    }

    current = current->next;
  }

  return(NULL);
}

/**
 * ags_thread_next_children_locked:
 * @thread an #AgsThread
 * 
 * Retrieve next locked thread following @thread
 */
AgsThread*
ags_thread_next_children_locked(AgsThread *thread)
{
  auto AgsThread* ags_thread_next_children_locked_recursive(AgsThread *thread);

  AgsThread* ags_thread_next_children_locked_recursive(AgsThread *child){
    AgsThread *current;

    current = ags_thread_last(child);

    while(current != NULL){
      ags_thread_next_children_locked_recursive(current->children);

      if((AGS_THREAD_WAITING_FOR_PARENT & (g_atomic_int_get(&current->flags))) != 0){
	return(current);
      }

      current = current->prev;
    }

    return(NULL);
  }

  return(ags_thread_next_children_locked(thread->children));
}

/**
 * ags_thread_lock_parent:
 * @thread an #AgsThread
 * @parent the parent #AgsThread where to stop.
 *
 * Lock parent tree structure.
 */
void
ags_thread_lock_parent(AgsThread *thread, AgsThread *parent)
{
  AgsThread *current;

  if(thread == NULL){
    return;
  }

  ags_thread_lock(thread);

  current = thread->parent;

  while(current != parent){
    ags_thread_lock(current);
    g_atomic_int_or(&current->flags,
		    AGS_THREAD_WAITING_FOR_CHILDREN);

    current = current->parent;
  }
}

/**
 * ags_thread_lock_sibling:
 * @thread an #AgsThread
 *
 * Lock sibling tree structure.
 */
void
ags_thread_lock_sibling(AgsThread *thread)
{
  AgsThread *current;

  if(thread == NULL){
    return;
  }

  ags_thread_lock(thread);

  current = ags_thread_first(thread);

  while(current != NULL){
    if(current == thread){
      current = current->next;
    
      continue;
    }

    ags_thread_lock(current);
    g_atomic_int_or(&current->flags,
		    AGS_THREAD_WAITING_FOR_SIBLING);

    current = current->next;
  }
}

/**
 * ags_thread_lock_children:
 * @thread an #AgsThread
 *
 * Lock child tree structure.
 */
void
ags_thread_lock_children(AgsThread *thread)
{
  auto void ags_thread_lock_children_recursive(AgsThread *child);
  
  void ags_thread_lock_children_recursive(AgsThread *child){
    AgsThread *current;

    current = ags_thread_last(child);

    while(current != NULL){
      ags_thread_lock_children_recursive(current->children);

      ags_thread_lock(current);
      g_atomic_int_or(&current->flags,
		      AGS_THREAD_WAITING_FOR_PARENT);
      
      current = current->prev;
    }
  }

  ags_thread_lock(thread);
  
  ags_thread_lock_children_recursive(thread->children);
}

void
ags_thread_lock_all(AgsThread *thread)
{
  ags_thread_lock_parent(thread, NULL);
  ags_thread_lock_sibling(thread);
  ags_thread_lock_children(thread);
}

/**
 * ags_thread_unlock_parent:
 * @thread an #AgsThread
 * @parent the parent #AgsThread where to stop.
 *
 * Unlock parent tree structure.
 */
void
ags_thread_unlock_parent(AgsThread *thread, AgsThread *parent)
{
  AgsThread *current;

  if(thread == NULL){
    return;
  }

  current = thread->parent;

  while(current != parent){
    g_atomic_int_and(&current->flags,
		     (~AGS_THREAD_WAITING_FOR_CHILDREN));

    if((AGS_THREAD_BROADCAST_PARENT & (g_atomic_int_get(&thread->flags))) == 0){
      pthread_cond_signal(&(current->cond));
    }else{
      pthread_cond_broadcast(&(current->cond));
    }

    ags_thread_unlock(current);

    current = current->parent;
  }
}

/**
 * ags_thread_unlock_sibling:
 * @thread an #AgsThread
 *
 * Unlock sibling tree structure.
 */
void
ags_thread_unlock_sibling(AgsThread *thread)
{
  AgsThread *current;

  if(thread == NULL){
    return;
  }

  current = ags_thread_first(thread);

  while(current != NULL){
    if(current == thread){
      current = current->next;
    
      continue;
    }

    g_atomic_int_and(&current->flags,
		     (~AGS_THREAD_WAITING_FOR_SIBLING));

    if((AGS_THREAD_BROADCAST_SIBLING & (g_atomic_int_get(&thread->flags))) == 0){
      pthread_cond_signal(&(current->cond));
    }else{
      pthread_cond_broadcast(&(current->cond));
    }

    ags_thread_unlock(current);

    current = current->next;
  }
}

/**
 * ags_thread_unlock_children:
 * @thread an #AgsThread
 *
 * Unlock child tree structure.
 */
void
ags_thread_unlock_children(AgsThread *thread)
{
  auto void ags_thread_unlock_children_recursive(AgsThread *child);
  
  void ags_thread_unlock_children_recursive(AgsThread *child){
    AgsThread *current;

    if(child == NULL){
      return;
    }

    current = ags_thread_last(child);

    while(current != NULL){
      ags_thread_unlock_children_recursive(current->children);

      g_atomic_int_and(&current->flags,
		       (~AGS_THREAD_WAITING_FOR_PARENT));

      if(!((AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&thread->flags))) != 0 &&
	   AGS_IS_AUDIO_LOOP(thread))){

	if((AGS_THREAD_BROADCAST_CHILDREN & (g_atomic_int_get(&thread->flags))) == 0){
	  pthread_cond_signal(&(current->cond));
	}else{
	  pthread_cond_broadcast(&(current->cond));
	}
      }

      ags_thread_unlock(current);

      current = current->prev;
    }
  }
  
  ags_thread_unlock_children_recursive(thread->children);
}

void
ags_thread_unlock_all(AgsThread *thread)
{
  ags_thread_unlock_parent(thread, NULL);
  ags_thread_unlock_sibling(thread);
  ags_thread_unlock_children(thread);
}

/**
 * ags_thread_wait_parent:
 * @thread an #AgsThread
 * @parent the parent #AgsThread where to stop.
 *
 * Wait on parent tree structure.
 */
void
ags_thread_wait_parent(AgsThread *thread, AgsThread *parent)
{
  AgsThread *current;

  if(thread == NULL || thread == parent){
    return;
  }

  /* wait parent */
  current = thread->parent;
    
  while((current != NULL && current != parent) &&
	(((AGS_THREAD_IDLE & (g_atomic_int_get(&current->flags))) != 0 ||
	  (AGS_THREAD_WAITING_FOR_CHILDREN & (g_atomic_int_get(&current->flags))) == 0) ||
	 current->parent != parent)){
    pthread_cond_wait(&(current->cond),
		      &(current->mutex));

    if(!((AGS_THREAD_IDLE & (current->flags)) != 0 ||
	 (AGS_THREAD_WAITING_FOR_CHILDREN & (g_atomic_int_get(&current->flags))) == 0)){
      current = current->parent;
    }
  }

  /* unset flag */
  g_atomic_int_and(&thread->flags,
		   (~AGS_THREAD_WAITING_FOR_PARENT));
}

/**
 * ags_thread_wait_sibling:
 * @thread an #AgsThread
 *
 * Wait on sibling tree structure.
 */
void
ags_thread_wait_sibling(AgsThread *thread)
{
  AgsThread *current;

  if(thread == NULL){
    return;
  }

  /* wait sibling */
  current = ags_thread_first(thread);
  
  while(current != NULL &&
	(((AGS_THREAD_IDLE & (g_atomic_int_get(&current->flags))) != 0 ||
	  (AGS_THREAD_WAITING_FOR_SIBLING & (g_atomic_int_get(&current->flags))) == 0) ||
	 current->next != NULL)){
    if(current == thread){
      current = current->next;
      
      continue;
    }
    
    pthread_cond_wait(&(current->cond),
		      &(current->mutex));

    if(!((AGS_THREAD_IDLE & (g_atomic_int_get(&current->flags))) != 0 ||
	 (AGS_THREAD_WAITING_FOR_SIBLING & (g_atomic_int_get(&current->flags))) == 0)){
      current = current->next;
    }
  }

  /* unset flags */
  g_atomic_int_and(&thread->flags,
		   (~AGS_THREAD_WAITING_FOR_SIBLING));
}

/**
 * ags_thread_wait_children:
 * @thread an #AgsThread
 *
 * Wait on child tree structure.
 */
void
ags_thread_wait_children(AgsThread *thread)
{
  auto void ags_thread_wait_children_recursive(AgsThread *child);
  
  void ags_thread_wait_children_recursive(AgsThread *child){
    gboolean initial_run;

    if(child == NULL){
      return;
    }

    initial_run = TRUE;

    while(child != NULL &&
	  (((AGS_THREAD_IDLE & (g_atomic_int_get(&child->flags))) != 0 ||
	    (AGS_THREAD_WAITING_FOR_PARENT & (g_atomic_int_get(&child->flags))) == 0) ||
	   child->next != NULL)){
      if(initial_run){
	ags_thread_wait_children_recursive(child->children);

	initial_run = FALSE;
      }

      pthread_cond_wait(&(child->cond),
			&(child->mutex));
     
      if(!((AGS_THREAD_IDLE & (g_atomic_int_get(&child->flags))) != 0 ||
	   (AGS_THREAD_WAITING_FOR_PARENT & (g_atomic_int_get(&child->flags))) == 0)){
	child = child->next;

	initial_run = TRUE;
      }
    }
  }

  if(thread == NULL){
    return;
  }

  /* wait children */
  ags_thread_wait_children_recursive(thread->children);

  /* unset flags */
  g_atomic_int_and(&thread->flags,
		   (~AGS_THREAD_WAITING_FOR_CHILDREN));
}

/**
 * ags_thread_signal_parent:
 * @thread an #AgsThread
 * @broadcast whether to perforam a signal or to broadcast
 *
 * Signals the tree in higher levels.
 */
void
ags_thread_signal_parent(AgsThread *thread, AgsThread *parent,
			 gboolean broadcast)
{
  AgsThread *current;

  current = thread->parent;

  while(current != NULL){
    if((AGS_THREAD_WAIT_FOR_CHILDREN & (g_atomic_int_get(&current->flags))) != 0){
      if(!broadcast){
	pthread_cond_signal(&(current->cond));
      }else{
	pthread_cond_broadcast(&(current->cond));
      }
    }

    current = current->parent;
  }
}

/**
 * ags_thread_signal_sibling:
 * @thread an #AgsThread
 * @broadcast whether to perforam a signal or to broadcast
 *
 * Signals the tree on same level.
 */
void
ags_thread_signal_sibling(AgsThread *thread, gboolean broadcast)
{
  AgsThread *current;

  current = ags_thread_first(thread);

  while(current != NULL){
    if((AGS_THREAD_WAIT_FOR_SIBLING & (g_atomic_int_get(&current->flags))) != 0){
      if(!broadcast){
	pthread_cond_signal(&(current->cond));
      }else{
	pthread_cond_broadcast(&(current->cond));
      }
    }
  }
}

/**
 * ags_thread_signal_children:
 * @thread an #AgsThread
 * @broadcast whether to perforam a signal or to broadcast
 *
 * Signals the tree in lower levels.
 */
void
ags_thread_signal_children(AgsThread *thread, gboolean broadcast)
{
  auto void ags_thread_signal_children_recursive(AgsThread *thread, gboolean broadcast);

  void ags_thread_signal_children_recursive(AgsThread *thread, gboolean broadcast){
    AgsThread *current;

    if(thread == NULL){
      return;
    }

    current = thread;

    while(current != NULL){
      if((AGS_THREAD_WAIT_FOR_PARENT & (g_atomic_int_get(&current->flags))) != 0){
	if(!broadcast){
	  pthread_cond_signal(&(current->cond));
	}else{
	  pthread_cond_broadcast(&(current->cond));
	}
      }
      
      ags_thread_signal_children_recursive(current, broadcast);

      current = current->next;
    }
  }

  ags_thread_signal_children(thread->children, broadcast);
}

void
ags_thread_real_start(AgsThread *thread)
{
  guint val;

  if(thread == NULL){
    return;
  }

  /* */
  val = g_atomic_int_get(&(thread->flags));
  
  if((AGS_THREAD_TIMELOCK_RUN & val) != 0){
    pthread_create(&(thread->timelock_thread), NULL,
		   &(ags_thread_timelock_loop), thread);
  }

  /*  */
  pthread_create(&(thread->thread), &(thread->thread_attr),
		 &(ags_thread_loop), thread);
}

/**
 * ags_thread_start:
 * @thread the #AgsThread instance
 *
 * Start the thread.
 */
void
ags_thread_start(AgsThread *thread)
{
  g_return_if_fail(AGS_IS_THREAD(thread));

  g_object_ref(G_OBJECT(thread));
  g_signal_emit(G_OBJECT(thread),
		thread_signals[START], 0);
  g_object_unref(G_OBJECT(thread));
}

void*
ags_thread_loop(void *ptr)
{
  AgsThread *thread, *main_loop;
  gboolean is_in_sync;
  gboolean wait_for_parent, wait_for_sibling, wait_for_children;
  guint current_tic;
  guint val, running, locked_greedy;

  auto void ags_thread_loop_sync(AgsThread *thread);

  void ags_thread_loop_sync(AgsThread *thread){
    guint tic;

    if(!ags_thread_is_tree_ready(thread)){
      g_atomic_int_or(&(thread->flags),
		      AGS_THREAD_WAIT_0);

      ags_thread_unlock(main_loop);

      if(thread != main_loop){
	if(tic == 0 && current_tic == 2){
	  return;
	}else if(tic == 1 && current_tic == 0){
	  return;
	}else if(tic == 2 && current_tic == 1){ 
	  return;
	}    
      }
    
      while(!ags_thread_is_current_ready(thread)){
	pthread_cond_wait(&(thread->cond),
			  &(thread->mutex));

	tic = ags_main_loop_get_tic(AGS_MAIN_LOOP(main_loop));
      }
    }else{
      guint next_tic;

      tic = ags_main_loop_get_tic(AGS_MAIN_LOOP(main_loop));

      ags_thread_unlock(main_loop);

      if(tic = 2){
	next_tic = 0;
      }else if(tic = 0){
	next_tic = 1;
      }else if(tic = 1){
	next_tic = 2;
      }

      ags_main_loop_set_tic(AGS_MAIN_LOOP(main_loop), next_tic);
      ags_thread_main_loop_unlock_children(main_loop);
      ags_main_loop_set_last_sync(AGS_MAIN_LOOP(main_loop), tic);
    }
  }

  ags_thread_self =
    thread = AGS_THREAD(ptr);

  main_loop = ags_thread_get_toplevel(thread);

  /*  */
  ags_thread_lock(main_loop);

  current_tic = ags_main_loop_get_tic(AGS_MAIN_LOOP(main_loop));

  g_atomic_int_or(&(thread->flags),
		  (AGS_THREAD_RUNNING |
		   AGS_THREAD_INITIAL_RUN));
  
  ags_thread_unlock(main_loop);

  running = g_atomic_int_get(&(thread->flags));

  while((AGS_THREAD_RUNNING & running) != 0){

    /* barrier */
    if((AGS_THREAD_WAITING_FOR_BARRIER & (g_atomic_int_get(&thread->flags))) != 0){
      int wait_count;

      if(thread->first_barrier){
	/* retrieve wait count */
	ags_thread_lock(thread);

	wait_count = thread->wait_count[0];

	ags_thread_unlock(thread);

	/* init and wait */
	pthread_barrier_init(&(thread->barrier[0]), NULL, wait_count);
	pthread_barrier_wait(&(thread->barrier[0]));
      }else{
	/* retrieve wait count */
	ags_thread_lock(thread);

	wait_count = thread->wait_count[1];

	ags_thread_unlock(thread);

	/* init and wait */
	pthread_barrier_init(&(thread->barrier[1]), NULL, wait_count);
	pthread_barrier_wait(&(thread->barrier[1]));
      }
    }

    /* run in hierarchy */
    ags_thread_lock(main_loop);
    ags_thread_lock(thread);

    ags_thread_loop_sync(thread);

    /*  */
    running = g_atomic_int_get(&(thread->flags));
    
    /* */
    switch(current_tic){
    case 2:
      {
	current_tic = 0;
	break;
      }
    case 1:
      {
	current_tic = 2;
	break;
      }
    case 0:
      {
	current_tic = 1;
	break;
      }
    }

    /* set idle flag */
    g_atomic_int_or(&(thread->flags),
		    AGS_THREAD_IDLE);

    if((AGS_THREAD_WAIT_FOR_PARENT & (g_atomic_int_get(&thread->flags))) != 0){
      wait_for_parent = TRUE;

      g_atomic_int_or(&thread->flags,
		      AGS_THREAD_WAITING_FOR_PARENT);
      ags_thread_lock_parent(thread, NULL);
    }else{
      wait_for_parent = FALSE;
    }

    /* lock sibling */
    if((AGS_THREAD_WAIT_FOR_SIBLING & (thread->flags)) != 0){
      wait_for_sibling = TRUE;

      g_atomic_int_or(&thread->flags,
		      AGS_THREAD_WAITING_FOR_SIBLING);
      ags_thread_lock_sibling(thread);
    }else{
      wait_for_sibling = FALSE;
    }

    /* lock_children */
    if((AGS_THREAD_WAIT_FOR_CHILDREN & (thread->flags)) != 0){
      wait_for_children = TRUE;

      g_atomic_int_or(&thread->flags,
		      AGS_THREAD_WAITING_FOR_CHILDREN);
      ags_thread_lock_children(thread);
    }else{
      wait_for_children = FALSE;
    }

    /* skip very first sync of AgsAudioLoop */
    if(!((AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&thread->flags))) != 0 &&
	 AGS_IS_AUDIO_LOOP(thread))){

      /* wait parent */
      if(wait_for_parent){
	ags_thread_wait_parent(thread, NULL);
      }

      /* wait sibling */
      if(wait_for_sibling){
	ags_thread_wait_sibling(thread);
      }

      /* wait children */
      if(wait_for_children){
	ags_thread_wait_children(thread);
      }
    }

    ags_thread_unlock(thread);

    /* check for greedy to announce */
    if(thread->greedy_locks != NULL){
      GList *greedy_locks;

      greedy_locks = thread->greedy_locks;
      
      while(greedy_locks != NULL){
	pthread_mutex_lock(&(AGS_THREAD(greedy_locks->data)->greedy_mutex));

	locked_greedy = g_atomic_int_get(&(AGS_THREAD(greedy_locks->data)->locked_greedy));
	locked_greedy++;

	g_atomic_int_set(&(AGS_THREAD(greedy_locks->data)->locked_greedy),
			 locked_greedy);

	pthread_mutex_unlock(&(AGS_THREAD(greedy_locks->data)->greedy_mutex));

	greedy_locks = greedy_locks->next;
      }
    }

    /* greedy work around */
    pthread_mutex_lock(&(thread->greedy_mutex));

    locked_greedy = g_atomic_int_get(&thread->locked_greedy);

    if(locked_greedy != 0){
      while(locked_greedy != 0){
	pthread_cond_wait(&(thread->greedy_cond),
			  &(thread->greedy_mutex));
	
	locked_greedy = g_atomic_int_get(&thread->locked_greedy);
      }
    }

    pthread_mutex_unlock(&(thread->greedy_mutex));


    /* */
    pthread_mutex_lock(&(thread->timelock_mutex));

    val = g_atomic_int_get(&(thread->flags));

    if((AGS_THREAD_TIMELOCK_RUN & val) != 0){
      guint locked;

      locked = g_atomic_int_get(&(thread->flags));
				
      g_atomic_int_and(&(thread->flags),
		       (~AGS_THREAD_TIMELOCK_WAIT));

      if((AGS_THREAD_TIMELOCK_WAIT & locked) != 0){	
       	pthread_cond_signal(&(thread->timelock_cond));
      }
    }

    pthread_mutex_unlock(&(thread->timelock_mutex));

    /* run */
    ags_thread_run(thread);

    /* check for greedy to release */
    if(thread->greedy_locks != NULL){
      GList *greedy_locks;

      greedy_locks = thread->greedy_locks;
      
      while(greedy_locks != NULL){
	pthread_mutex_lock(&(AGS_THREAD(greedy_locks->data)->greedy_mutex));

	locked_greedy = g_atomic_int_get(&AGS_THREAD(greedy_locks->data)->locked_greedy);
	locked_greedy--;

	g_atomic_int_set(&(AGS_THREAD(greedy_locks->data)->locked_greedy),
			 locked_greedy);

	pthread_cond_signal(&(AGS_THREAD(greedy_locks->data)->greedy_cond));

	pthread_mutex_unlock(&(AGS_THREAD(greedy_locks->data)->greedy_mutex));

	greedy_locks = greedy_locks->next;
      }
    }

    /**/
    ags_thread_lock(thread);

    /* unset idle flag */
    g_atomic_int_and(&thread->flags,
		     (~AGS_THREAD_IDLE));

    /* unlock parent */
    if(wait_for_parent){
      ags_thread_unlock_parent(thread, NULL);
    }

    /* unlock sibling */
    if(wait_for_sibling){
      ags_thread_unlock_sibling(thread);
    }

    /* unlock children */
    if(wait_for_children){
      ags_thread_unlock_children(thread);
    }

    /* unset initial run */
    if((AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&thread->flags))) != 0){
      g_atomic_int_and(&thread->flags,
		       (~AGS_THREAD_INITIAL_RUN));

      /* signal AgsAudioLoop */
      if(AGS_IS_TASK_THREAD(thread)){
	pthread_cond_signal(&(thread->start_cond));
      }
    }

    ags_thread_unlock(thread);
  }

  pthread_exit(NULL);
}

/**
 * ags_thread_run:
 * @thread the #AgsThread instance
 * 
 * Only for internal use of ags_thread_loop but you may want to set the your very own
 * class function namely your thread's routine.
 */
void
ags_thread_run(AgsThread *thread)
{
  g_return_if_fail(AGS_IS_THREAD(thread));

  g_object_ref(G_OBJECT(thread));
  g_signal_emit(G_OBJECT(thread),
		thread_signals[RUN], 0);
  g_object_unref(G_OBJECT(thread));
}

void
ags_thread_suspend(AgsThread *thread)
{
  g_return_if_fail(AGS_IS_THREAD(thread));

  g_object_ref(G_OBJECT(thread));
  g_signal_emit(G_OBJECT(thread),
		thread_signals[SUSPEND], 0);
  g_object_unref(G_OBJECT(thread));
}

void
ags_thread_resume(AgsThread *thread)
{
  g_return_if_fail(AGS_IS_THREAD(thread));

  g_object_ref(G_OBJECT(thread));
  g_signal_emit(G_OBJECT(thread),
		thread_signals[RESUME], 0);
  g_object_unref(G_OBJECT(thread));
}

void*
ags_thread_timelock_loop(void *ptr)
{
  AgsThread *thread;
  int retval;
  guint val;

  thread = AGS_THREAD(ptr);

  val = g_atomic_int_get(&(thread->flags));
  
  pthread_mutex_lock(&thread->timelock_mutex);

  g_atomic_int_or(&(thread->flags),
		  AGS_THREAD_TIMELOCK_WAIT);

  while((AGS_THREAD_RUNNING & (val)) != 0){

    g_atomic_int_or(&(thread->flags),
		    AGS_THREAD_TIMELOCK_WAIT);

    val = g_atomic_int_get(&(thread->flags));

    while((AGS_THREAD_TIMELOCK_WAIT & (val)) != 0){
      retval = pthread_cond_wait(&(thread->timelock_cond),
				 &(thread->timelock_mutex));

      val = g_atomic_int_get(&(thread->flags));
    }

    nanosleep(&(thread->timelock), NULL);

    val = g_atomic_int_get(&(thread->flags));

    if((AGS_THREAD_WAIT_0 & val) != 0){
      //g_message("realtime\0");
    }else{
      g_message("======== timelock ========\0");
      ags_thread_timelock(thread);
    }

    val = g_atomic_int_get(&(thread->flags));
  }

  pthread_mutex_unlock(&thread->timelock_mutex);
}

void
ags_thread_real_timelock(AgsThread *thread)
{
  AgsThread *main_loop;
  GList *greedy_locks;
  guint locked_greedy, val;

  pthread_mutex_lock(&(thread->greedy_mutex));

#if defined(AGS_PTHREAD_SUSPEND) || defined(AGS_LINUX_SIGNALS)
  val = g_atomic_int_get(&(thread->flags));

  if((AGS_THREAD_SKIP_NON_GREEDY & val) != 0){
#endif
    /*
     * bad choice because throughput will suffer but it's your only choice as long
     * pthread_suspend and pthread_resume will be missing.
     */

    ags_thread_lock(main_loop);
    greedy_locks = thread->greedy_locks;

    while(greedy_locks != NULL){
      pthread_mutex_lock(&(AGS_THREAD(greedy_locks->data)->greedy_mutex));
      
      g_atomic_int_or(&(AGS_THREAD(greedy_locks->data)->flags),
		      (AGS_THREAD_WAIT_0 | AGS_THREAD_SKIPPED_BY_TIMELOCK));

      pthread_mutex_unlock(&(AGS_THREAD(greedy_locks->data)->greedy_mutex));

      greedy_locks = greedy_locks->next;
    }

    ags_thread_unlock(main_loop);

    ags_thread_lock(main_loop);

    if(!ags_thread_is_tree_ready(thread)){
      g_atomic_int_or(&(thread->flags),
		      AGS_THREAD_WAIT_0);

      ags_thread_unlock(main_loop);

      while(!ags_thread_is_current_ready(thread)){
	pthread_cond_wait(&(thread->cond),
			  &(thread->greedy_mutex));
      }
    }else{
      guint tic, next_tic;

      /* you have the tic that's why you don't have to cheat */
      tic = ags_main_loop_get_tic(AGS_MAIN_LOOP(main_loop));

      ags_thread_unlock(main_loop);

      if(tic = 2){
	next_tic = 0;
      }else if(tic = 0){
	next_tic = 1;
      }else if(tic = 1){
	next_tic = 2;
      }

      ags_main_loop_set_tic(AGS_MAIN_LOOP(main_loop), next_tic);
      ags_thread_main_loop_unlock_children(main_loop);
      ags_main_loop_set_last_sync(AGS_MAIN_LOOP(main_loop), tic);
    }
#if defined(AGS_PTHREAD_SUSPEND) || defined(AGS_LINUX_SIGNALS)
  }else{
    g_atomic_int_or(&(thread->flags),
		    AGS_THREAD_TIMELOCK_RESUME);
    
    /* throughput is mandatory */
#ifdef AGS_PTHREAD_SUSPEND
    pthread_suspend(&(thread->thread));
#else
    pthread_kill(thread_id, AGS_THREAD_SUSPEND_SIG);
#endif

    /* allow non greedy to continue */
    greedy_locks = thread->greedy_locks;

    while(greedy_locks != NULL){
      pthread_mutex_lock(&(AGS_THREAD(greedy_locks->data)->greedy_mutex));
    
      locked_greedy = g_atomic_int_get(&AGS_THREAD(greedy_locks->data)->locked_greedy);

      locked_greedy--;

      g_atomic_int_set(&(AGS_THREAD(greedy_locks->data)->locked_greedy),
		       locked_greedy);

      pthread_cond_signal(&(AGS_THREAD(greedy_locks->data)->greedy_cond));

      pthread_mutex_unlock(&(AGS_THREAD(greedy_locks->data)->greedy_mutex));

      greedy_locks = greedy_locks->next;
    }

    /* skip syncing for greedy */
    main_loop = ags_thread_get_toplevel(thread);

    ags_thread_lock(main_loop);

    if(!ags_thread_is_tree_ready(thread)){
      g_atomic_int_or(&(thread->flags),
		      AGS_THREAD_WAIT_0);

      ags_thread_unlock(main_loop);
    
      while(!ags_thread_is_current_ready(thread)){
	pthread_cond_wait(&(thread->cond),
			  &(thread->greedy_mutex));
      }
    }else{
      guint tic, next_tic;

      /* you have the tic that's why you don't have to skip */
      tic = ags_main_loop_get_tic(AGS_MAIN_LOOP(main_loop));

      ags_thread_unlock(main_loop);

      if(tic = 2){
	next_tic = 0;
      }else if(tic = 0){
	next_tic = 1;
      }else if(tic = 1){
	next_tic = 2;
      }

      ags_main_loop_set_tic(AGS_MAIN_LOOP(main_loop), next_tic);
      ags_thread_main_loop_unlock_children(main_loop);
      ags_main_loop_set_last_sync(AGS_MAIN_LOOP(main_loop), tic);
    }

    /* your chance */
#ifdef AGS_PTHREAD_SUSPEND
    pthread_resume(&(thread->thread));
#else
    pthread_kill(thread_id, AGS_THREAD_RESUME_SIG);
#endif
  }
#endif
  pthread_mutex_unlock(&(thread->greedy_mutex));
}

void
ags_thread_timelock(AgsThread *thread)
{
  g_return_if_fail(AGS_IS_THREAD(thread));

  g_object_ref(G_OBJECT(thread));
  g_signal_emit(G_OBJECT(thread),
		thread_signals[TIMELOCK], 0);
  g_object_unref(G_OBJECT(thread));
}

void
ags_thread_real_stop(AgsThread *thread)
{
  g_atomic_int_and(&(thread->flags),
		   (~AGS_THREAD_RUNNING));
}

/**
 * ags_thread_stop:
 * @thread the #AgsThread instance
 * 
 * Stop the threads loop by unsetting AGS_THREAD_RUNNING flag.
 */
void
ags_thread_stop(AgsThread *thread)
{
  g_return_if_fail(AGS_IS_THREAD(thread));

  g_object_ref(G_OBJECT(thread));
  g_signal_emit(G_OBJECT(thread),
		thread_signals[STOP], 0);
  g_object_unref(G_OBJECT(thread));
}

/**
 * ags_thread_new:
 * @data an #GObject
 *
 * Create a new #AgsThread you may provide an #GObject as @data
 * to your thread routine.
 */
AgsThread*
ags_thread_new(GObject *data)
{
  AgsThread *thread;

  thread = (AgsThread *) g_object_new(AGS_TYPE_THREAD,
				      NULL);

  thread->data = data;

  return(thread);
}
