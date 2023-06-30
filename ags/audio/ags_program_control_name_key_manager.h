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

#ifndef __AGS_PROGRAM_CONTROL_NAME_KEY_MANAGER_H__
#define __AGS_PROGRAM_CONTROL_NAME_KEY_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define AGS_TYPE_PROGRAM_CONTROL_NAME_KEY_MANAGER                (ags_program_control_name_key_manager_get_type())
#define AGS_PROGRAM_CONTROL_NAME_KEY_MANAGER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PROGRAM_CONTROL_NAME_KEY_MANAGER, AgsProgramControlNameKeyManager))
#define AGS_PROGRAM_CONTROL_NAME_KEY_MANAGER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PROGRAM_CONTROL_NAME_KEY_MANAGER, AgsProgramControlNameKeyManagerClass))
#define AGS_IS_PROGRAM_CONTROL_NAME_KEY_MANAGER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PROGRAM_CONTROL_NAME_KEY_MANAGER))
#define AGS_IS_PROGRAM_CONTROL_NAME_KEY_MANAGER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PROGRAM_CONTROL_NAME_KEY_MANAGER))
#define AGS_PROGRAM_CONTROL_NAME_KEY_MANAGER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_PROGRAM_CONTROL_NAME_KEY_MANAGER, AgsProgramControlNameKeyManagerClass))

#define AGS_PROGRAM_CONTROL_NAME_KEY_MANAGER_GET_OBJ_MUTEX(obj) (&(((AgsProgramControlNameKeyManager *) obj)->obj_mutex))

typedef struct _AgsProgramControlNameKeyManager AgsProgramControlNameKeyManager;
typedef struct _AgsProgramControlNameKeyManagerClass AgsProgramControlNameKeyManagerClass;

struct _AgsProgramControlNameKeyManager
{
  GObject gobject;

  GRecMutex obj_mutex;

  GHashTable *control_name_key;
};

struct _AgsProgramControlNameKeyManagerClass
{
  GObjectClass gobject;
};

GType ags_program_control_name_key_manager_get_type(void);

GRecMutex* ags_program_control_name_key_manager_get_obj_mutex(AgsProgramControlNameKeyManager *program_control_name_key_manager);

/* find, insert and return key */
gpointer ags_program_control_name_key_manager_find_program(AgsProgramControlNameKeyManager *program_control_name_key_manager,
							   gchar *control_name);

/* instance */
AgsProgramControlNameKeyManager* ags_program_control_name_key_manager_get_instance();
AgsProgramControlNameKeyManager* ags_program_control_name_key_manager_new();

G_END_DECLS

#endif /*__AGS_PROGRAM_CONTROL_NAME_KEY_MANAGER_H__*/
