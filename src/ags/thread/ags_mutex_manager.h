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

#ifndef __AGS_MUTEX_MANAGER_H__
#define __AGS_MUTEX_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_MUTEX_MANAGER                (ags_mutex_manager_get_type())
#define AGS_MUTEX_MANAGER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MUTEX_MANAGER, AgsMutexManager))
#define AGS_MUTEX_MANAGER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_MUTEX_MANAGER, AgsMutexManagerClass))
#define AGS_IS_MUTEX_MANAGER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MUTEX_MANAGER))
#define AGS_IS_MUTEX_MANAGER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MUTEX_MANAGER))
#define AGS_MUTEX_MANAGER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_MUTEX_MANAGER, AgsMutexManagerClass))

typedef struct _AgsMutexManager AgsMutexManager;
typedef struct _AgsMutexManagerClass AgsMutexManagerClass;

struct _AgsMutexManager
{
  GObject object;

  GHashTable *lock_object;
};

struct _AgsMutexManagerClass
{
  GObjectClass object;
};

GType ags_mutex_manager_get_type();

gboolean ags_mutex_manager_insert(AgsMutexManager *mutex_manager,
				  GObject *lock_object, pthread_mutex_t *mutex);
gboolean ags_mutex_manager_remove(AgsMutexManager *mutex_manager,
				  GObject *lock_object);

pthread_mutex_t* ags_mutex_manager_lookup(AgsMutexManager *mutex_manager,
					  GObject *lock_object);

AgsMutexManager* ags_mutex_manager_get_instance();
AgsMutexManager* ags_mutex_manager_new();

#endif /*__AGS_MUTEX_MANAGER_H__*/
