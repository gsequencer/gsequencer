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

#include <ags/libags.h>

#include <ags/audio/osc/ags_osc_util.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <arpa/inet.h>

#ifdef __APPLE__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#include <ags/i18n.h>

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

void ags_osc_client_real_resolve(AgsOscClient *osc_client);
void ags_osc_client_real_connect(AgsOscClient *osc_client);

unsigned char* ags_osc_client_real_read_bytes(AgsOscClient *osc_client,
					      guint *data_length);
gboolean ags_osc_client_real_write_bytes(AgsOscClient *osc_client,
					 guchar *data, guint data_length);

/**
 * SECTION:ags_osc_client
 * @short_description: the OSC client
 * @title: AgsOscClient
 * @section_id:
 * @include: ags/audio/osc/ags_osc_client.h
 *
 * #AgsOscClient your OSC client.
 */

enum{
  PROP_0,
  PROP_DOMAIN,
  PROP_SERVER_PORT,
  PROP_IP4,
  PROP_IP6,
};

enum{
  RESOLVE,
  CONNECT,
  READ_BYTES,
  WRITE_BYTES,
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

  /* properties */
  /**
   * AgsOscClient:domain:
   *
   * The domain to lookup server.
   * 
   * Since: 2.1.0
   */
  param_spec = g_param_spec_string("domain",
				   i18n_pspec("domain"),
				   i18n_pspec("The domain to resolve"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DOMAIN,
				  param_spec);
  
  /**
   * AgsOscClient:server-port:
   *
   * The server port to connect.
   * 
   * Since: 2.1.0
   */
  param_spec = g_param_spec_uint("server-port",
				 i18n_pspec("server port"),
				 i18n_pspec("The server port to connect"),
				 0,
				 G_MAXUINT32,
				 AGS_OSC_CLIENT_DEFAULT_SERVER_PORT,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SERVER_PORT,
				  param_spec);

  /**
   * AgsOscClient:ip4:
   *
   * The IPv4 address as string of the server.
   * 
   * Since: 2.1.0
   */
  param_spec = g_param_spec_string("ip4",
				   i18n_pspec("ip4"),
				   i18n_pspec("The IPv4 address of the server"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_IP4,
				  param_spec);

  /**
   * AgsOscClient:ip6:
   *
   * The IPv6 address as string of the server.
   * 
   * Since: 2.1.0
   */
  param_spec = g_param_spec_string("ip6",
				   i18n_pspec("ip6"),
				   i18n_pspec("The IPv6 address of the server"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_IP6,
				  param_spec);

  /* AgsOscClientClass */
  osc_client->resolve = ags_osc_client_real_resolve;
  osc_client->connect = ags_osc_client_real_connect;
  
  osc_client->read_bytes = ags_osc_client_real_read_bytes;
  osc_client->write_bytes = ags_osc_client_real_write_bytes;

  /* signals */
  /**
   * AgsOscClient::resolve:
   * @osc_client: the #AgsOscClient
   *
   * The ::resolve signal is emited during resolve of domain.
   *
   * Since: 2.1.0
   */
  osc_client_signals[RESOLVE] =
    g_signal_new("resolve",
		 G_TYPE_FROM_CLASS(osc_client),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscClientClass, resolve),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

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

  /**
   * AgsOscClient::read-bytes:
   * @osc_client: the #AgsOscClient
   * @data_length: the return location of byte array's length
   *
   * The ::read-bytes signal is emited while read bytes.
   *
   * Returns: byte array read or %NULL if no data available
   * 
   * Since: 2.1.11
   */
  osc_client_signals[READ_BYTES] =
    g_signal_new("read-bytes",
		 G_TYPE_FROM_CLASS(osc_client),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscClientClass, read_bytes),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__POINTER,
		 G_TYPE_POINTER, 1,
		 G_TYPE_POINTER);

  /**
   * AgsOscClient::write-bytes:
   * @osc_client: the #AgsOscClient
   * @data: the byte array
   * @data_length: the length of byte array
   *
   * The ::write-bytes signal is emited while write bytes.
   *
   * Returns: %TRUE if all bytes written, otherwise %FALSE
   * 
   * Since: 2.1.0
   */
  osc_client_signals[WRITE_BYTES] =
    g_signal_new("write-bytes",
		 G_TYPE_FROM_CLASS(osc_client),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscClientClass, write_bytes),
		 NULL, NULL,
		 ags_cclosure_marshal_BOOLEAN__POINTER_UINT,
		 G_TYPE_BOOLEAN, 2,
		 G_TYPE_POINTER,
		 G_TYPE_UINT);
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

  osc_client->domain = g_strdup(AGS_OSC_CLIENT_DEFAULT_DOMAIN);
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

  osc_client->start_time = (struct timeval *) malloc(sizeof(struct timespec));

  osc_client->start_time->tv_sec = 0;
  osc_client->start_time->tv_nsec = 0;

  osc_client->buffer = (unsigned char *) malloc(AGS_OSC_CLIENT_CHUNK_SIZE * sizeof(unsigned char));
  osc_client->allocated_buffer_size = AGS_OSC_CLIENT_CHUNK_SIZE;

  osc_client->read_count = 0;
  
  osc_client->timeout_delay = (struct timespec *) malloc(sizeof(struct timespec));

  osc_client->timeout_delay->tv_sec = 0;
  osc_client->timeout_delay->tv_nsec = NSEC_PER_SEC / 30;
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
  case PROP_DOMAIN:
    {
      gchar *domain;

      domain = g_value_get_string(value);

      pthread_mutex_lock(osc_client_mutex);
      
      if(osc_client->domain == domain){
	pthread_mutex_unlock(osc_client_mutex);
	
	return;
      }

      g_free(osc_client->domain);

      osc_client->domain = g_strdup(domain);

      pthread_mutex_unlock(osc_client_mutex);
    }
    break;
  case PROP_SERVER_PORT:
    {
      guint server_port;

      server_port = g_value_get_uint(value);

      pthread_mutex_lock(osc_client_mutex);

      osc_client->server_port = server_port;
      
      pthread_mutex_unlock(osc_client_mutex);      
    }
    break;
  case PROP_IP4:
    {
      gchar *ip4;

      ip4 = g_value_get_string(value);

      pthread_mutex_lock(osc_client_mutex);
      
      if(osc_client->ip4 == ip4){
	pthread_mutex_unlock(osc_client_mutex);
	
	return;
      }

      g_free(osc_client->ip4);

      osc_client->ip4 = g_strdup(ip4);

      pthread_mutex_unlock(osc_client_mutex);
    }
    break;
  case PROP_IP6:
    {
      gchar *ip6;

      ip6 = g_value_get_string(value);

      pthread_mutex_lock(osc_client_mutex);
      
      if(osc_client->ip6 == ip6){
	pthread_mutex_unlock(osc_client_mutex);
	
	return;
      }

      g_free(osc_client->ip6);

      osc_client->ip6 = g_strdup(ip6);

      pthread_mutex_unlock(osc_client_mutex);
    }
    break;
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
  case PROP_DOMAIN:
    {
      pthread_mutex_lock(osc_client_mutex);

      g_value_set_string(value,
			 osc_client->domain);
      
      pthread_mutex_unlock(osc_client_mutex);
    }
    break;
  case PROP_SERVER_PORT:
    {
      pthread_mutex_lock(osc_client_mutex);
      
      g_value_set_uint(value,
		       osc_client->server_port);

      pthread_mutex_unlock(osc_client_mutex);
    }
    break;
  case PROP_IP4:
    {
      pthread_mutex_lock(osc_client_mutex);
      
      g_value_set_string(value,
			 osc_client->ip4);
      
      pthread_mutex_unlock(osc_client_mutex);
    }
    break;
  case PROP_IP6:
    {
      pthread_mutex_lock(osc_client_mutex);
      
      g_value_set_string(value,
			 osc_client->ip6);

      pthread_mutex_unlock(osc_client_mutex);
    }
    break;    
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

  g_free(osc_client->ip4);
  g_free(osc_client->ip6);

  if(osc_client->retry_delay != NULL){
    free(osc_client->retry_delay);
  }

  if(osc_client->start_time != NULL){
    free(osc_client->start_time);
  }

  if(osc_client->timeout_delay != NULL){
    free(osc_client->timeout_delay);
  }
  
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

/**
 * ags_osc_client_timeout_expired:
 * @start_time: the start time #timespec-struct
 * @timeout_delay: the delay #timespec-struct
 * 
 * Check @start_time plus @timeout_delay against current time.
 * 
 * Returns: %TRUE if timeout expired, otherwise %FALSE
 * 
 * Since: 2.1.11
 */
gboolean
ags_osc_client_timeout_expired(struct timespec *start_time,
			       struct timespec *timeout_delay)
{
  struct timespec current_time;
  struct timespec deadline;
  
  if(start_time == NULL ||
     timeout_delay == NULL){
    return(TRUE);
  }

#ifdef __APPLE__
  host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
      
  clock_get_time(cclock, &mts);
  mach_port_deallocate(mach_task_self(), cclock);
      
  current_time.tv_sec = mts.tv_sec;
  current_time.tv_nsec = mts.tv_nsec;
#else
  clock_gettime(CLOCK_MONOTONIC, &current_time);
#endif

  if(start_time->tv_nsec + timeout_delay->tv_nsec > NSEC_PER_SEC){
    deadline.tv_sec = start_time->tv_sec + timeout_delay->tv_sec + 1;
    deadline.tv_nsec = (start_time->tv_nsec + timeout_delay->tv_nsec) - NSEC_PER_SEC;
  }else{
    deadline.tv_sec = start_time->tv_sec + timeout_delay->tv_sec;
    deadline.tv_nsec = start_time->tv_nsec + timeout_delay->tv_nsec;
  }
  
  if(current_time.tv_sec > deadline.tv_sec){
    return(TRUE);
  }else{
    if(current_time.tv_sec == deadline.tv_sec &&
       current_time.tv_nsec > deadline.tv_nsec){
      return(TRUE);
    }
  }

  return(FALSE);
}

void
ags_osc_client_real_resolve(AgsOscClient *osc_client)
{
  struct addrinfo hints, *res;

  int rc;
  
  pthread_mutex_t *osc_client_mutex;

  /* get osc client mutex */
  pthread_mutex_lock(ags_osc_client_get_class_mutex());
  
  osc_client_mutex = osc_client->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_client_get_class_mutex());

  /* lock */
  pthread_mutex_lock(osc_client_mutex);
  
  /* IPv4 */
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_flags |= AI_CANONNAME;
  
  rc = getaddrinfo(osc_client->domain, NULL, &hints, &res);

  if(rc == 0){
    osc_client->ip4 = (char *) malloc(AGS_OSC_CLIENT_DEFAULT_MAX_ADDRESS_LENGTH * sizeof(char));
    inet_ntop(AF_INET, res->ai_addr->sa_data, osc_client->ip4,
	      AGS_OSC_CLIENT_DEFAULT_MAX_ADDRESS_LENGTH);
  }else{
    g_warning("failed to resolve IPv4 address");
  }
  
  /* IPv6 */
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET6;
  hints.ai_flags |= AI_CANONNAME;
  
  rc = getaddrinfo(osc_client->domain, NULL, &hints, &res);

  if(rc == 0){
    osc_client->ip6 = (char *) malloc(AGS_OSC_CLIENT_DEFAULT_MAX_ADDRESS_LENGTH * sizeof(char));
    inet_ntop(AF_INET6, res->ai_addr->sa_data, osc_client->ip6,
	      AGS_OSC_CLIENT_DEFAULT_MAX_ADDRESS_LENGTH);
  }else{
    g_warning("failed to resolve IPv6 address");
  }

  /* unlock */
  pthread_mutex_unlock(osc_client_mutex);
}

/**
 * ags_osc_client_resolve:
 * @osc_client: the #AgsOscClient
 * 
 * Resolve OSC client.
 * 
 * Since: 2.1.0
 */
void
ags_osc_client_resolve(AgsOscClient *osc_client)
{
  g_return_if_fail(AGS_IS_OSC_CLIENT(osc_client));
  
  g_object_ref((GObject *) osc_client);
  g_signal_emit(G_OBJECT(osc_client),
		osc_client_signals[RESOLVE], 0);
  g_object_unref((GObject *) osc_client);
}

void
ags_osc_client_real_connect(AgsOscClient *osc_client)
{
  gboolean ip4_success, ip6_success;
  gboolean ip4_udp_success, ip4_tcp_success;
  gboolean ip6_udp_success, ip6_tcp_success;
  gboolean ip4_connected, ip6_connected;
  guint i;
  
  pthread_mutex_t *osc_client_mutex;

  /* get osc client mutex */
  pthread_mutex_lock(ags_osc_client_get_class_mutex());
  
  osc_client_mutex = osc_client->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_client_get_class_mutex());

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
      
