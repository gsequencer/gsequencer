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

#include <ags/thread/ags_portable_thread.h>

#include <ags/object/ags_tree_iterator.h>
#include <ags-lib/object/ags_connectable.h>
#include <ags/object/ags_main_loop.h>

#include <ags/audio/ags_devout.h>

#include <stdio.h>
#include <math.h>

void ags_portable_thread_class_init(AgsPortableThreadClass *thread);
void ags_portable_thread_tree_iterator_interface_init(AgsTreeIteratorInterface *tree);
void ags_portable_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_portable_thread_init(AgsPortableThread *thread);
void ags_portable_thread_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec);
void ags_portable_thread_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec);
void ags_portable_thread_iterate_nested(AgsTreeIterator *tree,
			       gpointer node_id);
void ags_portable_thread_connect(AgsConnectable *connectable);
void ags_portable_thread_disconnect(AgsConnectable *connectable);
void ags_portable_thread_finalize(GObject *gobject);

void ags_portable_thread_set_devout(AgsPortableThread *thread, GObject *devout);

void ags_portable_thread_real_start(AgsPortableThread *thread);
void* ags_portable_thread_loop(void *ptr);
void ags_portable_thread_real_run(AgsPortableThread *thread);
void ags_portable_thread_real_timelock(AgsPortableThread *thread);
void* ags_portable_thread_timelock_loop(void *ptr);
void ags_portable_thread_real_stop(AgsPortableThread *thread);

enum{
  PROP_0,
  PROP_DEVOUT,
};

enum{
  START,
  RUN,
  TIMELOCK,
  STOP,
  LAST_SIGNAL,
};

static gpointer ags_portable_thread_parent_class = NULL;
static guint thread_signals[LAST_SIGNAL];

#define NSEC_PER_SEC    (1000000000) /* The number of nsecs per sec. */

