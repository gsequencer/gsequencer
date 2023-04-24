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

#include <ags/audio/osc/ags_osc_connection.h>

#include <ags/audio/osc/ags_osc_server.h>
#include <ags/audio/osc/ags_osc_response.h>
#include <ags/audio/osc/ags_osc_util.h>
#include <ags/audio/osc/ags_osc_buffer_util.h>

#include <stdlib.h>
#include <string.h>
#if !defined(AGS_W32API)
#include <unistd.h>
#endif
#include <time.h>

#ifdef __APPLE__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#include <ags/i18n.h>

void ags_osc_connection_class_init(AgsOscConnectionClass *osc_connection);
void ags_osc_connection_init(AgsOscConnection *osc_connection);
void ags_osc_connection_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_osc_connection_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_osc_connection_dispose(GObject *gobject);
void ags_osc_connection_finalize(GObject *gobject);

guchar* ags_osc_connection_real_read_bytes(AgsOscConnection *osc_connection,
						  guint *data_length);
gint64 ags_osc_connection_real_write_response(AgsOscConnection *osc_connection,
					      GObject *osc_response);

void ags_osc_connection_real_close(AgsOscConnection *osc_connection);

/**
 * SECTION:ags_osc_connection
 * @short_description: the OSC server side connection
 * @title: AgsOscConnection
 * @section_id:
 * @include: ags/audio/osc/ags_osc_connection.h
 *
 * #AgsOscConnection your OSC server side connection.
 */

enum{
  PROP_0,
  PROP_OSC_SERVER,
  PROP_IP4,
  PROP_IP6,
};

enum{
  READ_BYTES,
  WRITE_RESPONSE,
  CLOSE,
  LAST_SIGNAL,
};

static gpointer ags_osc_connection_parent_class = NULL;
static guint osc_connection_signals[LAST_SIGNAL];

GType
ags_osc_connection_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_osc_connection = 0;

    static const GTypeInfo ags_osc_connection_info = {
      sizeof (AgsOscConnectionClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_osc_connection_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOscConnection),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_osc_connection_init,
    };

    ags_type_osc_connection = g_type_register_static(G_TYPE_OBJECT,
						     "AgsOscConnection", &ags_osc_connection_info,
						     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_osc_connection);
  }

  return g_define_type_id__volatile;
}

GType
ags_osc_connection_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_OSC_CONNECTION_ACTIVE, "AGS_OSC_CONNECTION_ACTIVE", "osc-connection-active" },
      { AGS_OSC_CONNECTION_INET4, "AGS_OSC_CONNECTION_INET4", "osc-connection-inet4" },
      { AGS_OSC_CONNECTION_INET6, "AGS_OSC_CONNECTION_INET6", "osc-connection-inet6" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsOscConnectionFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
}

