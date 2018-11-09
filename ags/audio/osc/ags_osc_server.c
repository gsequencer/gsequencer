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

#include <ags/audio/osc/ags_osc_server.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <unistd.h>

#include <ags/i18n.h>

void ags_osc_server_class_init(AgsOscServerClass *osc_server);
void ags_osc_server_init(AgsOscServer *osc_server);
void ags_osc_server_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_osc_server_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_osc_server_finalize(GObject *gobject);

void ags_osc_server_real_start(AgsOscServer *osc_server);
void ags_osc_server_real_stop(AgsOscServer *osc_server);

void ags_osc_server_real_listen(AgsOscServer *osc_server);

void ags_osc_server_real_dispatch(AgsOscServer *osc_server);

/**
 * SECTION:ags_osc_server
 * @short_description: the OSC server
 * @title: AgsOscServer
 * @section_id:
 * @include: ags/audio/osc/ags_osc_server.h
 *
 * #AgsOscServer your osc server.
 */

enum{
  PROP_0,
  PROP_DOMAIN,
  PROP_SERVER_PORT,
  PROP_IP4,
  PROP_IP6,
};

enum{
  START,
  STOP,
  LISTEN,
  DISPATCH,
  LAST_SIGNAL,
};

static gpointer ags_osc_server_parent_class = NULL;
static guint osc_server_signals[LAST_SIGNAL];

static pthread_mutex_t ags_osc_server_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_osc_server_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_osc_server = 0;

    static const GTypeInfo ags_osc_server_info = {
      sizeof (AgsOscServerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_osc_server_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOscServer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_osc_server_init,
    };

    ags_type_osc_server = g_type_register_static(G_TYPE_OBJECT,
						 "AgsOscServer", &ags_osc_server_info,
						 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_osc_server);
  }

  return g_define_type_id__volatile;
}

