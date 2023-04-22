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

#include <ags/audio/osc/ags_osc_client.h>

#include <ags/audio/osc/ags_osc_util.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

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

guchar* ags_osc_client_real_read_bytes(AgsOscClient *osc_client,
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

GType
ags_osc_client_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_OSC_CLIENT_INET4, "AGS_OSC_CLIENT_INET4", "osc-client-inet4" },
      { AGS_OSC_CLIENT_INET6, "AGS_OSC_CLIENT_INET6", "osc-client-inet6" },
      { AGS_OSC_CLIENT_UDP, "AGS_OSC_CLIENT_UDP", "osc-client-udp" },
      { AGS_OSC_CLIENT_TCP, "AGS_OSC_CLIENT_TCP", "osc-client-tcp" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsOscClientFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
  osc_client->connectable_flags = 0;
  
  /* osc client mutex */
  g_rec_mutex_init(&(osc_client->obj_mutex));

  osc_client->ip4 = g_strdup(AGS_OSC_CLIENT_DEFAULT_INET4_ADDRESS);
  osc_client->ip6 = g_strdup(AGS_OSC_CLIENT_DEFAULT_INET6_ADDRESS);

  osc_client->domain = g_strdup(AGS_OSC_CLIENT_DEFAULT_DOMAIN);
  osc_client->server_port = AGS_OSC_CLIENT_DEFAULT_SERVER_PORT;
  
  osc_client->ip4_fd = -1;
  osc_client->ip6_fd = -1;

  osc_client->ip4_socket = NULL;
  osc_client->ip6_socket = NULL;

  osc_client->ip4_address = NULL;
  osc_client->ip6_address = NULL;
  
  osc_client->max_retry_count = AGS_OSC_CLIENT_DEFAULT_MAX_RETRY;

  osc_client->retry_delay = (struct timespec *) malloc(sizeof(struct timespec));

  osc_client->retry_delay->tv_sec = 5;
  osc_client->retry_delay->tv_nsec = 0;

  osc_client->start_time = (struct timespec *) malloc(sizeof(struct timespec));

  osc_client->start_time->tv_sec = 0;
  osc_client->start_time->tv_nsec = 0;

  osc_client->cache_data = (guchar *) malloc(AGS_OSC_CLIENT_DEFAULT_CACHE_DATA_LENGTH * sizeof(guchar));
  osc_client->cache_data_length = 0;
  
  osc_client->buffer = (guchar *) malloc(AGS_OSC_CLIENT_CHUNK_SIZE * sizeof(guchar));
  osc_client->allocated_buffer_size = AGS_OSC_CLIENT_CHUNK_SIZE;

  osc_client->read_count = 0;
  osc_client->has_valid_data = 0;
  
  osc_client->timeout_delay = (struct timespec *) malloc(sizeof(struct timespec));

  osc_client->timeout_delay->tv_sec = 0;
  osc_client->timeout_delay->tv_nsec = 4000;
}