void
ags_osc_connection_class_init(AgsOscConnectionClass *osc_connection)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;
  
  ags_osc_connection_parent_class = g_type_class_peek_parent(osc_connection);

  /* GObjectClass */
  gobject = (GObjectClass *) osc_connection;

  gobject->set_property = ags_osc_connection_set_property;
  gobject->get_property = ags_osc_connection_get_property;
  
  gobject->dispose = ags_osc_connection_dispose;
  gobject->finalize = ags_osc_connection_finalize;

  /* properties */
  /**
   * AgsOscConnection:osc-server:
   *
   * The assigned #AgsOscServer.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("osc-server",
				   i18n_pspec("assigned OSC server"),
				   i18n_pspec("The OSC server it is assigned with"),
				   AGS_TYPE_OSC_SERVER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OSC_SERVER,
				  param_spec);

  /**
   * AgsOscConnection:ip4:
   *
   * The IPv4 address as string of the server connection.
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
   * AgsOscConnection:ip6:
   *
   * The IPv6 address as string of the server connection.
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

  /* AgsOscConnectionClass */  
  osc_connection->read_bytes = ags_osc_connection_real_read_bytes;
  osc_connection->write_response = ags_osc_connection_real_write_response;

  osc_connection->close = ags_osc_connection_real_close;

  /* signals */
  /**
   * AgsOscConnection::read-bytes:
   * @osc_connection: the #AgsOscConnection
   * @data_length: the return location of byte array's length
   *
   * The ::read-bytes signal is emited while read bytes.
   *
   * Returns: byte array read or %NULL if no data available
   * 
   * Since: 3.0.0
   */
  osc_connection_signals[READ_BYTES] =
    g_signal_new("read-bytes",
		 G_TYPE_FROM_CLASS(osc_connection),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscConnectionClass, read_bytes),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__POINTER,
		 G_TYPE_POINTER, 1,
		 G_TYPE_POINTER);

  /**
   * AgsOscConnection::write-response:
   * @osc_connection: the #AgsOscConnection
   * @osc_response: the #AgsOscResponse
   *
   * The ::write-response signal is emited while write response.
   *
   * Returns: the count bytes written
   * 
   * Since: 3.0.0
   */
  osc_connection_signals[WRITE_RESPONSE] =
    g_signal_new("write-response",
		 G_TYPE_FROM_CLASS(osc_connection),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscConnectionClass, write_response),
		 NULL, NULL,
		 ags_cclosure_marshal_INT64__OBJECT,
		 G_TYPE_INT64, 1,
		 G_TYPE_OBJECT);

  /**
   * AgsOscConnection::close:
   * @osc_connection: the #AgsOscConnection
   *
   * The ::close signal is emited as closing the file descriptor.
   * 
   * Since: 3.0.0
   */
  osc_connection_signals[CLOSE] =
    g_signal_new("close",
		 G_TYPE_FROM_CLASS(osc_connection),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscConnectionClass, close),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_osc_connection_init(AgsOscConnection *osc_connection)
{
#ifdef __APPLE__
  clock_serv_t cclock;
  mach_timespec_t mts;
#endif

  osc_connection->flags = AGS_OSC_CONNECTION_INET4;
  osc_connection->connectable_flags = 0;
  
  /* osc connection mutex */
  g_rec_mutex_init(&(osc_connection->obj_mutex));

  osc_connection->ip4 = NULL;
  osc_connection->ip6 = NULL;

  osc_connection->fd = -1;

  osc_connection->socket = NULL;
  
  osc_connection->start_time = (struct timespec *) malloc(sizeof(struct timespec));

  osc_connection->start_time->tv_sec = 0;
  osc_connection->start_time->tv_nsec = 0;

  osc_connection->cache_data = (guchar *) malloc(AGS_OSC_CONNECTION_DEFAULT_CACHE_DATA_LENGTH * sizeof(guchar));
  osc_connection->cache_data_length = 0;
  
  osc_connection->buffer = (guchar *) malloc(AGS_OSC_CONNECTION_CHUNK_SIZE * sizeof(guchar));
  osc_connection->allocated_buffer_size = AGS_OSC_CONNECTION_CHUNK_SIZE;

  osc_connection->read_count = 0;
  osc_connection->has_valid_data = FALSE;
  
  osc_connection->timeout_delay = (struct timespec *) malloc(sizeof(struct timespec));

  osc_connection->timeout_delay->tv_sec = 0;
  osc_connection->timeout_delay->tv_nsec = 4000;

  osc_connection->timestamp = (struct timespec *) malloc(sizeof(struct timespec));

  osc_connection->timestamp->tv_sec = 0;
  osc_connection->timestamp->tv_nsec = 0;

#ifdef __APPLE__
  host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
      
  clock_get_time(cclock, &mts);
  mach_port_deallocate(mach_task_self(), cclock);
      
  osc_connection->timestamp->tv_sec = mts.tv_sec;
  osc_connection->timestamp->tv_nsec = mts.tv_nsec;
#else
  clock_gettime(CLOCK_MONOTONIC, osc_connection->timestamp);
#endif
}

