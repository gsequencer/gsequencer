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

#ifndef __AGS_CONCURRENT_TREE_H__
#define __AGS_CONCURRENT_TREE_H__

#include <glib-object.h>

#include <pthread.h>

#define AGS_TYPE_CONCURRENT_TREE                    (ags_concurrent_tree_get_type())
#define AGS_CONCURRENT_TREE(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CONCURRENT_TREE, AgsConcurrentTree))
#define AGS_CONCURRENT_TREE_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_CONCURRENT_TREE, AgsConcurrentTreeInterface))
#define AGS_IS_CONCURRENT_TREE(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_CONCURRENT_TREE))
#define AGS_IS_CONCURRENT_TREE_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_CONCURRENT_TREE))
#define AGS_CONCURRENT_TREE_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_CONCURRENT_TREE, AgsConcurrentTreeInterface))

typedef void AgsConcurrentTree;
typedef struct _AgsConcurrentTreeInterface AgsConcurrentTreeInterface;

struct _AgsConcurrentTreeInterface
{
  GTypeInterface interface;

  void (*set_parent_locked)(AgsConcurrentTree *concurrent_tree,
			    gboolean parent_locked);
  gboolean (*get_parent_locked)(AgsConcurrentTree *concurrent_tree);
  
  pthread_mutex_t* (*get_lock)(AgsConcurrentTree *concurrent_tree);
  pthread_mutex_t* (*get_parent_lock)(AgsConcurrentTree *concurrent_tree);
};

GType ags_concurrent_tree_get_type();

void ags_concurrent_tree_set_parent_locked(AgsConcurrentTree *concurrent_tree,
					   gboolean parent_locked);
gboolean ags_concurrent_tree_get_parent_locked(AgsConcurrentTree *concurrent_tree);

pthread_mutex_t* ags_concurrent_tree_get_lock(AgsConcurrentTree *concurrent_tree);
pthread_mutex_t* ags_concurrent_tree_get_parent_lock(AgsConcurrentTree *concurrent_tree);

gboolean ags_concurrent_tree_lock_context(AgsConcurrentTree *concurrent_tree);
void ags_concurrent_tree_unlock_context(AgsConcurrentTree *concurrent_tree);

#endif /*__AGS_CONCURRENT_TREE_H__*/