void
ags_osc_server_class_init(AgsOscServerClass *osc_server)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_osc_server_parent_class = g_type_class_peek_parent(osc_server);

  /* GObjectClass */
  gobject = (GObjectClass *) osc_server;

  gobject->set_property = ags_osc_server_set_property;
  gobject->get_property = ags_osc_server_get_property;
  
  gobject->finalize = ags_osc_server_finalize;

  /* properties */
  /**
   * AgsOscServer:domain:
   *
   * The domain to use.
   * 
   * Since: 2.1.0
   */
  param_spec = g_param_spec_string("domain",
				   i18n_pspec("domain"),
				   i18n_pspec("The domain to use"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DOMAIN,
				  param_spec);
  
  /**
   * AgsOscServer:server-port:
   *
   * The server port to use.
   * 
   * Since: 2.1.0
   */
  param_spec = g_param_spec_uint("server-port",
				 i18n_pspec("server port"),
				 i18n_pspec("The server port to use"),
				 0,
				 G_MAXUINT32,
				 AGS_OSC_SERVER_DEFAULT_SERVER_PORT,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SERVER_PORT,
				  param_spec);

  /**
   * AgsOscServer:ip4:
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
   * AgsOscServer:ip6:
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

  /* AgsOscServerClass */
  osc_server->start = ags_osc_server_real_start;
  osc_server->stop = ags_osc_server_real_stop;

  osc_server->listen = ags_osc_server_real_listen;

  osc_server->dispatch = ags_osc_server_real_dispatch;

  /* signals */
  /**
   * AgsOscServer::start:
   * @osc_server: the #AgsOscServer
   *
   * The ::start signal is emited during start of server.
   *
   * Since: 2.1.0
   */
  osc_server_signals[START] =
    g_signal_new("start",
		 G_TYPE_FROM_CLASS(osc_server),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscServerClass, start),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsOscServer::stop:
   * @osc_server: the #AgsOscServer
   *
   * The ::stop signal is emited during stop of server.
   *
   * Since: 2.1.0
   */
  osc_server_signals[STOP] =
    g_signal_new("stop",
		 G_TYPE_FROM_CLASS(osc_server),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscServerClass, stop),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsOscServer::listen:
   * @osc_server: the #AgsOscServer
   *
   * The ::listen signal is emited during listen of server.
   *
   * Since: 2.1.0
   */
  osc_server_signals[LISTEN] =
    g_signal_new("listen",
		 G_TYPE_FROM_CLASS(osc_server),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscServerClass, listen),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsOscServer::dispatch:
   * @osc_server: the #AgsOscServer
   *
   * The ::dispatch signal is emited during dispatch of server.
   *
   * Since: 2.1.0
   */
  osc_server_signals[DISPATCH] =
    g_signal_new("dispatch",
		 G_TYPE_FROM_CLASS(osc_server),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscServerClass, dispatch),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_osc_server_init(AgsOscServer *osc_server)
{
  osc_server->flags = 0;
  
  /* osc server mutex */
  osc_server->obj_mutexattr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(osc_server->obj_mutexattr);
  pthread_mutexattr_settype(osc_server->obj_mutexattr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(osc_server->obj_mutexattr,
				PTHREAD_PRIO_INHERIT);
#endif

  osc_server->obj_mutex =  (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(osc_server->obj_mutex,
		     osc_server->obj_mutexattr);

  osc_server->ip4 = g_strdup(AGS_OSC_SERVER_DEFAULT_INET4_ADDRESS);
  osc_server->ip6 = g_strdup(AGS_OSC_SERVER_DEFAULT_INET6_ADDRESS);

  osc_server->domain = g_strdup(AGS_OSC_SERVER_DEFAULT_DOMAIN);
  osc_server->server_port = AGS_OSC_SERVER_DEFAULT_SERVER_PORT;
  
  osc_server->ip4_fd = -1;
  osc_server->ip6_fd = -1;
  
  osc_server->ip4_address = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));
  memset(osc_server->ip4_address, 0, sizeof(struct sockaddr_in));
  
  osc_server->ip6_address = (struct sockaddr_in6 *) malloc(sizeof(struct sockaddr_in6));
  memset(osc_server->ip6_address, 0, sizeof(struct sockaddr_in6));

  osc_server->connection = NULL;
}

void
ags_osc_server_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsOscServer *osc_server;

  pthread_mutex_t *osc_server_mutex;

  osc_server = AGS_OSC_SERVER(gobject);

  /* get osc server mutex */
  pthread_mutex_lock(ags_osc_server_get_class_mutex());
  
  osc_server_mutex = osc_server->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_server_get_class_mutex());
  
  switch(prop_id){
  case PROP_DOMAIN:
    {
      gchar *domain;

      domain = g_value_get_string(value);

      pthread_mutex_lock(osc_server_mutex);
      
      if(osc_server->domain == domain){
	pthread_mutex_unlock(osc_server_mutex);
	
	return;
      }

      g_free(osc_server->domain);

      osc_server->domain = g_strdup(domain);

      pthread_mutex_unlock(osc_server_mutex);
    }
    break;
  case PROP_SERVER_PORT:
    {
      guint server_port;

      server_port = g_value_get_uint(value);

      pthread_mutex_lock(osc_server_mutex);

      osc_server->server_port = server_port;
      
      pthread_mutex_unlock(osc_server_mutex);      
    }
    break;
  case PROP_IP4:
    {
      gchar *ip4;

      ip4 = g_value_get_string(value);

      pthread_mutex_lock(osc_server_mutex);
      
      if(osc_server->ip4 == ip4){
	pthread_mutex_unlock(osc_server_mutex);
	
	return;
      }

      g_free(osc_server->ip4);

      osc_server->ip4 = g_strdup(ip4);

      pthread_mutex_unlock(osc_server_mutex);
    }
    break;
  case PROP_IP6:
    {
      gchar *ip6;

      ip6 = g_value_get_string(value);

      pthread_mutex_lock(osc_server_mutex);
      
      if(osc_server->ip6 == ip6){
	pthread_mutex_unlock(osc_server_mutex);
	
	return;
      }

      g_free(osc_server->ip6);

      osc_server->ip6 = g_strdup(ip6);

      pthread_mutex_unlock(osc_server_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_server_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsOscServer *osc_server;

  pthread_mutex_t *osc_server_mutex;

  osc_server = AGS_OSC_SERVER(gobject);

  /* get osc server mutex */
  pthread_mutex_lock(ags_osc_server_get_class_mutex());
  
  osc_server_mutex = osc_server->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_server_get_class_mutex());
  
  switch(prop_id){
  case PROP_DOMAIN:
    {
      pthread_mutex_lock(osc_server_mutex);

      g_value_set_string(value,
			 osc_server->domain);
      
      pthread_mutex_unlock(osc_server_mutex);
    }
    break;
  case PROP_SERVER_PORT:
    {
      pthread_mutex_lock(osc_server_mutex);
      
      g_value_set_uint(value,
		       osc_server->server_port);

      pthread_mutex_unlock(osc_server_mutex);
    }
    break;
  case PROP_IP4:
    {
      pthread_mutex_lock(osc_server_mutex);
      
      g_value_set_string(value,
			 osc_server->ip4);
      
      pthread_mutex_unlock(osc_server_mutex);
    }
    break;
  case PROP_IP6:
    {
      pthread_mutex_lock(osc_server_mutex);
      
      g_value_set_string(value,
			 osc_server->ip6);

      pthread_mutex_unlock(osc_server_mutex);
    }
    break;    
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_server_finalize(GObject *gobject)
{
  AgsOscServer *osc_server;
    
  osc_server = (AgsOscServer *) gobject;

  pthread_mutex_destroy(osc_server->obj_mutex);
  free(osc_server->obj_mutex);

  pthread_mutexattr_destroy(osc_server->obj_mutexattr);
  free(osc_server->obj_mutexattr);

  g_free(osc_server->domain);
  
  g_free(osc_server->ip4);
  g_free(osc_server->ip6);
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_server_parent_class)->finalize(gobject);
}

/**
 * ags_osc_server_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.1.0
 */
pthread_mutex_t*
ags_osc_server_get_class_mutex()
{
  return(&ags_osc_server_class_mutex);
}

/**
 * ags_osc_server_test_flags:
 * @osc_server: the #AgsOscServer
 * @flags: the flags
 *
 * Test @flags to be set on @osc_server.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 2.1.0
 */
gboolean
ags_osc_server_test_flags(AgsOscServer *osc_server, guint flags)
{
  gboolean retval;  
  
  pthread_mutex_t *osc_server_mutex;

  if(!AGS_IS_OSC_SERVER(osc_server)){
    return(FALSE);
  }

  /* get osc_server mutex */
  pthread_mutex_lock(ags_osc_server_get_class_mutex());
  
  osc_server_mutex = osc_server->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_server_get_class_mutex());

  /* test */
  pthread_mutex_lock(osc_server_mutex);

  retval = (flags & (osc_server->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(osc_server_mutex);

  return(retval);
}

/**
 * ags_osc_server_set_flags:
 * @osc_server: the #AgsOscServer
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 2.1.0
 */
void
ags_osc_server_set_flags(AgsOscServer *osc_server, guint flags)
{
  pthread_mutex_t *osc_server_mutex;

  if(!AGS_IS_OSC_SERVER(osc_server)){
    return;
  }

  /* get osc_server mutex */
  pthread_mutex_lock(ags_osc_server_get_class_mutex());
  
  osc_server_mutex = osc_server->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_server_get_class_mutex());

  /* set flags */
  pthread_mutex_lock(osc_server_mutex);

  osc_server->flags |= flags;

  pthread_mutex_unlock(osc_server_mutex);
}

/**
 * ags_osc_server_unset_flags:
 * @osc_server: the #AgsOscServer
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 2.1.0
 */
void
ags_osc_server_unset_flags(AgsOscServer *osc_server, guint flags)
{
  pthread_mutex_t *osc_server_mutex;

  if(!AGS_IS_OSC_SERVER(osc_server)){
    return;
  }

  /* get osc_server mutex */
  pthread_mutex_lock(ags_osc_server_get_class_mutex());
  
  osc_server_mutex = osc_server->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_server_get_class_mutex());

  /* set flags */
  pthread_mutex_lock(osc_server_mutex);

  osc_server->flags &= (~flags);

  pthread_mutex_unlock(osc_server_mutex);
}

void
ags_osc_server_real_start(AgsOscServer *osc_server)
{
  gboolean any_address;
  gboolean ip4_success, ip6_success;
  gboolean ip4_udp_success, ip4_tcp_success;
  gboolean ip6_udp_success, ip6_tcp_success;

  pthread_mutex_t *osc_server_mutex;

  /* get osc_server mutex */
  pthread_mutex_lock(ags_osc_server_get_class_mutex());
  
  osc_server_mutex = osc_server->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_server_get_class_mutex());

  any_address = ags_osc_server_test_flags(osc_server, AGS_OSC_SERVER_ANY_ADDRESS);
  
  ip4_success = FALSE;
  ip6_success = FALSE;
  
  ip4_udp_success = FALSE;
  ip4_tcp_success = FALSE;

  ip6_udp_success = FALSE;
  ip6_tcp_success = FALSE;
  
  if(ags_osc_server_test_flags(osc_server, AGS_OSC_SERVER_INET4)){
    ip4_success = TRUE;
  
    if(ags_osc_server_test_flags(osc_server, AGS_OSC_SERVER_UDP)){
      ip4_udp_success = TRUE;
      
      /* create socket */
      pthread_mutex_lock(osc_server_mutex);
      
      osc_server->ip4_fd = socket(AF_INET, SOCK_DGRAM, 0);
      
      pthread_mutex_unlock(osc_server_mutex);
    }else if(ags_osc_server_test_flags(osc_server, AGS_OSC_SERVER_TCP)){
      ip4_tcp_success = TRUE;

      /* create socket */
      pthread_mutex_lock(osc_server_mutex);
      
      osc_server->ip4_fd = socket(AF_INET, SOCK_STREAM, 0);

      pthread_mutex_unlock(osc_server_mutex);
    }else{
      g_critical("no flow control type");
    }

    /* get ip4 */
    if(any_address){
      pthread_mutex_lock(osc_server_mutex);  

      osc_server->ip4_address->sin_addr.s_addr = INADDR_ANY;
      osc_server->ip4_address->sin_port = htons(osc_server->server_port);

      pthread_mutex_unlock(osc_server_mutex);  
    }else{
      pthread_mutex_lock(osc_server_mutex);  

      inet_pton(AF_INET, osc_server->ip4, &(osc_server->ip4_address->sin_addr.s_addr));
      osc_server->ip4_address->sin_port = htons(osc_server->server_port);

      pthread_mutex_unlock(osc_server_mutex);
    }
  }

  if(ags_osc_server_test_flags(osc_server, AGS_OSC_SERVER_INET6)){    
    ip6_success = TRUE;
  
    if(ags_osc_server_test_flags(osc_server, AGS_OSC_SERVER_UDP)){
      ip6_udp_success = TRUE;

      /* create socket */
      pthread_mutex_lock(osc_server_mutex);
      
      osc_server->ip6_fd = socket(AF_INET6, SOCK_DGRAM, 0);

      pthread_mutex_unlock(osc_server_mutex);
    }else if(ags_osc_server_test_flags(osc_server, AGS_OSC_SERVER_TCP)){
      ip6_tcp_success = TRUE;

      /* create socket */
      pthread_mutex_lock(osc_server_mutex);
      
      osc_server->ip6_fd = socket(AF_INET6, SOCK_STREAM, 0);

      pthread_mutex_unlock(osc_server_mutex);
    }else{
      g_critical("no flow control type");
    }

    /* get ip6 */
    if(any_address){
      pthread_mutex_lock(osc_server_mutex);

      memcpy(&(osc_server->ip6_address->sin6_addr.s6_addr), &in6addr_any, sizeof(struct in6_addr));
      osc_server->ip6_address->sin6_port = htons(osc_server->server_port);
      
      pthread_mutex_unlock(osc_server_mutex);
    }else{
      pthread_mutex_lock(osc_server_mutex);

      inet_pton(AF_INET6, osc_server->ip6, &(osc_server->ip6_address->sin6_addr.s6_addr));
      osc_server->ip6_address->sin6_port = htons(osc_server->server_port);

      pthread_mutex_unlock(osc_server_mutex);
    }
  }
  
  if(!ip4_success && !ip6_success){
    g_critical("no protocol family");

    return;
  }

  if(ip4_success){
    int rc;
    
    rc = bind(osc_server->ip4_fd, osc_server->ip4_address, sizeof(struct sockaddr_in));

    if(rc < 0){
      g_critical("bind failed");
    }
  }
  
  if(ip6_success){
    int rc;
    
    rc = bind(osc_server->ip6_fd, osc_server->ip6_address, sizeof(struct sockaddr_in6));

    if(rc < 0){
      g_critical("bind failed");
    }
  }

  ags_osc_server_set_flags(osc_server, AGS_OSC_SERVER_STARTED);
}

