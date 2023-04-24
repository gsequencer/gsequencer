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

#include <ags/audio/osc/ags_osc_server.h>

#include <ags/audio/osc/ags_osc_connection.h>
#include <ags/audio/osc/ags_osc_response.h>
#include <ags/audio/osc/ags_osc_util.h>

#include <ags/audio/osc/controller/ags_osc_controller.h>
#include <ags/audio/osc/controller/ags_osc_front_controller.h>
#include <ags/audio/osc/controller/ags_osc_action_controller.h>
#include <ags/audio/osc/controller/ags_osc_config_controller.h>
#include <ags/audio/osc/controller/ags_osc_info_controller.h>
#include <ags/audio/osc/controller/ags_osc_meter_controller.h>
#include <ags/audio/osc/controller/ags_osc_node_controller.h>
#include <ags/audio/osc/controller/ags_osc_renew_controller.h>
#include <ags/audio/osc/controller/ags_osc_status_controller.h>

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
void ags_osc_server_dispose(GObject *gobject);
void ags_osc_server_finalize(GObject *gobject);

void ags_osc_server_real_start(AgsOscServer *osc_server);
void ags_osc_server_real_stop(AgsOscServer *osc_server);

gboolean ags_osc_server_real_listen(AgsOscServer *osc_server);

void ags_osc_server_real_dispatch(AgsOscServer *osc_server);

void* ags_osc_server_listen_thread(void *ptr);
void* ags_osc_server_dispatch_thread(void *ptr);

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
  PROP_CONNECTION,
  PROP_FRONT_CONTROLLER,
  PROP_CONTROLLER,
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

GType
ags_osc_server_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_OSC_SERVER_STARTED, "AGS_OSC_SERVER_STARTED", "osc-server-started" },
      { AGS_OSC_SERVER_RUNNING, "AGS_OSC_SERVER_RUNNING", "osc-server-running" },
      { AGS_OSC_SERVER_TERMINATING, "AGS_OSC_SERVER_TERMINATING", "osc-server-terminating" },
      { AGS_OSC_SERVER_INET4, "AGS_OSC_SERVER_INET4", "osc-server-inet4" },
      { AGS_OSC_SERVER_INET6, "AGS_OSC_SERVER_INET6", "osc-server-inet6" },
      { AGS_OSC_SERVER_UDP, "AGS_OSC_SERVER_UDP", "osc-server-udp" },
      { AGS_OSC_SERVER_TCP, "AGS_OSC_SERVER_TCP", "osc-server-tcp" },
      { AGS_OSC_SERVER_UNIX, "AGS_OSC_SERVER_UNIX", "osc-server-started" },
      { AGS_OSC_SERVER_ANY_ADDRESS, "AGS_OSC_SERVER_ANY_ADDRESS", "osc-server-any-address" },
      { AGS_OSC_SERVER_AUTO_START, "AGS_OSC_SERVER_AUTO_START", "osc-server-auto-start" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsOscServerFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
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
  
  gobject->dispose = ags_osc_server_dispose;
  gobject->finalize = ags_osc_server_finalize;

  /* properties */
  /**
   * AgsOscServer:domain:
   *
   * The domain to use.
   * 
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * AgsOscServer:ip6:
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

  /**
   * AgsOscServer:connection: (type GList(AgsOscConnection)) (transfer full)
   *
   * The assigned #AgsOscConnection providing default settings.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("connection",
				    i18n_pspec("assigned connection"),
				    i18n_pspec("The connection it is assigned with"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CONNECTION,
				  param_spec);

  /**
   * AgsOscServer:front-controller:
   *
   * The assigned #AgsOscFrontController.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("front-controller",
				   i18n_pspec("assigned OSC front controller"),
				   i18n_pspec("The OSC front controller it is assigned with"),
				   AGS_TYPE_OSC_FRONT_CONTROLLER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FRONT_CONTROLLER,
				  param_spec);
  
  /**
   * AgsOscServer:controller: (type GList(AgsOscController)) (transfer full)
   *
   * The assigned #AgsOscController providing default settings.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("controller",
				    i18n_pspec("assigned controller"),
				    i18n_pspec("The controller it is assigned with"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CONTROLLER,
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Returns: %TRUE as a new connection was initiated, otherwise %FALSE
   * 
   * Since: 3.0.0
   */
  osc_server_signals[LISTEN] =
    g_signal_new("listen",
		 G_TYPE_FROM_CLASS(osc_server),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscServerClass, listen),
		 NULL, NULL,
		 ags_cclosure_marshal_BOOLEAN__VOID,
		 G_TYPE_BOOLEAN, 0);

  /**
   * AgsOscServer::dispatch:
   * @osc_server: the #AgsOscServer
   *
   * The ::dispatch signal is emited during dispatch of server.
   *
   * Since: 3.0.0
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
  osc_server->connectable_flags = 0;
  
  /* osc server mutex */
  g_rec_mutex_init(&(osc_server->obj_mutex));

  osc_server->ip4 = g_strdup(AGS_OSC_SERVER_DEFAULT_INET4_ADDRESS);
  osc_server->ip6 = g_strdup(AGS_OSC_SERVER_DEFAULT_INET6_ADDRESS);

  osc_server->domain = g_strdup(AGS_OSC_SERVER_DEFAULT_DOMAIN);
  osc_server->server_port = AGS_OSC_SERVER_DEFAULT_SERVER_PORT;
  
  osc_server->ip4_fd = -1;
  osc_server->ip6_fd = -1;

  osc_server->ip4_socket = NULL;
  osc_server->ip6_socket = NULL;

  osc_server->ip4_address = NULL;
  osc_server->ip6_address = NULL;

  osc_server->accept_delay = (struct timespec *) malloc(sizeof(struct timespec));

  osc_server->accept_delay->tv_sec = 0;
  osc_server->accept_delay->tv_nsec = AGS_NSEC_PER_SEC / 1000;

  osc_server->dispatch_delay = (struct timespec *) malloc(sizeof(struct timespec));

  osc_server->dispatch_delay->tv_sec = 0;
  osc_server->dispatch_delay->tv_nsec = AGS_NSEC_PER_SEC / 1000;

  osc_server->listen_thread = NULL;
  osc_server->dispatch_thread = NULL;
  
  osc_server->connection = NULL;

  osc_server->front_controller = NULL;

  osc_server->controller = NULL;
}