      /* create socket */
      pthread_mutex_lock(osc_client_mutex);
      
      osc_client->ip4_fd = socket(AF_INET, SOCK_DGRAM, 0);
      
      pthread_mutex_unlock(osc_client_mutex);
    }else if(ags_osc_client_test_flags(osc_client, AGS_OSC_CLIENT_TCP)){
      ip4_tcp_success = TRUE;

      /* create socket */
      pthread_mutex_lock(osc_client_mutex);
      
      osc_client->ip4_fd = socket(AF_INET, SOCK_STREAM, 0);

      pthread_mutex_unlock(osc_client_mutex);
    }else{
      g_critical("no flow control type");
    }

    /* get ip4 */
    pthread_mutex_lock(osc_client_mutex);  

    osc_client->ip4_address->sin_family = AF_INET;
    inet_pton(AF_INET, osc_client->ip4, &(osc_client->ip4_address->sin_addr.s_addr));
    osc_client->ip4_address->sin_port = htons(osc_client->server_port);

    pthread_mutex_unlock(osc_client_mutex);
  }

  if(ags_osc_client_test_flags(osc_client, AGS_OSC_CLIENT_INET6)){    
    ip6_success = TRUE;
  
    if(ags_osc_client_test_flags(osc_client, AGS_OSC_CLIENT_UDP)){
      ip6_udp_success = TRUE;

      /* create socket */
      pthread_mutex_lock(osc_client_mutex);
      
      osc_client->ip6_fd = socket(AF_INET6, SOCK_DGRAM, 0);

      pthread_mutex_unlock(osc_client_mutex);
    }else if(ags_osc_client_test_flags(osc_client, AGS_OSC_CLIENT_TCP)){
      ip6_tcp_success = TRUE;

      /* create socket */
      pthread_mutex_lock(osc_client_mutex);
      
      osc_client->ip6_fd = socket(AF_INET6, SOCK_STREAM, 0);

      pthread_mutex_unlock(osc_client_mutex);
    }else{
      g_critical("no flow control type");
    }

    /* get ip6 */
    pthread_mutex_lock(osc_client_mutex);

    osc_client->ip6_address->sin6_family = AF_INET6;
    inet_pton(AF_INET6, osc_client->ip6, &(osc_client->ip6_address->sin6_addr.s6_addr));
    osc_client->ip6_address->sin6_port = htons(osc_client->server_port);

    pthread_mutex_unlock(osc_client_mutex);
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

	/* connect */
	pthread_mutex_lock(osc_client_mutex);
	
	rc = connect(osc_client->ip4_fd, osc_client->ip4_address, sizeof(struct sockaddr_in));

	pthread_mutex_unlock(osc_client_mutex);

	if(rc == 0){
	  ip4_connected = TRUE;
	}
      }
    }

    if(!ip6_connected){
      if(ip6_udp_success || ip6_tcp_success){
	int rc;
	
	/* connect */
	pthread_mutex_lock(osc_client_mutex);
	
	rc = connect(osc_client->ip6_fd, osc_client->ip6_address, sizeof(struct sockaddr_in6));

	pthread_mutex_unlock(osc_client_mutex);

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

  if(!ip4_connected && !ip6_connected){
    g_message("failed to connect to server");
  }
}
  
/**
 * ags_osc_client_connect:
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

unsigned char*
ags_osc_client_real_read_bytes(AgsOscClient *osc_client,
			       guint *data_length)
{
  unsigned char *buffer;
  unsigned char data[256];
  
  guint allocated_buffer_size;
  guint read_count;
  guint retval;
  gint64 start_data, end_data;
  int ip4_fd, ip6_fd;
  int fd;
  guint i, j;
  gboolean has_valid_data;
  gboolean success;

#ifdef __APPLE__
  clock_serv_t cclock;
  mach_timespec_t mts;
#endif

  pthread_mutex_t *osc_client_mutex;

  /* get osc_client mutex */
  pthread_mutex_lock(ags_osc_client_get_class_mutex());
  
  osc_client_mutex = osc_client->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_client_get_class_mutex());

  /* get fd */
  pthread_mutex_lock(osc_client_mutex);

