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

#include <ags/audio/osc/ags_osc_client.h>

#include <sys/types.h>
#include <sys/socket.h>

void ags_osc_client_class_init(AgsOscClientClass *osc_client);
void ags_osc_client_init(AgsOscClient *osc_client);
void ags_osc_client_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_osc_client_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_osc_client_finalize(GObject *gobject);

void ags_osc_client_real_connect(AgsOscClient *osc_client);

/**
 * SECTION:ags_osc_client
 * @short_description: the OSC client
 * @title: AgsOscClient
 * @section_id:
 * @include: ags/audio/osc/ags_osc_client.h
 *
 * #AgsOscClient your osc client.
 */

enum{
  PROP_0,
};

enum{
  CONNECT,
  LAST_SIGNAL,
};

static gpointer ags_osc_client_parent_class = NULL;
static guint osc_client_signals[LAST_SIGNAL];

static pthread_mutex_t ags_osc_client_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_osc_client_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_osc_client = 0;

    static const GTypeInfo ags_osc_client_info = {
      sizeof (AgsOscClientClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_osc_client_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOscClient),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_osc_client_init,
    };

    ags_type_osc_client = g_type_register_static(G_TYPE_OBJECT,
						 "AgsOscClient", &ags_osc_client_info,
						 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_osc_client);
  }

  return g_define_type_id__volatile;
}

void
ags_osc_client_class_init(AgsOscClientClass *osc_client)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_osc_client_parent_class = g_type_class_peek_parent(osc_client);

  /* GObjectClass */
  gobject = (GObjectClass *) osc_client;

  gobject->set_property = ags_osc_client_set_property;
  gobject->get_property = ags_osc_client_get_property;
  
  gobject->finalize = ags_osc_client_finalize;

  /* AgsOscClientClass */
  osc_client->connect = ags_osc_client_real_connect;
  
  /* signals */
  /**
   * AgsOscClient::connect:
   * @osc_client: the #AgsOscClient
   *
   * The ::connect signal is emited as connection is establishing.
   *
   * Since: 2.1.0
   */
  osc_client_signals[CONNECT] =
    g_signal_new("connect",
		 G_TYPE_FROM_CLASS(osc_client),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscClientClass, connect),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_osc_client_init(AgsOscClient *osc_client)
{
  osc_client->flags = AGS_OSC_CLIENT_INET4;
  
  /* osc client mutex */
  osc_client->obj_mutexattr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(osc_client->obj_mutexattr);
  pthread_mutexattr_settype(osc_client->obj_mutexattr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(osc_client->obj_mutexattr,
				PTHREAD_PRIO_INHERIT);
#endif

  osc_client->obj_mutex =  (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(osc_client->obj_mutex,
		     osc_client->obj_mutexattr);

  osc_client->ip4 = g_strdup(AGS_OSC_CLIENT_DEFAULT_INET4_ADDRESS);
  osc_client->ip6 = g_strdup(AGS_OSC_CLIENT_DEFAULT_INET6_ADDRESS);

  osc_client->server_port = AGS_OSC_CLIENT_DEFAULT_SERVER_PORT;
  
  osc_client->ip4_fd = -1;
  osc_client->ip6_fd = -1;
  
  osc_client->ip4_address = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));
  memset(osc_client->ip4_address, 0, sizeof(struct sockaddr_in));
  
  osc_client->ip6_address = (struct sockaddr_in6 *) malloc(sizeof(struct sockaddr_in6));
  memset(osc_client->ip6_address, 0, sizeof(struct sockaddr_in6));

  osc_client->max_retry_count = AGS_OSC_CLIENT_DEFAULT_MAX_RETRY;

  osc_client->retry_delay = (struct timespec *) malloc(sizeof(struct timespec));

  osc_client->retry_delay->tv_sec = 5;
  osc_client->retry_delay->tv_nsec = 0;
}

