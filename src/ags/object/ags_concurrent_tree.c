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
 * @short_description: unique access to recalls
 * @title: AgsConcurrentTree
 * @section_id:
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
						      "AgsConcurrentTree\0", &ags_concurrent_tree_info,
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
 * ags_concurrent_tree_get_lock:
 * @concurrent_tree an #AgsConcurrent_Tree
 *
 * Get tree node's lock.
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
 * @concurrent_tree an #AgsConcurrent_Tree
 *
 * Get tree node's parent lock.
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

gboolean
ags_concurrent_tree_lock_context(AgsConcurrentTree *concurrent_tree)
{
  pthread_mutex_t *parent_mutex, *mutex;

  gboolean lock_parent = TRUE;
  
  parent_mutex = ags_concurrent_tree_get_parent_lock(concurrent_tree);
  mutex = ags_concurrent_tree_get_lock(concurrent_tree);

  if(lock_parent){
    static pthread_mutex_t exclusive_lock = PTHREAD_MUTEX_INITIALIZER;

    pthread_mutex_lock(&exclusive_lock);
    
    pthread_mutex_lock(parent_mutex);
    pthread_mutex_lock(mutex);

    pthread_mutex_unlock(&exclusive_lock);

    return(TRUE);
  }
}

void
ags_concurrent_tree_unlock_context(AgsConcurrentTree *concurrent_tree)
{
  pthread_mutex_t *parent_mutex, *mutex;

  gboolean lock_parent = TRUE;
  
  parent_mutex = ags_concurrent_tree_get_parent_lock(concurrent_tree);
  mutex = ags_concurrent_tree_get_lock(concurrent_tree);

  if(lock_parent){
    pthread_mutex_unlock(mutex);
    pthread_mutex_unlock(parent_mutex);
  }
}
