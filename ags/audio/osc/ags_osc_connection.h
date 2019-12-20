/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_OSC_CONNECTION_H__
#define __AGS_OSC_CONNECTION_H__

#include <glib.h>
#include <glib-object.h>

#include <gio/gio.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_OSC_CONNECTION                (ags_osc_connection_get_type ())
#define AGS_OSC_CONNECTION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OSC_CONNECTION, AgsOscConnection))
#define AGS_OSC_CONNECTION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_OSC_CONNECTION, AgsOscConnectionClass))
#define AGS_IS_OSC_CONNECTION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_OSC_CONNECTION))
#define AGS_IS_OSC_CONNECTION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_OSC_CONNECTION))
#define AGS_OSC_CONNECTION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_OSC_CONNECTION, AgsOscConnectionClass))

#define AGS_OSC_CONNECTION_GET_OBJ_MUTEX(obj) (&(((AgsOscConnection *) obj)->obj_mutex))

#define AGS_OSC_CONNECTION_TIMEOUT_USEC (250)
#define AGS_OSC_CONNECTION_DEAD_LINE_USEC (60000000)
#define AGS_OSC_CONNECTION_CHUNK_SIZE (131072)

#define AGS_OSC_CONNECTION_DEFAULT_CACHE_DATA_LENGTH (256)

typedef struct _AgsOscConnection AgsOscConnection;
typedef struct _AgsOscConnectionClass AgsOscConnectionClass;

typedef enum{
  AGS_OSC_CONNECTION_ACTIVE     = 1,
  AGS_OSC_CONNECTION_INET4      = 1 <<  1,
  AGS_OSC_CONNECTION_INET6      = 1 <<  2,
}AgsOscConnectionFlags;

struct _AgsOscConnection
{
  GObject gobject;

  guint flags;

  GRecMutex obj_mutex;

  GObject *osc_server;
  
  gchar *ip4;
  gchar *ip6;
  
  int fd;

  GSocket *socket;

  struct timespec *start_time;

  guchar *cache_data;

  guint cache_data_length;
  
  guchar *buffer;
  guint allocated_buffer_size;
  
  guint read_count;
  gboolean has_valid_data;
  
  struct timespec *timeout_delay;
  struct timespec *timestamp;
};

struct _AgsOscConnectionClass
{
  GObjectClass gobject;

  guchar* (*read_bytes)(AgsOscConnection *osc_connection,
			guint *data_length);
  gint64 (*write_response)(AgsOscConnection *osc_connection,
			   GObject *osc_response);
  
  void (*close)(AgsOscConnection *osc_connection);
};

GType ags_osc_connection_get_type(void);

gboolean ags_osc_connection_test_flags(AgsOscConnection *osc_connection, guint flags);
void ags_osc_connection_set_flags(AgsOscConnection *osc_connection, guint flags);
void ags_osc_connection_unset_flags(AgsOscConnection *osc_connection, guint flags);

gboolean ags_osc_connection_timeout_expired(struct timespec *start_time,
					    struct timespec *timeout_delay);

/* events */
guchar* ags_osc_connection_read_bytes(AgsOscConnection *osc_connection,
					     guint *data_length);

gint64 ags_osc_connection_write_response(AgsOscConnection *osc_connection,
					 GObject *osc_response);

void ags_osc_connection_close(AgsOscConnection *osc_connection);

/* instance */
AgsOscConnection* ags_osc_connection_new(GObject *osc_server);

G_END_DECLS

#endif /*__AGS_OSC_CONNECTION_H__*/
