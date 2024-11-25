/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#ifndef __AGS_ATOMIC_H__
#define __AGS_ATOMIC_H__

#include <glib.h>
#include <glib-object.h>

#include <stdatomic.h>

G_BEGIN_DECLS

#define ags_atomic_boolean_get(ptr) ((gboolean) __atomic_load_n(((volatile gboolean *) ptr), __ATOMIC_RELEASE))
#define ags_atomic_uint_get(ptr) ((guint) __atomic_load_n(((volatile guint *) ptr), __ATOMIC_RELEASE))
#define ags_atomic_int_get(ptr) ((gint) __atomic_load_n(((volatile gint *) ptr), __ATOMIC_RELEASE))
#define ags_atomic_pointer_get(ptr) ((gpointer) __atomic_load_n(((volatile gpointer *) ptr), __ATOMIC_RELEASE))

#define ags_atomic_boolean_set(ptr, bval) (__atomic_store_n((volatile gboolean *) ptr, bval, __ATOMIC_SEQ_CST))
#define ags_atomic_uint_set(ptr, uval) (__atomic_store_n((volatile guint *) ptr, uval, __ATOMIC_SEQ_CST))
#define ags_atomic_int_set(ptr, ival) (__atomic_store_n((volatile gint *) ptr, ival, __ATOMIC_SEQ_CST))
#define ags_atomic_pointer_set(ptr, pval) (__atomic_store_n((volatile gpointer *) ptr, pval, __ATOMIC_SEQ_CST))

#define ags_atomic_uint_or(ptr, uval) ((guint) __atomic_or_fetch((volatile guint *) ptr, uval, __ATOMIC_RELEASE))
#define ags_atomic_int_or(ptr, ival) ((gint) __atomic_or_fetch((volatile gint *) ptr, ival, __ATOMIC_RELEASE))

#define ags_atomic_uint_and(ptr, uval) ((guint) __atomic_and_fetch((volatile guint *) ptr, uval, __ATOMIC_RELEASE))
#define ags_atomic_int_and(ptr, ival) ((gint) __atomic_and_fetch((volatile gint *) ptr, ival, __ATOMIC_RELEASE))

#define ags_atomic_uint_increment(ptr) ((guint) __atomic_add_fetch((volatile guint *) ptr, 1, __ATOMIC_RELEASE))
#define ags_atomic_int_increment(ptr) ((gint) __atomic_add_fetch((volatile gint *) ptr, 1, __ATOMIC_RELEASE))

#define ags_atomic_uint_decrement(ptr) ((guint) __atomic_sub_fetch((volatile guint *) ptr, 1, __ATOMIC_RELEASE))
#define ags_atomic_int_decrement(ptr) ((gint) __atomic_sub_fetch((volatile gint *) ptr, 1, __ATOMIC_RELEASE))

#define ags_atomic_uint_add(ptr, uval) ((guint) __atomic_add_fetch((volatile guint *) ptr, uval, __ATOMIC_RELEASE))
#define ags_atomic_int_add(ptr, ival) ((gint) __atomic_add_fetch((volatile gint *) ptr, ival, __ATOMIC_RELEASE))

#define ags_atomic_uint_sub(ptr, uval) ((guint) __atomic_sub_fetch((volatile guint *) ptr, uval, __ATOMIC_RELEASE))
#define ags_atomic_int_sub(ptr, ival) ((gint) __atomic_sub_fetch((volatile gint *) ptr, ival, __ATOMIC_RELEASE))

G_END_DECLS

#endif /*__AGS_ATOMIC_H__*/
