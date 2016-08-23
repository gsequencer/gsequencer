/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/thread/ags_polling_thread.h>

#include <ags/object/ags_connectable.h>

#include <ags/thread/ags_poll_fd.h>

void ags_polling_thread_class_init(AgsPollingThreadClass *polling_thread);
void ags_polling_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_polling_thread_init(AgsPollingThread *polling_thread);
void ags_polling_thread_connect(AgsConnectable *connectable);
void ags_polling_thread_disconnect(AgsConnectable *connectable);
void ags_polling_thread_finalize(GObject *gobject);

void ags_polling_thread_start(AgsThread *thread);
void ags_polling_thread_run(AgsThread *thread);
void ags_polling_thread_stop(AgsThread *thread);

/**
 * SECTION:ags_polling_thread
 * @short_description: polling thread
 * @title: AgsPollingThread
 * @section_id:
 * @include: ags/thread/ags_polling_thread.h
 *
 * The #AgsPollingThread polls on the given file descriptors.
 */

static gpointer ags_polling_thread_parent_class = NULL;

GType
ags_polling_thread_get_type()
{
  static GType ags_type_polling_thread = 0;

  if(!ags_type_polling_thread){
    static const GTypeInfo ags_polling_thread_info = {
      sizeof (AgsPollingThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_polling_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPollingThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_polling_thread_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_polling_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_polling_thread = g_type_register_static(AGS_TYPE_THREAD,
						     "AgsPollingThread\0",
						     &ags_polling_thread_info,
						     0);
    
    g_type_add_interface_static(ags_type_polling_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_polling_thread);
}

void
ags_polling_thread_class_init(AgsPollingThreadClass *polling_thread)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;

  ags_polling_thread_parent_class = g_type_class_peek_parent(polling_thread);

  /* GObject */
  gobject = (GObjectClass *) polling_thread;

  gobject->finalize = ags_polling_thread_finalize;

  /* AgsThread */
  thread = (AgsThreadClass *) polling_thread;

  thread->start = ags_polling_thread_start;
  thread->run = ags_polling_thread_run;
  thread->stop = ags_polling_thread_stop;
}

void
ags_polling_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_polling_thread_connect;
  connectable->disconnect = ags_polling_thread_disconnect;
}

void
ags_polling_thread_init(AgsPollingThread *polling_thread)
{
  AgsThread *thread;

  thread = polling_thread;

  thread->freq = AGS_POLLING_THREAD_DEFAULT_JIFFIE;
  
  polling_thread->flags = 0;

  polling_thread->fd_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(polling_thread->fd_mutex,
		     NULL);
  
  polling_thread->fds = NULL;

  polling_thread->poll_fd = NULL;
}

void
ags_polling_thread_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_polling_thread_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_polling_thread_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_polling_thread_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_polling_thread_start(AgsThread *thread)
{
  AgsPollingThread *polling_thread;

  /*  */
  polling_thread = AGS_POLLING_THREAD(thread);

  /*  */
  if((AGS_THREAD_SINGLE_LOOP & (g_atomic_int_get(&(thread->flags)))) == 0){
    AGS_THREAD_CLASS(ags_polling_thread_parent_class)->start(thread);
  }
}

void
ags_polling_thread_run(AgsThread *thread)
{
  AgsPollingThread *polling_thread;

  GList *list;
  
  struct timespec timeout;

  sigset_t sigmask;
  gint position;
  int ret;

  polling_thread = AGS_POLLING_THREAD(thread);

  /* real-time setup */
  if((AGS_THREAD_RT_SETUP & (g_atomic_int_get(&(thread->flags)))) == 0){
    struct sched_param param;
    
    /* Declare ourself as a real time task */
    param.sched_priority = AGS_POLLING_THREAD_RT_PRIORITY;
      
    if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
      perror("sched_setscheduler failed\0");
    }

    g_atomic_int_or(&(thread->flags),
		    AGS_THREAD_RT_SETUP);
  }

  sigemptyset(&sigmask);

  timeout.tv_sec = 0;
  timeout.tv_nsec = (NSEC_PER_SEC / thread->freq - 40000);
  
  pthread_mutex_lock(polling_thread->fd_mutex);

  if((AGS_POLLING_THREAD_OMIT & (polling_thread->flags)) == 0){
    list = polling_thread->poll_fd;

    /* pre flag */
    while(list != NULL){
      position = ags_polling_thread_fd_position(polling_thread,
						AGS_POLL_FD(list->data)->fd);

      if(AGS_POLL_FD(list->data)->poll_fd != NULL){
	AGS_POLL_FD(list->data)->poll_fd->events = polling_thread->fds[position].events;
      }
      
      list = list->next;
    }  

    /* poll */	
    if(polling_thread->fds != NULL){
      ppoll(polling_thread->fds,
	    g_list_length(polling_thread->poll_fd),
	    &timeout,
	    &sigmask);
    }

    /* post flag */
    list = polling_thread->poll_fd;

    while(list != NULL){
      position = ags_polling_thread_fd_position(polling_thread,
						AGS_POLL_FD(list->data)->fd);

      if((POLLIN & (polling_thread->fds[position].revents)) != 0){
	AGS_POLL_FD(list->data)->flags |= AGS_POLL_FD_INPUT;
      }

      if((POLLPRI & (polling_thread->fds[position].revents)) != 0){
	AGS_POLL_FD(list->data)->flags |= AGS_POLL_FD_PRIORITY_INPUT;
      }

      if((POLLOUT & (polling_thread->fds[position].revents)) != 0){
	AGS_POLL_FD(list->data)->flags |= AGS_POLL_FD_OUTPUT;
      }

      if((POLLHUP & (polling_thread->fds[position].revents)) != 0){
	AGS_POLL_FD(list->data)->flags |= AGS_POLL_FD_HANG_UP;
      }

      /* do legacy */
      if(AGS_POLL_FD(list->data)->poll_fd != NULL){
	AGS_POLL_FD(list->data)->poll_fd->revents = polling_thread->fds[position].revents;
      }
      
      ags_poll_fd_dispatch(list->data);
      AGS_POLL_FD(list->data)->flags &= (~(AGS_POLL_FD_INPUT |
					   AGS_POLL_FD_OUTPUT |
					   AGS_POLL_FD_HANG_UP));
      AGS_POLL_FD(list->data)->poll_fd->revents = 0;
      
      list = list->next;
    }  
  }
  
  pthread_mutex_unlock(polling_thread->fd_mutex);
}