void
ags_osc_client_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsOscClient *osc_client;

  pthread_mutex_t *osc_client_mutex;

  osc_client = AGS_OSC_CLIENT(gobject);

  /* get osc client mutex */
  pthread_mutex_lock(ags_osc_client_get_class_mutex());
  
  osc_client_mutex = osc_client->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_client_get_class_mutex());
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_client_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsOscClient *osc_client;

  pthread_mutex_t *osc_client_mutex;

  osc_client = AGS_OSC_CLIENT(gobject);

  /* get osc client mutex */
  pthread_mutex_lock(ags_osc_client_get_class_mutex());
  
  osc_client_mutex = osc_client->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_client_get_class_mutex());
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_client_finalize(GObject *gobject)
{
  AgsOscClient *osc_client;
    
  osc_client = (AgsOscClient *) gobject;

  pthread_mutex_destroy(osc_client->obj_mutex);
  free(osc_client->obj_mutex);

  pthread_mutexattr_destroy(osc_client->obj_mutexattr);
  free(osc_client->obj_mutexattr);

  /* call parent */
  G_OBJECT_CLASS(ags_osc_client_parent_class)->finalize(gobject);
}

/**
 * ags_osc_client_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.1.0
 */
pthread_mutex_t*
ags_osc_client_get_class_mutex()
{
  return(&ags_osc_client_class_mutex);
}

/**
 * ags_osc_client_test_flags:
 * @osc_client: the #AgsOscClient
 * @flags: the flags
 *
 * Test @flags to be set on @osc_client.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 2.1.0
 */
gboolean
ags_osc_client_test_flags(AgsOscClient *osc_client, guint flags)
{
  gboolean retval;  
  
  pthread_mutex_t *osc_client_mutex;

  if(!AGS_IS_OSC_CLIENT(osc_client)){
    return(FALSE);
  }

  /* get osc_client mutex */
  pthread_mutex_lock(ags_osc_client_get_class_mutex());
  
  osc_client_mutex = osc_client->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_client_get_class_mutex());

  /* test */
  pthread_mutex_lock(osc_client_mutex);

  retval = (flags & (osc_client->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(osc_client_mutex);

  return(retval);
}

/**
 * ags_osc_client_set_flags:
 * @osc_client: the #AgsOscClient
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 2.1.0
 */
void
ags_osc_client_set_flags(AgsOscClient *osc_client, guint flags)
{
  pthread_mutex_t *osc_client_mutex;

  if(!AGS_IS_OSC_CLIENT(osc_client)){
    return;
  }

  /* get osc_client mutex */
  pthread_mutex_lock(ags_osc_client_get_class_mutex());
  
  osc_client_mutex = osc_client->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_client_get_class_mutex());

  /* set flags */
  pthread_mutex_lock(osc_client_mutex);

  osc_client->flags |= flags;

  pthread_mutex_unlock(osc_client_mutex);
}

/**
 * ags_osc_client_unset_flags:
 * @osc_client: the #AgsOscClient
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 2.1.0
 */
void
ags_osc_client_unset_flags(AgsOscClient *osc_client, guint flags)
{
  pthread_mutex_t *osc_client_mutex;

  if(!AGS_IS_OSC_CLIENT(osc_client)){
    return;
  }

  /* get osc_client mutex */
  pthread_mutex_lock(ags_osc_client_get_class_mutex());
  
  osc_client_mutex = osc_client->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_client_get_class_mutex());

  /* set flags */
  pthread_mutex_lock(osc_client_mutex);

  osc_client->flags &= (~flags);

  pthread_mutex_unlock(osc_client_mutex);
}