#ifdef __APPLE__
  host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
      
  clock_get_time(cclock, &mts);
  mach_port_deallocate(mach_task_self(), cclock);
      
  osc_client->start_time->tv_sec = mts.tv_sec;
  osc_client->start_time->tv_nsec = mts.tv_nsec;
#else
  clock_gettime(CLOCK_MONOTONIC, osc_client->start_time);
#endif
  
  ip4_fd = osc_client->ip4_fd;
  ip6_fd = osc_client->ip6_fd;
  
  buffer = osc_client->buffer;
  allocated_buffer_size = osc_client->allocated_buffer_size;
  
  read_count = osc_client->read_count;
  
  pthread_mutex_unlock(osc_client_mutex);

  if(ip4_fd != -1){
    fd = ip4_fd;
  }else{
    fd = ip6_fd;
  }
  
  if(fd == -1){  
    if(data_length != NULL){
      *data_length = 0;
    }
    
    return(NULL);
  }
  
  /*  */
  has_valid_data = FALSE;

  start_data = -1;
  end_data = -1;

  j = 0;
  
  while(!ags_osc_client_timeout_expired(osc_client->start_time,
					osc_client->timeout_delay)){
    retval = read(fd, data, 256);

    if(retval == 0){
      continue;
    }

    if(retval == -1){
      if(errno == EAGAIN ||
	 errno == EWOULDBLOCK){
	continue;
      }

      g_message("error during reading data from socket");
      
      if(errno == EBADFD ||
	 errno == ECONNRESET ||
	 errno == ENETRESET){
	pthread_mutex_lock(osc_client_mutex);

	if(ip4_fd == fd){
	  osc_client->ip4_fd = -1;
	}else if(ip6_fd == fd){
	  osc_client->ip6_fd = -1;
	}
	
	pthread_mutex_unlock(osc_client_mutex);
      }
      
      break;      
    }

    success = FALSE;
      
    if(start_data == -1 &&
       read_count == 0){
      for(i = 0; i < retval; i++){
	if(data[i] == AGS_OSC_UTIL_SLIP_END){
	  success = TRUE;
	  
	  break;
	}
      }
    }
    
    if(success){
      start_data = i;

    ags_osc_client_read_bytes_REPEAT_0:
      
      success = FALSE;
	
      for(i = start_data + 1; i < retval; i++){
	if(data[i] == AGS_OSC_UTIL_SLIP_END){
	  success = TRUE;
	  
	  break;
	}
      }

      if(success){	  
	end_data = i;

	if(start_data + 1 == end_data){
	  start_data = end_data;
	  j++;
	  
	  if(j < 2 ||
	     !ags_osc_client_timeout_expired(osc_client->start_time,
					     osc_client->timeout_delay)){
	    goto ags_osc_client_read_bytes_REPEAT_0;
	  }else{
	    break;
	  }
	}
	
	pthread_mutex_lock(osc_client_mutex);

	memcpy(osc_client->buffer, data + start_data, (end_data - start_data) * sizeof(unsigned char));
	  
	pthread_mutex_unlock(osc_client_mutex);

	if(data_length != NULL){
	  *data_length = end_data - start_data;
	}
	  
	return(osc_client->buffer);
      }else{
	read_count += (retval - start_data);
	  
	pthread_mutex_lock(osc_client_mutex);

	memcpy(osc_client->buffer, data + start_data, (retval - start_data) * sizeof(unsigned char));
	  
	pthread_mutex_unlock(osc_client_mutex);

	has_valid_data = TRUE;

	continue;
      }
    }else{
      if(has_valid_data){
	success = FALSE;

	for(i = 0; i < retval; i++){
	  if(data[i] == AGS_OSC_UTIL_SLIP_END){
	    success = TRUE;
	  
	    break;
	  }
	}

	if(success){
	  if(read_count + i >= AGS_OSC_CLIENT_CHUNK_SIZE){
	    if(data_length != NULL){
	      *data_length = 0;
	    }
	    
	    return(NULL);
	  }
	  
	  pthread_mutex_lock(osc_client_mutex);

	  memcpy(osc_client->buffer + read_count, data, (i) * sizeof(unsigned char));
	
	  pthread_mutex_unlock(osc_client_mutex);

	  read_count += i;
	  
	  if(data_length != NULL){
	    *data_length = read_count;
	  }

	  return(osc_client->buffer);
	}else{
	  if(read_count + retval >= AGS_OSC_CLIENT_CHUNK_SIZE){
	    if(data_length != NULL){
	      *data_length = 0;
	    }
	    
	    return(NULL);
	  }
	  
	  pthread_mutex_lock(osc_client_mutex);
	
	  memcpy(osc_client->buffer + read_count, data, (retval) * sizeof(unsigned char));

	  pthread_mutex_unlock(osc_client_mutex);

	  read_count += retval;

	  continue;
	}
      }
    }
  }

  osc_client->read_count = read_count;
  
  if(data_length != NULL){
    *data_length = 0;
  }

  return(NULL);
}

