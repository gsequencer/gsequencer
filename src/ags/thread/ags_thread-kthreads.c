/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#include <ags/thread/ags_thread-kthreads.h>

#include <linux/futex.h>
#include <sys/time.h>

void ags_thread_class_init(AgsThreadClass *thread);
void ags_thread_tree_iterator_interface_init(AgsTreeIteratorInterface *tree);
void ags_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_thread_stackable_interface_init(AgsStackableInterface *stackable);
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

#define CSIGNAL         0x000000ff      /* signal mask to be sent at exit */
#define CLONE_VM        0x00000100      /* set if VM shared between processes */
#define CLONE_FS        0x00000200      /* set if fs info shared between processes */
#define CLONE_FILES     0x00000400      /* set if open files shared between processes */
#define CLONE_SIGHAND   0x00000800      /* set if signal handlers shared */

static gpointer ags_thread_parent_class = NULL;
static guint thread_signals[LAST_SIGNAL];

__thread AgsThread *ags_thread_self = NULL;

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

    const GInterfaceInfo ags_stackable_interface_info = {
      (GInterfaceInitFunc) ags_thread_stackable_interface_init,
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

    g_type_add_interface_static(ags_type_thread,
				AGS_TYPE_STACKABLE,
				&ags_stackable_interface_info);
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
ags_thread_stackable_interface_init(AgsStackableInterface *stackable)
{
  stackable->push = NULL;
  stackable->pop = NULL;
}

void
ags_thread_init(AgsThread *thread)
{
  g_atomic_int_set(&(thread->flags),
		   0);

  //TODO:JK: implement me
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
ags_thread_finalize(GObject *gobject)
{
  AgsThread *thread;

  thread = AGS_THREAD(gobject);

  if(thread->devout != NULL){
    g_object_unref(G_OBJECT(thread->devout));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_thread_parent_class)->finalize(gobject);
}

void
ags_thread_resume_handler(int sig)
{
#ifdef AGS_DEBUG
  g_message("thread resume\0");
#endif

  g_atomic_int_and(&(ags_thread_self->flags),
		   (~AGS_THREAD_SUSPENDED));

  ags_thread_resume(ags_thread_self);
}

void
ags_thread_suspend_handler(int sig)
{
#ifdef AGS_DEBUG
  g_message("thread suspend\0");
#endif

  if(ags_thread_self == NULL)
    return;

  if ((AGS_THREAD_SUSPENDED & (g_atomic_int_get(&(ags_thread_self->flags)))) != 0) return;

  g_atomic_int_or(&(ags_thread_self->flags),
		  AGS_THREAD_SUSPENDED);

  ags_thread_suspend(ags_thread_self);

  do sigsuspend(&(ags_thread_self->wait_mask)); while ((AGS_THREAD_SUSPENDED & (g_atomic_int_get(&(ags_thread_self->flags)))) != 0);
}

/**
 * ags_thread_set_sync:
 * @thread an #AgsThread
 * @dyntic the dyntic as sync occured.
 * 
 * Unsets AGS_THREAD_WAIT_0, AGS_THREAD_WAIT_1 or AGS_THREAD_WAIT_2.
 * Additionaly the thread is woken up by this function if waiting.
 */
void
ags_thread_set_sync(AgsThread *thread, guint dyntic)
{
  unsigned int flags;

  if(thread == NULL){
    return;
  }

  flags = (unsigned int) atomic_read(&(thread->flags));

  switch(dyntic){
  case 0:
    {
      atomic_set(&(thread->flags),
		 ((~AGS_THREAD_WAIT_0) & flags));
    }
    break;
  case 1:
    {
      atomic_set(&(thread->flags),
		 ((~AGS_THREAD_WAIT_1) & flags));
    }
    break;
  case 2:
    {
      atomic_set(&(thread->flags),
		 ((~AGS_THREAD_WAIT_2) & flags));
    }
    break;
  }
}

/**
 * ags_thread_set_sync:
 * @thread an #AgsThread
 * @tic the tic as sync occured.
 * 
 * Calls ags_thread_set_sync() on all threads.
 */
void
ags_thread_set_sync_all(AgsThread *thread, guint tic)
{
  AgsThread *toplevel;

  auto void ags_thread_set_sync_all_recursive(AgsThread *thread, guint tic);

  void ags_thread_set_sync_all_recursive(AgsThread *thread, guint tic){
    AgsThread *child;

    ags_thread_set_sync(thread, tic);

    child = thread->children;

    while(child != NULL){
      ags_thread_set_sync_all_recursive(child, tic);
      
      child = child->next;
    }
  }

  toplevel = ags_thread_get_toplevel(thread);
  ags_thread_set_sync_all_recursive(toplevel, tic);
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
  AgsAsyncQueue *async_queue;
  AgsThread *audio_loop;
  unsigned int flags;

  audio_loop = ags_thread_get_toplevle(thread);
  async_queue = ags_main_loop_get_async_queue(AGS_MAIN_LOOP(audio_loop));

  flags = (unsigned int) atomic_read(&(thread->flags));

  while(((AGS_THREAD_LOCKED & flags) != 0 &&
	 thread != g_queue_peek_head(async_queue->stack))){
    ags_async_queue_idle(async_queue);
  }

  atomic_inc(&(thread->lock_count));

  if(thread == g_queue_peek_head(async_queue->stack) &&
     (AGS_THREAD_LOCKED & flags) != 0){
    return;
  }
  
  atomic_set(&(thread->flags),
	     (AGS_THREAD_LOCKED | flags));
  atomic_set(&(thread->monitor),
	     1);
  
  futex(&(thread->monitor), FUTEX_WAIT, 1,
	NULL,
	NULL, 0);
}

gboolean
ags_thread_trylock(AgsThread *thread)
{
  //TODO:JK: implement me
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
  AgsAsyncQueue *async_queue;
  AgsThread *audio_loop;
  unsigned int flags;

  audio_loop = ags_thread_get_toplevel(thread);
  async_queue = ags_main_loop_get_async_queue(AGS_MAIN_LOOP(audio_loop));

  flags = (unsigned int) atomic_read(&(thread->flags));

  if((AGS_THREAD_LOCKED & flags) == 0 ||
     thread != g_queue_peek_head(async_queue->stack)){
    return;
  }

  if(atomic_dec_and_test(&(thread->lock_count)) == 0){
    atomic_set(&(thread->flags),
	       ((~AGS_THREAD_LOCKED) & flags));
    atomic_set(&(thread->monitor),
	       0);

    futex(&(thread->monitor), FUTEX_WAKE, 0,
	  NULL,
	  NULL, 0);
  }
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
  if(thread == NULL ||
     child == NULL ||
     child->parent != thread){
    return;
  }
  
  if(child->prev != NULL){
    child->prev->next = child->next;
  }else{
    thread->children = child->next;
  }

  if(child->next != NULL){
    child->next->prev = child->prev;
  }
}

void
ags_thread_add_child(AgsThread *thread, AgsThread *child)
{
  if(thread == NULL ||
     child == NULL){
    return;
  }

  if(thread->children == NULL){
    thread->children = child;
  }else{
    AgsThread *last_sibling;

    last_sibling = ags_thread_last(thread->children);

    last_sibling->next = child;
    child->prev = last_sibling;
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
    if((AGS_THREAD_LOCKED & (atomic_read(&(current->flags)))) != 0){

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
    if((AGS_THREAD_LOCKED & (atomic_read(&(thread->flags)))) != 0){
      return(TRUE);
    }

    thread = thread->next;
  }

  return(FALSE);

}

/**
 * ags_thread_children_is_locked:
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
    AgsThread *child;

    if(thread == NULL){
      return(FALSE);
    }

    if((AGS_THREAD_LOCKED & (atomic_read(&(thread->flags)))) != 0){
      return(TRUE);
    }

    child = thread->children;

    while(child != NULL){
      if(ags_thread_children_is_locked_recursive(child)){
	return(TRUE);
      }

      child = child->next;
    }

    return(FALSE);
  }

  if(thread == NULL){
    return(FALSE);
  }

  return(ags_thread_children_is_locked_recursive(thread));
}

gboolean
ags_thread_is_current_ready(AgsThread *current)
{
  unsigned int flags;

  flags = (unsigned int) atomic_read(&(thread->flags));

  if((AGS_THREAD_READY & flags) != 0 ||
     ((thread->dyntic == 0 && (AGS_THREAD_WAIT_0 & flags) != 0) ||
      (thread->dyntic == 1 && (AGS_THREAD_WAIT_1 & flags) != 0) ||
      (thread->dyntic == 2 && (AGS_THREAD_WAIT_2 & flags) != 0))){
    return(TRUE);
  }else{
    return(FALSE);
  }
}

gboolean
ags_thread_is_tree_ready(AgsThread *thread)
{
  AgsThread *audio_loop;
  gboolean retval;

  auto gboolean ags_thread_is_tree_ready_recursive(AgsThread *current);

  gboolean ags_thread_is_tree_ready_recursive(AgsThread *current){
    AgsThread *children;

    children = current->children;

    if(!ags_thread_is_current_ready(current)){
      return(FALSE);
    }

    while(children != NULL){
      if(!ags_thread_is_tree_ready_recursive(children)){
	return(FALSE);
      }

      children = children->next;
    }

    return(TRUE);
  }

  audio_loop = ags_thread_get_toplevel(thread);

  retval = ags_thread_is_tree_ready_recursive(audio_loop);

  return(retval);
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
    if((AGS_THREAD_WAITING_FOR_CHILDREN & (atomic_read(&(current->flags)))) != 0){
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

    if((AGS_THREAD_WAITING_FOR_SIBLING & (atomic_read(&(thread->flags)))) != 0){
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

      if((AGS_THREAD_WAITING_FOR_PARENT & (atomic_read(&(current->flags)))) != 0){
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
    atomic_or(AGS_THREAD_WAITING_FOR_CHILDREN,
	      &(current->flags));

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
    atomic_or(AGS_THREAD_WAITING_FOR_SIBLING,
	      &(current->flags));

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
      atomic_or(AGS_THREAD_WAITING_FOR_PARENT,
		&(current->flags));
      
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
  AgsAsyncQueue *async_queue;
  AgsThread *audio_loop;
  AgsThread *current;

  if(thread == NULL){
    return;
  }

  audio_loop = ags_thread_get_toplevle(thread);
  async_queue = ags_main_loop_get_async_queue(AGS_MAIN_LOOP(audio_loop));

  current = thread->parent;

  while(current != parent){
    atomic_and((~AGS_THREAD_WAITING_FOR_CHILDREN),
	       &(current->flags));

    if((AGS_THREAD_BROADCAST_PARENT & (atomic_read(&(thread->flags)))) == 0){
      //TODO:JK: implement me
    }else{
      //TODO:JK: implement me
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

    atomic_and((~AGS_THREAD_WAITING_FOR_SIBLING),
	       &(current->flags));

    if((AGS_THREAD_BROADCAST_PARENT & (atomic_read(&(thread->flags)))) == 0){
      //TODO:JK: implement me
    }else{
      //TODO:JK: implement me
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

      atomic_and((~AGS_THREAD_WAITING_FOR_PARENT),
		 &(current->flags));

      if((AGS_THREAD_BROADCAST_PARENT & (atomic_read(&(thread->flags)))) == 0){
	//TODO:JK: implement me
      }else{
	//TODO:JK: implement me
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
  //TODO:JK: implement me
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
  //TODO:JK: implement me
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
  //TODO:JK: implement me
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
  //TODO:JK: implement me
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
  //TODO:JK: implement me
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
  //TODO:JK: implement me
}

void
ags_thread_real_start(AgsThread *thread)
{
  long retval;
  void **newstack;

  /*
   * allocate new stack for subthread
   */
  newstack = (void **) malloc(AGS_THREAD_STACKSIZE);

  if(!newstack)
    return;

  /*
   * Set up the stack for child function, put the (void *)
   * argument on the stack.
   */
  newstack = (void **) (AGS_THREAD_STACKSIZE + (char *) newstack);
  *--newstack = (void *) thread;

  /* clone new thread */
  clone(ags_thread_loop, newstack,
	CLONE_SIGHAND | CLONE_FS | CLONE_VM | CLONE_FILES | CLONE_THREAD,
	NULL);
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
  AgsThread *thread;
  unsigned int flags;

  auto void ags_thread_loop_do_barrier(AgsThread *thread);
  auto void ags_thread_loop_do_cond(AgsThread *thread);

  void ags_thread_loop_do_barrier(AgsThread *thread){
    unsigned int flags;
    int barrier, barrier_count;

    ags_thread_lock(thread);

    flags = (unsigned int) atomic_read(&(thread->flags));
    barrier = atomic_read(&(thread->barrier));
    barrier_count = atomic_read(&(thread->barrier_count));

    if(barrier == 0){
      ags_thread_unlock(thread);

      return;
    }

    atomic_set(&(thread->barrier),
	       barrier++);

    if((AGS_THREAD_RUNNING & flags) != 0){
      if(barrier == barrier_count){
	ags_thread_unlock_all(thread);
      }else{
	while(barrier != barrier_count){
	  ags_thread_suspend(thread);
	}
      }
    }else{
      if(barrier == barrier_count){
	ags_thread_unlock_all(thread);
      }
    }

    ags_thread_unlock(thread);
  }
  void ags_thread_loop_do_cond(AgsThread *thread){
    unsigned int flags;

    ags_thread_lock(thread);

    flags = (unsigned int) atomic_read(&(thread->flags));

    if((AGS_THREAD_RUNNING & flags) != 0){
      if(ags_thread_is_tree_ready(thread)){
	ags_thread_set_sync_all(thread);
      }else{
	while(!ags_thread_is_current_ready(thread)){
	  ags_thread_suspend(thread);
	}
      }
    }else{
      if(ags_thread_is_tree_ready(thread)){
	ags_thread_set_sync_all(thread);
      }
    }

    ags_thread_unlock(thread);
  }

  ags_thread_self =
    thread = AGS_THREAD(ptr);

  flags = (unsigned int) atomic_read(&(thread->flags));
  atomic_set(&(thread->flags),
	     (AGS_THREAD_INITIAL_RUN & flags));

  while((AGS_THREAD_RUNNING & flags) != 0){
    /* do barrier */
    ags_thread_loop_do_barrier(thread);

    /* do cond */
    ags_thread_loop_do_cond(thread);

    /* run */
    if((AGS_THREAD_SKIPPED_BY_TIMELOCK & flags) == 0 ||
       ((AGS_THREAD_SKIP_NON_GREEDY & flags) != 0 &&
	(AGS_THREAD_TIMELOCK_RUN & flags) == 0)){
      ags_thread_run(thread);
    }

    /* lock parent */
    if((AGS_THREAD_WAIT_FOR_PARENT & flags) != 0){
      ags_thread_lock_parent(thread);
    }

    /* lock sibling */
    if((AGS_THREAD_WAIT_FOR_SIBLING & flags) != 0){
      ags_thread_lock_sibling(thread);
    }

    /* lock children */
    if((AGS_THREAD_WAIT_FOR_CHILDREN & flags) != 0){
      ags_thread_lock_children(thread);
    }

    /* read flags */
    flags = (unsigned int) atomic_read(&(thread->flags));
  }

  /* do barrier */
  ags_thread_loop_do_barrier(thread);

  /* do cond */
  ags_thread_loop_do_cond(thread);


  return(NULL);
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
  unsigned int flags;
  int dyntic;

  thread = AGS_THREAD(ptr);

  while((AGS_THREAD_RUNNING & (atomic_read(&(thread->flags)))) != 0){
    nanosleep(&(thread->timed_suspend), NULL);

    flags = (unsigned int) atomic_read(&(thread->flags));

    if(!((AGS_THREAD_WAIT_O & flags) ||
	 (AGS_THREAD_WAIT_1 & flags) ||
	 (AGS_THREAD_WAIT_2 & flags))){
      dyntic = atomic_read(&(thread->dyntic));

      if(dyntic < 0){
	dyntic += 1;
	dyntic *= -1;
      }

      atomic_set(&(thread->dyntic),
		 dyntic);

      switch(dyntic){
      case 0:
      case 1:
	{
	  dyntic += 1;
	}
	break;
      case 2:
	{
	  dyntic = 0;
	}
	break;
      }

      atomic_int_set(&(thread->flags),
		     (AGS_THREAD_READY | flags));
      ags_thread_suspend(thread);

      dyntic *= -1;
      dyntic -= 1;

      atomic_set(&(thread->dyntic),
		 dyntic);
    }
  }
}

void
ags_thread_real_timelock(AgsThread *thread)
{
  //TODO:JK: implement me
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
  atomic_set(&(thread->flags),
	     (atomic_read(&(thread->flags)) & (~AGS_THREAD_RUNNING)));
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
 * ags_thread_hangcheck:
 * @thread the #AgsThread instance
 *
 * Performs hangcheck of thread.
 */
void
ags_thread_hangcheck(AgsThread *thread)
{
  //TODO:JK: implement me
}

/**
 * ags_thread_new:
 * @data an #GObject
 *
 * Create a new #AgsThread you may provide a #gpointer as @data
 * to your thread routine.
 */
AgsThread*
ags_thread_new(gpointer data)
{
  AgsThread *thread;

  thread = (AgsThread *) g_object_new(AGS_TYPE_THREAD,
				      NULL);

  thread->data = data;

  return(thread);
}
