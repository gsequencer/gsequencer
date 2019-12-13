/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __AGS_BUSINESS_GROUP_MANAGER_H__
#define __AGS_BUSINESS_GROUP_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/server/security/ags_business_group.h>

G_BEGIN_DECLS

#define AGS_TYPE_BUSINESS_GROUP_MANAGER                (ags_business_group_manager_get_type())
#define AGS_BUSINESS_GROUP_MANAGER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_BUSINESS_GROUP_MANAGER, AgsBusinessGroupManager))
#define AGS_BUSINESS_GROUP_MANAGER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_BUSINESS_GROUP_MANAGER, AgsBusinessGroupManagerClass))
#define AGS_IS_BUSINESS_GROUP_MANAGER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_BUSINESS_GROUP_MANAGER))
#define AGS_IS_BUSINESS_GROUP_MANAGER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_BUSINESS_GROUP_MANAGER))
#define AGS_BUSINESS_GROUP_MANAGER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_BUSINESS_GROUP_MANAGER, AgsBusinessGroupManagerClass))

#define AGS_BUSINESS_GROUP_MANAGER_GET_OBJ_MUTEX(obj) (&(((AgsBusinessGroupManager *) obj)->obj_mutex))

typedef struct _AgsBusinessGroupManager AgsBusinessGroupManager;
typedef struct _AgsBusinessGroupManagerClass AgsBusinessGroupManagerClass;

struct _AgsBusinessGroupManager
{
  GObject gobject;

  GRecMutex obj_mutex;
  
  GList *business_group;
};

struct _AgsBusinessGroupManagerClass
{
  GObjectClass gobject;
};

GType ags_business_group_manager_get_type(void);

GList* ags_business_group_manager_get_business_group(AgsBusinessGroupManager *business_group_manager);

void ags_business_group_manager_add_business_group(AgsBusinessGroupManager *business_group_manager,
						   GObject *business_group);
void ags_business_group_manager_remove_business_group(AgsBusinessGroupManager *business_group_manager,
						      GObject *business_group);

/*  */
AgsBusinessGroupManager* ags_business_group_manager_get_instance();

AgsBusinessGroupManager* ags_business_group_manager_new();

G_END_DECLS

#endif /*__AGS_BUSINESS_GROUP_MANAGER_H__*/
