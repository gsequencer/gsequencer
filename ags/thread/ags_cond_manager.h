/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __AGS_CONDITION_MANAGER_H__
#define __AGS_CONDITION_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_CONDITION_MANAGER                (ags_condition_manager_get_type())
#define AGS_CONDITION_MANAGER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CONDITION_MANAGER, AgsConditionManager))
#define AGS_CONDITION_MANAGER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_CONDITION_MANAGER, AgsConditionManagerClass))
#define AGS_IS_CONDITION_MANAGER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CONDITION_MANAGER))
#define AGS_IS_CONDITION_MANAGER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CONDITION_MANAGER))
#define AGS_CONDITION_MANAGER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_CONDITION_MANAGER, AgsConditionManagerClass))

typedef struct _AgsConditionManager AgsConditionManager;
typedef struct _AgsConditionManagerClass AgsConditionManagerClass;

struct _AgsConditionManager
{
  GObject object;

  GHashTable *lock_object;
};

struct _AgsConditionManagerClass
{
  GObjectClass object;
};

GType ags_condition_manager_get_type();

pthread_cond_t* ags_condition_manager_get_application_condition(AgsConditionManager *condition_manager);

gboolean ags_condition_manager_insert(AgsConditionManager *condition_manager,
				      GObject *lock_object, pthread_cond_t *condition);
gboolean ags_condition_manager_remove(AgsConditionManager *condition_manager,
				      GObject *lock_object);

pthread_cond_t* ags_condition_manager_lookup(AgsConditionManager *condition_manager,
					     GObject *lock_object);

AgsConditionManager* ags_condition_manager_get_instance();
AgsConditionManager* ags_condition_manager_new();

#endif /*__AGS_CONDITION_MANAGER_H__*/