void
ags_polling_thread_stop(AgsThread *thread)
{
  /*  */
  AGS_THREAD_CLASS(ags_polling_thread_parent_class)->stop(thread);  
}

gint
ags_polling_thread_fd_position(AgsPollingThread *polling_thread,
			       int fd)
{
  gint i;

  for(i = 0; i < g_list_length(polling_thread->poll_fd); i++){
    if(fd == polling_thread->fds[i].fd){
      return(i);
    }
  }

  return(-1);
}

void
ags_polling_thread_add_poll_fd(AgsPollingThread *polling_thread,
			       GObject *gobject)
{
  guint length;
  gint nth;
  
  if(polling_thread == NULL ||
     gobject == NULL){
    return;
  }

  pthread_mutex_lock(polling_thread->fd_mutex);


  nth = g_list_position(polling_thread->poll_fd,
			gobject);

  if(nth >= 0){
    pthread_mutex_unlock(polling_thread->fd_mutex);
    
    return;
  }

  AGS_POLL_FD(gobject)->polling_thread = polling_thread;
  
  length = g_list_length(polling_thread->poll_fd);

  if(length == 0){
    polling_thread->fds = (struct pollfd *) malloc(sizeof(struct pollfd));
  }else{
    polling_thread->fds = (struct pollfd *) realloc(polling_thread->fds,
						    (length + 1) * sizeof(struct pollfd));
  }

  polling_thread->fds[length].fd = AGS_POLL_FD(gobject)->fd;
  polling_thread->fds[length].events = 0;
  polling_thread->fds[length].revents = 0;
  
  /* add to list */
  g_object_ref(gobject);
  polling_thread->poll_fd = g_list_prepend(polling_thread->poll_fd,
					   gobject);

  pthread_mutex_unlock(polling_thread->fd_mutex);
}

void
ags_polling_thread_remove_poll_fd(AgsPollingThread *polling_thread,
				  GObject *gobject)
{
  struct pollfd *fds;

  guint length;
  gint nth;
  
  if(polling_thread == NULL ||
     gobject == NULL){
    return;
  }

  pthread_mutex_lock(polling_thread->fd_mutex);
  
  /* find fd */
  nth = g_list_position(polling_thread->poll_fd,
			gobject);

  if(nth < 0){
    pthread_mutex_unlock(polling_thread->fd_mutex);
    
    return;
  }
  
  AGS_POLL_FD(gobject)->polling_thread = NULL;

  /* realloc array */
  length = g_list_length(polling_thread->poll_fd);
  
  fds = (struct pollfd *) malloc((length - 1) * sizeof(struct pollfd));

  if(nth != 0){
    memcpy(fds,
	   polling_thread->fds,
	   nth * sizeof(struct pollfd));
  }

  if(nth + 1 < length){
    memcpy(&(fds[nth]),
	   &(polling_thread->fds[nth + 1]),
	   (length - nth - 1) * sizeof(struct pollfd));
  }

  free(polling_thread->fds);
  polling_thread->fds = fds;

  /* remove from list */
  polling_thread->poll_fd = g_list_remove(polling_thread->poll_fd,
					  gobject);
  g_object_unref(gobject);

  pthread_mutex_unlock(polling_thread->fd_mutex);
}

/**
 * ags_polling_thread_new:
 *
 * Create a new #AgsPollingThread.
 *
 * Returns: the new #AgsPollingThread
 *
 * Since: 0.7.50
 */
AgsPollingThread*
ags_polling_thread_new()
{
  AgsPollingThread *polling_thread;
  
  polling_thread = (AgsPollingThread *) g_object_new(AGS_TYPE_POLLING_THREAD,
						     NULL);

  return(polling_thread);
}
