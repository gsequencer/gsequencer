/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#ifndef __AGS_AUTOMATION_CONTROL_NAME_KEY_MANAGER_H__
#define __AGS_AUTOMATION_CONTROL_NAME_KEY_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define AGS_TYPE_AUTOMATION_CONTROL_NAME_KEY_MANAGER                (ags_automation_control_name_key_manager_get_type())
#define AGS_AUTOMATION_CONTROL_NAME_KEY_MANAGER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUTOMATION_CONTROL_NAME_KEY_MANAGER, AgsAutomationControlNameKeyManager))
#define AGS_AUTOMATION_CONTROL_NAME_KEY_MANAGER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUTOMATION_CONTROL_NAME_KEY_MANAGER, AgsAutomationControlNameKeyManagerClass))
#define AGS_IS_AUTOMATION_CONTROL_NAME_KEY_MANAGER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUTOMATION_CONTROL_NAME_KEY_MANAGER))
#define AGS_IS_AUTOMATION_CONTROL_NAME_KEY_MANAGER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUTOMATION_CONTROL_NAME_KEY_MANAGER))
#define AGS_AUTOMATION_CONTROL_NAME_KEY_MANAGER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_AUTOMATION_CONTROL_NAME_KEY_MANAGER, AgsAutomationControlNameKeyManagerClass))

#define AGS_AUTOMATION_CONTROL_NAME_KEY_MANAGER_GET_OBJ_MUTEX(obj) (&(((AgsAutomationControlNameKeyManager *) obj)->obj_mutex))

typedef struct _AgsAutomationControlNameKeyManager AgsAutomationControlNameKeyManager;
typedef struct _AgsAutomationControlNameKeyManagerClass AgsAutomationControlNameKeyManagerClass;

struct _AgsAutomationControlNameKeyManager
{
  GObject gobject;

  GRecMutex obj_mutex;

  GHashTable *control_name_key;
};

struct _AgsAutomationControlNameKeyManagerClass
{
  GObjectClass gobject;
};

GType ags_automation_control_name_key_manager_get_type(void);

GRecMutex* ags_automation_control_name_key_manager_get_obj_mutex(AgsAutomationControlNameKeyManager *automation_control_name_key_manager);

/* find, insert and return key */
gpointer ags_automation_control_name_key_manager_find_automation(AgsAutomationControlNameKeyManager *automation_control_name_key_manager,
								 gchar *control_name);

/* instance */
AgsAutomationControlNameKeyManager* ags_automation_control_name_key_manager_get_instance();
AgsAutomationControlNameKeyManager* ags_automation_control_name_key_manager_new();

G_END_DECLS

#endif /*__AGS_AUTOMATION_CONTROL_NAME_KEY_MANAGER_H__*/