void
ags_osc_connection_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsOscConnection *osc_connection;

  GRecMutex *osc_connection_mutex;

  osc_connection = AGS_OSC_CONNECTION(gobject);

  /* get osc connection mutex */
  osc_connection_mutex = AGS_OSC_CONNECTION_GET_OBJ_MUTEX(osc_connection);
  
  switch(prop_id){
  case PROP_OSC_SERVER:
    {
      GObject *osc_server;

      osc_server = g_value_get_object(value);

      g_rec_mutex_lock(osc_connection_mutex);

      if(osc_connection->osc_server == osc_server){
	g_rec_mutex_unlock(osc_connection_mutex);

	return;
      }

      if(osc_connection->osc_server != NULL){
	g_object_unref(osc_connection->osc_server);
      }
      
      if(osc_server != NULL){
	g_object_ref(osc_server);
      }
      
      osc_connection->osc_server = osc_server;

      g_rec_mutex_unlock(osc_connection_mutex);
    }
    break;
  case PROP_IP4:
    {
      gchar *ip4;

      ip4 = g_value_get_string(value);

      g_rec_mutex_lock(osc_connection_mutex);
      
      if(osc_connection->ip4 == ip4){
	g_rec_mutex_unlock(osc_connection_mutex);
	
	return;
      }

      g_free(osc_connection->ip4);

      osc_connection->ip4 = g_strdup(ip4);

      g_rec_mutex_unlock(osc_connection_mutex);
    }
    break;
  case PROP_IP6:
    {
      gchar *ip6;

      ip6 = g_value_get_string(value);

      g_rec_mutex_lock(osc_connection_mutex);
      
      if(osc_connection->ip6 == ip6){
	g_rec_mutex_unlock(osc_connection_mutex);
	
	return;
      }

      g_free(osc_connection->ip6);

      osc_connection->ip6 = g_strdup(ip6);

      g_rec_mutex_unlock(osc_connection_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_connection_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec)
{
  AgsOscConnection *osc_connection;

  GRecMutex *osc_connection_mutex;

  osc_connection = AGS_OSC_CONNECTION(gobject);

  /* get osc connection mutex */
  osc_connection_mutex = AGS_OSC_CONNECTION_GET_OBJ_MUTEX(osc_connection);
  
  switch(prop_id){
  case PROP_OSC_SERVER:
    {
      g_rec_mutex_lock(osc_connection_mutex);

      g_value_set_object(value, osc_connection->osc_server);

      g_rec_mutex_unlock(osc_connection_mutex);
    }
    break;
  case PROP_IP4:
    {
      g_rec_mutex_lock(osc_connection_mutex);
      
      g_value_set_string(value,
			 osc_connection->ip4);
      
      g_rec_mutex_unlock(osc_connection_mutex);
    }
    break;
  case PROP_IP6:
    {
      g_rec_mutex_lock(osc_connection_mutex);
      
      g_value_set_string(value,
			 osc_connection->ip6);

      g_rec_mutex_unlock(osc_connection_mutex);
    }
    break;    
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_connection_dispose(GObject *gobject)
{
  AgsOscConnection *osc_connection;
    
  osc_connection = (AgsOscConnection *) gobject;

  if(osc_connection->osc_server != NULL){
    g_object_unref(osc_connection->osc_server);

    osc_connection->osc_server = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_connection_parent_class)->dispose(gobject);
}

void
ags_osc_connection_finalize(GObject *gobject)
{
  AgsOscConnection *osc_connection;
    
  osc_connection = (AgsOscConnection *) gobject;

  if(osc_connection->osc_server != NULL){
    g_object_unref(osc_connection->osc_server);
  }

  g_free(osc_connection->ip4);
  g_free(osc_connection->ip6);

  if(osc_connection->start_time != NULL){
    free(osc_connection->start_time);
  }

  if(osc_connection->timeout_delay != NULL){
    free(osc_connection->timeout_delay);
  }

  if(osc_connection->timestamp != NULL){
    free(osc_connection->timestamp);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_connection_parent_class)->finalize(gobject);
}

/**
 * ags_osc_connection_test_flags:
 * @osc_connection: the #AgsOscConnection
 * @flags: the flags
 *
 * Test @flags to be set on @osc_connection.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_osc_connection_test_flags(AgsOscConnection *osc_connection, AgsOscConnectionFlags flags)
{
  gboolean retval;  
  
  GRecMutex *osc_connection_mutex;

  if(!AGS_IS_OSC_CONNECTION(osc_connection)){
    return(FALSE);
  }

  /* get osc_connection mutex */
  osc_connection_mutex = AGS_OSC_CONNECTION_GET_OBJ_MUTEX(osc_connection);

  /* test */
  g_rec_mutex_lock(osc_connection_mutex);

  retval = (flags & (osc_connection->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(osc_connection_mutex);

  return(retval);
}

/**
 * ags_osc_connection_set_flags:
 * @osc_connection: the #AgsOscConnection
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 3.0.0
 */
void
ags_osc_connection_set_flags(AgsOscConnection *osc_connection, AgsOscConnectionFlags flags)
{
  GRecMutex *osc_connection_mutex;

  if(!AGS_IS_OSC_CONNECTION(osc_connection)){
    return;
  }

  /* get osc_connection mutex */
  osc_connection_mutex = AGS_OSC_CONNECTION_GET_OBJ_MUTEX(osc_connection);

  /* set flags */
  g_rec_mutex_lock(osc_connection_mutex);

  osc_connection->flags |= flags;

  g_rec_mutex_unlock(osc_connection_mutex);
}

/**
 * ags_osc_connection_unset_flags:
 * @osc_connection: the #AgsOscConnection
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 3.0.0
 */
void
ags_osc_connection_unset_flags(AgsOscConnection *osc_connection, AgsOscConnectionFlags flags)
{
  GRecMutex *osc_connection_mutex;

  if(!AGS_IS_OSC_CONNECTION(osc_connection)){
    return;
  }

  /* get osc_connection mutex */
  osc_connection_mutex = AGS_OSC_CONNECTION_GET_OBJ_MUTEX(osc_connection);

  /* set flags */
  g_rec_mutex_lock(osc_connection_mutex);

  osc_connection->flags &= (~flags);

  g_rec_mutex_unlock(osc_connection_mutex);
}

/**
 * ags_osc_connection_timeout_expired:
 * @start_time: the start time #timespec-struct
 * @timeout_delay: the delay #timespec-struct
 * 
 * Check @start_time plus @timeout_delay against current time.
 * 
 * Returns: %TRUE if timeout expired, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_osc_connection_timeout_expired(struct timespec *start_time,
				   struct timespec *timeout_delay)
{
  struct timespec current_time;
  struct timespec deadline;

#ifdef __APPLE__
  clock_serv_t cclock;
  mach_timespec_t mts;
#endif

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

  if(start_time->tv_nsec + timeout_delay->tv_nsec > AGS_NSEC_PER_SEC){
    deadline.tv_sec = start_time->tv_sec + timeout_delay->tv_sec + 1;
    deadline.tv_nsec = (start_time->tv_nsec + timeout_delay->tv_nsec) - AGS_NSEC_PER_SEC;
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

guchar*
ags_osc_connection_real_read_bytes(AgsOscConnection *osc_connection,
				   guint *data_length)
{
  guchar *buffer;
  guchar data[AGS_OSC_CONNECTION_DEFAULT_CACHE_DATA_LENGTH];
  
  guint allocated_buffer_size;
  guint read_count;
  int retval;
  guint available_data_length;
  gint64 start_data, end_data;
  int fd;
  guint i, j;
  gboolean has_valid_data;
  gboolean success;

#ifdef __APPLE__
  clock_serv_t cclock;
  mach_timespec_t mts;
#endif

  GError *error;
  
  GRecMutex *osc_connection_mutex;

  /* get osc_connection mutex */
  osc_connection_mutex = AGS_OSC_CONNECTION_GET_OBJ_MUTEX(osc_connection);

  /* get fd */
  g_rec_mutex_lock(osc_connection_mutex);

#ifdef __APPLE__
  host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
      
  clock_get_time(cclock, &mts);
  mach_port_deallocate(mach_task_self(), cclock);
      
  osc_connection->start_time->tv_sec = mts.tv_sec;
  osc_connection->start_time->tv_nsec = mts.tv_nsec;
#else
  clock_gettime(CLOCK_MONOTONIC, osc_connection->start_time);
#endif

  fd = osc_connection->fd;
  
  buffer = osc_connection->buffer;
  allocated_buffer_size = osc_connection->allocated_buffer_size;
  
  read_count = osc_connection->read_count;
  has_valid_data = osc_connection->has_valid_data;
  
  g_rec_mutex_unlock(osc_connection_mutex);

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
  
  while(!ags_osc_connection_timeout_expired(osc_connection->start_time,
					    osc_connection->timeout_delay)){
    available_data_length = 0;
    
    if(osc_connection->cache_data_length > 0){
      memcpy(data,
	     osc_connection->cache_data,
	     osc_connection->cache_data_length * sizeof(guchar));

      available_data_length += osc_connection->cache_data_length;
    }

    retval = 0;

    error = NULL;
    
    g_rec_mutex_lock(osc_connection_mutex);
    
    if(osc_connection->cache_data_length < AGS_OSC_CONNECTION_DEFAULT_CACHE_DATA_LENGTH){
      retval = g_socket_receive(osc_connection->socket,
				data + osc_connection->cache_data_length,
				AGS_OSC_CONNECTION_DEFAULT_CACHE_DATA_LENGTH - osc_connection->cache_data_length,
				NULL,
				&error);
      
      //TODO:JK: check remove
#if 0
      retval = read(fd,
		    data + osc_connection->cache_data_length,
		    AGS_OSC_CONNECTION_DEFAULT_CACHE_DATA_LENGTH - osc_connection->cache_data_length);
#endif
      
      if(retval > 0){
	available_data_length += retval;
      }
    }

    osc_connection->cache_data_length = 0;
    
    g_rec_mutex_unlock(osc_connection_mutex);
    
    if(error != NULL){
      if(!g_error_matches(error, G_IO_ERROR, G_IO_ERROR_WOULD_BLOCK)){
	g_critical("AgsOscConnection - %s", error->message);
      }

      if(g_error_matches(error, G_IO_ERROR, G_IO_ERROR_WOULD_BLOCK)){
	if(available_data_length == 0){
	  g_error_free(error);

	  continue;
	}
      }else{
	if(g_error_matches(error, G_IO_ERROR, G_IO_ERROR_CONNECTION_CLOSED)){
	  GError *close_error;
	  
	  g_rec_mutex_lock(osc_connection_mutex);

	  close_error = NULL;
	  g_socket_close(osc_connection->socket,
			 &close_error);
	  g_object_unref(osc_connection->socket);

	  osc_connection->socket = NULL;
	  osc_connection->fd = -1;
	
	  g_rec_mutex_unlock(osc_connection_mutex);

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

    ags_osc_connection_read_bytes_REPEAT_0:
      
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
	  start_data = end_data;
	  j++;
	  
	  if(j < 2 ||
	     !ags_osc_connection_timeout_expired(osc_connection->start_time,
						 osc_connection->timeout_delay)){
	    goto ags_osc_connection_read_bytes_REPEAT_0;
	  }else{
	    break;
	  }
	}
	
	g_rec_mutex_lock(osc_connection_mutex);

	memcpy(osc_connection->buffer, data + start_data, (end_data - start_data + 1) * sizeof(guchar));
	  
	g_rec_mutex_unlock(osc_connection_mutex);

	if(data_length != NULL){
	  data_length[0] = end_data - start_data + 1;
	}

	/* fill cache */
	if(end_data > 0 &&
	   end_data < available_data_length){
	  memcpy(osc_connection->cache_data,
		 data + end_data,
		 (available_data_length - end_data) * sizeof(guchar));

	  osc_connection->cache_data_length = available_data_length - end_data;
	}

	osc_connection->read_count = 0;
	osc_connection->has_valid_data = FALSE;
	
	return(osc_connection->buffer);
      }else{
	read_count += (available_data_length - start_data);
	  
	g_rec_mutex_lock(osc_connection_mutex);

	memcpy(osc_connection->buffer, data + start_data, (available_data_length - start_data) * sizeof(guchar));
	  
	g_rec_mutex_unlock(osc_connection_mutex);

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
	  if(read_count + i >= AGS_OSC_CONNECTION_CHUNK_SIZE){
	    if(data_length != NULL){
	      data_length[0] = 0;
	    }

	    osc_connection->read_count = 0;
	    osc_connection->has_valid_data = FALSE;
	    
	    return(NULL);
	  }
	  
	  g_rec_mutex_lock(osc_connection_mutex);

	  memcpy(osc_connection->buffer + read_count, data, (i + 1) * sizeof(guchar));
	
	  g_rec_mutex_unlock(osc_connection_mutex);

	  read_count += (i + 1);
	  
	  if(data_length != NULL){
	    data_length[0] = read_count;
	  }

	  /* fill cache */
	  if(i < available_data_length){
	    memcpy(osc_connection->cache_data,
		   data + i,
		   (available_data_length - i) * sizeof(guchar));

	    osc_connection->cache_data_length = available_data_length - i;
	  }

	  osc_connection->read_count = 0;
	  osc_connection->has_valid_data = FALSE;
	  
	  return(osc_connection->buffer);
	}else{
	  if(read_count + available_data_length >= AGS_OSC_CONNECTION_CHUNK_SIZE){
	    if(data_length != NULL){
	      data_length[0] = 0;
	    }

	    osc_connection->read_count = 0;
	    osc_connection->has_valid_data = FALSE;
	    
	    return(NULL);
	  }
	  
	  g_rec_mutex_lock(osc_connection_mutex);
	
	  memcpy(osc_connection->buffer + read_count, data, (available_data_length) * sizeof(guchar));

	  g_rec_mutex_unlock(osc_connection_mutex);

	  read_count += available_data_length;

	  continue;
	}
      }
    }
  }

  osc_connection->read_count = read_count;
  osc_connection->has_valid_data = has_valid_data;
  
  if(data_length != NULL){
    data_length[0] = 0;
  }

  return(NULL);
}

/**
 * ags_osc_connection_read_bytes:
 * @osc_connection: the #AgsOscConnection
 * @data_length: the return location of byte array's length
 * 
 * Read bytes.
 * 
 * Returns: byte array read or %NULL if no data available
 * 
 * Since: 3.0.0
 */
guchar*
ags_osc_connection_read_bytes(AgsOscConnection *osc_connection,
			      guint *data_length)
{
  guchar *buffer;
  
  g_return_val_if_fail(AGS_IS_OSC_CONNECTION(osc_connection), NULL);
  
  g_object_ref((GObject *) osc_connection);
  g_signal_emit(G_OBJECT(osc_connection),
		osc_connection_signals[READ_BYTES], 0,
		data_length,
		&buffer);
  g_object_unref((GObject *) osc_connection);

  return(buffer);
}

gint64
ags_osc_connection_real_write_response(AgsOscConnection *osc_connection,
				       GObject *osc_response)
{
  guchar *slip_buffer;
  
  int fd;
  guint slip_buffer_length;
  gint64 num_write;

  GError *error;
  
  GRecMutex *osc_connection_mutex;
  GRecMutex *osc_response_mutex;

  if(!AGS_IS_OSC_RESPONSE(osc_response)){
    return(-1);
  }

  /* get osc connection mutex */
  osc_connection_mutex = AGS_OSC_CONNECTION_GET_OBJ_MUTEX(osc_connection);

  /* get fd */
  g_rec_mutex_lock(osc_connection_mutex);

  fd = osc_connection->fd;

  g_rec_mutex_unlock(osc_connection_mutex);
  
  if(fd == -1){
    return(0);
  }
  
  /* get osc response mutex */
  osc_response_mutex = AGS_OSC_RESPONSE_GET_OBJ_MUTEX(osc_response);

  /* write */
  g_rec_mutex_lock(osc_response_mutex);

  slip_buffer = ags_osc_util_slip_encode(AGS_OSC_RESPONSE(osc_response)->packet,
					 AGS_OSC_RESPONSE(osc_response)->packet_size,
					 &slip_buffer_length);
  
  g_rec_mutex_unlock(osc_response_mutex);

  g_rec_mutex_lock(osc_connection_mutex);

  num_write = 0;

  error = NULL;
  
  if(osc_connection->socket != NULL){
    num_write = g_socket_send(osc_connection->socket,
			      slip_buffer,
			      slip_buffer_length * sizeof(guchar),
			      NULL,
			      &error);
  }
  
  g_rec_mutex_unlock(osc_connection_mutex);
  
  if(error != NULL){
    if(!g_error_matches(error, G_IO_ERROR, G_IO_ERROR_WOULD_BLOCK)){
      g_critical("AgsOscConnection - %s", error->message);
    }
    
    g_error_free(error);
  }
  
  //TODO:JK: check remove
#if 0  
  num_write = write(fd, slip_buffer, slip_buffer_length * sizeof(guchar));
#endif
  
  if(slip_buffer != NULL){
    free(slip_buffer);
  }
  
  return(num_write);
}

/**
 * ags_osc_connection_write_response:
 * @osc_connection: the #AgsOscConnection
 * @osc_response: the #AgsOscResponse
 * 
 * Write response.
 * 
 * Returns: the count of bytes written
 * 
 * Since: 3.0.0
 */
gint64
ags_osc_connection_write_response(AgsOscConnection *osc_connection,
				  GObject *osc_response)
{
  gint64 num_write;
  
  g_return_val_if_fail(AGS_IS_OSC_CONNECTION(osc_connection), -1);
  
  g_object_ref((GObject *) osc_connection);
  g_signal_emit(G_OBJECT(osc_connection),
		osc_connection_signals[WRITE_RESPONSE], 0,
		osc_response,
		&num_write);
  g_object_unref((GObject *) osc_connection);

  return(num_write);
}

void
ags_osc_connection_real_close(AgsOscConnection *osc_connection)
{
  GError *error;
  
  GRecMutex *osc_connection_mutex;
  
  /* get osc_connection mutex */
  osc_connection_mutex = AGS_OSC_CONNECTION_GET_OBJ_MUTEX(osc_connection);

  /* set flags */
  g_rec_mutex_lock(osc_connection_mutex);

  error = NULL;

  if(osc_connection->socket != NULL){
    g_socket_close(osc_connection->socket,
		   &error);
    g_object_unref(osc_connection->socket);
  }
  
  osc_connection->socket = NULL;
  osc_connection->fd = -1;
  
  g_rec_mutex_unlock(osc_connection_mutex);

  if(error != NULL){
    g_critical("AgsOscConnection - %s", error->message);

    g_error_free(error);
  }
  
  ags_osc_connection_unset_flags(osc_connection,
				 AGS_OSC_CONNECTION_ACTIVE);
}

/**
 * ags_osc_connection_close:
 * @osc_connection: the #AgsOscConnection
 * 
 * Close @osc_connection.
 * 
 * Since: 3.0.0
 */
void
ags_osc_connection_close(AgsOscConnection *osc_connection)
{
  g_return_if_fail(AGS_IS_OSC_CONNECTION(osc_connection));
  
  g_object_ref((GObject *) osc_connection);
  g_signal_emit(G_OBJECT(osc_connection),
		osc_connection_signals[CLOSE], 0);
  g_object_unref((GObject *) osc_connection);
}

/**
 * ags_osc_connection_new:
 * @osc_server: the #AgsOscServer
 * 
 * Creates a new instance of #AgsOscConnection
 *
 * Returns: the new #AgsOscConnection
 * 
 * Since: 3.0.0
 */
AgsOscConnection*
ags_osc_connection_new(GObject *osc_server)
{
  AgsOscConnection *osc_connection;
   
  osc_connection = (AgsOscConnection *) g_object_new(AGS_TYPE_OSC_CONNECTION,
						     "osc-server", osc_server,
						     NULL);
  
  return(osc_connection);
}
