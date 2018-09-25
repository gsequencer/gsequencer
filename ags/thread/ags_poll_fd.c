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

#include <ags/thread/ags_poll_fd.h>

#include <stdlib.h>

void ags_poll_fd_class_init(AgsPollFdClass *poll_fd);
void ags_poll_fd_init(AgsPollFd *poll_fd);
void ags_poll_fd_finalize(GObject *gobject);

/**
 * SECTION:ags_poll_fd
 * @short_description: Perform operations in a thread safe context.
 * @title: AgsPollFd
 * @section_id: 
 * @include: ags/thread/ags_poll_fd.h
 *
 * #AgsPollFd object acts an interceptor in a thread safe context.
 */

enum{
  DISPATCH,
  LAST_SIGNAL,
};

static gpointer ags_poll_fd_parent_class = NULL;
static guint poll_fd_signals[LAST_SIGNAL];

static pthread_mutex_t ags_poll_fd_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_poll_fd_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_poll_fd = 0;

    static const GTypeInfo ags_poll_fd_info = {
      sizeof (AgsPollFdClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_poll_fd_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPollFd),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_poll_fd_init,
    };

    ags_type_poll_fd = g_type_register_static(G_TYPE_OBJECT,
					      "AgsPollFd",
					      &ags_poll_fd_info,
					      0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_poll_fd);
  }

  return g_define_type_id__volatile;
}

void
ags_poll_fd_class_init(AgsPollFdClass *poll_fd)
{
  GObjectClass *gobject;

  ags_poll_fd_parent_class = g_type_class_peek_parent(poll_fd);

  /* GObjectClass */
  gobject = (GObjectClass *) poll_fd;

  gobject->finalize = ags_poll_fd_finalize;

  /* AgsPollFdClass */
  poll_fd->dispatch = NULL;

  /**
   * AgsPollFd::dispatch:
   * @poll_fd: the #AgsPollFd
   *
   * The ::dispatch signal is emited during poll
   *
   * Since: 2.0.0
   */
  poll_fd_signals[DISPATCH] =
    g_signal_new("dispatch",
		 G_TYPE_FROM_CLASS (poll_fd),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsPollFdClass, dispatch),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_poll_fd_init(AgsPollFd *poll_fd)
{
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

#ifdef __linux__
  int err;
#endif
  
  poll_fd->flags = 0;

  /* thread mutex */
  poll_fd->obj_mutexattr =
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));

  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  err = pthread_mutexattr_setprotocol(attr,
				      PTHREAD_PRIO_INHERIT);

  if(err != 0){
    g_warning("no priority inheritance");
  }
#endif
  
  poll_fd->obj_mutex =
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  /*   */
  poll_fd->polling_thread = NULL;

  poll_fd->poll_fd = NULL;

  /*  */
  poll_fd->delay = 0.0;
  poll_fd->delay_counter = 0.0;
}

void
ags_poll_fd_finalize(GObject *gobject)
{
  /* empty */

  G_OBJECT_CLASS(ags_poll_fd_parent_class)->finalize(gobject);
}

/**
 * ags_poll_fd_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_poll_fd_get_class_mutex()
{
  return(&ags_poll_fd_class_mutex);
}

/**
 * ags_poll_fd_dispatch:
 * @poll_fd: the #AgsPollFd
 *
 * Dispatch IO.
 *
 * Since: 2.0.0
 */
void
ags_poll_fd_dispatch(AgsPollFd *poll_fd)
{
  pthread_mutex_t *mutex;

  pthread_mutex_lock(ags_poll_fd_get_class_mutex());
  
  mutex = poll_fd->obj_mutex;
  
  pthread_mutex_unlock(ags_poll_fd_get_class_mutex());

  pthread_mutex_lock(mutex);
  
  g_return_if_fail(AGS_IS_POLL_FD(poll_fd));

  g_object_ref(G_OBJECT(poll_fd));
  g_signal_emit(G_OBJECT(poll_fd),
		poll_fd_signals[DISPATCH], 0);
  g_object_unref(G_OBJECT(poll_fd));

  pthread_mutex_unlock(mutex);
}

/**
 * ags_poll_fd_new:
 *
 * Create a new instance of #AgsPollFd
 *
 * Returns: the new #AgsPollFd
 *
 * Since: 2.0.0
 */
AgsPollFd*
ags_poll_fd_new()
{
  AgsPollFd *poll_fd;

  poll_fd = (AgsPollFd *) g_object_new(AGS_TYPE_POLL_FD,
				       NULL);

  return(poll_fd);
}
