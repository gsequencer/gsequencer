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

  pthread_mutexattr_init(&(thread->mutexattr));
  pthread_mutexattr_settype(&(thread->mutexattr), PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(&(thread->mutex), &(thread->mutexattr));
  pthread_cond_init(&(thread->start_cond), NULL);
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
  if(thread == NULL){
    return;
  }

  pthread_mutex_lock(&(thread->mutex));
  thread->flags |= AGS_THREAD_LOCKED;
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

  thread->flags &= (~AGS_THREAD_LOCKED);
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

    return(FALSE);
  }

  if(thread == NULL){
    return(FALSE);
  }

  return(ags_thread_children_is_locked_recursive(thread->children));
}

gboolean
ags_thread_is_tree_syncing(AgsThread *thread)
{
  AgsThread *current;

  current = thread->parent;

  while(current != NULL){
    if((AGS_THREAD_TREE_SYNC & (thread->flags)) != 0){
      return(TRUE);
    }

    current = current->parent;
  }

  return(FALSE);
}

gboolean
ags_thread_is_tree_in_sync(AgsThread *thread)
{
  AgsThread *main_loop;
  guint wait_count;

  auto gboolean ags_thread_is_tree_in_sync(AgsThread *child, guint *wait_count);

  gboolean ags_thread_is_tree_in_sync(AgsThread *child, guint *wait_count){
    AgsThread *current;

    current = child;

    while(current != NULL){
      if(current == thread){
	current = current->next;

	continue;
      }

      (*wait_count)--;

      if((AGS_THREAD_WAIT & (current->flags)) == 0 &&
	 ((AGS_THREAD_TREE_SYNC & (current->flags) == 0 &&
	   (AGS_THREAD_WAIT & (current->flags)) == 0))){
	(*wait_count)++;
      }

      if((*wait_count) <= -2){
	return(FALSE);
      }

      ags_thread_is_tree_in_sync(child->children, wait_count);

      current = current->next;
    }

    if((*wait_count) <= -2){
      return(FALSE);
    }else{
      return(TRUE);
    }
  }

  main_loop = ags_thread_get_toplevel(thread);
  wait_count = 0;

  if(main_loop != thread){
    wait_count--;

    if((AGS_THREAD_WAIT & (main_loop->flags)) == 0 &&
       ((AGS_THREAD_TREE_SYNC & (main_loop->flags) == 0 &&
	 (AGS_THREAD_WAIT & (main_loop->flags)) == 0))){
      wait_count++;
    }
  }

  ags_thread_is_tree_in_sync(main_loop->children, &wait_count);
  
  if(wait_count <= -2){
    return(FALSE);
  }else{
    return(TRUE);
  }
}