void
ags_osc_server_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsOscServer *osc_server;

  GRecMutex *osc_server_mutex;

  osc_server = AGS_OSC_SERVER(gobject);

  /* get osc server mutex */
  osc_server_mutex = AGS_OSC_SERVER_GET_OBJ_MUTEX(osc_server);
  
  switch(prop_id){
  case PROP_DOMAIN:
    {
      gchar *domain;

      domain = g_value_get_string(value);

      g_rec_mutex_lock(osc_server_mutex);
      
      if(osc_server->domain == domain){
	g_rec_mutex_unlock(osc_server_mutex);
	
	return;
      }

      g_free(osc_server->domain);

      osc_server->domain = g_strdup(domain);

      g_rec_mutex_unlock(osc_server_mutex);
    }
    break;
  case PROP_SERVER_PORT:
    {
      guint server_port;

      server_port = g_value_get_uint(value);

      g_rec_mutex_lock(osc_server_mutex);

      osc_server->server_port = server_port;
      
      g_rec_mutex_unlock(osc_server_mutex);      
    }
    break;
  case PROP_IP4:
    {
      gchar *ip4;

      ip4 = g_value_get_string(value);

      g_rec_mutex_lock(osc_server_mutex);
      
      if(osc_server->ip4 == ip4){
	g_rec_mutex_unlock(osc_server_mutex);
	
	return;
      }

      g_free(osc_server->ip4);

      osc_server->ip4 = g_strdup(ip4);

      g_rec_mutex_unlock(osc_server_mutex);
    }
    break;
  case PROP_IP6:
    {
      gchar *ip6;

      ip6 = g_value_get_string(value);

      g_rec_mutex_lock(osc_server_mutex);
      
      if(osc_server->ip6 == ip6){
	g_rec_mutex_unlock(osc_server_mutex);
	
	return;
      }

      g_free(osc_server->ip6);

      osc_server->ip6 = g_strdup(ip6);

      g_rec_mutex_unlock(osc_server_mutex);
    }
    break;
  case PROP_CONNECTION:
    {
      GObject *connection;

      connection = g_value_get_pointer(value);

      g_rec_mutex_lock(osc_server_mutex);

      if(g_list_find(osc_server->connection, connection) != NULL){
	g_rec_mutex_unlock(osc_server_mutex);
	
	return;
      }

      g_rec_mutex_unlock(osc_server_mutex);

      ags_osc_server_add_connection(osc_server,
				    connection);
    }
    break;
  case PROP_FRONT_CONTROLLER:
    {
      GObject *front_controller;

      front_controller = g_value_get_object(value);

      g_rec_mutex_lock(osc_server_mutex);

      if(osc_server->front_controller == front_controller){
	g_rec_mutex_unlock(osc_server_mutex);
	
	return;
      }

      if(osc_server->front_controller != NULL){
	g_object_unref(osc_server->front_controller);
      }
      
      if(front_controller != NULL){
	g_object_ref(front_controller);
      }
      
      osc_server->front_controller = front_controller;

      g_rec_mutex_unlock(osc_server_mutex);
    }
    break;
  case PROP_CONTROLLER:
    {
      GObject *controller;

      controller = g_value_get_pointer(value);

      g_rec_mutex_lock(osc_server_mutex);

      if(g_list_find(osc_server->controller, controller) != NULL){
	g_rec_mutex_unlock(osc_server_mutex);
	
	return;
      }

      g_rec_mutex_unlock(osc_server_mutex);

      ags_osc_server_add_controller(osc_server,
				    controller);
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

  GRecMutex *osc_server_mutex;

  osc_server = AGS_OSC_SERVER(gobject);

  /* get osc server mutex */
  osc_server_mutex = AGS_OSC_SERVER_GET_OBJ_MUTEX(osc_server);
  
  switch(prop_id){
  case PROP_DOMAIN:
    {
      g_rec_mutex_lock(osc_server_mutex);

      g_value_set_string(value,
			 osc_server->domain);
      
      g_rec_mutex_unlock(osc_server_mutex);
    }
    break;
  case PROP_SERVER_PORT:
    {
      g_rec_mutex_lock(osc_server_mutex);
      
      g_value_set_uint(value,
		       osc_server->server_port);

      g_rec_mutex_unlock(osc_server_mutex);
    }
    break;
  case PROP_IP4:
    {
      g_rec_mutex_lock(osc_server_mutex);
      
      g_value_set_string(value,
			 osc_server->ip4);
      
      g_rec_mutex_unlock(osc_server_mutex);
    }
    break;
  case PROP_IP6:
    {
      g_rec_mutex_lock(osc_server_mutex);
      
      g_value_set_string(value,
			 osc_server->ip6);

      g_rec_mutex_unlock(osc_server_mutex);
    }
    break;    
  case PROP_CONNECTION:
    {
      g_rec_mutex_lock(osc_server_mutex);

      g_value_set_pointer(value,
			  g_list_copy_deep(osc_server->connection,
					   (GCopyFunc) g_object_ref,
					   NULL));

      g_rec_mutex_unlock(osc_server_mutex);
    }
    break;
  case PROP_FRONT_CONTROLLER:
    {
      g_rec_mutex_lock(osc_server_mutex);

      g_value_set_object(value, osc_server->front_controller);

      g_rec_mutex_unlock(osc_server_mutex);
    }
    break;
  case PROP_CONTROLLER:
    {
      g_rec_mutex_lock(osc_server_mutex);

      g_value_set_pointer(value,
			  g_list_copy_deep(osc_server->controller,
					   (GCopyFunc) g_object_ref,
					   NULL));

      g_rec_mutex_unlock(osc_server_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_server_dispose(GObject *gobject)
{
  AgsOscServer *osc_server;

  GList *start_list, *list;
  
  osc_server = (AgsOscServer *) gobject;

  if(osc_server->connection != NULL){
    list = 
      start_list = osc_server->connection;

    while(list != NULL){
      g_object_set(list->data,
		   "osc-server", NULL,
		   NULL);
    }
  
    g_list_free_full(start_list,
		     g_object_unref);

    osc_server->connection = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_osc_server_parent_class)->dispose(gobject);
}

void
ags_osc_server_finalize(GObject *gobject)
{
  AgsOscServer *osc_server;
    
  osc_server = (AgsOscServer *) gobject;
  
  g_free(osc_server->domain);
  
  g_free(osc_server->ip4);
  g_free(osc_server->ip6);

  if(osc_server->accept_delay != NULL){
    free(osc_server->accept_delay);
  }

  if(osc_server->dispatch_delay != NULL){
    free(osc_server->dispatch_delay);
  }
    
  g_list_free_full(osc_server->connection,
		   g_object_unref);
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_server_parent_class)->finalize(gobject);
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
 * Since: 3.0.0
 */
gboolean
ags_osc_server_test_flags(AgsOscServer *osc_server, AgsOscServerFlags flags)
{
  gboolean retval;  
  
  GRecMutex *osc_server_mutex;

  if(!AGS_IS_OSC_SERVER(osc_server)){
    return(FALSE);
  }

  /* get osc_server mutex */
  osc_server_mutex = AGS_OSC_SERVER_GET_OBJ_MUTEX(osc_server);

  /* test */
  g_rec_mutex_lock(osc_server_mutex);

  retval = ((flags & (osc_server->flags)) != 0) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(osc_server_mutex);

  return(retval);
}

/**
 * ags_osc_server_set_flags:
 * @osc_server: the #AgsOscServer
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 3.0.0
 */
void
ags_osc_server_set_flags(AgsOscServer *osc_server, AgsOscServerFlags flags)
{
  GRecMutex *osc_server_mutex;

  if(!AGS_IS_OSC_SERVER(osc_server)){
    return;
  }

  /* get osc_server mutex */
  osc_server_mutex = AGS_OSC_SERVER_GET_OBJ_MUTEX(osc_server);

  /* set flags */
  g_rec_mutex_lock(osc_server_mutex);

  osc_server->flags = (flags | (osc_server->flags));

  g_rec_mutex_unlock(osc_server_mutex);
}

/**
 * ags_osc_server_unset_flags:
 * @osc_server: the #AgsOscServer
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 3.0.0
 */
void
ags_osc_server_unset_flags(AgsOscServer *osc_server, AgsOscServerFlags flags)
{
  GRecMutex *osc_server_mutex;

  if(!AGS_IS_OSC_SERVER(osc_server)){
    return;
  }

  /* get osc_server mutex */
  osc_server_mutex = AGS_OSC_SERVER_GET_OBJ_MUTEX(osc_server);

  /* set flags */
  g_rec_mutex_lock(osc_server_mutex);

  osc_server->flags = ((~flags) & (osc_server->flags));

  g_rec_mutex_unlock(osc_server_mutex);
}

/**
 * ags_osc_server_add_connection:
 * @osc_server: the #AgsOscServer
 * @osc_connection: the #AgsOscConnection
 *
 * Add @osc_connection to @osc_server.
 * 
 * Since: 3.0.0
 */
void
ags_osc_server_add_connection(AgsOscServer *osc_server,
			      GObject *osc_connection)
{
  gboolean success;

  GRecMutex *osc_server_mutex;

  if(!AGS_IS_OSC_SERVER(osc_server) ||
     !AGS_IS_OSC_CONNECTION(osc_connection)){
    return;
  }

  /* get osc_server mutex */
  osc_server_mutex = AGS_OSC_SERVER_GET_OBJ_MUTEX(osc_server);

  success = FALSE;
  
  g_rec_mutex_lock(osc_server_mutex);

  if(g_list_find(osc_server->connection, osc_connection) == NULL){
    success = TRUE;
    
    g_object_ref(osc_connection);
    osc_server->connection = g_list_prepend(osc_server->connection,
					    osc_connection);
  }

  g_rec_mutex_unlock(osc_server_mutex);

  if(success){
    g_object_set(osc_connection,
		 "osc-server", osc_server,
		 NULL);
  }
}

/**
 * ags_osc_server_remove_connection:
 * @osc_server: the #AgsOscServer
 * @osc_connection: the #AgsOscConnection
 *
 * Remove @osc_connection from @osc_server.
 * 
 * Since: 3.0.0
 */
void
ags_osc_server_remove_connection(AgsOscServer *osc_server,
				 GObject *osc_connection)
{
  gboolean success;
  
  GRecMutex *osc_server_mutex;

  if(!AGS_IS_OSC_SERVER(osc_server) ||
     !AGS_IS_OSC_CONNECTION(osc_connection)){
    return;
  }

  /* get osc_server mutex */
  osc_server_mutex = AGS_OSC_SERVER_GET_OBJ_MUTEX(osc_server);

  success = FALSE;
  
  g_rec_mutex_lock(osc_server_mutex);

  if(g_list_find(osc_server->connection, osc_connection) != NULL){
    success = TRUE;
    
    osc_server->connection = g_list_remove(osc_server->connection,
					   osc_connection);
  }
  
  g_rec_mutex_unlock(osc_server_mutex);

  if(success){
    g_object_set(osc_connection,
		 "osc-server", NULL,
		 NULL);

    g_object_unref(osc_connection);
  }
}

/**
 * ags_osc_server_add_controller:
 * @osc_server: the #AgsOscServer
 * @osc_controller: the #AgsOscController
 *
 * Add @osc_controller to @osc_server.
 * 
 * Since: 3.0.0
 */
void
ags_osc_server_add_controller(AgsOscServer *osc_server,
			      GObject *osc_controller)
{
  gboolean success;
  
  GRecMutex *osc_server_mutex;

  if(!AGS_IS_OSC_SERVER(osc_server) ||
     !AGS_IS_OSC_CONTROLLER(osc_controller)){
    return;
  }

  /* get osc_server mutex */
  osc_server_mutex = AGS_OSC_SERVER_GET_OBJ_MUTEX(osc_server);

  success = FALSE;
  
  g_rec_mutex_lock(osc_server_mutex);

  if(g_list_find(osc_server->controller, osc_controller) == NULL){
    success = TRUE;
    
    g_object_ref(osc_controller);
    osc_server->controller = g_list_prepend(osc_server->controller,
					    osc_controller);
  }
  
  g_rec_mutex_unlock(osc_server_mutex);

  if(success){
    g_object_set(osc_controller,
		 "osc-server", osc_server,
		 NULL);
  }
}

/**
 * ags_osc_server_remove_controller:
 * @osc_server: the #AgsOscServer
 * @osc_controller: the #AgsOscController
 *
 * Remove @osc_controller from @osc_server.
 * 
 * Since: 3.0.0
 */
void
ags_osc_server_remove_controller(AgsOscServer *osc_server,
				 GObject *osc_controller)
{
  gboolean success;
  
  GRecMutex *osc_server_mutex;

  if(!AGS_IS_OSC_SERVER(osc_server) ||
     !AGS_IS_OSC_CONTROLLER(osc_controller)){
    return;
  }

  /* get osc_server mutex */
  osc_server_mutex = AGS_OSC_SERVER_GET_OBJ_MUTEX(osc_server);

  success = FALSE;
  
  g_rec_mutex_lock(osc_server_mutex);

  if(g_list_find(osc_server->controller, osc_controller) != NULL){
    success = TRUE;
    
    osc_server->controller = g_list_remove(osc_server->controller,
					   osc_controller);

    g_object_unref(osc_controller);
  }
  
  g_rec_mutex_unlock(osc_server_mutex);

  if(success){
    g_object_set(osc_controller,
		 "osc-server", NULL,
		 NULL);
  }
}

/**
 * ags_osc_server_add_default_controller:
 * @osc_server: the #AgsOscServer
 *
 * Add default controlers to @osc_server.
 * 
 * Since: 3.0.0
 */
void
ags_osc_server_add_default_controller(AgsOscServer *osc_server)
{
  AgsOscFrontController *front_controller;
  AgsOscActionController *action_controller;
  AgsOscConfigController *config_controller;
  AgsOscInfoController *info_controller;
  AgsOscMeterController *meter_controller;
  AgsOscNodeController *node_controller;
  AgsOscRenewController *renew_controller;
  AgsOscStatusController *status_controller;
  
  if(!AGS_IS_OSC_SERVER(osc_server)){
    return;
  }

  /* front controller */
  front_controller = ags_osc_front_controller_new();
  g_object_set(front_controller,
	       "osc-server", osc_server,
	       NULL);

  g_object_set(osc_server,
	       "front-controller", front_controller,
	       NULL);

  /* action controller */
  action_controller = ags_osc_action_controller_new();
  g_object_set(action_controller,
	       "osc-server", osc_server,
	       NULL);

  ags_osc_server_add_controller(osc_server,
				(GObject *) action_controller);

  /* config controller */
  config_controller = ags_osc_config_controller_new();
  g_object_set(config_controller,
	       "osc-server", osc_server,
	       NULL);

  ags_osc_server_add_controller(osc_server,
				(GObject *) config_controller);

  /* info controller */
  info_controller = ags_osc_info_controller_new();
  g_object_set(info_controller,
	       "osc-server", osc_server,
	       NULL);

  ags_osc_server_add_controller(osc_server,
				(GObject *) info_controller);

  /* meter controller */
  meter_controller = ags_osc_meter_controller_new();
  g_object_set(meter_controller,
	       "osc-server", osc_server,
	       NULL);

  ags_osc_server_add_controller(osc_server,
				(GObject *) meter_controller);

  /* node controller */
  node_controller = ags_osc_node_controller_new();
  g_object_set(node_controller,
	       "osc-server", osc_server,
	       NULL);

  ags_osc_server_add_controller(osc_server,
				(GObject *) node_controller);

  /* renew controller */
  renew_controller = ags_osc_renew_controller_new();
  g_object_set(renew_controller,
	       "osc-server", osc_server,
	       NULL);

  ags_osc_server_add_controller(osc_server,
				(GObject *) renew_controller);

  /* status controller */
  status_controller = ags_osc_status_controller_new();
  g_object_set(status_controller,
	       "osc-server", osc_server,
	       NULL);

  ags_osc_server_add_controller(osc_server,
				(GObject *) status_controller);
}

void
ags_osc_server_real_start(AgsOscServer *osc_server)
{
  AgsOscFrontController *osc_front_controller;
  
  GList *start_controller, *controller;
  
  gboolean any_address;
  gboolean ip4_success, ip6_success;
  gboolean ip4_udp_success, ip4_tcp_success;
  gboolean ip6_udp_success, ip6_tcp_success;

  GError *error;
  
  GRecMutex *osc_server_mutex;

  if(ags_osc_server_test_flags(osc_server, AGS_OSC_SERVER_STARTED)){
    return;
  }

  ags_osc_server_set_flags(osc_server, AGS_OSC_SERVER_STARTED);

  /* get osc_server mutex */
  osc_server_mutex = AGS_OSC_SERVER_GET_OBJ_MUTEX(osc_server);

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
      g_rec_mutex_lock(osc_server_mutex);
      
      error = NULL;      
      osc_server->ip4_socket = g_socket_new(G_SOCKET_FAMILY_IPV4,
					    G_SOCKET_TYPE_DATAGRAM,
					    G_SOCKET_PROTOCOL_UDP,
					    &error);
      osc_server->ip4_fd = g_socket_get_fd(osc_server->ip4_socket);
      
      g_socket_set_listen_backlog(osc_server->ip4_socket,
				  AGS_OSC_SERVER_DEFAULT_BACKLOG);

      g_rec_mutex_unlock(osc_server_mutex);

      if(error != NULL){
	g_critical("AgsOscServer - %s", error->message);

	g_error_free(error);
      }
    }else if(ags_osc_server_test_flags(osc_server, AGS_OSC_SERVER_TCP)){
      ip4_tcp_success = TRUE;

      /* create socket */
      g_rec_mutex_lock(osc_server_mutex);
      
      error = NULL;      
      osc_server->ip4_socket = g_socket_new(G_SOCKET_FAMILY_IPV4,
					    G_SOCKET_TYPE_STREAM,
					    G_SOCKET_PROTOCOL_TCP,
					    &error);
      osc_server->ip4_fd = g_socket_get_fd(osc_server->ip4_socket);
      
      g_socket_set_listen_backlog(osc_server->ip4_socket,
				  AGS_OSC_SERVER_DEFAULT_BACKLOG);
      
      g_rec_mutex_unlock(osc_server_mutex);

      if(error != NULL){
	g_critical("AgsOscServer - %s", error->message);

	g_error_free(error);
      }
    }else{
      g_critical("no flow control type");
    }

    /* get ip4 */
    if(any_address){
      g_rec_mutex_lock(osc_server_mutex);  

      osc_server->ip4_address = g_inet_socket_address_new(g_inet_address_new_any(G_SOCKET_FAMILY_IPV4),
							  osc_server->server_port);

      g_rec_mutex_unlock(osc_server_mutex);  
    }else{
      g_rec_mutex_lock(osc_server_mutex);  

      osc_server->ip4_address = g_inet_socket_address_new(g_inet_address_new_from_string(osc_server->ip4),
							  osc_server->server_port);

      g_rec_mutex_unlock(osc_server_mutex);
    }
  }

  if(ags_osc_server_test_flags(osc_server, AGS_OSC_SERVER_INET6)){    
    ip6_success = TRUE;
  
    if(ags_osc_server_test_flags(osc_server, AGS_OSC_SERVER_UDP)){
      ip6_udp_success = TRUE;

      /* create socket */
      g_rec_mutex_lock(osc_server_mutex);
      
      error = NULL;      
      osc_server->ip6_socket = g_socket_new(G_SOCKET_FAMILY_IPV6,
					    G_SOCKET_TYPE_DATAGRAM,
					    G_SOCKET_PROTOCOL_UDP,
					    &error);
      osc_server->ip6_fd = g_socket_get_fd(osc_server->ip6_socket);

      g_socket_set_listen_backlog(osc_server->ip6_socket,
				  AGS_OSC_SERVER_DEFAULT_BACKLOG);

      g_rec_mutex_unlock(osc_server_mutex);

      if(error != NULL){
	g_critical("AgsOscServer - %s", error->message);

	g_error_free(error);
      }
    }else if(ags_osc_server_test_flags(osc_server, AGS_OSC_SERVER_TCP)){
      ip6_tcp_success = TRUE;

      /* create socket */
      g_rec_mutex_lock(osc_server_mutex);
      
      error = NULL;      
      osc_server->ip6_socket = g_socket_new(G_SOCKET_FAMILY_IPV6,
					    G_SOCKET_TYPE_STREAM,
					    G_SOCKET_PROTOCOL_TCP,
					    &error);
      osc_server->ip6_fd = g_socket_get_fd(osc_server->ip6_socket);

      g_socket_set_listen_backlog(osc_server->ip6_socket,
				  AGS_OSC_SERVER_DEFAULT_BACKLOG);

      g_rec_mutex_unlock(osc_server_mutex);

      if(error != NULL){
	g_critical("AgsOscServer - %s", error->message);

	g_error_free(error);
      }
    }else{
      g_critical("no flow control type");
    }

    /* get ip6 */
    if(any_address){
      g_rec_mutex_lock(osc_server_mutex);

      osc_server->ip6_address = g_inet_socket_address_new(g_inet_address_new_any(G_SOCKET_FAMILY_IPV6),
							  osc_server->server_port);
      
      g_rec_mutex_unlock(osc_server_mutex);
    }else{
      g_rec_mutex_lock(osc_server_mutex);

      osc_server->ip6_address = g_inet_socket_address_new(g_inet_address_new_from_string(osc_server->ip6),
							  osc_server->server_port);

      g_rec_mutex_unlock(osc_server_mutex);
    }
  }
  
  if(ip4_success != TRUE && ip6_success != TRUE){
    g_critical("no protocol family");

    return;
  }

  if(ip4_success){
    error = NULL;
    g_socket_bind(osc_server->ip4_socket,
		  osc_server->ip4_address,
		  TRUE,
		  &error);
    
    if(error != NULL){
      g_critical("AgsOscServer - %s", error->message);

      g_error_free(error);
    }
  }
  
  if(ip6_success){
    error = NULL;
    g_socket_bind(osc_server->ip6_socket,
		  osc_server->ip6_address,
		  TRUE,
		  &error);

    if(error != NULL){
      g_critical("AgsOscServer - %s", error->message);

      g_error_free(error);
    }
  }

  if(osc_server->ip4_fd != -1){
    //TODO:JK: check remove
#if 0
    int flags;

    flags = fcntl(osc_server->ip4_fd, F_GETFL, 0);
    fcntl(osc_server->ip4_fd, F_SETFL, flags | O_NONBLOCK);
#else
    g_object_set(osc_server->ip4_socket,
		 "blocking", FALSE,
		 NULL);
#endif
  }

  if(osc_server->ip6_fd != -1){
    //TODO:JK: check remove
#if 0
    int flags;

    flags = fcntl(osc_server->ip6_fd, F_GETFL, 0);
    fcntl(osc_server->ip6_fd, F_SETFL, flags | O_NONBLOCK);
#else
    g_object_set(osc_server->ip6_socket,
		 "blocking", FALSE,
		 NULL);
#endif
  }

  ags_osc_server_set_flags(osc_server, AGS_OSC_SERVER_RUNNING);

  g_message("starting OSC listen and dispatch threads");

  /* create listen and dispatch thread */
  osc_server->listen_thread = g_thread_new("Advanced Gtk+ Sequencer OSC Server - listen thread",
					   ags_osc_server_listen_thread,
					   osc_server);

  osc_server->dispatch_thread = g_thread_new("Advanced Gtk+ Sequencer OSC Server - dispatch thread",
					     ags_osc_server_dispatch_thread,
					     osc_server);

  /* controller */
  g_object_get(osc_server,
	       "front-controller", &osc_front_controller,
	       "controller", &start_controller,
	       NULL);

  ags_osc_front_controller_start_delegate(osc_front_controller);
  
  controller = start_controller;

  while(controller != NULL){
    if(AGS_IS_OSC_METER_CONTROLLER(controller->data)){
      ags_osc_meter_controller_start_monitor(controller->data);
    }
    
    controller = controller->next;
  }

  g_object_unref(osc_front_controller);
  
  g_list_free_full(start_controller,
		   g_object_unref);
}

/**
 * ags_osc_server_start:
 * @osc_server: the #AgsOscServer
 * 
 * Start OSC server.
 * 
 * Since: 3.0.0
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
  AgsOscFrontController *osc_front_controller;
  
  GList *start_controller, *controller;

  GError *error;
  
  GRecMutex *osc_server_mutex;

  if(!ags_osc_server_test_flags(osc_server, AGS_OSC_SERVER_RUNNING)){
    return;
  }
  
  /* get OSC server mutex */
  osc_server_mutex = AGS_OSC_SERVER_GET_OBJ_MUTEX(osc_server);

  /* stop */
  ags_osc_server_set_flags(osc_server, AGS_OSC_SERVER_TERMINATING);
  ags_osc_server_unset_flags(osc_server, AGS_OSC_SERVER_RUNNING);

  //TODO:JK: this was disabled as a work-around
#if 1
  g_thread_join(osc_server->listen_thread);
  g_thread_join(osc_server->dispatch_thread);
#endif
  
  /* close fd */
  g_rec_mutex_lock(osc_server_mutex);

  if(osc_server->ip4_fd != -1){
    error = NULL;
    g_socket_close(osc_server->ip4_socket,
		   &error);
    g_object_unref(osc_server->ip4_socket);

    osc_server->ip4_socket = NULL;
    osc_server->ip4_fd = -1;
  }

  if(osc_server->ip6_fd != -1){
    error = NULL;
    g_socket_close(osc_server->ip6_socket,
		   &error);
    g_object_unref(osc_server->ip6_socket);

    osc_server->ip6_socket = NULL;
    osc_server->ip6_fd = -1;
  }

  g_rec_mutex_unlock(osc_server_mutex);

  /* controller */
  g_object_get(osc_server,
	       "front-controller", &osc_front_controller,
	       "controller", &start_controller,
	       NULL);

  ags_osc_front_controller_stop_delegate(osc_front_controller);
  
  controller = start_controller;

  while(controller != NULL){
    if(AGS_IS_OSC_METER_CONTROLLER(controller->data)){
      ags_osc_meter_controller_stop_monitor(controller->data);
    }
    
    controller = controller->next;
  }

  g_object_unref(osc_front_controller);
  
  g_list_free_full(start_controller,
		   g_object_unref);

  ags_osc_server_unset_flags(osc_server, (AGS_OSC_SERVER_STARTED |
					  AGS_OSC_SERVER_TERMINATING));
}

/**
 * ags_osc_server_stop:
 * @osc_server: the #AgsOscServer
 * 
 * Stop OSC server.
 * 
 * Since: 3.0.0
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

gboolean
ags_osc_server_real_listen(AgsOscServer *osc_server)
{
  gboolean created_connection;

  GError *error;

  GRecMutex *osc_server_mutex;
  
  if(!ags_osc_server_test_flags(osc_server, AGS_OSC_SERVER_STARTED)){
    return(FALSE);
  }
  
  /* get OSC server mutex */
  osc_server_mutex = AGS_OSC_SERVER_GET_OBJ_MUTEX(osc_server);

  if(osc_server->ip4_fd != -1){
    g_rec_mutex_lock(osc_server_mutex);

    error = NULL;
    g_socket_listen(osc_server->ip4_socket,
		    &error);
    
    g_rec_mutex_unlock(osc_server_mutex);

    if(error != NULL){
      g_critical("AgsOscServer - %s", error->message);

      g_error_free(error);
    }
  }

  if(osc_server->ip6_fd != -1){
    g_rec_mutex_lock(osc_server_mutex);

    error = NULL;
    g_socket_listen(osc_server->ip6_socket,
		    &error);
    
    g_rec_mutex_unlock(osc_server_mutex);

    if(error != NULL){
      g_critical("AgsOscServer - %s", error->message);

      g_error_free(error);
    }
  }  
  
  created_connection = FALSE;
  
  if(osc_server->ip4_fd != -1){
    GSocket *connection_socket;
    
    g_rec_mutex_lock(osc_server_mutex);

    error = NULL;
    connection_socket = g_socket_accept(osc_server->ip4_socket,
					NULL,
					&error);

    g_rec_mutex_unlock(osc_server_mutex);

    if(error != NULL){
      if(!g_error_matches(error, G_IO_ERROR, G_IO_ERROR_WOULD_BLOCK)){
	g_critical("AgsOscServer - %s", error->message);
      }

      g_error_free(error);
    }

    if(connection_socket != NULL){
      AgsOscConnection *osc_connection;

      created_connection = TRUE;

      osc_connection = ags_osc_connection_new((GObject *) osc_server);

      osc_connection->socket = connection_socket;
      osc_connection->fd = g_socket_get_fd(connection_socket);

      g_object_set(connection_socket,
		   "blocking", FALSE,
		   NULL);
      
      ags_osc_connection_set_flags(osc_connection,
				   (AGS_OSC_CONNECTION_ACTIVE |
				    AGS_OSC_CONNECTION_INET4));
      
      ags_osc_server_add_connection(osc_server,
				    (GObject *) osc_connection);
    }
  }
    
  if(osc_server->ip6_fd != -1){
    GSocket *connection_socket;
    
    g_rec_mutex_lock(osc_server_mutex);

    error = NULL;
    connection_socket = g_socket_accept(osc_server->ip6_socket,
					NULL,
					&error);

    g_rec_mutex_unlock(osc_server_mutex);

    if(error != NULL){
      if(!g_error_matches(error, G_IO_ERROR, G_IO_ERROR_WOULD_BLOCK)){
	g_critical("AgsOscServer - %s", error->message);
      }

      g_error_free(error);
    }

    if(connection_socket != NULL){
      AgsOscConnection *osc_connection;

      created_connection = TRUE;

      osc_connection = ags_osc_connection_new((GObject *) osc_server);

      osc_connection->socket = connection_socket;
      osc_connection->fd = g_socket_get_fd(connection_socket);

      g_object_set(connection_socket,
		   "blocking", FALSE,
		   NULL);
      
      ags_osc_connection_set_flags(osc_connection,
				   (AGS_OSC_CONNECTION_ACTIVE |
				    AGS_OSC_CONNECTION_INET6));
      
      ags_osc_server_add_connection(osc_server,
				    (GObject *) osc_connection);
    }
  }

  return(created_connection);
}

/**
 * ags_osc_server_listen:
 * @osc_server: the #AgsOscServer
 * 
 * Listen as OSC server.
 * 
 * Returns: %TRUE as a new connection was initiated, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_osc_server_listen(AgsOscServer *osc_server)
{
  gboolean created_connection;

  g_return_val_if_fail(AGS_IS_OSC_SERVER(osc_server), FALSE);
  
  g_object_ref((GObject *) osc_server);
  g_signal_emit(G_OBJECT(osc_server),
		osc_server_signals[LISTEN], 0,
		&created_connection);
  g_object_unref((GObject *) osc_server);

  return(created_connection);
}

void
ags_osc_server_real_dispatch(AgsOscServer *osc_server)
{
  GList *start_list, *list;

  guchar *slip_buffer;

  guint data_length;

  if(!ags_osc_server_test_flags(osc_server, AGS_OSC_SERVER_STARTED)){
    return;
  }

  g_object_get(osc_server,
	       "connection", &start_list,
	       NULL);
  
  list = start_list;

  while(list != NULL){
    int fd;

    GRecMutex *osc_connection_mutex;

    /* get osc_connection mutex */
    osc_connection_mutex = AGS_OSC_CONNECTION_GET_OBJ_MUTEX(list->data);
    
    /* get fd */
    g_rec_mutex_lock(osc_connection_mutex);

    fd = AGS_OSC_CONNECTION(list->data)->fd;
    
    g_rec_mutex_unlock(osc_connection_mutex);

    if(fd == -1){
      ags_osc_server_remove_connection(osc_server,
				       list->data);
      
      list = list->next;

      continue;
    }

    g_object_ref(list->data);
    slip_buffer = ags_osc_connection_read_bytes(list->data,
						&data_length);

    if(slip_buffer != NULL){
      GList *start_osc_response, *osc_response;

      guchar *packet;

      guint packet_size;

      packet = ags_osc_util_slip_decode(slip_buffer,
					data_length,
					&packet_size);

      osc_response = 
	start_osc_response = ags_osc_front_controller_do_request((AgsOscFrontController *) osc_server->front_controller,
								 list->data,
								 packet, packet_size);

      while(osc_response != NULL){
	ags_osc_connection_write_response(list->data,
					  osc_response->data);

	osc_response = osc_response->next;
      }

      //      g_list_free_full(start_osc_response,
      //	       g_object_unref);

      /* free packet */
      if(packet != NULL){
	free(packet);
      }
    }
    
    g_object_unref(list->data);
    
    list = list->next;
  }
      
  g_list_free_full(start_list,
		   g_object_unref);
}

