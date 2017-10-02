/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/object/ags_concurrent_tree.h>

#include <stdio.h>

void ags_concurrent_tree_base_init(AgsConcurrentTreeInterface *interface);

/**
 * SECTION:ags_concurrent_tree
 * @short_description: thread-safe locking of tree
 * @title: AgsConcurrentTree
 * @section_id: AgsConcurrentTree
 * @include: ags/object/ags_concurrent_tree.h
 *
 * The #AgsConcurrentTree interface provides essential locks to process tree
 * with parallelism.
 */

GType
ags_concurrent_tree_get_type()
{
  static GType ags_type_concurrent_tree = 0;

  if(!ags_type_concurrent_tree){
    static const GTypeInfo ags_concurrent_tree_info = {
      sizeof(AgsConcurrentTreeInterface),
      (GBaseInitFunc) ags_concurrent_tree_base_init,
      NULL, /* base_finalize */
    };

    ags_type_concurrent_tree = g_type_register_static(G_TYPE_INTERFACE,
						      "AgsConcurrentTree", &ags_concurrent_tree_info,
						      0);
  }

  return(ags_type_concurrent_tree);
}

void
ags_concurrent_tree_base_init(AgsConcurrentTreeInterface *interface)
{
  /* empty */
}

/**
 * ags_concurrent_tree_set_parent_locked:
 * @concurrent_tree: an #AgsConcurrent_Tree
 * @parent_locked: %TRUE means the parent is locked
 *
 * Set context owns parent lock
 *
 * Since: 1.0.0
 */
void
ags_concurrent_tree_set_parent_locked(AgsConcurrentTree *concurrent_tree,
				      gboolean parent_locked)
{
  AgsConcurrentTreeInterface *concurrent_tree_interface;

  g_return_if_fail(AGS_IS_CONCURRENT_TREE(concurrent_tree));
  concurrent_tree_interface = AGS_CONCURRENT_TREE_GET_INTERFACE(concurrent_tree);
  g_return_if_fail(concurrent_tree_interface->set_parent_locked);

  concurrent_tree_interface->set_parent_locked(concurrent_tree,
					       parent_locked);
}

/**
 * ags_concurrent_tree_get_parent_locked:
 * @concurrent_tree: an #AgsConcurrent_Tree
 *
 * If context owns parent lock
 *
 * Returns: the %TRUE if parent locked, else %FALSE
 *
 * Since: 1.0.0
 */
gboolean
ags_concurrent_tree_get_parent_locked(AgsConcurrentTree *concurrent_tree)
{
  AgsConcurrentTreeInterface *concurrent_tree_interface;

  g_return_val_if_fail(AGS_IS_CONCURRENT_TREE(concurrent_tree), FALSE);
  concurrent_tree_interface = AGS_CONCURRENT_TREE_GET_INTERFACE(concurrent_tree);
  g_return_val_if_fail(concurrent_tree_interface->get_parent_locked, FALSE);

  return(concurrent_tree_interface->get_parent_locked(concurrent_tree));
}

/**
 * ags_concurrent_tree_get_lock:
 * @concurrent_tree: an #AgsConcurrent_Tree
 *
 * Get tree node's lock.
 *
 * Returns: the pthread_mutex_t
 *
 * Since: 0.6
 */
pthread_mutex_t*
ags_concurrent_tree_get_lock(AgsConcurrentTree *concurrent_tree)
{
  AgsConcurrentTreeInterface *concurrent_tree_interface;

  g_return_val_if_fail(AGS_IS_CONCURRENT_TREE(concurrent_tree), NULL);
  concurrent_tree_interface = AGS_CONCURRENT_TREE_GET_INTERFACE(concurrent_tree);
  g_return_val_if_fail(concurrent_tree_interface->get_lock, NULL);

  return(concurrent_tree_interface->get_lock(concurrent_tree));
}

/**
 * ags_concurrent_tree_get_parent_lock:
 * @concurrent_tree: an #AgsConcurrent_Tree
 *
 * Get tree node's parent lock.
 *
 * Returns: the pthread_mutex_t
 *
 * Since: 0.6
 */
pthread_mutex_t*
ags_concurrent_tree_get_parent_lock(AgsConcurrentTree *concurrent_tree)
{
  AgsConcurrentTreeInterface *concurrent_tree_interface;

  g_return_val_if_fail(AGS_IS_CONCURRENT_TREE(concurrent_tree), NULL);
  concurrent_tree_interface = AGS_CONCURRENT_TREE_GET_INTERFACE(concurrent_tree);
  g_return_val_if_fail(concurrent_tree_interface->get_parent_lock, NULL);
  
  return(concurrent_tree_interface->get_parent_lock(concurrent_tree));
}

/**
 * ags_concurrent_tree_lock_context:
 * @concurrent_tree: the #AgsConcurrentTree
 *
 * Locks the context of @concurrent_tree and blocks until success.
 *
 * Returns: %TRUE if parent locked
 *
 * Since: 1.0.0
 */
gboolean
ags_concurrent_tree_lock_context(AgsConcurrentTree *concurrent_tree)
{
  pthread_mutex_t *parent_mutex, *mutex;

  int err;
  gboolean parent_locked;
  
  static pthread_mutex_t exclusive_lock = PTHREAD_MUTEX_INITIALIZER;

  parent_mutex = ags_concurrent_tree_get_parent_lock(concurrent_tree);
  mutex = ags_concurrent_tree_get_lock(concurrent_tree);


  pthread_mutex_lock(&exclusive_lock);

  //FIXME:JK: check depenendencies - probably should be pthread_mutex_trylock()
  pthread_mutex_lock(parent_mutex);
  parent_locked = TRUE;
  
  pthread_mutex_lock(mutex);

  pthread_mutex_unlock(&exclusive_lock);

  return(parent_locked);
}

/**
 * ags_concurrent_tree_unlock_context:
 * @concurrent_tree: the #AgsConcurrentTree
 * 
 * Unlocks the context of @concurrent_tree.
 *
 * Since: 1.0.0
 */
void
ags_concurrent_tree_unlock_context(AgsConcurrentTree *concurrent_tree)
{
  pthread_mutex_t *parent_mutex, *mutex;

  parent_mutex = ags_concurrent_tree_get_parent_lock(concurrent_tree);
  mutex = ags_concurrent_tree_get_lock(concurrent_tree);

  pthread_mutex_unlock(mutex);
  pthread_mutex_unlock(parent_mutex);
}