/**
 * ags_osc_server_start:
 * @osc_server: the #AgsOscServer
 * 
 * Start OSC server.
 * 
 * Since: 2.1.0
 */
void
ags_osc_server_start(AgsOscServer *osc_server)
{
  g_return_if_fail(AGS_IS_OSC_SERVER(osc_server));
  
  g_object_ref((GObject *) osc_server);
  g_signal_emit(G_OBJECT(osc_server),
		osc_server_signals[START], 0);
  g_object_unref((GObject *) osc_server);
}

void
ags_osc_server_real_stop(AgsOscServer *osc_server)
{
  pthread_mutex_t *osc_server_mutex;

  /* get OSC server mutex */
  pthread_mutex_lock(ags_osc_server_get_class_mutex());
  
  osc_server_mutex = osc_server->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_server_get_class_mutex());

  /* close fd */
  pthread_mutex_lock(osc_server_mutex);

  if(osc_server->ip4_fd != -1){
    close(osc_server->ip4_fd);
  }

  if(osc_server->ip6_fd != -1){
    close(osc_server->ip6_fd);
  }

  pthread_mutex_unlock(osc_server_mutex);
}

/**
 * ags_osc_server_stop:
 * @osc_server: the #AgsOscServer
 * 
 * Stop OSC server.
 * 
 * Since: 2.1.0
 */
