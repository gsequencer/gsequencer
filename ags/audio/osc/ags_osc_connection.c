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

#include <ags/audio/osc/ags_osc_connection.h>

#include <ags/libags.h>

#include <ags/audio/osc/ags_osc_server.h>
#include <ags/audio/osc/ags_osc_response.h>
#include <ags/audio/osc/ags_osc_util.h>
#include <ags/audio/osc/ags_osc_buffer_util.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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

unsigned char* ags_osc_connection_real_read_bytes(AgsOscConnection *osc_connection,
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

static pthread_mutex_t ags_osc_connection_class_mutex = PTHREAD_MUTEX_INITIALIZER;

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
   * Since: 2.1.0
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
   * AgsOscConnection:ip6:
   *
   * The IPv6 address as string of the server connection.
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
   * Since: 2.1.0
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
   * Since: 2.1.0
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
   * Since: 2.1.0
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
  
  /* osc connection mutex */
  osc_connection->obj_mutexattr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(osc_connection->obj_mutexattr);
  pthread_mutexattr_settype(osc_connection->obj_mutexattr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(osc_connection->obj_mutexattr,
				PTHREAD_PRIO_INHERIT);
#endif

  osc_connection->obj_mutex =  (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(osc_connection->obj_mutex,
		     osc_connection->obj_mutexattr);

  osc_connection->ip4 = NULL;
  osc_connection->ip6 = NULL;

  osc_connection->start_time = (struct timeval *) malloc(sizeof(struct timespec));

  osc_connection->start_time->tv_sec = 0;
  osc_connection->start_time->tv_nsec = 0;

  osc_connection->buffer = (unsigned char *) malloc(AGS_OSC_CONNECTION_CHUNK_SIZE * sizeof(unsigned char));
  osc_connection->allocated_buffer_size = AGS_OSC_CONNECTION_CHUNK_SIZE;

  osc_connection->read_count = 0;
  
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

  pthread_mutex_t *osc_connection_mutex;

  osc_connection = AGS_OSC_CONNECTION(gobject);

  /* get osc connection mutex */
  pthread_mutex_lock(ags_osc_connection_get_class_mutex());
  
  osc_connection_mutex = osc_connection->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_connection_get_class_mutex());
  
  switch(prop_id){
  case PROP_OSC_SERVER:
    {
      GObject *osc_server;

      osc_server = g_value_get_object(value);

      pthread_mutex_lock(osc_connection_mutex);

      if(osc_connection->osc_server == osc_server){
	pthread_mutex_unlock(osc_connection_mutex);

	return;
      }

      if(osc_connection->osc_server != NULL){
	g_object_unref(osc_connection->osc_server);
      }
      
      if(osc_server != NULL){
	g_object_ref(osc_server);
      }
      
      osc_connection->osc_server = osc_server;

      pthread_mutex_unlock(osc_connection_mutex);
    }
    break;
  case PROP_IP4:
    {
      gchar *ip4;

      ip4 = g_value_get_string(value);

      pthread_mutex_lock(osc_connection_mutex);
      
      if(osc_connection->ip4 == ip4){
	pthread_mutex_unlock(osc_connection_mutex);
	
	return;
      }

      g_free(osc_connection->ip4);

      osc_connection->ip4 = g_strdup(ip4);

      pthread_mutex_unlock(osc_connection_mutex);
    }
    break;
  case PROP_IP6:
    {
      gchar *ip6;

      ip6 = g_value_get_string(value);

      pthread_mutex_lock(osc_connection_mutex);
      
      if(osc_connection->ip6 == ip6){
	pthread_mutex_unlock(osc_connection_mutex);
	
	return;
      }

      g_free(osc_connection->ip6);

      osc_connection->ip6 = g_strdup(ip6);

      pthread_mutex_unlock(osc_connection_mutex);
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

  pthread_mutex_t *osc_connection_mutex;

  osc_connection = AGS_OSC_CONNECTION(gobject);

  /* get osc connection mutex */
  pthread_mutex_lock(ags_osc_connection_get_class_mutex());
  
  osc_connection_mutex = osc_connection->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_connection_get_class_mutex());
  
  switch(prop_id){
  case PROP_OSC_SERVER:
    {
      pthread_mutex_lock(osc_connection_mutex);

      g_value_set_object(value, osc_connection->osc_server);

      pthread_mutex_unlock(osc_connection_mutex);
    }
    break;
  case PROP_IP4:
    {
      pthread_mutex_lock(osc_connection_mutex);
      
      g_value_set_string(value,
			 osc_connection->ip4);
      
      pthread_mutex_unlock(osc_connection_mutex);
    }
    break;
  case PROP_IP6:
    {
      pthread_mutex_lock(osc_connection_mutex);
      
      g_value_set_string(value,
			 osc_connection->ip6);

      pthread_mutex_unlock(osc_connection_mutex);
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

  pthread_mutex_destroy(osc_connection->obj_mutex);
  free(osc_connection->obj_mutex);

  pthread_mutexattr_destroy(osc_connection->obj_mutexattr);
  free(osc_connection->obj_mutexattr);

  if(osc_connection->osc_server != NULL){
    g_object_unref(osc_connection->osc_server);
  }

  g_free(osc_connection->ip4);
  g_free(osc_connection->ip6);

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
 * ags_osc_connection_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.1.0
 */
pthread_mutex_t*
ags_osc_connection_get_class_mutex()
{
  return(&ags_osc_connection_class_mutex);
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
 * Since: 2.1.0
 */
gboolean
ags_osc_connection_test_flags(AgsOscConnection *osc_connection, guint flags)
{
  gboolean retval;  
  
  pthread_mutex_t *osc_connection_mutex;

  if(!AGS_IS_OSC_CONNECTION(osc_connection)){
    return(FALSE);
  }

  /* get osc_connection mutex */
  pthread_mutex_lock(ags_osc_connection_get_class_mutex());
  
  osc_connection_mutex = osc_connection->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_connection_get_class_mutex());

  /* test */
  pthread_mutex_lock(osc_connection_mutex);

  retval = (flags & (osc_connection->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(osc_connection_mutex);

  return(retval);
}

/**
 * ags_osc_connection_set_flags:
 * @osc_connection: the #AgsOscConnection
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 2.1.0
 */
void
ags_osc_connection_set_flags(AgsOscConnection *osc_connection, guint flags)
{
  pthread_mutex_t *osc_connection_mutex;

  if(!AGS_IS_OSC_CONNECTION(osc_connection)){
    return;
  }

  /* get osc_connection mutex */
  pthread_mutex_lock(ags_osc_connection_get_class_mutex());
  
  osc_connection_mutex = osc_connection->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_connection_get_class_mutex());

  /* set flags */
  pthread_mutex_lock(osc_connection_mutex);

  osc_connection->flags |= flags;

  pthread_mutex_unlock(osc_connection_mutex);
}

/**
 * ags_osc_connection_unset_flags:
 * @osc_connection: the #AgsOscConnection
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 2.1.0
 */
void
ags_osc_connection_unset_flags(AgsOscConnection *osc_connection, guint flags)
{
  pthread_mutex_t *osc_connection_mutex;

  if(!AGS_IS_OSC_CONNECTION(osc_connection)){
    return;
  }

  /* get osc_connection mutex */
  pthread_mutex_lock(ags_osc_connection_get_class_mutex());
  
  osc_connection_mutex = osc_connection->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_connection_get_class_mutex());

  /* set flags */
  pthread_mutex_lock(osc_connection_mutex);

  osc_connection->flags &= (~flags);

  pthread_mutex_unlock(osc_connection_mutex);
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
 * Since: 2.1.10
 */
gboolean
ags_osc_connection_timeout_expired(struct timespec *start_time,
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

unsigned char*
ags_osc_connection_real_read_bytes(AgsOscConnection *osc_connection,
				   guint *data_length)
{
  unsigned char *buffer;
  unsigned char data[256];
  
  guint allocated_buffer_size;
  guint read_count;
  guint retval;
  gint64 start_data, end_data;
  int fd;
  guint i, j;
  gboolean has_valid_data;
  gboolean success;

#ifdef __APPLE__
  clock_serv_t cclock;
  mach_timespec_t mts;
#endif

  pthread_mutex_t *osc_connection_mutex;

  /* get osc_connection mutex */
  pthread_mutex_lock(ags_osc_connection_get_class_mutex());
  
  osc_connection_mutex = osc_connection->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_connection_get_class_mutex());

  /* get fd */
  pthread_mutex_lock(osc_connection_mutex);

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
  
  pthread_mutex_unlock(osc_connection_mutex);

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
  
  while(!ags_osc_connection_timeout_expired(osc_connection->start_time,
					    osc_connection->timeout_delay)){
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
	pthread_mutex_lock(osc_connection_mutex);

	osc_connection->fd = -1;
	
	pthread_mutex_unlock(osc_connection_mutex);
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

    ags_osc_connection_read_bytes_REPEAT_0:
      
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
	     !ags_osc_connection_timeout_expired(osc_connection->start_time,
						 osc_connection->timeout_delay)){
	    goto ags_osc_connection_read_bytes_REPEAT_0;
	  }else{
	    break;
	  }
	}
	
	pthread_mutex_lock(osc_connection_mutex);

	memcpy(osc_connection->buffer, data + start_data, (end_data - start_data) * sizeof(unsigned char));
	  
	pthread_mutex_unlock(osc_connection_mutex);

	if(data_length != NULL){
	  *data_length = end_data - start_data;
	}
	  
	return(osc_connection->buffer);
      }else{
	read_count += (retval - start_data);
	  
	pthread_mutex_lock(osc_connection_mutex);

	memcpy(osc_connection->buffer, data + start_data, (retval - start_data) * sizeof(unsigned char));
	  
	pthread_mutex_unlock(osc_connection_mutex);

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
	  if(read_count + i >= AGS_OSC_CONNECTION_CHUNK_SIZE){
	    if(data_length != NULL){
	      *data_length = 0;
	    }
	    
	    return(NULL);
	  }
	  
	  pthread_mutex_lock(osc_connection_mutex);

	  memcpy(osc_connection->buffer + read_count, data, (i) * sizeof(unsigned char));
	
	  pthread_mutex_unlock(osc_connection_mutex);

	  read_count += i;
	  
	  if(data_length != NULL){
	    *data_length = read_count;
	  }

	  return(osc_connection->buffer);
	}else{
	  if(read_count + retval >= AGS_OSC_CONNECTION_CHUNK_SIZE){
	    if(data_length != NULL){
	      *data_length = 0;
	    }
	    
	    return(NULL);
	  }
	  
	  pthread_mutex_lock(osc_connection_mutex);
	
	  memcpy(osc_connection->buffer + read_count, data, (retval) * sizeof(unsigned char));

	  pthread_mutex_unlock(osc_connection_mutex);

	  read_count += retval;

	  continue;
	}
      }
    }
  }

  osc_connection->read_count = read_count;
  
  if(data_length != NULL){
    *data_length = 0;
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
 * Since: 2.1.0
 */
unsigned char*
ags_osc_connection_read_bytes(AgsOscConnection *osc_connection,
			      guint *data_length)
{
  unsigned char *buffer;
  
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
  unsigned char *slip_buffer;
  
  int fd;
  guint slip_buffer_length;
  gint64 num_write;

  pthread_mutex_t *osc_connection_mutex;
  pthread_mutex_t *osc_response_mutex;

  /* get osc connection mutex */
  pthread_mutex_lock(ags_osc_connection_get_class_mutex());
  
  osc_connection_mutex = osc_connection->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_connection_get_class_mutex());

  /* get fd */
  pthread_mutex_lock(osc_connection_mutex);

  fd = osc_connection->fd;

  pthread_mutex_unlock(osc_connection_mutex);
  
  if(fd == -1){
    return(0);
  }
  
  /* get osc response mutex */
  pthread_mutex_lock(ags_osc_response_get_class_mutex());
  
  osc_response_mutex = AGS_OSC_RESPONSE(osc_response)->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_response_get_class_mutex());

  /* write */
  pthread_mutex_lock(osc_response_mutex);

  slip_buffer = ags_osc_util_slip_encode(AGS_OSC_RESPONSE(osc_response)->packet,
					 AGS_OSC_RESPONSE(osc_response)->packet_size,
					 &slip_buffer_length);

  pthread_mutex_unlock(osc_response_mutex);
  
  num_write = write(fd, slip_buffer, slip_buffer_length * sizeof(unsigned char));

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
 * Since: 2.1.0
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
  pthread_mutex_t *osc_connection_mutex;

  /* get osc_connection mutex */
  pthread_mutex_lock(ags_osc_connection_get_class_mutex());
  
  osc_connection_mutex = osc_connection->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_connection_get_class_mutex());

  /* set flags */
  pthread_mutex_lock(osc_connection_mutex);

  close(osc_connection->fd);
  osc_connection->fd = -1;
  
  pthread_mutex_unlock(osc_connection_mutex);

  ags_osc_connection_unset_flags(osc_connection,
				 AGS_OSC_CONNECTION_ACTIVE);
}

/**
 * ags_osc_connection_close:
 * @osc_connection: the #AgsOscConnection
 * 
 * Close @osc_connection.
 * 
 * Since: 2.1.0
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
 * Since: 2.1.0
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
