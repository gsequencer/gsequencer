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

#include <ags/object/ags_connectable.h>

#include <ags/audio/ags_devout.h>

void ags_thread_class_init(AgsThreadClass *thread);
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
void ags_thread_connect(AgsConnectable *connectable);
void ags_thread_disconnect(AgsConnectable *connectable);
void ags_thread_finalize(GObject *gobject);

void ags_thread_set_devout(AgsThread *thread, GObject *devout);

void ags_thread_real_start(AgsThread *thread);
void* ags_thread_loop(void *ptr);
void ags_thread_real_run(AgsThread *thread);
void ags_thread_real_stop(AgsThread *thread);

enum{
  PROP_0,
  PROP_DEVOUT,
};

enum{
  START,
  RUN,
  STOP,
  LAST_SIGNAL,
};

static gpointer ags_thread_parent_class = NULL;
static guint thread_signals[LAST_SIGNAL];

GType
ags_thread_get_type()
{
  static GType ags_type_thread = 0;

  if(!ags_type_thread){
    static const GTypeInfo ags_thread_info = {
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

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_thread = g_type_register_static(G_TYPE_OBJECT,
					     "AgsThread\0",
					     &ags_thread_info,
					     0);
    
    g_type_add_interface_static(ags_type_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_thread);
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
ags_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_thread_connect;
  connectable->disconnect = ags_thread_disconnect;
}

void
ags_thread_init(AgsThread *thread)
{
  thread->flags = 0;

  pthread_mutex_init(&(thread->mutex), NULL);
  pthread_cond_init(&(thread->cond), NULL);
  thread->unlocked = NULL;

  thread->first_barrier = TRUE;
  thread->wait_count[0] = 1;
  thread->wait_count[1] = 1;

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

      devout = (AgsDevout *) g_value_get_object(value);

      if(thread->devout != NULL){
	g_object_unref(G_OBJECT(thread->devout));
      }

      if(devout != NULL){
	g_object_ref(G_OBJECT(devout));
      }

      thread->devout = G_OBJECT(devout);
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
  }
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
  g_list_free(thread->unlocked);

  g_object_unref(G_OBJECT(thread->devout));

  /* call parent */
  G_OBJECT_CLASS(ags_thread_parent_class)->finalize(gobject);
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
  pthread_mutex_t mutex;
  
  mutex = ags_thread_get_toplevel(thread)->mutex;

  pthread_mutex_lock(&mutex);

  pthread_mutex_lock(&(thread->mutex));
  thread->flags |= AGS_THREAD_LOCKED;

  pthread_mutex_unlock(&mutex);
}

/**
 * ags_thread_lock:
 * @thread an #AgsThread
 *
 * Unlocks the threads own mutex and unsets the appropriate flag.
 */
void
ags_thread_unlock(AgsThread *thread)
{
  pthread_mutex_t mutex;
  
  mutex = ags_thread_get_toplevel(thread)->mutex;

  pthread_mutex_lock(&mutex);
  thread->flags &= (~AGS_THREAD_LOCKED);
  pthread_mutex_unlock(&(thread->mutex));

  pthread_mutex_unlock(&mutex);
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
    child->prev = child->next;
  }

  if(child->next != NULL){
    child->next = child->prev;
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

  if(thread->children == NULL){
    thread->children = child;
  }else{
    AgsThread *sibling;

    sibling = ags_thread_last(thread->children);

    sibling->next = thread;
    thread->prev = sibling;
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
ags_thread_parental_is_locked(AgsThread *thread,
			      AgsThread *parent)
{
  AgsThread *current;

  if(thread == NULL){
    return(FALSE);
  }

  current = thread->parent;

  while(current != parent){
    if((AGS_THREAD_LOCKED & (current->flags)) != 0){
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
    if((AGS_THREAD_LOCKED & (thread->flags)) != 0){
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
      if((AGS_THREAD_LOCKED & (thread->flags)) != 0){
	return(TRUE);
      }

      if(ags_thread_children_is_locked_recursive(current->children)){
	return(TRUE);
      }

      current = current->next;
    }
  }

  if(thread == NULL){
    return(FALSE);
  }

  return(ags_thread_children_is_locked_recursive(thread->children));
}

/**
 * ags_thread_parental_is_unlocked:
 * @thread an #AgsThread
 * @parent where to stop iteration
 * @lock the mutex that holds the lock
 * Returns: TRUE if mutex isn't locked otherwise FALSE
 *
 * Searches the tree for @lock to be unlocked within parental levels.
 */
gboolean
ags_thread_parental_is_unlocked(AgsThread *thread, AgsThread *parent,
				pthread_mutex_t lock)
{
  AgsThread *current;

  if(thread == NULL){
    return(TRUE);
  }

  current = thread->parent;

  while(current != parent){
    if(g_list_find(current->unlocked, PTHREAD_MUTEX_TO_POINTER(lock)) == NULL){
      return(FALSE);
    }
  }

  return(TRUE);
}

/**
 * ags_thread_sibling_is_unlocked:
 * @thread an #AgsThread
 * @lock the mutex that holds the lock
 * Returns: TRUE if mutex isn't locked otherwise FALSE
 *
 * Searches the tree for @lock to be unlocked within same level.
 */
gboolean
ags_thread_sibling_is_unlocked(AgsThread *thread,
			       pthread_mutex_t lock)
{
  AgsThread *current;

  if(thread == NULL){
    return(TRUE);
  }
 
  current = ags_thread_first(thread);

  while(current != NULL){
    if(g_list_find(current->unlocked, PTHREAD_MUTEX_TO_POINTER(lock)) == NULL){
      return(FALSE);
    }

    current = current->next;
  }

  return(TRUE);
}

/**
 * ags_thread_children_is_unlocked:
 * @thread an #AgsThread
 * @lock the mutex that holds the lock
 * Returns: TRUE if mutex isn't locked otherwise FALSE
 *
 * Searches the tree for @lock to be unlocked within children.
 */
gboolean
ags_thread_children_is_unlocked(AgsThread *thread,
				pthread_mutex_t lock)
{
  auto gboolean ags_thread_children_is_unlocked_recursive(AgsThread *child, pthread_mutex_t lock);

  gboolean ags_thread_children_is_unlocked_recursive(AgsThread *child, pthread_mutex_t lock){
    AgsThread *current;

    if(thread == NULL){
      return(TRUE);
    }
    
    while(current != NULL){
      if(g_list_find(current->unlocked, PTHREAD_MUTEX_TO_POINTER(lock)) == NULL){
	return(FALSE);
      }

      if(!ags_thread_children_is_unlocked(current->children, lock)){
	return(FALSE);
      }

      current = current->next;
    }

    return(TRUE);
  }

  return(ags_thread_children_is_unlocked_recursive(thread->children, lock));
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
    if((AGS_THREAD_WAITING_FOR_CHILDREN & (current->flags)) != 0 &&
       g_list_find(current->unlocked, PTHREAD_MUTEX_TO_POINTER(thread->mutex)) == NULL){
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

    if((AGS_THREAD_WAITING_FOR_SIBLING & (thread->flags)) != 0 &&
       g_list_find(current->unlocked, PTHREAD_MUTEX_TO_POINTER(thread->mutex)) == NULL){
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

      if((AGS_THREAD_WAITING_FOR_PARENT & (current->flags)) != 0 &&
	 g_list_find(current->unlocked, PTHREAD_MUTEX_TO_POINTER(thread->mutex)) == NULL){
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
 * @toplevel_mutex the main posix mutex
 *
 * Lock parent tree structure.
 */
void
ags_thread_lock_parent(AgsThread *thread, AgsThread *parent,
		       pthread_mutex_t toplevel_mutex)
{
  AgsThread *current;

  if(thread == NULL){
    return;
  }

  current = thread->parent;

  while(current != parent){
    pthread_mutex_lock(&toplevel_mutex);
    current->flags |= AGS_THREAD_WAITING_FOR_CHILDREN;
    pthread_mutex_unlock(&toplevel_mutex);

    ags_thread_lock(current);

    current = current->parent;
  }
}

/**
 * ags_thread_lock_sibling:
 * @thread an #AgsThread
 * @toplevel_mutex the main posix mutex
 *
 * Lock sibling tree structure.
 */
void
ags_thread_lock_sibling(AgsThread *thread,
			pthread_mutex_t toplevel_mutex)
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

    pthread_mutex_lock(&toplevel_mutex);
    current->flags |= AGS_THREAD_WAITING_FOR_SIBLING;
    pthread_mutex_unlock(&toplevel_mutex);

    ags_thread_lock(current);

    current = current->next;
  }
}

/**
 * ags_thread_lock_children:
 * @thread an #AgsThread
 * @toplevel_mutex the main posix mutex
 *
 * Lock child tree structure.
 */
void
ags_thread_lock_children(AgsThread *thread,
			 pthread_mutex_t toplevel_mutex)
{
  auto void ags_thread_lock_children_recursive(AgsThread *child,
					       pthread_mutex_t toplevel_mutex);
  
  void ags_thread_lock_children_recursive(AgsThread *child,
					  pthread_mutex_t toplevel_mutex){
    AgsThread *current;

    current = ags_thread_last(child);

    while(current != NULL){
      ags_thread_lock_children_recursive(current->children,
					 toplevel_mutex);

      pthread_mutex_lock(&toplevel_mutex);
      current->flags |= AGS_THREAD_WAITING_FOR_PARENT;
      pthread_mutex_unlock(&toplevel_mutex);
      
      ags_thread_lock(current);
    
      current = current->prev;
    }
  }
  
  ags_thread_lock_children_recursive(thread->children,
				     toplevel_mutex);
}

/**
 * ags_thread_unlock_parent:
 * @thread an #AgsThread
 * @parent the parent #AgsThread where to stop.
 * @toplevel_mutex the main posix mutex
 *
 * Unlock parent tree structure.
 */
void
ags_thread_unlock_parent(AgsThread *thread, AgsThread *parent,
			 pthread_mutex_t toplevel_mutex)
{
  AgsThread *current;

  if(thread == NULL){
    return;
  }

  current = thread->parent;

  while(current != parent){
    pthread_mutex_lock(&toplevel_mutex);
    current->flags &= (~AGS_THREAD_WAITING_FOR_CHILDREN);
    pthread_mutex_unlock(&toplevel_mutex);

    ags_thread_unlock(current);

    current = current->parent;
  }
}

/**
 * ags_thread_unlock_sibling:
 * @thread an #AgsThread
 * @toplevel_mutex the main posix mutex
 *
 * Unlock sibling tree structure.
 */
void
ags_thread_unlock_sibling(AgsThread *thread,
			  pthread_mutex_t toplevel_mutex)
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

    pthread_mutex_lock(&toplevel_mutex);
    current->flags &= (~AGS_THREAD_WAITING_FOR_SIBLING);
    pthread_mutex_unlock(&toplevel_mutex);

    ags_thread_unlock(current);

    current = current->next;
  }
}

/**
 * ags_thread_unlock_children:
 * @thread an #AgsThread
 * @toplevel_mutex the main posix mutex
 *
 * Unlock child tree structure.
 */
void
ags_thread_unlock_children(AgsThread *thread,
			   pthread_mutex_t toplevel_mutex)
{
  auto void ags_thread_unlock_children_recursive(AgsThread *child,
						 pthread_mutex_t toplevel_mutex);
  
  void ags_thread_unlock_children_recursive(AgsThread *child,
					    pthread_mutex_t toplevel_mutex){
    AgsThread *current;

    current = ags_thread_last(child);

    while(current != NULL){
      ags_thread_unlock_children_recursive(current->children,
					   toplevel_mutex);

      pthread_mutex_lock(&toplevel_mutex);
      current->flags &= (~AGS_THREAD_WAITING_FOR_PARENT);
      pthread_mutex_unlock(&toplevel_mutex);
      
      ags_thread_unlock(current);
    
      current = current->prev;
    }
  }
  
  ags_thread_unlock_children_recursive(thread->children,
				       toplevel_mutex);
}

/**
 * ags_thread_wait_parent:
 * @thread an #AgsThread
 * @parent the parent #AgsThread where to stop.
 * @toplevel_mutex the main posix mutex
 *
 * Wait on parent tree structure.
 */
void
ags_thread_wait_parent(AgsThread *thread, AgsThread *parent,
		       pthread_mutex_t toplevel_mutex)
{
  AgsThread *current;

  if(thread == NULL){
    return;
  }

  /* set flag */
  thread->flags |= AGS_THREAD_WAITING_FOR_PARENT; 
  
  pthread_mutex_unlock(&toplevel_mutex);
    
  /* wait parent */
  ags_thread_lock(thread->parent);

  current = thread->parent;
    
  while(current != parent &&
	!ags_thread_parental_is_unlocked(thread, parent,
					 toplevel_mutex)){
    pthread_cond_wait(&(current->cond),
		      &(current->mutex));

    if(&(toplevel_mutex) != &(current->mutex)){
      pthread_mutex_lock(&toplevel_mutex);
    }

    if((AGS_THREAD_LOCKED & (current->flags)) == 0){
      if(current->parent != NULL){
	ags_thread_lock(current->parent);
      }
    
      ags_thread_unlock(current);

      current = current->parent;
    }

    if(&(toplevel_mutex) != &(current->mutex)){
      pthread_mutex_unlock(&toplevel_mutex);
    }
  }

  ags_thread_unlock(current);
    
  /* unset flag */
  pthread_mutex_lock(&toplevel_mutex);

  thread->flags &= (~AGS_THREAD_WAITING_FOR_PARENT);

  pthread_mutex_unlock(&toplevel_mutex);
    
  /* signal/broadcast */
  pthread_mutex_lock(&toplevel_mutex);

  if((AGS_THREAD_BROADCAST_PARENT & (thread->flags)) == 0){
    pthread_cond_signal(&(thread->cond));
  }else{
    pthread_cond_broadcast(&(thread->cond));
  }
}

/**
 * ags_thread_wait_sibling:
 * @thread an #AgsThread
 * @toplevel_mutex the main posix mutex
 *
 * Wait on sibling tree structure.
 */
void
ags_thread_wait_sibling(AgsThread *thread,
			pthread_mutex_t toplevel_mutex)
{
  AgsThread *current;

  if(thread == NULL){
    return;
  }

  /* set flags */
  thread->flags |= AGS_THREAD_WAITING_FOR_SIBLING;
  
  pthread_mutex_unlock(&toplevel_mutex);
  
  /* wait sibling */
  current = ags_thread_first(thread);
  
  while(ags_thread_sibling_is_locked(thread)){
    if(current == thread){
      current = current->next;
      
      continue;
    }
    
    pthread_cond_wait(&(thread->cond),
		      &(thread->mutex));

    pthread_mutex_lock(&toplevel_mutex);

    if((AGS_THREAD_LOCKED & (current->flags)) == 0){
      current = current->next;
    }

    pthread_mutex_unlock(&toplevel_mutex);
  }

  /* unset flags */
  pthread_mutex_lock(&toplevel_mutex);

  thread->flags &= (~AGS_THREAD_WAITING_FOR_SIBLING);

  pthread_mutex_unlock(&toplevel_mutex);

  /* signal/broadcast */
  pthread_mutex_lock(&toplevel_mutex);

  if((AGS_THREAD_BROADCAST_SIBLING & (thread->flags)) == 0){
    pthread_cond_signal(&(thread->cond));
  }else{
    pthread_cond_broadcast(&(thread->cond));
  }
}

/**
 * ags_thread_wait_children:
 * @thread an #AgsThread
 * @toplevel_mutex the main posix mutex
 *
 * Wait on child tree structure.
 */
void
ags_thread_wait_children(AgsThread *thread,
			 pthread_mutex_t toplevel_mutex)
{
  AgsThread *current;

  auto void ags_thread_wait_children_recursive(AgsThread *child,
					       pthread_mutex_t toplevel_mutex);
  
  void ags_thread_wait_children_recursive(AgsThread *child,
					  pthread_mutex_t toplevel_mutex){
    while(ags_thread_children_is_locked(child)){
      ags_thread_wait_children_recursive(child->children,
					 toplevel_mutex);

      pthread_cond_wait(&(child->cond),
			&(child->mutex));
      
      pthread_mutex_lock(&toplevel_mutex);
      
      if((AGS_THREAD_LOCKED & (child->flags)) == 0){
	child = child->next;
      }
      
      pthread_mutex_unlock(&toplevel_mutex);
    }
  }

  if(thread == NULL){
    return;
  }

  /* set flags */
  thread->flags |= AGS_THREAD_WAITING_FOR_CHILDREN;

  pthread_mutex_unlock(&toplevel_mutex);

  /* wait children */
  ags_thread_wait_children_recursive(thread->children,
				     toplevel_mutex);

  /* unset flags */
  pthread_mutex_lock(&toplevel_mutex);

  thread->flags &= (~AGS_THREAD_WAITING_FOR_CHILDREN);

  pthread_mutex_unlock(&toplevel_mutex);

  /* signal/broadcast */
  pthread_mutex_lock(&toplevel_mutex);

  if((AGS_THREAD_BROADCAST_CHILDREN & (thread->flags)) == 0){
    pthread_cond_signal(&(thread->cond));
  }else{
    pthread_cond_broadcast(&(thread->cond));
  }
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
    if((AGS_THREAD_WAIT_FOR_CHILDREN & (current->flags)) != 0){
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
    if((AGS_THREAD_WAIT_FOR_SIBLING & (current->flags)) != 0){
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
      if((AGS_THREAD_WAIT_FOR_PARENT & (current->flags)) != 0){
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
  if(thread == NULL){
    return;
  }

  pthread_create(&(thread->thread), NULL,
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
  AgsThread *thread;
  pthread_mutex_t mutex;
  int wait_count;
  gboolean initial_run;

  thread = AGS_THREAD(ptr);

  initial_run = TRUE;

  while((AGS_THREAD_RUNNING & (thread->flags)) != 0){
    g_message("%s\0", G_OBJECT_TYPE_NAME(thread));

    mutex = ags_thread_get_toplevel(thread)->mutex;

    /* lock */
    pthread_mutex_lock(&mutex);

    /* parent */
    if((AGS_THREAD_WAIT_FOR_PARENT & (thread->flags)) != 0 &&
       ags_thread_parental_is_locked(thread, NULL)){
      ags_thread_wait_parent(thread, NULL,
			     mutex);
    }

    /* sibling */
    if((AGS_THREAD_WAIT_FOR_SIBLING & (thread->flags)) != 0 &&
       ags_thread_sibling_is_locked(thread)){
      ags_thread_wait_sibling(thread, mutex);
    }

    /* children */
    if((AGS_THREAD_WAIT_FOR_CHILDREN & (thread->flags)) != 0 &&
       ags_thread_children_is_locked(thread)){
      ags_thread_wait_children(thread, mutex);
    }

    /* unlock */
    pthread_mutex_unlock(&mutex);

    /* barrier */
    if((AGS_THREAD_WAITING_FOR_BARRIER & (thread->flags)) != 0){
      if(thread->first_barrier){
	/* retrieve wait count */
	pthread_mutex_lock(&mutex);

	wait_count = thread->wait_count[0];

	pthread_mutex_unlock(&mutex);

	/* init and wait */
	pthread_barrier_init(&(thread->barrier[0]), NULL, wait_count);
	pthread_barrier_wait(&(thread->barrier[0]));
      }else{
	/* retrieve wait count */
	pthread_mutex_lock(&mutex);

	wait_count = thread->wait_count[1];

	pthread_mutex_unlock(&mutex);

	/* init and wait */
	pthread_barrier_init(&(thread->barrier[1]), NULL, wait_count);
	pthread_barrier_wait(&(thread->barrier[1]));
      }
    }

    /* set idle flag */
    pthread_mutex_lock(&mutex);

    thread->flags |= AGS_THREAD_IDLE;

    pthread_mutex_unlock(&mutex);

    /* run */
    ags_thread_run(thread);

    /* unset idle flag */
    pthread_mutex_lock(&mutex);

    thread->flags &= (~AGS_THREAD_IDLE);

    pthread_mutex_unlock(&mutex);

    /* unset initial run */
    if(initial_run){
      initial_run = FALSE;
    }
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
ags_thread_real_stop(AgsThread *thread)
{
  thread->flags &= (~AGS_THREAD_RUNNING);
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