void
ags_osc_server_stop(AgsOscServer *osc_server)
{
  g_return_if_fail(AGS_IS_OSC_SERVER(osc_server));
  
  g_object_ref((GObject *) osc_server);
  g_signal_emit(G_OBJECT(osc_server),
		osc_server_signals[STOP], 0);
  g_object_unref((GObject *) osc_server);
}

void
ags_osc_server_real_listen(AgsOscServer *osc_server)
{
  if(ags_osc_server_test_flags(osc_server, AGS_OSC_SERVER_STARTED)){
    return;
  }

  if(osc_server->ip4_fd != -1){
    listen(osc_server->ip4_fd, AGS_OSC_SERVER_DEFAULT_BACKLOG);
  }
  
  if(osc_server->ip6_fd != -1){
    listen(osc_server->ip6_fd, AGS_OSC_SERVER_DEFAULT_BACKLOG);
  }

  while(ags_osc_server_test_flags(osc_server, AGS_OSC_SERVER_RUNNING)){
    //TODO:JK: implement me
  }
}

/**
 * ags_osc_server_listen:
 * @osc_server: the #AgsOscServer
 * 
 * Listen as OSC server.
 * 
 * Since: 2.1.0
 */
void
ags_osc_server_listen(AgsOscServer *osc_server)
{
  g_return_if_fail(AGS_IS_OSC_SERVER(osc_server));
  
  g_object_ref((GObject *) osc_server);
  g_signal_emit(G_OBJECT(osc_server),
		osc_server_signals[LISTEN], 0);
  g_object_unref((GObject *) osc_server);
}

void
ags_osc_server_real_dispatch(AgsOscServer *osc_server)
{
  //TODO:JK: implement me
}

/**
 * ags_osc_server_dispatch:
 * @osc_server: the #AgsOscServer
 * 
 * Dispatch OSC messages.
 * 
 * Since: 2.1.0
 */
void
ags_osc_server_dispatch(AgsOscServer *osc_server)
{
  g_return_if_fail(AGS_IS_OSC_SERVER(osc_server));
  
  g_object_ref((GObject *) osc_server);
  g_signal_emit(G_OBJECT(osc_server),
		osc_server_signals[DISPATCH], 0);
  g_object_unref((GObject *) osc_server);
}

/**
 * ags_osc_server_new:
 * 
 * Creates a new instance of #AgsOscServer
 *
 * Returns: the new #AgsOscServer
 * 
 * Since: 2.1.0
 */
AgsOscServer*
ags_osc_server_new()
{
  AgsOscServer *osc_server;
   
  osc_server = (AgsOscServer *) g_object_new(AGS_TYPE_OSC_SERVER,
					     NULL);
  
  return(osc_server);
}