GType
ags_portable_thread_get_type()
{
  static GType ags_type_thread = 0;

  if(!ags_type_thread){
    const GTypeInfo ags_portable_thread_info = {
      sizeof (AgsPortableThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_portable_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPortableThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_portable_thread_init,
    };

    const GInterfaceInfo ags_tree_iterator_interface_info = {
      (GInterfaceInitFunc) ags_portable_thread_tree_iterator_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_portable_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_thread = g_type_module_register(G_TYPE_OBJECT,
					     "AgsPortableThread\0",
					     &ags_portable_thread_info,
					     0);
    
    g_type_module_add_interface(ags_type_thread,
				AGS_TYPE_TREE_ITERATOR,
				&ags_tree_iterator_interface_info);

    g_type_module_add_interface(ags_type_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_thread);
}

void
ags_portable_thread_class_init(AgsPortableThreadClass *thread)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_portable_thread_parent_class = g_type_class_peek_parent(thread);

  /* GObject */
  gobject = (GObjectClass *) thread;

  gobject->set_property = ags_portable_thread_set_property;
  gobject->get_property = ags_portable_thread_get_property;

  gobject->finalize = ags_portable_thread_finalize;

  /* properties */
  param_spec = g_param_spec_object("devout\0",
				   "devout assigned to\0",
				   "The AgsDevout it is assigned to.\0",
				   AGS_TYPE_DEVOUT,
				   G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVOUT,
				  param_spec);

  /* AgsPortableThread */
  thread->start = ags_portable_thread_real_start;
  thread->run = NULL;
  thread->timelock = ags_portable_thread_real_timelock;
  thread->stop = ags_portable_thread_real_stop;

  /* signals */
  thread_signals[START] =
    g_signal_new("start\0",
		 G_TYPE_FROM_CLASS (thread),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsPortableThreadClass, start),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  thread_signals[RUN] =
    g_signal_new("run\0",
		 G_TYPE_FROM_CLASS (thread),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsPortableThreadClass, run),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  thread_signals[TIMELOCK] =
    g_signal_new("timelock\0",
		 G_TYPE_FROM_CLASS (thread),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsPortableThreadClass, timelock),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  thread_signals[STOP] =
    g_signal_new("stop\0",
		 G_TYPE_FROM_CLASS (thread),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsPortableThreadClass, stop),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_portable_thread_tree_iterator_interface_init(AgsTreeIteratorInterface *tree)
{
  tree->iterate_nested = ags_portable_thread_iterate_nested;
}

void
ags_portable_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_portable_thread_connect;
  connectable->disconnect = ags_portable_thread_disconnect;
}

void
ags_portable_thread_init(AgsPortableThread *thread)
{
  g_atomic_int_set(&(thread->flags),
		   0);

  pth_attr_init(&thread->thread_attr);

  pth_mutex_init(&thread->mutex);
  pth_cond_init(&thread->cond);

  pth_mutex_init(&thread->start_mutex);
  pth_cond_init(&thread->start_cond);

  thread->first_barrier = TRUE;
  thread->wait_count[0] = 1;
  thread->wait_count[1] = 1;

  pth_mutex_init(&thread->timelock_mutex);
  pth_cond_init(&thread->timelock_cond);
  thread->timelock.tv_sec = 0;
  thread->timelock.tv_nsec = floor(NSEC_PER_SEC *
				   ((double) AGS_DEVOUT_DEFAULT_SAMPLERATE / (double) AGS_DEVOUT_DEFAULT_BUFFER_SIZE));

  thread->devout = NULL;

  thread->parent = NULL;
  thread->next = NULL;
  thread->prev = NULL;
  thread->children = NULL;

  thread->data = NULL;
}

void
ags_portable_thread_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsPortableThread *thread;

  thread = AGS_PORTABLE_THREAD(gobject);

  switch(prop_id){
  case PROP_DEVOUT:
    {
      AgsDevout *devout;
      AgsPortableThread *current;

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
ags_portable_thread_get_property(GObject *gobject,
			guint prop_id,
			GValue *value,
			GParamSpec *param_spec)
{
  AgsPortableThread *thread;

  thread = AGS_PORTABLE_THREAD(gobject);

  switch(prop_id){
  case PROP_DEVOUT:
    {
      g_value_set_object(value, G_OBJECT(thread->devout));
    }
    break;
  }
}

void
ags_portable_thread_iterate_nested(AgsTreeIterator *tree,
			  gpointer node_id)
{
  //TODO:JK: implement me
}

void
ags_portable_thread_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_portable_thread_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_portable_thread_finalize(GObject *gobject)
{
  AgsPortableThread *thread;

  thread = AGS_PORTABLE_THREAD(gobject);

  pth_mutex_destroy(&(thread->mutex));
  pth_cond_destroy(&(thread->cond));

  pth_mutex_destroy(&(thread->start_mutex));
  pth_cond_destroy(&(thread->start_cond));

  g_object_unref(G_OBJECT(thread->devout));

  /* call parent */
  G_OBJECT_CLASS(ags_portable_thread_parent_class)->finalize(gobject);
}

void
ags_portable_thread_set_devout(AgsPortableThread *thread, GObject *devout)
{
  //TODO:JK: implement me
}

/**
 * ags_portable_thread_lock:
 * @thread an #AgsPortableThread
 * 
 * Locks the threads own mutex and sets the appropriate flag.
 */
void
ags_portable_thread_lock(AgsPortableThread *thread)
{
  AgsPortableThread *main_loop;

  if(thread == NULL){
    return;
  }

  main_loop = ags_portable_thread_get_toplevel(thread);

  if(main_loop == thread){
    pth_mutex_acquire(&(thread->mutex), FALSE, NULL);
    g_atomic_int_or(&(thread->flags),
		     (AGS_PORTABLE_THREAD_LOCKED));
  }else{
    pth_mutex_acquire(&(main_loop->mutex), FALSE, NULL);
    pth_mutex_acquire(&(thread->mutex), FALSE, NULL);
    g_atomic_int_or(&(thread->flags),
		     (AGS_PORTABLE_THREAD_LOCKED));
    pth_mutex_release(&(main_loop->mutex));
  }
}


gboolean
ags_portable_thread_trylock(AgsPortableThread *thread)
{
  AgsPortableThread *main_loop;

  if(thread == NULL){
    return(FALSE);
  }

  main_loop = ags_portable_thread_get_toplevel(thread);

  if(main_loop == thread){
    if(pth_mutex_(&(thread->mutex)) != 0){
      return(FALSE);
    }

    g_atomic_int_or(&(thread->flags),
		     (AGS_PORTABLE_THREAD_LOCKED));
  }else{
    if(pth_mutex_acquire(&(main_loop->mutex, TRUE, NULL)) != 0){
      return(FALSE);
    }

    if(pth_mutex_acquire(&(thread->mutex), TRUE, NULL) != 0){
      pth_mutex_release(&(main_loop->mutex));
      return(FALSE);
    }

    g_atomic_int_or(&(thread->flags),
		     (AGS_PORTABLE_THREAD_LOCKED));
    pth_mutex_release(&(main_loop->mutex));
  }

  return(TRUE);
}

/**
 * ags_portable_thread_unlock:
 * @thread an #AgsPortableThread
 *
 * Unlocks the threads own mutex and unsets the appropriate flag.
 */
void
ags_portable_thread_unlock(AgsPortableThread *thread)
{
  if(thread == NULL){
    return;
  }

  g_atomic_int_and(&(thread->flags),
		   (~AGS_PORTABLE_THREAD_LOCKED));

  pth_mutex_release(&(thread->mutex));
}

/**
 * ags_portable_thread_get_toplevel:
 * @thread an #AgsPortableThread
 * Returns: the toplevevel #AgsPortableThread
 *
 * Retrieve toplevel thread.
 */
AgsPortableThread*
ags_portable_thread_get_toplevel(AgsPortableThread *thread)
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
 * ags_portable_thread_first:
 * @thread an #AgsPortableThread
 * Returns: the very first #AgsPortableThread within same tree level
 *
 * Retrieve first sibling.
 */
AgsPortableThread*
ags_portable_thread_first(AgsPortableThread *thread)
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
 * ags_portable_thread_last:
 * @thread an #AgsPortableThread
 * Returns: the very last @AgsPortableThread within same tree level
 * 
 * Retrieve last sibling.
 */
AgsPortableThread*
ags_portable_thread_last(AgsPortableThread *thread)
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
ags_portable_thread_remove_child(AgsPortableThread *thread, AgsPortableThread *child)
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
ags_portable_thread_add_child(AgsPortableThread *thread, AgsPortableThread *child)
{
  if(thread == NULL || child == NULL){
    return;
  }
  
  if(child->parent != NULL){
    ags_portable_thread_remove_child(child->parent, child);
  }

  /*  */
  if(thread->children == NULL){
    thread->children = child;
    child->parent = thread;
  }else{
    AgsPortableThread *sibling;

    sibling = ags_portable_thread_last(thread->children);

    sibling->next = child;
    child->prev = sibling;
    child->parent = thread;
  }

  if((AGS_PORTABLE_THREAD_RUNNING & (thread->flags)) != 0){
    ags_portable_thread_start(child);
  }
}

/**
 * ags_portable_thread_parental_is_locked:
 * @thread an #AgsPortableThread
 * @parent where to stop iteration
 * Returns: TRUE if locked otherwise FALSE
 *
 * Check the AGS_PORTABLE_THREAD_LOCKED flag in parental levels.
 */
gboolean
ags_portable_thread_parental_is_locked(AgsPortableThread *thread, AgsPortableThread *parent)
{
  AgsPortableThread *current;

  if(thread == NULL){
    return(FALSE);
  }

  current = thread->parent;

  while(current != parent){
    if((AGS_PORTABLE_THREAD_LOCKED & (current->flags)) != 0){

      return(TRUE);
    }

    current = current->parent;
  }

  return(FALSE);
}

/**
 * ags_portable_thread_sibling_is_locked:
 * @thread an #AgsPortableThread
 * Returns: TRUE if locked otherwise FALSE
 *
 * Check the AGS_PORTABLE_THREAD_LOCKED flag within sibling.
 */
gboolean
ags_portable_thread_sibling_is_locked(AgsPortableThread *thread)
{
  if(thread == NULL){
    return(FALSE);
  }

  thread = ags_portable_thread_first(thread);

  while(thread->next != NULL){
    if((AGS_PORTABLE_THREAD_LOCKED & (thread->flags)) != 0){
      return(TRUE);
    }

    thread = thread->next;
  }

  return(FALSE);
}


/**
 * ags_portable_thread_sibling_is_locked:
 * @thread an #AgsPortableThread
 * Returns: TRUE if locked otherwise FALSE
 *
 * Check the AGS_PORTABLE_THREAD_LOCKED flag within sibling.
 */
gboolean
ags_portable_thread_children_is_locked(AgsPortableThread *thread)
{
  auto gboolean ags_portable_thread_children_is_locked_recursive(AgsPortableThread *thread);

  gboolean ags_portable_thread_children_is_locked_recursive(AgsPortableThread *thread){
    AgsPortableThread *current;

    if(thread == NULL){
      return(FALSE);
    }

    current = thread;

    while(current != NULL){
      if((AGS_PORTABLE_THREAD_LOCKED & (thread->flags)) != 0){
	return(TRUE);
      }

      if(ags_portable_thread_children_is_locked_recursive(current->children)){
	return(TRUE);
      }

      current = current->next;
    }

    return(FALSE);
  }

  if(thread == NULL){
    return(FALSE);
  }

  return(ags_portable_thread_children_is_locked_recursive(thread->children));
}

gboolean
ags_portable_thread_is_current_ready(AgsPortableThread *current)
{
  gint val;

  val = g_atomic_int_get(&(current->flags));

  if((AGS_PORTABLE_THREAD_RUNNING & (val)) == 0){
    return(TRUE);
  }

  if((AGS_PORTABLE_THREAD_WAIT_0 & (val)) != 0){
    return(TRUE);
  }

  return(FALSE);
}

gboolean
ags_portable_thread_is_tree_ready(AgsPortableThread *thread)
{
  AgsPortableThread *main_loop;

  auto gboolean ags_portable_thread_is_tree_ready_recursive(AgsPortableThread *current);

  gboolean ags_portable_thread_is_tree_ready_recursive(AgsPortableThread *current){

    while(current != NULL){
      if(current == thread){
	return(!ags_portable_thread_is_tree_ready_recursive(current->children));
      }

      if(thread != current && !ags_portable_thread_is_current_ready(current)){
	return(FALSE);
      }

      if(!ags_portable_thread_is_tree_ready_recursive(current->children)){
	return(FALSE);
      }

      current = current->next;
    }

    return(TRUE);
  }

  main_loop = ags_portable_thread_get_toplevel(thread);

  return(ags_portable_thread_is_tree_ready_recursive(main_loop));
}

void
ags_portable_thread_main_loop_unlock_children(AgsPortableThread *thread)
{
  AgsPortableThread *main_loop;

  auto void ags_portable_thread_main_loop_unlock_children_recursive(AgsPortableThread *child);

  void ags_portable_thread_main_loop_unlock_children_recursive(AgsPortableThread *child){
    if(child == NULL){
      return;
    }

    while(child != NULL){
      g_atomic_int_and(&(child->flags),
		       (~AGS_PORTABLE_THREAD_WAIT_0));

      ags_portable_thread_main_loop_unlock_children_recursive(child->children);

      if((AGS_PORTABLE_THREAD_BROADCAST_PARENT & (thread->flags)) == 0){
	pth_cond_notify(&(child->cond), FALSE);
      }else{
	pth_cond_broadcast(&(child->cond), TRUE);
      }

      child = child->next;
    }
  }

  main_loop = ags_portable_thread_get_toplevel(thread);

  ags_portable_thread_main_loop_unlock_children_recursive(main_loop);
}

/**
 * ags_portable_thread_next_parent_locked:
 * @thread an #AgsPortableThread
 * @parent the parent #AgsPortableThread where to stop.
 * 
 * Retrieve next locked thread above @thread.
 */
AgsPortableThread*
ags_portable_thread_next_parent_locked(AgsPortableThread *thread, AgsPortableThread *parent)
{
  AgsPortableThread *current;

  current = thread->parent;

  while(current != parent){
    if((AGS_PORTABLE_THREAD_WAITING_FOR_CHILDREN & (current->flags)) != 0){
      return(current);
    }

    current = current->parent;
  }

  return(NULL);
}

/**
 * ags_portable_thread_next_sibling_locked:
 * @thread an #AgsPortableThread
 *
 * Retrieve next locked thread neighbooring @thread
 */
AgsPortableThread*
ags_portable_thread_next_sibling_locked(AgsPortableThread *thread)
{
  AgsPortableThread *current;

  current = ags_portable_thread_first(thread);

  while(current != NULL){
    if(current == thread){
      current = current->next;
      
      continue;
    }

    if((AGS_PORTABLE_THREAD_WAITING_FOR_SIBLING & (thread->flags)) != 0){
      return(current);
    }

    current = current->next;
  }

  return(NULL);
}

/**
 * ags_portable_thread_next_children_locked:
 * @thread an #AgsPortableThread
 * 
 * Retrieve next locked thread following @thread
 */
AgsPortableThread*
ags_portable_thread_next_children_locked(AgsPortableThread *thread)
{
  auto AgsPortableThread* ags_portable_thread_next_children_locked_recursive(AgsPortableThread *thread);

  AgsPortableThread* ags_portable_thread_next_children_locked_recursive(AgsPortableThread *child){
    AgsPortableThread *current;

    current = ags_portable_thread_last(child);

    while(current != NULL){
      ags_portable_thread_next_children_locked_recursive(current->children);

      if((AGS_PORTABLE_THREAD_WAITING_FOR_PARENT & (current->flags)) != 0){
	return(current);
      }

      current = current->prev;
    }

    return(NULL);
  }

  return(ags_portable_thread_next_children_locked(thread->children));
}

/**
 * ags_portable_thread_lock_parent:
 * @thread an #AgsPortableThread
 * @parent the parent #AgsPortableThread where to stop.
 *
 * Lock parent tree structure.
 */
void
ags_portable_thread_lock_parent(AgsPortableThread *thread, AgsPortableThread *parent)
{
  AgsPortableThread *current;

  if(thread == NULL){
    return;
  }

  ags_portable_thread_lock(thread);

  current = thread->parent;

  while(current != parent){
    ags_portable_thread_lock(current);
    current->flags |= AGS_PORTABLE_THREAD_WAITING_FOR_CHILDREN;

    current = current->parent;
  }
}

/**
 * ags_portable_thread_lock_sibling:
 * @thread an #AgsPortableThread
 *
 * Lock sibling tree structure.
 */
void
ags_portable_thread_lock_sibling(AgsPortableThread *thread)
{
  AgsPortableThread *current;

  if(thread == NULL){
    return;
  }

  ags_portable_thread_lock(thread);

  current = ags_portable_thread_first(thread);

  while(current != NULL){
    if(current == thread){
      current = current->next;
    
      continue;
    }

    ags_portable_thread_lock(current);
    current->flags |= AGS_PORTABLE_THREAD_WAITING_FOR_SIBLING;

    current = current->next;
  }
}

/**
 * ags_portable_thread_lock_children:
 * @thread an #AgsPortableThread
 *
 * Lock child tree structure.
 */
void
ags_portable_thread_lock_children(AgsPortableThread *thread)
{
  auto void ags_portable_thread_lock_children_recursive(AgsPortableThread *child);
  
  void ags_portable_thread_lock_children_recursive(AgsPortableThread *child){
    AgsPortableThread *current;

    current = ags_portable_thread_last(child);

    while(current != NULL){
      ags_portable_thread_lock_children_recursive(current->children);

      ags_portable_thread_lock(current);
      current->flags |= AGS_PORTABLE_THREAD_WAITING_FOR_PARENT;
      
      current = current->prev;
    }
  }

  ags_portable_thread_lock(thread);
  
  ags_portable_thread_lock_children_recursive(thread->children);
}

void
ags_portable_thread_lock_all(AgsPortableThread *thread)
{
  ags_portable_thread_lock_parent(thread, NULL);
  ags_portable_thread_lock_sibling(thread);
  ags_portable_thread_lock_children(thread);
}

/**
 * ags_portable_thread_unlock_parent:
 * @thread an #AgsPortableThread
 * @parent the parent #AgsPortableThread where to stop.
 *
 * Unlock parent tree structure.
 */
void
ags_portable_thread_unlock_parent(AgsPortableThread *thread, AgsPortableThread *parent)
{
  AgsPortableThread *current;

  if(thread == NULL){
    return;
  }

  current = thread->parent;

  while(current != parent){
    current->flags &= (~AGS_PORTABLE_THREAD_WAITING_FOR_CHILDREN);

    if((AGS_PORTABLE_THREAD_BROADCAST_PARENT & (thread->flags)) == 0){
      pth_cond_notify(&(current->cond), FALSE);
    }else{
      pth_cond_notify(&(current->cond),  TRUE);
    }

    ags_portable_thread_unlock(current);

    current = current->parent;
  }
}

/**
 * ags_portable_thread_unlock_sibling:
 * @thread an #AgsPortableThread
 *
 * Unlock sibling tree structure.
 */
void
ags_portable_thread_unlock_sibling(AgsPortableThread *thread)
{
  AgsPortableThread *current;

  if(thread == NULL){
    return;
  }

  current = ags_portable_thread_first(thread);

  while(current != NULL){
    if(current == thread){
      current = current->next;
    
      continue;
    }

    current->flags &= (~AGS_PORTABLE_THREAD_WAITING_FOR_SIBLING);

    if((AGS_PORTABLE_THREAD_BROADCAST_SIBLING & (thread->flags)) == 0){
      pth_cond_notify(&(current->cond), FALSE);
    }else{
      pth_cond_notify(&(current->cond), TRUE);
    }

    ags_portable_thread_unlock(current);

    current = current->next;
  }
}

/**
 * ags_portable_thread_unlock_children:
 * @thread an #AgsPortableThread
 *
 * Unlock child tree structure.
 */
void
ags_portable_thread_unlock_children(AgsPortableThread *thread)
{
  auto void ags_portable_thread_unlock_children_recursive(AgsPortableThread *child);
  
  void ags_portable_thread_unlock_children_recursive(AgsPortableThread *child){
    AgsPortableThread *current;

    if(child == NULL){
      return;
    }

    current = ags_portable_thread_last(child);

    while(current != NULL){
      ags_portable_thread_unlock_children_recursive(current->children);

      current->flags &= (~AGS_PORTABLE_THREAD_WAITING_FOR_PARENT);

      if(!((AGS_PORTABLE_THREAD_INITIAL_RUN & (thread->flags)) != 0 &&
	   AGS_IS_AUDIO_LOOP(thread))){

	if((AGS_PORTABLE_THREAD_BROADCAST_CHILDREN & (thread->flags)) == 0){
	  pth_cond_notify(&(current->cond), FALSE);
	}else{
	  pth_cond_notify(&(current->cond), TRUE);
	}
      }

      ags_portable_thread_unlock(current);

      current = current->prev;
    }
  }
  
  ags_portable_thread_unlock_children_recursive(thread->children);
}

void
ags_portable_thread_unlock_all(AgsPortableThread *thread)
{
  ags_portable_thread_unlock_parent(thread, NULL);
  ags_portable_thread_unlock_sibling(thread);
  ags_portable_thread_unlock_children(thread);
}

/**
 * ags_portable_thread_wait_parent:
 * @thread an #AgsPortableThread
 * @parent the parent #AgsPortableThread where to stop.
 *
 * Wait on parent tree structure.
 */
void
ags_portable_thread_wait_parent(AgsPortableThread *thread, AgsPortableThread *parent)
{
  AgsPortableThread *current;

  if(thread == NULL || thread == parent){
    return;
  }

  /* wait parent */
  current = thread->parent;
    
  while((current != NULL && current != parent) &&
	(((AGS_PORTABLE_THREAD_IDLE & (current->flags)) != 0 ||
	  (AGS_PORTABLE_THREAD_WAITING_FOR_CHILDREN & (current->flags)) == 0) ||
	 current->parent != parent)){
    pth_cond_await(&(current->cond),
		   &(current->mutex),
		   NULL);

    if(!((AGS_PORTABLE_THREAD_IDLE & (current->flags)) != 0 ||
	 (AGS_PORTABLE_THREAD_WAITING_FOR_CHILDREN & (current->flags)) == 0)){    
      current = current->parent;
    }
  }

  /* unset flag */
  thread->flags &= (~AGS_PORTABLE_THREAD_WAITING_FOR_PARENT);
}

/**
 * ags_portable_thread_wait_sibling:
 * @thread an #AgsPortableThread
 *
 * Wait on sibling tree structure.
 */
void
ags_portable_thread_wait_sibling(AgsPortableThread *thread)
{
  AgsPortableThread *current;

  if(thread == NULL){
    return;
  }

  /* wait sibling */
  current = ags_portable_thread_first(thread);
  
  while(current != NULL &&
	(((AGS_PORTABLE_THREAD_IDLE & (current->flags)) != 0 ||
	  (AGS_PORTABLE_THREAD_WAITING_FOR_SIBLING & (current->flags)) == 0) ||
	 current->next != NULL)){
    if(current == thread){
      current = current->next;
      
      continue;
    }
    
    pth_cond_await(&(current->cond),
		   &(current->mutex),
		   NULL);

    if(!((AGS_PORTABLE_THREAD_IDLE & (current->flags)) != 0 ||
	 (AGS_PORTABLE_THREAD_WAITING_FOR_SIBLING & (current->flags)) == 0)){
      current = current->next;
    }
  }

  /* unset flags */
  thread->flags &= (~AGS_PORTABLE_THREAD_WAITING_FOR_SIBLING);
}

/**
 * ags_portable_thread_wait_children:
 * @thread an #AgsPortableThread
 *
 * Wait on child tree structure.
 */
void
ags_portable_thread_wait_children(AgsPortableThread *thread)
{
  auto void ags_portable_thread_wait_children_recursive(AgsPortableThread *child);
  
  void ags_portable_thread_wait_children_recursive(AgsPortableThread *child){
    gboolean initial_run;

    if(child == NULL){
      return;
    }

    initial_run = TRUE;

    while(child != NULL &&
	  (((AGS_PORTABLE_THREAD_IDLE & (child->flags)) != 0 ||
	    (AGS_PORTABLE_THREAD_WAITING_FOR_PARENT & (child->flags)) == 0) ||
	   child->next != NULL)){
      if(initial_run){
	ags_portable_thread_wait_children_recursive(child->children);

	initial_run = FALSE;
      }

      pth_cond_await(&(child->cond),
		     &(child->mutex),
		     NULL);
     
      if(!((AGS_PORTABLE_THREAD_IDLE & (child->flags)) != 0 ||
	   (AGS_PORTABLE_THREAD_WAITING_FOR_PARENT & (child->flags)) == 0)){
	child = child->next;

	initial_run = TRUE;
      }
    }
  }

  if(thread == NULL){
    return;
  }

  /* wait children */
  ags_portable_thread_wait_children_recursive(thread->children);

  /* unset flags */
  thread->flags &= (~AGS_PORTABLE_THREAD_WAITING_FOR_CHILDREN);
}

/**
 * ags_portable_thread_signal_parent:
 * @thread an #AgsPortableThread
 * @broadcast whether to perforam a signal or to broadcast
 *
 * Signals the tree in higher levels.
 */
void
ags_portable_thread_signal_parent(AgsPortableThread *thread, AgsPortableThread *parent,
			 gboolean broadcast)
{
  AgsPortableThread *current;

  current = thread->parent;

  while(current != NULL){
    if((AGS_PORTABLE_THREAD_WAIT_FOR_CHILDREN & (current->flags)) != 0){
      if(!broadcast){
	pth_cond_notify(&(current->cond), FALSE);
      }else{
	pth_cond_notify(&(current->cond), TRUE);
      }
    }

    current = current->parent;
  }
}

/**
 * ags_portable_thread_signal_sibling:
 * @thread an #AgsPortableThread
 * @broadcast whether to perforam a signal or to broadcast
 *
 * Signals the tree on same level.
 */
void
ags_portable_thread_signal_sibling(AgsPortableThread *thread, gboolean broadcast)
{
  AgsPortableThread *current;

  current = ags_portable_thread_first(thread);

  while(current != NULL){
    if((AGS_PORTABLE_THREAD_WAIT_FOR_SIBLING & (current->flags)) != 0){
      if(!broadcast){
	pth_cond_notify(&(current->cond), FALSE);
      }else{
	pth_cond_notify(&(current->cond), TRUE);
      }
    }
  }
}

/**
 * ags_portable_thread_signal_children:
 * @thread an #AgsPortableThread
 * @broadcast whether to perforam a signal or to broadcast
 *
 * Signals the tree in lower levels.
 */
void
ags_portable_thread_signal_children(AgsPortableThread *thread, gboolean broadcast)
{
  auto void ags_portable_thread_signal_children_recursive(AgsPortableThread *thread, gboolean broadcast);

  void ags_portable_thread_signal_children_recursive(AgsPortableThread *thread, gboolean broadcast){
    AgsPortableThread *current;

    if(thread == NULL){
      return;
    }

    current = thread;

    while(current != NULL){
      if((AGS_PORTABLE_THREAD_WAIT_FOR_PARENT & (current->flags)) != 0){
	if(!broadcast){
	  pth_cond_nofify(&(current->cond), FALSE);
	}else{
	  pth_cond_notify(&(current->cond), TRUE);
	}
      }
      
      ags_portable_thread_signal_children_recursive(current, broadcast);

      current = current->next;
    }
  }

  ags_portable_thread_signal_children(thread->children, broadcast);
}

void
ags_portable_thread_real_start(AgsPortableThread *thread)
{
  AgsPortableThread *main_loop;
  guint current_tic;
  guint val;

  if(thread == NULL){
    return;
  }

  /*  */
  main_loop = ags_portable_thread_get_toplevel(thread);

  ags_portable_thread_lock(main_loop);

  current_tic = ags_main_loop_get_tic(AGS_MAIN_LOOP(main_loop));

  g_atomic_int_or(&(thread->flags),
		  (AGS_PORTABLE_THREAD_RUNNING |
		   AGS_PORTABLE_THREAD_INITIAL_RUN));
  
  ags_portable_thread_unlock(main_loop);

  /*  */
  thread->thread = pth_spawn(&(thread->thread_attr),
			     &(ags_portable_thread_loop),
			     thread);
  
  /* */
  val = g_atomic_int_get(&(thread->flags));
  
  if((AGS_PORTABLE_THREAD_TIMELOCK_RUN & val) != 0){
    thread->timelock_thread = pth_create(NULL,
					 &(ags_portable_thread_timelock_loop),
					 thread);
  }
}

/**
 * ags_portable_thread_start:
 * @thread the #AgsPortableThread instance
 *
 * Start the thread.
 */
void
ags_portable_thread_start(AgsPortableThread *thread)
{
  g_return_if_fail(AGS_IS_THREAD(thread));

  g_object_ref(G_OBJECT(thread));
  g_signal_emit(G_OBJECT(thread),
		thread_signals[START], 0);
  g_object_unref(G_OBJECT(thread));
}

void*
ags_portable_thread_loop(void *ptr)
{
  AgsPortableThread *thread, *main_loop;
  gboolean is_in_sync;
  gboolean wait_for_parent, wait_for_sibling, wait_for_children;
  guint current_tic;
  guint val;

  auto void ags_portable_thread_loop_sync(AgsPortableThread *thread);

  void ags_portable_thread_loop_sync(AgsPortableThread *thread){
    guint tic;

    ags_portable_thread_lock(main_loop);

    if(!ags_portable_thread_is_tree_ready(thread)){
      g_atomic_int_or(&(thread->flags),
		      AGS_PORTABLE_THREAD_WAIT_0);

      ags_portable_thread_unlock(main_loop);

      if(thread != main_loop){
	if(tic == 0 && current_tic == 2){
	  return;
	}else if(tic == 1 && current_tic == 0){
	  return;
	}else if(tic == 2 && current_tic == 1){ 
	  return;
	}    
      }
    
      while(!ags_portable_thread_is_current_ready(thread)){
	pth_cond_await(&(thread->cond),
		       &(thread->mutex),
		       NULL);

	tic = ags_main_loop_get_tic(AGS_MAIN_LOOP(main_loop));
      }
    }else{
      guint next_tic;

      tic = ags_main_loop_get_tic(AGS_MAIN_LOOP(main_loop));

      ags_portable_thread_unlock(main_loop);

      if(tic = 2){
	next_tic = 0;
      }else if(tic = 0){
	next_tic = 1;
      }else if(tic = 1){
	next_tic = 2;
      }

      ags_main_loop_set_tic(AGS_MAIN_LOOP(main_loop), next_tic);
      ags_portable_thread_main_loop_unlock_children(main_loop);
      ags_main_loop_set_last_sync(AGS_MAIN_LOOP(main_loop), tic);
    }
  }

  thread = AGS_PORTABLE_THREAD(ptr);
  main_loop = ags_portable_thread_get_toplevel(thread);

  current_tic = ags_main_loop_get_tic(AGS_MAIN_LOOP(main_loop));

  while((AGS_PORTABLE_THREAD_RUNNING & (thread->flags)) != 0){

    /* barrier */
    if((AGS_PORTABLE_THREAD_WAITING_FOR_BARRIER & (thread->flags)) != 0){
      int wait_count;

      if(thread->first_barrier){
	/* retrieve wait count */
	ags_portable_thread_lock(thread);

	wait_count = thread->wait_count[0];

	ags_portable_thread_unlock(thread);

	/* init and wait */
	pth_barrier_init(&(thread->barrier[0]), NULL, wait_count);
	pth_barrier_wait(&(thread->barrier[0]));
      }else{
	/* retrieve wait count */
	ags_portable_thread_lock(thread);

	wait_count = thread->wait_count[1];

	ags_portable_thread_unlock(thread);

	/* init and wait */
	pth_barrier_init(&(thread->barrier[1]), NULL, wait_count);
	pth_barrier_wait(&(thread->barrier[1]));
      }
    }

    /* run in hierarchy */
    ags_portable_thread_lock(thread);

    ags_portable_thread_loop_sync(thread);

    /* */
    val = g_atomic_int_get(&(thread->flags));

    if((AGS_PORTABLE_THREAD_TIMELOCK_RUN & val) != 0){
      pth_mutex_acquire(&(thread->timelock_mutex),
			NULL);

      g_atomic_int_and(&(thread->flags),
		       (~AGS_PORTABLE_THREAD_TIMELOCK_WAIT));

      pth_cond_notify(&(thread->timelock_cond),
		      FALSE);

      pth_mutex_release(&(thread->timelock_mutex));
    }
    
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
		    AGS_PORTABLE_THREAD_IDLE);

    if((AGS_PORTABLE_THREAD_WAIT_FOR_PARENT & (thread->flags)) != 0){
      wait_for_parent = TRUE;

      thread->flags |= AGS_PORTABLE_THREAD_WAITING_FOR_PARENT;
      ags_portable_thread_lock_parent(thread, NULL);
    }else{
      wait_for_parent = FALSE;
    }

    /* lock sibling */
    if((AGS_PORTABLE_THREAD_WAIT_FOR_SIBLING & (thread->flags)) != 0){
      wait_for_sibling = TRUE;

      thread->flags |= AGS_PORTABLE_THREAD_WAITING_FOR_SIBLING;
      ags_portable_thread_lock_sibling(thread);
    }else{
      wait_for_sibling = FALSE;
    }

    /* lock_children */
    if((AGS_PORTABLE_THREAD_WAIT_FOR_CHILDREN & (thread->flags)) != 0){
      wait_for_children = TRUE;

      thread->flags |= AGS_PORTABLE_THREAD_WAITING_FOR_CHILDREN;
      ags_portable_thread_lock_children(thread);
    }else{
      wait_for_children = FALSE;
    }

    /* skip very first sync of AgsAudioLoop */
    if(!((AGS_PORTABLE_THREAD_INITIAL_RUN & (thread->flags)) != 0 &&
	 AGS_IS_AUDIO_LOOP(thread))){

      /* wait parent */
      if(wait_for_parent){
	ags_portable_thread_wait_parent(thread, NULL);
      }

      /* wait sibling */
      if(wait_for_sibling){
	ags_portable_thread_wait_sibling(thread);
      }

      /* wait children */
      if(wait_for_children){
	ags_portable_thread_wait_children(thread);
      }
    }

    ags_portable_thread_unlock(thread);

    /* run */
    ags_portable_thread_run(thread);

    /**/
    ags_portable_thread_lock(thread);

    /* unset idle flag */
    thread->flags &= (~AGS_PORTABLE_THREAD_IDLE);

    /* unlock parent */
    if(wait_for_parent){
      ags_portable_thread_unlock_parent(thread, NULL);
    }

    /* unlock sibling */
    if(wait_for_sibling){
      ags_portable_thread_unlock_sibling(thread);
    }

    /* unlock children */
    if(wait_for_children){
      ags_portable_thread_unlock_children(thread);
    }

    /* unset initial run */
    if((AGS_PORTABLE_THREAD_INITIAL_RUN & (thread->flags)) != 0){
      thread->flags &= (~AGS_PORTABLE_THREAD_INITIAL_RUN);

      /* signal AgsAudioLoop */
      if(AGS_IS_TASK_THREAD(thread)){
    
	thread->flags &= (~AGS_PORTABLE_THREAD_INITIAL_RUN);
	pth_cond_notify(&(thread->start_cond),
			FALSE);
      }
    }

    ags_portable_thread_unlock(thread);
  }

  pth_exit(NULL);
}

/**
 * ags_portable_thread_run:
 * @thread the #AgsPortableThread instance
 * 
 * Only for internal use of ags_portable_thread_loop but you may want to set the your very own
 * class function namely your thread's routine.
 */
void
ags_portable_thread_run(AgsPortableThread *thread)
{
  g_return_if_fail(AGS_IS_THREAD(thread));

  g_object_ref(G_OBJECT(thread));
  g_signal_emit(G_OBJECT(thread),
		thread_signals[RUN], 0);
  g_object_unref(G_OBJECT(thread));
}

void*
ags_portable_thread_timelock_loop(void *ptr)
{
  AgsPortableThread *thread;
  guint val;

  thread = AGS_PORTABLE_THREAD(ptr);

  val = g_atomic_int_get(&(thread->flags));

  while((AGS_PORTABLE_THREAD_RUNNING & (val)) != 0){
    pth_mutex_acquire(&thread->timelock_mutex,
		      FALSE);

    val = g_atomic_int_get(&(thread->flags));

    while((AGS_PORTABLE_THREAD_TIMELOCK_WAIT & (val)) != 0){
      pth_cond_await(&(thread->timelock_cond),
		     &(thread->timelock_mutex),
		     NULL);

      val = g_atomic_int_get(&(thread->flags));
    }

    g_atomic_int_or(&(thread->flags),
		    AGS_PORTABLE_THREAD_TIMELOCK_WAIT);

    pth_mutex_release(&thread->timelock_mutex);

    nanosleep(&(thread->timelock), NULL);

    g_atomic_int_or(&(thread->flags),
		    AGS_PORTABLE_THREAD_TIMELOCK_RESUME);

    ags_portable_thread_timelock(thread);

    val = g_atomic_int_get(&(thread->flags));
  }
}

void
ags_portable_thread_real_timelock(AgsPortableThread *thread)
{
  AgsPortableThread *main_loop;
  int sig;
  static sigset_t set;

  ags_portable_thread_lock(thread);

  g_atomic_int_or(&(thread->flags),
		  AGS_PORTABLE_THREAD_TIMELOCK_RESUME);

  pth_suspend(&(thread->thread));

  main_loop = ags_portable_thread_get_toplevel(thread);

  ags_portable_thread_lock(main_loop);

  if(!ags_portable_thread_is_tree_ready(thread)){
    g_atomic_int_or(&(thread->flags),
		    AGS_PORTABLE_THREAD_WAIT_0);

    ags_portable_thread_unlock(main_loop);
    
    while(!ags_portable_thread_is_current_ready(thread)){
      pth_cond_await(&(thread->cond),
		     &(thread->mutex),
		     NULL);
    }
  }else{
    guint tic, next_tic;

    tic = ags_main_loop_get_tic(AGS_MAIN_LOOP(main_loop));

    ags_portable_thread_unlock(main_loop);

    if(tic = 2){
      next_tic = 0;
    }else if(tic = 0){
      next_tic = 1;
    }else if(tic = 1){
      next_tic = 2;
    }

    ags_main_loop_set_tic(AGS_MAIN_LOOP(main_loop), next_tic);
    ags_portable_thread_main_loop_unlock_children(main_loop);
    ags_main_loop_set_last_sync(AGS_MAIN_LOOP(main_loop), tic);
  }

  pth_resume(&(thread->thread));

  ags_portable_thread_unlock(thread);
}

void
ags_portable_thread_timelock(AgsPortableThread *thread)
{
  g_return_if_fail(AGS_IS_THREAD(thread));

  g_object_ref(G_OBJECT(thread));
  g_signal_emit(G_OBJECT(thread),
		thread_signals[TIMELOCK], 0);
  g_object_unref(G_OBJECT(thread));
}

void
ags_portable_thread_real_stop(AgsPortableThread *thread)
{
  g_atomic_int_and(&(thread->flags),
		   (~AGS_PORTABLE_THREAD_RUNNING));
}

/**
 * ags_portable_thread_stop:
 * @thread the #AgsPortableThread instance
 * 
 * Stop the threads loop by unsetting AGS_PORTABLE_THREAD_RUNNING flag.
 */
void
ags_portable_thread_stop(AgsPortableThread *thread)
{
  g_return_if_fail(AGS_IS_THREAD(thread));

  g_object_ref(G_OBJECT(thread));
  g_signal_emit(G_OBJECT(thread),
		thread_signals[STOP], 0);
  g_object_unref(G_OBJECT(thread));
}

/**
 * ags_portable_thread_new:
 * @data an #GObject
 *
 * Create a new #AgsPortableThread you may provide an #GObject as @data
 * to your thread routine.
 */
AgsPortableThread*
ags_portable_thread_new(GObject *data)
{
  AgsPortableThread *thread;

  thread = (AgsPortableThread *) g_object_new(AGS_TYPE_THREAD,
					      NULL);

  thread->data = data;

  return(thread);
}
