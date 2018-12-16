/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <netinet/in.h>

#define AGS_TYPE_OSC_CLIENT                (ags_osc_client_get_type ())
#define AGS_OSC_CLIENT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OSC_CLIENT, AgsOscClient))
#define AGS_OSC_CLIENT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_OSC_CLIENT, AgsOscClientClass))
#define AGS_IS_OSC_CLIENT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_OSC_CLIENT))
#define AGS_IS_OSC_CLIENT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_OSC_CLIENT))
#define AGS_OSC_CLIENT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_OSC_CLIENT, AgsOscClientClass))

#define AGS_OSC_CLIENT_DEFAULT_MAX_ADDRESS_LENGTH (2048)

#define AGS_OSC_CLIENT_DEFAULT_SERVER_PORT (9000)
#define AGS_OSC_CLIENT_DEFAULT_DOMAIN "localhost"
#define AGS_OSC_CLIENT_DEFAULT_INET4_ADDRESS "127.0.0.1"
#define AGS_OSC_CLIENT_DEFAULT_INET6_ADDRESS "::1"

#define AGS_OSC_CLIENT_DEFAULT_MAX_RETRY (16)

#define AGS_OSC_CLIENT_CHUNK_SIZE (131072)

typedef struct _AgsOscClient AgsOscClient;
typedef struct _AgsOscClientClass AgsOscClientClass;

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

  pthread_mutex_t *obj_mutex;
  pthread_mutexattr_t *obj_mutexattr;
  
  gchar *ip4;
  gchar *ip6;

  gchar *domain;
  guint server_port;
  
  int ip4_fd;
  int ip6_fd;
  
  struct sockaddr_in *ip4_address;
  struct sockaddr_in6 *ip6_address;

  guint max_retry_count;

  struct timespec *retry_delay;

  struct timespec *start_time;

  unsigned char *buffer;
  guint allocated_buffer_size;
  
  guint read_count;

  struct timespec *timeout_delay;
};

struct _AgsOscClientClass
{
  GObjectClass gobject;

  void (*resolve)(AgsOscClient *osc_client);
  void (*connect)(AgsOscClient *osc_client);

  unsigned char* (*read_bytes)(AgsOscClient *osc_client,
			       guint *data_length);
  gboolean (*write_bytes)(AgsOscClient *osc_client,
			  guchar *data, guint data_length);
};

GType ags_osc_client_get_type(void);

pthread_mutex_t* ags_osc_client_get_class_mutex();

gboolean ags_osc_client_test_flags(AgsOscClient *osc_client, guint flags);
void ags_osc_client_set_flags(AgsOscClient *osc_client, guint flags);
void ags_osc_client_unset_flags(AgsOscClient *osc_client, guint flags);

gboolean ags_osc_client_timeout_expired(struct timespec *start_time,
					struct timespec *timeout_delay);

void ags_osc_client_resolve(AgsOscClient *osc_client);
void ags_osc_client_connect(AgsOscClient *osc_client);

unsigned char* ags_osc_client_read_bytes(AgsOscClient *osc_client,
					 guint *data_length);

gboolean ags_osc_client_write_bytes(AgsOscClient *osc_client,
				    guchar *data, guint data_length);

AgsOscClient* ags_osc_client_new();

#endif /*__AGS_OSC_CLIENT_H__*/