/**
 * ags_osc_client_read_bytes:
 * @osc_client: the #AgsOscClient
 * @data_length: the return location of byte array's length
 * 
 * Read bytes.
 * 
 * Returns: byte array read or %NULL if no data available
 * 
 * Since: 2.1.11
 */
unsigned char*
ags_osc_client_read_bytes(AgsOscClient *osc_client,
			  guint *data_length)
{
  unsigned char *buffer;
  
  g_return_val_if_fail(AGS_IS_OSC_CLIENT(osc_client), NULL);
  
  g_object_ref((GObject *) osc_client);
  g_signal_emit(G_OBJECT(osc_client),
		osc_client_signals[READ_BYTES], 0,
		data_length,
		&buffer);
  g_object_unref((GObject *) osc_client);

  return(buffer);
}

gboolean
ags_osc_client_real_write_bytes(AgsOscClient *osc_client,
				guchar *data, guint data_length)
{
  int ip4_fd, ip6_fd;
  int num_write;
  gboolean success;

  pthread_mutex_t *osc_client_mutex;

  /* get osc client mutex */
  pthread_mutex_lock(ags_osc_client_get_class_mutex());
  
  osc_client_mutex = osc_client->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_client_get_class_mutex());

  /* get fd */
  pthread_mutex_lock(osc_client_mutex);
  
  ip4_fd = osc_client->ip4_fd;
  ip6_fd = osc_client->ip6_fd;

  pthread_mutex_unlock(osc_client_mutex);

  /* initialize success */
  success = TRUE;

  /* write on IPv4 socket */
  if(ags_osc_client_test_flags(osc_client, AGS_OSC_CLIENT_INET4)){
    num_write = write(ip4_fd,
		      data, data_length);

    if(num_write != data_length){
      success = FALSE;
    }
  }

  /* write on IPv6 socket */
  if(ags_osc_client_test_flags(osc_client, AGS_OSC_CLIENT_INET6)){    
    num_write = write(ip6_fd,
		      data, data_length);

    if(num_write != data_length){
      success = FALSE;
    }
  }
  
  return(success);
}

/**
 * ags_osc_client_write_bytes:
 * @osc_client: the #AgsOscClient
 * @data: the byte array
 * @data_length: the length of byte array
 * 
 * Write @data using OSC client.
 * 
 * Returns: %TRUE if all bytes written, otherwise %FALSE
 * 
 * Since: 2.1.0
 */
gboolean
ags_osc_client_write_bytes(AgsOscClient *osc_client,
			   guchar *data, guint data_length)
{
  gboolean success;
  
  g_return_val_if_fail(AGS_IS_OSC_CLIENT(osc_client), FALSE);
  
  g_object_ref((GObject *) osc_client);
  g_signal_emit(G_OBJECT(osc_client),
		osc_client_signals[WRITE_BYTES], 0,
		data, data_length,
		&success);
  g_object_unref((GObject *) osc_client);

  return(success);
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