/**
 * ags_osc_server_dispatch:
 * @osc_server: the #AgsOscServer
 * 
 * Dispatch the OSC server.
 * 
 * Since: 3.0.0
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

void*
ags_osc_server_listen_thread(void *ptr)
{
  AgsOscServer *osc_server;

  gboolean created_connection;

  osc_server = AGS_OSC_SERVER(ptr);

  while(ags_osc_server_test_flags(osc_server, AGS_OSC_SERVER_RUNNING)){    
    created_connection = ags_osc_server_listen(osc_server);

    if(!created_connection){
      nanosleep(osc_server->accept_delay, NULL);
    }
  }
  
  g_thread_exit(NULL);

  return(NULL);
}

void*
ags_osc_server_dispatch_thread(void *ptr)
{
  AgsOscServer *osc_server;

  osc_server = AGS_OSC_SERVER(ptr);
  
  while(ags_osc_server_test_flags(osc_server, AGS_OSC_SERVER_RUNNING)){
    ags_osc_server_dispatch(osc_server);

    nanosleep(osc_server->dispatch_delay, NULL);
  }
  
  g_thread_exit(NULL);

  return(NULL);
}

/**
 * ags_osc_server_new:
 * 
 * Creates a new instance of #AgsOscServer
 *
 * Returns: the new #AgsOscServer
 * 
 * Since: 3.0.0
 */
AgsOscServer*
ags_osc_server_new()
{
  AgsOscServer *osc_server;
   
  osc_server = (AgsOscServer *) g_object_new(AGS_TYPE_OSC_SERVER,
					     NULL);
  
  return(osc_server);
}