void
ags_osc_client_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsOscClient *osc_client;

  GRecMutex *osc_client_mutex;

  osc_client = AGS_OSC_CLIENT(gobject);

  /* get osc client mutex */
  osc_client_mutex = AGS_OSC_CLIENT_GET_OBJ_MUTEX(osc_client);
  
  switch(prop_id){
  case PROP_DOMAIN:
    {
      gchar *domain;

      domain = g_value_get_string(value);

      g_rec_mutex_lock(osc_client_mutex);
      
      if(osc_client->domain == domain){
	g_rec_mutex_unlock(osc_client_mutex);
	
	return;
      }

      g_free(osc_client->domain);

      osc_client->domain = g_strdup(domain);

      g_rec_mutex_unlock(osc_client_mutex);
    }
    break;
  case PROP_SERVER_PORT:
    {
      guint server_port;

      server_port = g_value_get_uint(value);

      g_rec_mutex_lock(osc_client_mutex);

      osc_client->server_port = server_port;
      
      g_rec_mutex_unlock(osc_client_mutex);      
    }
    break;
  case PROP_IP4:
    {
      gchar *ip4;

      ip4 = g_value_get_string(value);

      g_rec_mutex_lock(osc_client_mutex);
      
      if(osc_client->ip4 == ip4){
	g_rec_mutex_unlock(osc_client_mutex);
	
	return;
      }

      g_free(osc_client->ip4);

      osc_client->ip4 = g_strdup(ip4);

      g_rec_mutex_unlock(osc_client_mutex);
    }
    break;
  case PROP_IP6:
    {
      gchar *ip6;

      ip6 = g_value_get_string(value);

      g_rec_mutex_lock(osc_client_mutex);
      
      if(osc_client->ip6 == ip6){
	g_rec_mutex_unlock(osc_client_mutex);
	
	return;
      }

      g_free(osc_client->ip6);

      osc_client->ip6 = g_strdup(ip6);

      g_rec_mutex_unlock(osc_client_mutex);
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

  GRecMutex *osc_client_mutex;

  osc_client = AGS_OSC_CLIENT(gobject);

  /* get osc client mutex */
  osc_client_mutex = AGS_OSC_CLIENT_GET_OBJ_MUTEX(osc_client);
  
  switch(prop_id){
  case PROP_DOMAIN:
    {
      g_rec_mutex_lock(osc_client_mutex);

      g_value_set_string(value,
			 osc_client->domain);
      
      g_rec_mutex_unlock(osc_client_mutex);
    }
    break;
  case PROP_SERVER_PORT:
    {
      g_rec_mutex_lock(osc_client_mutex);
      
      g_value_set_uint(value,
		       osc_client->server_port);

      g_rec_mutex_unlock(osc_client_mutex);
    }
    break;
  case PROP_IP4:
    {
      g_rec_mutex_lock(osc_client_mutex);
      
      g_value_set_string(value,
			 osc_client->ip4);
      
      g_rec_mutex_unlock(osc_client_mutex);
    }
    break;
  case PROP_IP6:
    {
      g_rec_mutex_lock(osc_client_mutex);
      
      g_value_set_string(value,
			 osc_client->ip6);

      g_rec_mutex_unlock(osc_client_mutex);
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

  g_free(osc_client->ip4);
  g_free(osc_client->ip6);

  if(osc_client->retry_delay != NULL){
    free(osc_client->retry_delay);
  }

  if(osc_client->start_time != NULL){
    free(osc_client->start_time);
  }

  if(osc_client->buffer != NULL){
    free(osc_client->buffer);
  }

  if(osc_client->timeout_delay != NULL){
    free(osc_client->timeout_delay);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_client_parent_class)->finalize(gobject);
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
 * Since: 3.0.0
 */
gboolean
ags_osc_client_test_flags(AgsOscClient *osc_client, AgsOscClientFlags flags)
{
  gboolean retval;  
  
  GRecMutex *osc_client_mutex;

  if(!AGS_IS_OSC_CLIENT(osc_client)){
    return(FALSE);
  }

  /* get osc_client mutex */
  osc_client_mutex = AGS_OSC_CLIENT_GET_OBJ_MUTEX(osc_client);

  /* test */
  g_rec_mutex_lock(osc_client_mutex);

  retval = (flags & (osc_client->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(osc_client_mutex);

  return(retval);
}

/**
 * ags_osc_client_set_flags:
 * @osc_client: the #AgsOscClient
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 3.0.0
 */
void
ags_osc_client_set_flags(AgsOscClient *osc_client, AgsOscClientFlags flags)
{
  GRecMutex *osc_client_mutex;

  if(!AGS_IS_OSC_CLIENT(osc_client)){
    return;
  }

  /* get osc_client mutex */
  osc_client_mutex = AGS_OSC_CLIENT_GET_OBJ_MUTEX(osc_client);

  /* set flags */
  g_rec_mutex_lock(osc_client_mutex);

  osc_client->flags |= flags;

  g_rec_mutex_unlock(osc_client_mutex);
}

/**
 * ags_osc_client_unset_flags:
 * @osc_client: the #AgsOscClient
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 3.0.0
 */
void
ags_osc_client_unset_flags(AgsOscClient *osc_client, AgsOscClientFlags flags)
{
  GRecMutex *osc_client_mutex;

  if(!AGS_IS_OSC_CLIENT(osc_client)){
    return;
  }

  /* get osc_client mutex */
  osc_client_mutex = AGS_OSC_CLIENT_GET_OBJ_MUTEX(osc_client);

  /* set flags */
  g_rec_mutex_lock(osc_client_mutex);

  osc_client->flags &= (~flags);

  g_rec_mutex_unlock(osc_client_mutex);
}

void
ags_osc_client_real_resolve(AgsOscClient *osc_client)
{
  GResolver *resolver;
  
  GList *start_list, *list;

  gchar *ip4, *ip6;
  
  GError *error;
  
  GRecMutex *osc_client_mutex;

  /* get osc client mutex */
  osc_client_mutex = AGS_OSC_CLIENT_GET_OBJ_MUTEX(osc_client);

  resolver = g_resolver_get_default();
  
  /* lock */
  g_rec_mutex_lock(osc_client_mutex);

#if 0  
  /* IPv4 */
  error = NULL;
  start_list = g_resolver_lookup_by_name_with_flags(resolver,
						    osc_client->domain,
						    G_RESOLVER_NAME_LOOKUP_FLAGS_IPV4_ONLY,
						    NULL,
						    &error);

  if(error != NULL){
    g_warning("AgsOscClient - %s", error->message);

    g_error_free(error);
  }

  if(start_list != NULL){
    osc_client->ip4 = g_inet_address_to_string(start_list->data);
    g_resolver_free_addresses(start_list);
  }

  /* IPv6 */
  error = NULL;
  start_list = g_resolver_lookup_by_name_with_flags(resolver,
						    osc_client->domain,
						    G_RESOLVER_NAME_LOOKUP_FLAGS_IPV6_ONLY,
						    NULL,
						    &error);

  if(error != NULL){
    g_warning("AgsOscClient - %s", error->message);

    g_error_free(error);
  }

  if(start_list != NULL){
    osc_client->ip6 = g_inet_address_to_string(start_list->data);
    g_resolver_free_addresses(start_list);
  }
#else
  error = NULL;
  start_list = g_resolver_lookup_by_name(resolver,
					 osc_client->domain,
					 NULL,
					 &error);

  if(error != NULL){
    g_warning("AgsOscClient - %s", error->message);

    g_error_free(error);
  }

  list = start_list;

  ip4 = NULL;
  ip6 = NULL;
  
  while(list != NULL &&
	(ip4 == NULL ||
	 ip6 == NULL)){
    guint address_family;
    
    g_object_get(list->data,
		 "family", &address_family,
		 NULL);

    if(address_family == G_SOCKET_FAMILY_IPV4){
      ip4 = g_inet_address_to_string(list->data);
    }else if(address_family == G_SOCKET_FAMILY_IPV6){
      ip6 = g_inet_address_to_string(list->data);
    }

    list = list->next;
  }
  
  osc_client->ip4 = ip4;
  osc_client->ip6 = ip6;
  
  g_resolver_free_addresses(start_list);
#endif
  
  /* unlock */
  g_rec_mutex_unlock(osc_client_mutex);

  g_object_unref(resolver);
}

/**
 * ags_osc_client_resolve:
 * @osc_client: the #AgsOscClient
 * 
 * Resolve OSC client.
 * 
 * Since: 3.0.0
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

  GError *error;
  
  GRecMutex *osc_client_mutex;

  /* get osc client mutex */
  osc_client_mutex = AGS_OSC_CLIENT_GET_OBJ_MUTEX(osc_client);

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
      g_rec_mutex_lock(osc_client_mutex);
      
      error = NULL;      
      osc_client->ip4_socket = g_socket_new(G_SOCKET_FAMILY_IPV4,
					    G_SOCKET_TYPE_DATAGRAM,
					    G_SOCKET_PROTOCOL_UDP,
					    &error);
      osc_client->ip4_fd = g_socket_get_fd(osc_client->ip4_socket);
      
      g_rec_mutex_unlock(osc_client_mutex);

      if(error != NULL){
	g_critical("AgsOscClient - %s", error->message);

	g_error_free(error);
      }
    }else if(ags_osc_client_test_flags(osc_client, AGS_OSC_CLIENT_TCP)){
      ip4_tcp_success = TRUE;

      /* create socket */
      g_rec_mutex_lock(osc_client_mutex);
      
      error = NULL;      
      osc_client->ip4_socket = g_socket_new(G_SOCKET_FAMILY_IPV4,
					    G_SOCKET_TYPE_STREAM,
					    G_SOCKET_PROTOCOL_TCP,
					    &error);
      osc_client->ip4_fd = g_socket_get_fd(osc_client->ip4_socket);

      g_rec_mutex_unlock(osc_client_mutex);

      if(error != NULL){
	g_critical("AgsOscClient - %s", error->message);

	g_error_free(error);
      }
    }else{
      g_critical("no flow control type");
    }

    /* get ip4 */
    g_rec_mutex_lock(osc_client_mutex);  

    osc_client->ip4_address = g_inet_socket_address_new(g_inet_address_new_from_string(osc_client->ip4),
							osc_client->server_port);

    g_rec_mutex_unlock(osc_client_mutex);
  }

  if(ags_osc_client_test_flags(osc_client, AGS_OSC_CLIENT_INET6)){    
    ip6_success = TRUE;
  
    if(ags_osc_client_test_flags(osc_client, AGS_OSC_CLIENT_UDP)){
      ip6_udp_success = TRUE;

      /* create socket */
      g_rec_mutex_lock(osc_client_mutex);
      
      error = NULL;      
      osc_client->ip6_socket = g_socket_new(G_SOCKET_FAMILY_IPV6,
					    G_SOCKET_TYPE_DATAGRAM,
					    G_SOCKET_PROTOCOL_UDP,
					    &error);
      osc_client->ip6_fd = g_socket_get_fd(osc_client->ip6_socket);

      g_rec_mutex_unlock(osc_client_mutex);

      if(error != NULL){
	g_critical("AgsOscClient - %s", error->message);

	g_error_free(error);
      }
    }else if(ags_osc_client_test_flags(osc_client, AGS_OSC_CLIENT_TCP)){
      ip6_tcp_success = TRUE;

      /* create socket */
      g_rec_mutex_lock(osc_client_mutex);
      
      error = NULL;      
      osc_client->ip6_socket = g_socket_new(G_SOCKET_FAMILY_IPV6,
					    G_SOCKET_TYPE_STREAM,
					    G_SOCKET_PROTOCOL_TCP,
					    &error);
      osc_client->ip6_fd = g_socket_get_fd(osc_client->ip6_socket);

      g_rec_mutex_unlock(osc_client_mutex);

      if(error != NULL){
	g_critical("AgsOscClient - %s", error->message);

	g_error_free(error);
      }
    }else{
      g_critical("no flow control type");
    }

    /* get ip6 */
    g_rec_mutex_lock(osc_client_mutex);

    osc_client->ip6_address = g_inet_socket_address_new(g_inet_address_new_from_string(osc_client->ip6),
							osc_client->server_port);

    g_rec_mutex_unlock(osc_client_mutex);
  }
  
  if(!ip4_success && !ip6_success){
    g_critical("no protocol family");

    return;
  }

  if(!ip4_udp_success && !ip4_tcp_success &&
     !ip6_udp_success && !ip6_tcp_success){
    return;
  }

  if(osc_client->ip4_fd != -1){
    //TODO:JK: check remove
#if 0
    int flags;

    flags = fcntl(osc_client->ip4_fd, F_GETFL, 0);
    fcntl(osc_client->ip4_fd, F_SETFL, flags | O_NONBLOCK);
#else
    //    g_object_set(osc_client->ip4_socket,
    //		 "blocking", FALSE,
    //		 NULL);
#endif
  }

  if(osc_client->ip6_fd != -1){
    //TODO:JK: check remove
#if 0
    int flags;

    flags = fcntl(osc_client->ip6_fd, F_GETFL, 0);
    fcntl(osc_client->ip6_fd, F_SETFL, flags | O_NONBLOCK);
#else
    //    g_object_set(osc_client->ip6_socket,
    //		 "blocking", FALSE,
    //		 NULL);
#endif
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
	g_rec_mutex_lock(osc_client_mutex);

	error = NULL;
	g_socket_connect(osc_client->ip4_socket,
			 osc_client->ip4_address,
			 NULL,
			 &error);

	g_rec_mutex_unlock(osc_client_mutex);

	if(error == NULL){
	  ip4_connected = TRUE;
	}else{
	  g_critical("AgsOscClient - %s", error->message);

	  g_error_free(error);
	}
      }
    }

    if(!ip6_connected){
      if(ip6_udp_success || ip6_tcp_success){
	int rc;
	
	/* connect */
	g_rec_mutex_lock(osc_client_mutex);
	
	error = NULL;
	g_socket_connect(osc_client->ip6_socket,
			 osc_client->ip6_address,
			 NULL,
			 &error);

	g_rec_mutex_unlock(osc_client_mutex);

	if(error == NULL){
	  ip6_connected = TRUE;
	}else{
	  g_critical("AgsOscClient - %s", error->message);

	  g_error_free(error);
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
 * Since: 3.0.0
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

guchar*
ags_osc_client_real_read_bytes(AgsOscClient *osc_client,
			       guint *data_length)
{
  GSocket *socket;
  
  guchar *buffer;
  guchar data[AGS_OSC_CLIENT_DEFAULT_CACHE_DATA_LENGTH];
  
  guint allocated_buffer_size;
  guint read_count;
  int retval;
  guint available_data_length;
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

  GError *error;
  
  GRecMutex *osc_client_mutex;

  /* get osc_client mutex */
  osc_client_mutex = AGS_OSC_CLIENT_GET_OBJ_MUTEX(osc_client);

  /* get fd */
  g_rec_mutex_lock(osc_client_mutex);

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
  has_valid_data = osc_client->has_valid_data;
  
  if(ip4_fd != -1){
    fd = ip4_fd;
    
    socket = osc_client->ip4_socket;
  }else{
    fd = ip6_fd;

    socket = osc_client->ip6_socket;
  }

  g_rec_mutex_unlock(osc_client_mutex);
  
  if(fd == -1){  
    if(data_length != NULL){
      *data_length = 0;
    }
    
    return(NULL);
  }
  
  /*  */
  start_data = -1;
  end_data = -1;

  j = 0;
  
  while(TRUE){
    gboolean has_expired;
    
    g_rec_mutex_lock(osc_client_mutex);
    
    has_expired = ags_time_timeout_expired(osc_client->start_time,
					   osc_client->timeout_delay);

    g_rec_mutex_unlock(osc_client_mutex);
    
    if(has_expired){
      break;
    }
    
    available_data_length = 0;
    
    if(osc_client->cache_data_length > 0){
      memcpy(data,
	     osc_client->cache_data,
	     osc_client->cache_data_length * sizeof(guchar));

      available_data_length += osc_client->cache_data_length;
    }

    retval = 0;

    error = NULL;

    g_rec_mutex_lock(osc_client_mutex);
    
    if(osc_client->cache_data_length < AGS_OSC_CLIENT_DEFAULT_CACHE_DATA_LENGTH){
      retval = g_socket_receive(socket,
				data + osc_client->cache_data_length,
				AGS_OSC_CLIENT_DEFAULT_CACHE_DATA_LENGTH - osc_client->cache_data_length,
				NULL,
				&error);

      if(retval > 0){
	available_data_length += retval;
      }
    }

    osc_client->cache_data_length = 0;
    
    g_rec_mutex_unlock(osc_client_mutex);

    if(error != NULL){
      if(!g_error_matches(error, G_IO_ERROR, G_IO_ERROR_WOULD_BLOCK)){
	g_critical("AgsOscClient - %s", error->message);
      }

      if(g_error_matches(error, G_IO_ERROR, G_IO_ERROR_WOULD_BLOCK)){
	if(available_data_length == 0){
	  g_error_free(error);
	  
	  continue;
	}
      }else{
	if(g_error_matches(error, G_IO_ERROR, G_IO_ERROR_CONNECTION_CLOSED)){
	  GError *close_error;
	  
	  g_rec_mutex_lock(osc_client_mutex);

	  close_error = NULL;
	  g_socket_close(socket,
			 &close_error);
	  g_object_unref(socket);

	  if(ip4_fd == fd){
	    osc_client->ip4_fd = -1;

	    osc_client->ip4_socket = NULL;
	  }else if(ip6_fd == fd){
	    osc_client->ip6_fd = -1;

	    osc_client->ip6_socket = NULL;
	  }
	
	  g_rec_mutex_unlock(osc_client_mutex);

	  if(close_error != NULL){
	    g_error_free(close_error);
	  }
	  
	  g_error_free(error);	  
      
	  break;
	}
      }

      g_error_free(error);
    }

    if(available_data_length == 0){
      continue;
    }

    success = FALSE;
      
    if(start_data == -1 &&
       read_count == 0){
      for(i = 0; i < available_data_length; i++){
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
	
      for(i = start_data + 1; i < available_data_length; i++){
	if(data[i] == AGS_OSC_UTIL_SLIP_END){
	  success = TRUE;
	  
	  break;
	}
      }

      if(success){	  
	end_data = i;

	if(start_data + 1 == end_data){
	  gboolean has_expired;
	  
	  start_data = end_data;
	  j++;

	  g_rec_mutex_lock(osc_client_mutex);
	  
	  has_expired = ags_time_timeout_expired(osc_client->start_time,
						 osc_client->timeout_delay);

	  g_rec_mutex_unlock(osc_client_mutex);
	  
	  if(j < 2 ||
	     !has_expired){
	    goto ags_osc_client_read_bytes_REPEAT_0;
	  }else{
	    break;
	  }
	}
	
	g_rec_mutex_lock(osc_client_mutex);

	memcpy(osc_client->buffer, data + start_data, (end_data - start_data + 1) * sizeof(guchar));
	  
	g_rec_mutex_unlock(osc_client_mutex);

	if(data_length != NULL){
	  data_length[0] = end_data - start_data + 1;
	}

	/* fill cache */
	if(end_data > 0 &&
	   end_data < available_data_length){
	  memcpy(osc_client->cache_data,
		 data + end_data,
		 (available_data_length - end_data) * sizeof(guchar));

	  osc_client->cache_data_length = available_data_length - end_data;
	}
	
	osc_client->read_count = 0;
	osc_client->has_valid_data = FALSE;
	
	return(osc_client->buffer);
      }else{
	read_count += (available_data_length - start_data);
	  
	g_rec_mutex_lock(osc_client_mutex);

	memcpy(osc_client->buffer, data + start_data, (available_data_length - start_data) * sizeof(guchar));
	  
	g_rec_mutex_unlock(osc_client_mutex);

	has_valid_data = TRUE;

	continue;
      }
    }else{
      if(has_valid_data){
	success = FALSE;

	for(i = 0; i < available_data_length; i++){
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
	    
	    osc_client->read_count = 0;
	    osc_client->has_valid_data = FALSE;
	    
	    return(NULL);
	  }
	  
	  g_rec_mutex_lock(osc_client_mutex);

	  memcpy(osc_client->buffer + read_count, data, (i + 1) * sizeof(guchar));
	
	  g_rec_mutex_unlock(osc_client_mutex);

	  read_count += i;
	  
	  if(data_length != NULL){
	    data_length[0] = read_count + 1;
	  }

	  /* fill cache */
	  if(i < available_data_length){
	    memcpy(osc_client->cache_data,
		   data + i,
		   (available_data_length - i) * sizeof(guchar));

	    osc_client->cache_data_length = available_data_length - i;
	  }

	  osc_client->read_count = 0;
	  osc_client->has_valid_data = FALSE;

	  return(osc_client->buffer);
	}else{
	  if(read_count + available_data_length >= AGS_OSC_CLIENT_CHUNK_SIZE){
	    if(data_length != NULL){
	      *data_length = 0;
	    }
	    
	    osc_client->read_count = 0;
	    osc_client->has_valid_data = FALSE;

	    return(NULL);
	  }
	  
	  g_rec_mutex_lock(osc_client_mutex);
	
	  memcpy(osc_client->buffer + read_count, data, (available_data_length) * sizeof(guchar));

	  g_rec_mutex_unlock(osc_client_mutex);

	  read_count += available_data_length;

	  continue;
	}
      }
    }
  }

  osc_client->read_count = read_count;
  osc_client->has_valid_data = has_valid_data;
  
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
 * Since: 3.0.0
 */
guchar*
ags_osc_client_read_bytes(AgsOscClient *osc_client,
			  guint *data_length)
{
  guchar *buffer;
  
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

  GError *error;
  
  GRecMutex *osc_client_mutex;

  /* get osc client mutex */
  osc_client_mutex = AGS_OSC_CLIENT_GET_OBJ_MUTEX(osc_client);

  /* initialize success */
  success = TRUE;

  /* get fd */
  g_rec_mutex_lock(osc_client_mutex);
  
  ip4_fd = osc_client->ip4_fd;
  ip6_fd = osc_client->ip6_fd;
  
  g_rec_mutex_unlock(osc_client_mutex);
  
  /* write on IPv4 socket */
  if(ags_osc_client_test_flags(osc_client, AGS_OSC_CLIENT_INET4)){
    g_rec_mutex_lock(osc_client_mutex);

    num_write = 0;

    error = NULL;
    
    if(osc_client->ip4_socket != NULL){
      num_write = g_socket_send(osc_client->ip4_socket,
				data,
				data_length,
				NULL,
				&error);
    }
    
    g_rec_mutex_unlock(osc_client_mutex);

    if(num_write != data_length){
      success = FALSE;
    }

    if(error != NULL){
      g_critical("AgsOscClient - %s", error->message);

      g_error_free(error);
    }
  }

  /* write on IPv6 socket */
  if(ags_osc_client_test_flags(osc_client, AGS_OSC_CLIENT_INET6)){    
    g_rec_mutex_lock(osc_client_mutex);

    num_write = 0;

    error = NULL;
    
    if(osc_client->ip6_socket != NULL){
      num_write = g_socket_send(osc_client->ip6_socket,
				data,
				data_length,
				NULL,
				&error);
    }
    
    g_rec_mutex_unlock(osc_client_mutex);

    if(num_write != data_length){
      success = FALSE;
    }

    if(error != NULL){
      g_critical("AgsOscClient - %s", error->message);

      g_error_free(error);
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
 * Since: 3.0.0
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
 * Since: 3.0.0
 */
AgsOscClient*
ags_osc_client_new()
{
  AgsOscClient *osc_client;
   
  osc_client = (AgsOscClient *) g_object_new(AGS_TYPE_OSC_CLIENT,
					     NULL);
  
  return(osc_client);
}
