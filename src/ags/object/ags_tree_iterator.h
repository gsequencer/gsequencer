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

#ifndef __AGS_TREE_ITERATOR_H__
#define __AGS_TREE_ITERATOR_H__

#include <glib-object.h>

#define AGS_TYPE_TREE_ITERATOR                    (ags_tree_iterator_get_type())
#define AGS_TREE_ITERATOR(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_TREE_ITERATOR, AgsTreeIterator))
#define AGS_TREE_ITERATOR_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_TREE_ITERATOR, AgsTreeIteratorInterface))
#define AGS_IS_TREE_ITERATOR(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_TREE_ITERATOR))
#define AGS_IS_TREE_ITERATOR_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_TREE_ITERATOR))
#define AGS_TREE_ITERATOR_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_TREE_ITERATOR, AgsTreeIteratorInterface))

typedef void AgsTreeIterator;
typedef struct _AgsTreeIteratorInterface AgsTreeIteratorInterface;

struct _AgsTreeIteratorInterface
{
  GTypeInterface interface;

  void (*set_inverse_mode)(AgsTreeIterator *tree, gboolean mode);
  gboolean (*is_inverse_mode)(AgsTreeIterator *tree);
  
  void (*iterate)(AgsTreeIterator *tree);
  void (*iterate_nested)(AgsTreeIterator *tree);

  void (*safe_iterate)(AgsTreeIterator *toplevel, AgsTreeIterator *current);
  void (*safe_iterate_nested)(AgsTreeIterator *toplevel, AgsTreeIterator *current);
};

GType ags_tree_iterator_get_type();

void ags_tree_iterator_set_inverse_mode(AgsTreeIterator *tree, gboolean mode);
gboolean ags_tree_iterator_is_inverse_mode(AgsTreeIterator *tree);

void ags_tree_iterator_iterate(AgsTreeIterator *tree);
void ags_tree_iterator_iterate_nested(AgsTreeIterator *tree);

void ags_tree_iterator_safe_iterate(AgsTreeIterator *toplevel, AgsTreeIterator *current);
void ags_tree_iterator_safe_iterate_nested(AgsTreeIterator *toplevel, AgsTreeIterator *current);

#endif /*__AGS_TREE_ITERATOR_H__*/
