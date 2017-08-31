/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __AGS_SERVER_STATUS_H__
#define __AGS_SERVER_STATUS_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_SERVER_STATUS                (ags_server_status_get_type())
#define AGS_SERVER_STATUS(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SERVER_STATUS, AgsServerStatus))
#define AGS_SERVER_STATUS_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_SERVER_STATUS, AgsServerStatusClass))
#define AGS_IS_SERVER_STATUS(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SERVER_STATUS))
#define AGS_IS_SERVER_STATUS_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SERVER_STATUS))
#define AGS_SERVER_STATUS_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_SERVER_STATUS, AgsServerStatusClass))

typedef struct _AgsServerStatus AgsServerStatus;
typedef struct _AgsServerStatusClass AgsServerStatusClass;

typedef enum{
  AGS_SERVER_STATUS_CONNECTED      = 1,
}AgsServerStatusFlags;

struct _AgsServerStatus
{
  GObject object;

  guint flags;

  gchar **authentication_module;
  
  guint maximum_connections;
  guint connection_count;
};

struct _AgsServerStatusClass
{
  GObjectClass object;
};

GType ags_server_status_get_type();

AgsServerStatus* ags_server_status_new();

#endif /*__AGS_SERVER_STATUS_H__*/