void
ags_osc_client_real_connect(AgsOscClient *osc_client)
{
  gboolean ip4_success, ip6_success;
  gboolean ip4_udp_success, ip4_tcp_success;
  gboolean ip6_udp_success, ip6_tcp_success;
  gboolean ip4_connected, ip6_connected;
  guint i;
  
  ip4_success = FALSE;
  ip6_success = FALSE;
  
  ip4_udp_success = FALSE;
  ip4_tcp_success = FALSE;

  ip6_udp_success = FALSE;
  ip6_tcp_success = FALSE;
  
  if(ags_osc_client_test_flags(osc_client, AGS_OSC_CLIENT_INET4)){
    ip4_success = TRUE;
  
    if(ags_osc_client_test_flags(osc_client, AGS_OSC_CLIENT_UDP)){
      ip4_udp_success = TRUE;
      
      osc_client->ip4_fd = socket(AF_INET, SOCK_DGRAM, 0);
    }else if(ags_osc_client_test_flags(osc_client, AGS_OSC_CLIENT_TCP)){
      ip4_tcp_success = TRUE;

      osc_client->ip4_fd = socket(AF_INET, SOCK_STREAM, 0);
    }else{
      g_critical("no flow control type");
    }

    inet_pton(AF_INET, osc_client->ip4, &(osc_client->ip4_address->sin_addr.s_addr));
    osc_client->ip4_address->sin_port = htons(osc_client->server_port);
  }

  if(ags_osc_client_test_flags(osc_client, AGS_OSC_CLIENT_INET6)){
    ip6_success = TRUE;
  
    if(ags_osc_client_test_flags(osc_client, AGS_OSC_CLIENT_UDP)){
      ip6_udp_success = TRUE;

      osc_client->ip6_fd = socket(AF_INET6, SOCK_DGRAM, 0);
    }else if(ags_osc_client_test_flags(osc_client, AGS_OSC_CLIENT_TCP)){
      ip6_tcp_success = TRUE;

      osc_client->ip6_fd = socket(AF_INET6, SOCK_STREAM, 0);
    }else{
      g_critical("no flow control type");
    }

    inet_pton(AF_INET6, osc_client->ip6, &(osc_client->ip6_address->sin6_addr.s6_addr));
    osc_client->ip6_address->sin6_port = htons(osc_client->server_port);
  }
  
  if(!ip4_success && !ip6_success){
    g_critical("no protocol family");

    return;
  }

  if(!ip4_udp_success && !ip4_tcp_success &&
     !ip6_udp_success && !ip6_tcp_success){
    return;
  }

  ip4_connected = FALSE;
  ip6_connected = FALSE;

  i = 0;
  
  while(((ip4_udp_success || ip4_tcp_success) && !ip4_connected) ||
	((ip6_udp_success || ip6_tcp_success) && !ip6_connected)){
    if(!ip4_connected){
      if(ip4_udp_success || ip4_tcp_success){
	int rc;
	
	rc = connect(osc_client->ip4_fd, osc_client->ip4_address, sizeof(struct sockaddr_in));

	if(rc == 0){
	  ip4_connected = TRUE;
	}
      }
    }

    if(!ip6_connected){
      if(ip6_udp_success || ip6_tcp_success){
	int rc;
	
	rc = connect(osc_client->ip6_fd, osc_client->ip6_address, sizeof(struct sockaddr_in6));

	if(rc == 0){
	  ip6_connected = TRUE;
	}
      }
    }

    if(i >= osc_client->max_retry_count){
      break;
    }

    nanosleep(osc_client->retry_delay, NULL);

    i++;
  }
}
  
/**
 * Ags_osc_client_connect:
 * @osc_client: the #AgsOscClient
 * 
 * Connect OSC client.
 * 
 * Since: 2.1.0
 */
void
ags_osc_client_connect(AgsOscClient *osc_client)
{
  g_return_if_fail(AGS_IS_OSC_CLIENT(osc_client));
  
  g_object_ref((GObject *) osc_client);
  g_signal_emit(G_OBJECT(osc_client),
		osc_client_signals[CONNECT], 0);
  g_object_unref((GObject *) osc_client);
}

/**
 * ags_osc_client_new:
 * 
 * Creates a new instance of #AgsOscClient
 *
 * Returns: the new #AgsOscClient
 * 
 * Since: 2.1.0
 */
AgsOscClient*
ags_osc_client_new()
{
  AgsOscClient *osc_client;
   
  osc_client = (AgsOscClient *) g_object_new(AGS_TYPE_OSC_CLIENT,
					     NULL);
  
  return(osc_client);
}
