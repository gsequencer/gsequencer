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

#ifndef __AGS_CONNECTION_MANAGER_H__
#define __AGS_CONNECTION_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/object/ags_connection.h>

#define AGS_TYPE_CONNECTION_MANAGER                (ags_connection_manager_get_type())
#define AGS_CONNECTION_MANAGER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CONNECTION_MANAGER, AgsConnectionManager))
#define AGS_CONNECTION_MANAGER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CONNECTION_MANAGER, AgsConnectionManagerClass))
#define AGS_IS_CONNECTION_MANAGER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CONNECTION_MANAGER))
#define AGS_IS_CONNECTION_MANAGER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CONNECTION_MANAGER))
#define AGS_CONNECTION_MANAGER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_CONNECTION_MANAGER, AgsConnectionManagerClass))

typedef struct _AgsConnectionManager AgsConnectionManager;
typedef struct _AgsConnectionManagerClass AgsConnectionManagerClass;

struct _AgsConnectionManager
{
  GObject object;
  
  GList *connection;
};

struct _AgsConnectionManagerClass
{
  GObjectClass object;
};

GType ags_connection_manager_get_type(void);

GList* ags_connection_manager_get_connection(AgsConnectionManager *connection_manager);

void ags_connection_manager_add_connection(AgsConnectionManager *connection_manager,
					   AgsConnection *connection);
void ags_connection_manager_remove_connection(AgsConnectionManager *connection_manager,
					      AgsConnection *connection);

/*  */
AgsConnectionManager* ags_connection_manager_get_instance();

AgsConnectionManager* ags_connection_manager_new();

#endif /*__AGS_CONNECTION_MANAGER_H__*/
