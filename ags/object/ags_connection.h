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

#ifndef __AGS_CONNECTION_H__
#define __AGS_CONNECTION_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_CONNECTION                (ags_connection_get_type())
#define AGS_CONNECTION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CONNECTION, AgsConnection))
#define AGS_CONNECTION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CONNECTION, AgsConnectionClass))
#define AGS_IS_CONNECTION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CONNECTION))
#define AGS_IS_CONNECTION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CONNECTION))
#define AGS_CONNECTION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_CONNECTION, AgsConnectionClass))

typedef struct _AgsConnection AgsConnection;
typedef struct _AgsConnectionClass AgsConnectionClass;

struct _AgsConnection
{
  GObject object;

  pthread_mutexattr_t *mutexattr;
  pthread_mutex_t *mutex;
  
  GObject *data_object;
};

struct _AgsConnectionClass
{
  GObjectClass object;
};

GType ags_connection_get_type(void);

GList* ags_connection_find_type(GList *connection,
				GType connection_type);
GList* ags_connection_find_type_and_data_object_type(GList *connection,
						     GType connection_type,
						     GType data_object_type);
				
/*  */
AgsConnection* ags_connection_new();

#endif /*__AGS_CONNECTION_H__*/
