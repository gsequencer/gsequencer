/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_OSC_CLIENT_H__
#define __AGS_OSC_CLIENT_H__

#include <glib.h>
#include <glib-object.h>

#include <gio/gio.h>

G_BEGIN_DECLS

#define AGS_TYPE_OSC_CLIENT                (ags_osc_client_get_type ())
#define AGS_OSC_CLIENT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OSC_CLIENT, AgsOscClient))
#define AGS_OSC_CLIENT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_OSC_CLIENT, AgsOscClientClass))
#define AGS_IS_OSC_CLIENT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_OSC_CLIENT))
#define AGS_IS_OSC_CLIENT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_OSC_CLIENT))
#define AGS_OSC_CLIENT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_OSC_CLIENT, AgsOscClientClass))

#define AGS_OSC_CLIENT_GET_OBJ_MUTEX(obj) (&(((AgsOscClient *) obj)->obj_mutex))

#define AGS_OSC_CLIENT_DEFAULT_MAX_ADDRESS_LENGTH (2048)

#define AGS_OSC_CLIENT_DEFAULT_SERVER_PORT (9000)
#define AGS_OSC_CLIENT_DEFAULT_DOMAIN "localhost"
#define AGS_OSC_CLIENT_DEFAULT_INET4_ADDRESS "127.0.0.1"
#define AGS_OSC_CLIENT_DEFAULT_INET6_ADDRESS "::1"

#define AGS_OSC_CLIENT_DEFAULT_MAX_RETRY (16)

#define AGS_OSC_CLIENT_CHUNK_SIZE (131072)

#define AGS_OSC_CLIENT_DEFAULT_CACHE_DATA_LENGTH (256)

typedef struct _AgsOscClient AgsOscClient;
typedef struct _AgsOscClientClass AgsOscClientClass;

/**
 * AgsOscClientFlags:
 * @AGS_OSC_CLIENT_INET4: use IPv4 socket
 * @AGS_OSC_CLIENT_INET6: use IPv6 socket
 * @AGS_OSC_CLIENT_UDP: use UDP transport protocol
 * @AGS_OSC_CLIENT_TCP: used TCP transport protocol
 * 
 * Enum values to configure OSC client.
 */
typedef enum{
  AGS_OSC_CLIENT_INET4      = 1,
  AGS_OSC_CLIENT_INET6      = 1 <<  1,
  AGS_OSC_CLIENT_UDP        = 1 <<  2,
  AGS_OSC_CLIENT_TCP        = 1 <<  3,
}AgsOscClientFlags;

struct _AgsOscClient
{
  GObject gobject;

  guint flags;
  guint connectable_flags;
  
  GRecMutex obj_mutex;
  
  gchar *ip4;
  gchar *ip6;

  gchar *domain;
  guint server_port;
  
  int ip4_fd;
  int ip6_fd;

  GSocket *ip4_socket;
  GSocket *ip6_socket;

  GSocketAddress *ip4_address;
  GSocketAddress *ip6_address;
  
  guint max_retry_count;

  struct timespec *retry_delay;

  struct timespec *start_time;

  guchar *cache_data;
  guint cache_data_length;

  guchar *buffer;
  guint allocated_buffer_size;
  
  guint read_count;
  gboolean has_valid_data;

  struct timespec *timeout_delay;
};

struct _AgsOscClientClass
{
  GObjectClass gobject;

  void (*resolve)(AgsOscClient *osc_client);
  void (*connect)(AgsOscClient *osc_client);

  guchar* (*read_bytes)(AgsOscClient *osc_client,
			guint *data_length);
  gboolean (*write_bytes)(AgsOscClient *osc_client,
			  guchar *data, guint data_length);
};

GType ags_osc_client_get_type(void);
GType ags_osc_client_flags_get_type();

gboolean ags_osc_client_test_flags(AgsOscClient *osc_client, guint flags);
void ags_osc_client_set_flags(AgsOscClient *osc_client, guint flags);
void ags_osc_client_unset_flags(AgsOscClient *osc_client, guint flags);

void ags_osc_client_resolve(AgsOscClient *osc_client);
void ags_osc_client_connect(AgsOscClient *osc_client);

guchar* ags_osc_client_read_bytes(AgsOscClient *osc_client,
				  guint *data_length);

gboolean ags_osc_client_write_bytes(AgsOscClient *osc_client,
				    guchar *data, guint data_length);

AgsOscClient* ags_osc_client_new();

G_END_DECLS

#endif /*__AGS_OSC_CLIENT_H__*/