void
ags_thread_main_loop_unlock_children(AgsThread *thread)
{
  AgsThread *main_loop;

  auto void ags_thread_main_loop_unlock_children_recursive(AgsThread *child);

  void ags_thread_main_loop_unlock_children_recursive(AgsThread *child){
    AgsThread *current;

    if(child == NULL){
      return;
    }

    current = child;

    while(child != NULL){
      child->flags &= (~AGS_THREAD_TREE_SYNC);

      ags_thread_main_loop_unlock_children_recursive(child->children);

      if((AGS_THREAD_BROADCAST_PARENT & (thread->flags)) == 0){
	pthread_cond_signal(&(child->cond));
      }else{
	pthread_cond_broadcast(&(child->cond));
      }

      child = child->next;
    }
  }

  main_loop = ags_thread_get_toplevel(thread);
  main_loop->flags &= (~AGS_THREAD_MAIN_LOOP_WAIT);

  pthread_cond_signal(&(thread->cond));
      
  ags_thread_main_loop_unlock_children_recursive(thread->children);
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
    current->flags |= AGS_THREAD_WAITING_FOR_CHILDREN;

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
    current->flags |= AGS_THREAD_WAITING_FOR_SIBLING;

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
      current->flags |= AGS_THREAD_WAITING_FOR_PARENT;
      
      current = current->prev;
    }
  }

  ags_thread_lock(thread);
  
  ags_thread_lock_children_recursive(thread->children);
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
    current->flags &= (~AGS_THREAD_WAITING_FOR_CHILDREN);

    if((AGS_THREAD_BROADCAST_PARENT & (thread->flags)) == 0){
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

    current->flags &= (~AGS_THREAD_WAITING_FOR_SIBLING);

    if((AGS_THREAD_BROADCAST_SIBLING & (thread->flags)) == 0){
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

    current = ags_thread_last(child);

    while(current != NULL){
      ags_thread_unlock_children_recursive(current->children);

      current->flags &= (~AGS_THREAD_WAITING_FOR_PARENT);

      if(!((AGS_THREAD_INITIAL_RUN & (thread->flags)) != 0 &&
	   AGS_IS_AUDIO_LOOP(thread))){

	if((AGS_THREAD_BROADCAST_CHILDREN & (thread->flags)) == 0){
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
	(((AGS_THREAD_IDLE & (current->flags)) != 0 ||
	  (AGS_THREAD_WAITING_FOR_CHILDREN & (current->flags)) == 0) ||
	 current->parent != parent)){
    pthread_cond_wait(&(current->cond),
		      &(current->mutex));

    if(!((AGS_THREAD_IDLE & (current->flags)) != 0 ||
	 (AGS_THREAD_WAITING_FOR_CHILDREN & (current->flags)) == 0)){    
      current = current->parent;
    }
  }

  /* unset flag */
  thread->flags &= (~AGS_THREAD_WAITING_FOR_PARENT);
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
	(((AGS_THREAD_IDLE & (current->flags)) != 0 ||
	  (AGS_THREAD_WAITING_FOR_SIBLING & (current->flags)) == 0) ||
	current->next != NULL)){
    if(current == thread){
      current = current->next;
      
      continue;
    }
    
    pthread_cond_wait(&(current->cond),
		      &(current->mutex));

    if(!((AGS_THREAD_IDLE & (current->flags)) != 0 ||
	 (AGS_THREAD_WAITING_FOR_SIBLING & (current->flags)) == 0)){
      current = current->next;
    }
  }

  /* unset flags */
  thread->flags &= (~AGS_THREAD_WAITING_FOR_SIBLING);
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
	  (((AGS_THREAD_IDLE & (child->flags)) != 0 ||
	    (AGS_THREAD_WAITING_FOR_PARENT & (child->flags)) == 0) ||
	   child->next != NULL)){
      if(initial_run){
	ags_thread_wait_children_recursive(child->children);

	initial_run = FALSE;
      }

      pthread_cond_wait(&(child->cond),
			&(child->mutex));
     
      if(!((AGS_THREAD_IDLE & (child->flags)) != 0 ||
	   (AGS_THREAD_WAITING_FOR_PARENT & (child->flags)) == 0)){
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
  thread->flags &= (~AGS_THREAD_WAITING_FOR_CHILDREN);
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

  ags_thread_lock(thread);

  thread->flags |= AGS_THREAD_INITIAL_RUN;

  ags_thread_unlock(thread);

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
  gboolean is_in_sync;
  gboolean wait_for_parent, wait_for_sibling, wait_for_children;

  thread = AGS_THREAD(ptr);

  /* signal AgsAudioLoop */
  if((AGS_THREAD_INITIAL_RUN & (thread->flags)) != 0 &&
     AGS_IS_TASK_THREAD(thread)){
    
    ags_thread_lock(thread);
    
    thread->flags &= (~AGS_THREAD_INITIAL_RUN);
    pthread_cond_signal(&(thread->start_cond));

    ags_thread_unlock(thread);
  }
  
  while((AGS_THREAD_RUNNING & (thread->flags)) != 0){

    /* barrier */
    if((AGS_THREAD_WAITING_FOR_BARRIER & (thread->flags)) != 0){
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
    ags_thread_lock(thread);

    g_message("tree sync\0");

    if(thread->parent != NULL){
      is_in_sync = FALSE;

      thread->parent->flags |= AGS_THREAD_TREE_SYNC;
    }else{
      if(ags_thread_is_tree_in_sync(thread)){
	ags_thread_main_loop_unlock_children(thread);
      }else{
	while((AGS_THREAD_MAIN_LOOP_WAIT & (thread->flags)) != 0 &&
	      !ags_thread_is_tree_in_sync(thread)){
	  pthread_cond_wait(&(thread->cond),
			    &(thread->mutex));
	}	
      }

      thread->flags |= AGS_THREAD_MAIN_LOOP_WAIT;
    }

    ags_thread_unlock(thread);

    if(ags_thread_is_tree_in_sync(thread)){
      AgsThread *main_loop;
      
      main_loop = ags_thread_get_toplevel(thread);

      ags_thread_lock(thread);

      ags_thread_main_loop_unlock_children(main_loop);

      ags_thread_unlock(thread);
    }else{
      ags_thread_lock(thread);

      while(thread->parent != NULL && (!is_in_sync ||
				       ags_thread_is_tree_syncing(thread))){
	pthread_cond_wait(&(thread->cond),
			  &(thread->mutex));
	
	if(!is_in_sync && (thread->parent->parent == NULL ||
			   (AGS_THREAD_TREE_SYNC & (thread->parent->parent->flags)) != 0)){
	  is_in_sync = TRUE;
	}
      }
	
      ags_thread_unlock(thread);
    }

    /* lock parent */
    ags_thread_lock(thread);

    /* set idle flag */
    thread->flags |= AGS_THREAD_IDLE;

    if((AGS_THREAD_WAIT_FOR_PARENT & (thread->flags)) != 0){
      g_message("%s - %s\0", G_OBJECT_TYPE_NAME(thread),
		"lock parent\0");

      wait_for_parent = TRUE;

      ags_thread_unlock(thread);
      thread->flags |= AGS_THREAD_WAITING_FOR_PARENT;
      ags_thread_lock_parent(thread, NULL);
    }else{
      ags_thread_unlock(thread);

      wait_for_parent = FALSE;
    }

    /* lock sibling */
    if((AGS_THREAD_WAIT_FOR_SIBLING & (thread->flags)) != 0){
      g_message("%s - %s\0", G_OBJECT_TYPE_NAME(thread),
		"lock sibling\0");

      wait_for_sibling = TRUE;

      thread->flags |= AGS_THREAD_WAITING_FOR_SIBLING;
      ags_thread_lock_sibling(thread);
    }else{
      wait_for_sibling = FALSE;
    }

    /* lock_children */
    if((AGS_THREAD_WAIT_FOR_CHILDREN & (thread->flags)) != 0){
      g_message("%s - %s\0", G_OBJECT_TYPE_NAME(thread),
		"lock children\0");
      if(AGS_IS_AUDIO_LOOP(thread) &&
	 AGS_AUDIO_LOOP(thread)->initial_passed){
	AGS_AUDIO_LOOP(thread)->initial_passed = FALSE;

	ags_thread_unlock(thread);
      }

      wait_for_children = TRUE;

      thread->flags |= AGS_THREAD_WAITING_FOR_CHILDREN;
      ags_thread_lock_children(thread);
    }else{
      wait_for_children = FALSE;
    }

    /* skip very first sync of AgsAudioLoop */
    if(!((AGS_THREAD_INITIAL_RUN & (thread->flags)) != 0 &&
	 AGS_IS_AUDIO_LOOP(thread))){

      /* wait parent */
      if(wait_for_parent){
	g_message("%s - %s\0", G_OBJECT_TYPE_NAME(thread),
		  "wait for parent\0");

	ags_thread_wait_parent(thread, NULL);
      }

      /* wait sibling */
      if(wait_for_sibling){
	g_message("%s - %s\0", G_OBJECT_TYPE_NAME(thread),
		  "wait for sibling\0");

	ags_thread_wait_sibling(thread);
      }

      /* wait children */
      if(wait_for_children){
	g_message("%s - %s\0", G_OBJECT_TYPE_NAME(thread),
		  "wait for children\0");

	ags_thread_wait_children(thread);
      }
  }

    /* run */
    ags_thread_run(thread);

    /* unset idle flag */
    thread->flags &= (~AGS_THREAD_IDLE);

    /* unlock parent */
    if(wait_for_parent){
      ags_thread_unlock_parent(thread, NULL);

      g_message("%s - %s\0", G_OBJECT_TYPE_NAME(thread),
		"unlock parent\0");
    }

    /* unlock sibling */
    if(wait_for_sibling){
      ags_thread_unlock_sibling(thread);

      g_message("%s - %s\0", G_OBJECT_TYPE_NAME(thread),
		"unlock sibling\0");
    }

    /* unlock children */
    if(wait_for_children){
      ags_thread_unlock_children(thread);

      g_message("%s - %s\0", G_OBJECT_TYPE_NAME(thread),
		"unlock children\0");
    }

    /* unset initial run */
    if((AGS_THREAD_INITIAL_RUN & (thread->flags)) != 0){
      ags_thread_lock(thread);

      thread->flags &= (~AGS_THREAD_INITIAL_RUN);

      ags_thread_unlock(thread);
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
