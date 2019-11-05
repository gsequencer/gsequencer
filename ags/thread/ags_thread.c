/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/thread/ags_thread-posix.h>

#include <ags/object/ags_main_loop.h>
#include <ags/object/ags_config.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_async_queue.h>
#include <ags/object/ags_marshal.h>

#include <ags/thread/ags_returnable_thread.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#ifdef __APPLE__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#include <ags/i18n.h>

#include <errno.h>

void ags_thread_class_init(AgsThreadClass *thread);
void ags_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_thread_init(AgsThread *thread);
void ags_thread_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec);
void ags_thread_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec);
void ags_thread_finalize(GObject *gobject);

AgsUUID* ags_thread_get_uuid(AgsConnectable *connectable);
gboolean ags_thread_has_resource(AgsConnectable *connectable);
gboolean ags_thread_is_ready(AgsConnectable *connectable);
void ags_thread_add_to_registry(AgsConnectable *connectable);
void ags_thread_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_thread_list_resource(AgsConnectable *connectable);
xmlNode* ags_thread_xml_compose(AgsConnectable *connectable);
void ags_thread_xml_parse(AgsConnectable *connectable,
			  xmlNode *node);
gboolean ags_thread_is_connected(AgsConnectable *connectable);
void ags_thread_connect(AgsConnectable *connectable);
void ags_thread_disconnect(AgsConnectable *connectable);

void ags_thread_set_sync_all_reset(AgsThread *thread);
void ags_thread_set_sync_all_recursive(AgsThread *thread, guint tic);

gboolean ags_thread_children_is_locked_recursive(AgsThread *thread);

gboolean ags_thread_is_tree_ready_current_tic(AgsThread *current,
					      guint tic);
gboolean ags_thread_is_tree_ready_recursive(AgsThread *current,
					    guint tic);

AgsThread* ags_thread_next_children_locked_recursive(AgsThread *thread);

void ags_thread_lock_children_recursive(AgsThread *child);  

void ags_thread_unlock_children_recursive(AgsThread *thread, AgsThread *child);

void ags_thread_wait_children_recursive(AgsThread *child);

void* ags_thread_loop(void *ptr);

guint ags_thread_real_clock(AgsThread *thread);

void ags_thread_real_start(AgsThread *thread);
void ags_thread_real_stop(AgsThread *thread);

static void ags_thread_self_create();

/**
 * SECTION:ags_thread-posix
 * @short_description: threads
 * @title: AgsThread
 * @section_id:
 * @include: ags/thread/ags_thread.h
 *
 * The #AgsThread base class. It supports organizing them within a tree,
 * perform syncing and frequencies.
 */

enum{
  PROP_0,
  PROP_FREQUENCY,
  PROP_MAX_PRECISION,
};

enum{
  CLOCK,
  START,
  RUN,
  STOP,
  LAST_SIGNAL,
};

static gpointer ags_thread_parent_class = NULL;
static guint thread_signals[LAST_SIGNAL];

static GPrivate ags_thread_key;

static gboolean ags_thread_global_use_sync_counter = TRUE;

GType
ags_thread_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_thread = 0;

    const GTypeInfo ags_thread_info = {
      sizeof (AgsThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_thread_init,
    };

    const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_thread = g_type_register_static(G_TYPE_OBJECT,
					     "AgsThread",
					     &ags_thread_info,
					     0);
        
    g_type_add_interface_static(ags_type_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_thread);
  }

  return g_define_type_id__volatile;
}

void
ags_thread_class_init(AgsThreadClass *thread)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_thread_parent_class = g_type_class_peek_parent(thread);
  
  /* GObject */
  gobject = (GObjectClass *) thread;
  
  gobject->set_property = ags_thread_set_property;
  gobject->get_property = ags_thread_get_property;

  gobject->finalize = ags_thread_finalize;

  /* properties */
  /**
   * AgsThread:frequency:
   *
   * The frequency to run at in Hz.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("frequency",
				   i18n_pspec("frequency as JIFFIE"),
				   i18n_pspec("frequency as JIFFIE"),
				   0.01,
				   AGS_THREAD_DEFAULT_MAX_PRECISION,
				   AGS_THREAD_DEFAULT_JIFFIE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FREQUENCY,
				  param_spec);
  
  /**
   * AgsThread:max-precision:
   *
   * The max-frequency to run at in Hz.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("max-precision",
				   i18n_pspec("max precision as JIFFIE"),
				   i18n_pspec("The max precision as JIFFIE"),
				   0.01,
				   AGS_THREAD_MAX_PRECISION,
				   AGS_THREAD_DEFAULT_MAX_PRECISION,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MAX_PRECISION,
				  param_spec);

  /* AgsThread */
  thread->clock = ags_thread_real_clock;
  
  thread->start = ags_thread_real_start;
  thread->run = NULL;
  thread->stop = ags_thread_real_stop;
  
  /* signals */
  /**
   * AgsThread::clock:
   * @thread: the #AgsThread
   *
   * The ::clock() signal is invoked every thread tic.
   *
   * Returns: the number of cycles to perform
   * 
   * Since: 3.0.0
   */
  thread_signals[CLOCK] =
    g_signal_new("clock",
		 G_TYPE_FROM_CLASS (thread),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsThreadClass, clock),
		 NULL, NULL,
		 ags_cclosure_marshal_UINT__VOID,
		 G_TYPE_UINT, 0);


  /**
   * AgsThread::start:
   * @thread: the #AgsThread
   *
   * The ::start() signal is invoked as thread started.
   * 
   * Since: 3.0.0
   */
  thread_signals[START] =
    g_signal_new("start",
		 G_TYPE_FROM_CLASS (thread),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsThreadClass, start),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsThread::run:
   * @thread: the #AgsThread
   *
   * The ::run() signal is invoked during run loop.
   * 
   * Since: 3.0.0
   */
  thread_signals[RUN] =
    g_signal_new("run",
		 G_TYPE_FROM_CLASS (thread),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsThreadClass, run),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsThread::stop:
   * @thread: the #AgsThread
   *
   * The ::stop() signal is invoked as @thread stopped.
   * 
   * Since: 3.0.0
   */
  thread_signals[STOP] =
    g_signal_new("stop",
		 G_TYPE_FROM_CLASS (thread),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsThreadClass, stop),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_thread_get_uuid;
  connectable->has_resource = ags_thread_has_resource;

  connectable->is_ready = ags_thread_is_ready;
  connectable->add_to_registry = ags_thread_add_to_registry;
  connectable->remove_from_registry = ags_thread_remove_from_registry;

  connectable->list_resource = ags_thread_list_resource;
  connectable->xml_compose = ags_thread_xml_compose;
  connectable->xml_parse = ags_thread_xml_parse;

  connectable->is_connected = ags_thread_is_connected;
  connectable->connect = ags_thread_connect;
  connectable->disconnect = ags_thread_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_thread_init(AgsThread *thread)
{
  AgsConfig *config;
  
  gchar *str;

  int err;

  config = ags_config_get_instance();
  
  /* the obj mutex */
  g_rec_mutex_init(&(thread->obj_mutex));

  /* flags and status flags */
  thread->flags = 0;
  
  g_atomic_int_set(&(thread->status_flags),
		   0);

  /* uuid */
  thread->uuid = ags_uuid_alloc();
  ags_uuid_generate(thread->uuid);
    
  /* clock */
  thread->delay = 0;

  thread->tic_delay = 0;

  thread->current_tic = 0;

  thread->cycle_iteration = 0;

  thread->time_late = 0;

  thread->computing_time = (struct timespec *) malloc(sizeof(struct timespec));
  thread->computing_time->tv_sec = 0;
  thread->computing_time->tv_nsec = 0;
    
  /* the wait mutex and cond */
  g_mutex_init(&(thread->wait_mutex));

  g_cond_init(&(thread->wait_cond));
  
  /* thread, tic mutex and cond */
  thread->thread = NULL;

  thread->freq = AGS_THREAD_DEFAULT_JIFFIE;

  thread->max_precision = AGS_THREAD_DEFAULT_MAX_PRECISION;

  /* max precision */
  str = ags_config_get_value(config,
			     AGS_CONFIG_THREAD,
			     "max-precision");
  
  if(str != NULL){
    thread->max_precision = g_ascii_strtod(str,
					   NULL);

    g_free(str);
  }
  
  g_mutex_init(&(thread->tic_mutex));

  g_cond_init(&(thread->tic_cond));

  /* start notify */
  thread->start_queue = NULL;
  
  g_mutex_init(&(thread->start_mutex));

  g_cond_init(&(thread->start_cond));

  g_atomic_int_set(&(thread->start_wait),
		   FALSE);
  g_atomic_int_set(&(thread->start_done),
		   FALSE);

  /* tree */
  thread->parent = NULL;
  
  thread->next = NULL;
  thread->prev = NULL;

  thread->children = NULL;

  thread->data = NULL;
}

void
ags_thread_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsThread *thread;

  GRecMutex *thread_mutex;

  thread = AGS_THREAD(gobject);

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  switch(prop_id){
  case PROP_FREQUENCY:
    {
      gdouble freq;

      g_rec_mutex_lock(thread_mutex);

      freq = g_value_get_double(value);

      if(freq == thread->freq){
	g_rec_mutex_unlock(thread_mutex);

	return;
      }

      thread->freq = freq;
      thread->delay = (guint) ceil((AGS_THREAD_HERTZ_JIFFIE / thread->freq) / (AGS_THREAD_HERTZ_JIFFIE / thread->max_precision));
      thread->tic_delay = 0;

      if(ags_thread_test_flags(thread, AGS_THREAD_INTERMEDIATE_POST_SYNC)){
	thread->tic_delay = thread->delay;
      }else if(ags_thread_test_flags(thread, AGS_THREAD_INTERMEDIATE_PRE_SYNC)){
	thread->tic_delay = 1;
      }else{
	thread->tic_delay = 0;
      }

      g_rec_mutex_unlock(thread_mutex);
    }
    break;
  case PROP_MAX_PRECISION:
    {
      gdouble max_precision;
      gdouble old_max_precision;

      g_rec_mutex_lock(thread_mutex);

      max_precision = g_value_get_double(value);

      if(max_precision == thread->max_precision){
	g_rec_mutex_unlock(thread_mutex);

	return;
      }

      old_max_precision = thread->max_precision;
      
      thread->max_precision = max_precision;

      thread->freq = thread->freq / old_max_precision * max_precision;
      thread->delay = (guint) ceil((AGS_THREAD_HERTZ_JIFFIE / thread->freq) / (AGS_THREAD_HERTZ_JIFFIE / thread->max_precision));
      thread->tic_delay = 0;

      if(ags_thread_test_flags(thread, AGS_THREAD_INTERMEDIATE_POST_SYNC)){
	thread->tic_delay = thread->delay;
      }else if(ags_thread_test_flags(thread, AGS_THREAD_INTERMEDIATE_PRE_SYNC)){
	thread->tic_delay = 1;
      }else{
	thread->tic_delay = 0;
      }

      g_rec_mutex_unlock(thread_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_thread_get_property(GObject *gobject,
			guint prop_id,
			GValue *value,
			GParamSpec *param_spec)
{
  AgsThread *thread;

  GRecMutex *thread_mutex;

  thread = AGS_THREAD(gobject);

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  switch(prop_id){
  case PROP_FREQUENCY:
    {
      g_rec_mutex_lock(thread_mutex);

      g_value_set_double(value, thread->freq);

      g_rec_mutex_unlock(thread_mutex);
    }
    break;
  case PROP_MAX_PRECISION:
    {
      g_rec_mutex_lock(thread_mutex);

      g_value_set_double(value, thread->max_precision);

      g_rec_mutex_unlock(thread_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_thread_finalize(GObject *gobject)
{
  AgsThread *thread, *parent;

  GThread *thread_ptr;
  
  gboolean running;
  gboolean do_exit;
  
  thread = AGS_THREAD(gobject);

  if(thread == ags_thread_self()){
    do_exit = TRUE;
  }else{
    do_exit = FALSE;
  }
  
  thread_ptr = thread->thread;

  running = ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_RUNNING);
  
#ifdef AGS_DEBUG
  g_message("fin %s", G_OBJECT_TYPE_NAME(gobject));
#endif

  //FIXME:JK: may dead-lock
  g_list_free(thread->start_queue);
  
  if((parent = thread->parent) != NULL){
    g_rec_mutex_lock(AGS_THREAD_GET_OBJ_MUTEX(parent));

    parent->start_queue = g_list_remove(parent->start_queue,
					thread);
    
    g_rec_mutex_unlock(AGS_THREAD_GET_OBJ_MUTEX(parent));
    
    ags_thread_remove_child(parent,
			    thread);
  }

  /* UUID */
  ags_uuid_free(thread->uuid);
  
  /* computing time */
  if(thread->computing_time != NULL){
    free(thread->computing_time);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_thread_parent_class)->finalize(gobject);

  if(do_exit){
    g_thread_exit(NULL);
  }
}

AgsUUID*
ags_thread_get_uuid(AgsConnectable *connectable)
{
  AgsThread *thread;
  
  AgsUUID *ptr;

  GRecMutex *thread_mutex;

  thread = AGS_THREAD(connectable);

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  /* get UUID */
  g_rec_mutex_lock(thread_mutex);

  ptr = thread->uuid;

  g_rec_mutex_unlock(thread_mutex);
  
  return(ptr);
}

gboolean
ags_thread_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_thread_is_ready(AgsConnectable *connectable)
{
  AgsThread *thread;
  
  gboolean is_ready;

  thread = AGS_THREAD(connectable);

  /* check is added */
  is_ready = ags_thread_test_flags(thread, AGS_THREAD_ADDED_TO_REGISTRY);
  
  return(is_ready);
}

void
ags_thread_add_to_registry(AgsConnectable *connectable)
{
  AgsThread *thread;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  thread = AGS_THREAD(connectable);

  ags_thread_set_flags(thread, AGS_THREAD_ADDED_TO_REGISTRY);
}

void
ags_thread_remove_from_registry(AgsConnectable *connectable)
{
  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  //TODO:JK: implement me
}

xmlNode*
ags_thread_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_thread_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_thread_xml_parse(AgsConnectable *connectable,
		    xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_thread_is_connected(AgsConnectable *connectable)
{
  AgsThread *thread;
  
  gboolean is_connected;

  thread = AGS_THREAD(connectable);

  /* check is connected */
  is_connected = ags_thread_test_flags(thread, AGS_THREAD_CONNECTED);
  
  return(is_connected);
}

void
ags_thread_connect(AgsConnectable *connectable)
{
  AgsThread *thread;
  AgsThread *children, *current_child, *next_child;

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  thread = AGS_THREAD(connectable);

  ags_thread_set_flags(thread, AGS_THREAD_CONNECTED);

  /* recursive connect */
  children = ags_thread_children(thread);

  if(children != NULL){
    current_child = children;
    g_object_ref(current_child);
    
    while(current_child != NULL){
      ags_connectable_connect(AGS_CONNECTABLE(current_child));

      /* iterate */
      next_child = ags_thread_next(current_child);

      g_object_unref(current_child);

      current_child = next_child;
    }

    g_object_unref(children);
  }
}

void
ags_thread_disconnect(AgsConnectable *connectable)
{
  AgsThread *thread;
  AgsThread *children, *current_child, *next_child;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  thread = AGS_THREAD(connectable);

  ags_thread_unset_flags(thread, AGS_THREAD_CONNECTED);


  /* recursive connect */
  children = ags_thread_children(thread);

  if(children != NULL){
    current_child = children;
    g_object_ref(current_child);
    
    while(current_child != NULL){
      ags_connectable_disconnect(AGS_CONNECTABLE(current_child));

      /* iterate */
      next_child = ags_thread_next(current_child);

      g_object_unref(current_child);

      current_child = next_child;
    }

    g_object_unref(children);
  }
}


/**
 * ags_thread_global_get_use_sync_counter:
 * 
 * Get global config value use sync counter.
 *
 * Returns: if %TRUE use sync counter, else not
 * 
 * Since: 3.0.0
 */
gboolean
ags_thread_global_get_use_sync_counter()
{
  gboolean use_sync_counter;

//  g_rec_mutex_lock(ags_thread_get_class_mutex());

  use_sync_counter = ags_thread_global_use_sync_counter;

//  g_rec_mutex_unlock(ags_thread_get_class_mutex());
  
  return(use_sync_counter);
}

/**
 * ags_thread_test_flags:
 * @thread: the #AgsThread
 * @flags: the flags
 *
 * Test @flags to be set on @thread.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_thread_test_flags(AgsThread *thread, guint flags)
{
  gboolean retval;  

  GRecMutex *thread_mutex;

  if(!AGS_IS_THREAD(thread)){
    return(FALSE);
  }

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  /* test flags */
  g_rec_mutex_lock(thread_mutex);

  retval = ((flags & (thread->my_flags)) != 0) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(thread_mutex);
    
  return(retval);
}

/**
 * ags_thread_set_flags:
 * @thread: the #AgsThread
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 3.0.0
 */
void
ags_thread_set_flags(AgsThread *thread, guint flags)
{
  GRecMutex *thread_mutex;

  if(!AGS_IS_THREAD(thread)){
    return;
  }

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  /* set flags */
  g_rec_mutex_lock(thread_mutex);

  thread->my_flags |= flags;

  g_rec_mutex_unlock(thread_mutex);
}

/**
 * ags_thread_unset_flags:
 * @thread: the #AgsThread
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 3.0.0
 */
void
ags_thread_unset_flags(AgsThread *thread, guint flags)
{
  GRecMutex *thread_mutex;

  if(!AGS_IS_THREAD(thread)){
    return;
  }

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  /* set flags */
  g_rec_mutex_lock(thread_mutex);

  thread->my_flags &= (~flags);

  g_rec_mutex_unlock(thread_mutex);
}

/**
 * ags_thread_test_status_flags:
 * @thread: the #AgsThread
 * @status_flags: the syn flags
 *
 * Test @status_flags to be set on @thread.
 * 
 * Returns: %TRUE if status flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_thread_test_status_flags(AgsThread *thread, guint status_flags)
{
  gboolean retval;  

  if(!AGS_IS_THREAD(thread)){
    return(FALSE);
  }
  
  retval = ((status_flags & (g_atomic_int_get(&(thread->status_flags)))) != 0) ? TRUE: FALSE;
    
  return(retval);
}

/**
 * ags_thread_set_status_flags:
 * @thread: the #AgsThread
 * @status_flags: the status flags
 *
 * Set status flags.
 * 
 * Since: 3.0.0
 */
void
ags_thread_set_status_flags(AgsThread *thread, guint status_flags)
{
  if(!AGS_IS_THREAD(thread)){
    return;
  }

  g_atomic_int_or(&(thread->status_flags), status_flags);
}

/**
 * ags_thread_unset_status_flags:
 * @thread: the #AgsThread
 * @status_flags: the status flags
 *
 * Unset status flags.
 * 
 * Since: 3.0.0
 */
void
ags_thread_unset_status_flags(AgsThread *thread, guint status_flags)
{
  if(!AGS_IS_THREAD(thread)){
    return;
  }

  g_atomic_int_and(&(thread->status_flags), (~status_flags));
}

/**
 * ags_thread_parent:
 * @thread: the #AgsThread
 *
 * Get parent thread.
 * 
 * Returns: the parent #AgsThread
 * 
 * Since: 3.0.0
 */
AgsThread*
ags_thread_parent(AgsThread *thread)
{
  AgsThread *parent;

  GRecMutex *thread_mutex;
  
  if(!AGS_IS_THREAD(thread)){
    return(NULL);
  }

  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  g_rec_mutex_lock(thread_mutex);

  parent = thread->parent;

  if(parent != NULL){
    g_object_ref(parent);
  }
  
  g_rec_mutex_unlock(thread_mutex);

  return(parent);
}

/**
 * ags_thread_next:
 * @thread: the #AgsThread
 *
 * Get next thread.
 * 
 * Returns: the next #AgsThread
 * 
 * Since: 3.0.0
 */
AgsThread*
ags_thread_next(AgsThread *thread)
{
  AgsThread *next;

  GRecMutex *thread_mutex;
  
  if(!AGS_IS_THREAD(thread)){
    return(NULL);
  }

  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  g_rec_mutex_lock(thread_mutex);

  next = thread->next;

  if(next != NULL){
    g_object_ref(next);
  }
  
  g_rec_mutex_unlock(thread_mutex);

  return(next);
}

/**
 * ags_thread_prev:
 * @thread: the #AgsThread
 *
 * Get prev thread.
 * 
 * Returns: the prev #AgsThread
 * 
 * Since: 3.0.0
 */
AgsThread*
ags_thread_prev(AgsThread *thread)
{
  AgsThread *prev;

  GRecMutex *thread_mutex;
  
  if(!AGS_IS_THREAD(thread)){
    return(NULL);
  }

  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  g_rec_mutex_lock(thread_mutex);

  prev = thread->prev;

  if(prev != NULL){
    g_object_ref(prev);
  }
  
  g_rec_mutex_unlock(thread_mutex);

  return(prev);
}

/**
 * ags_thread_children:
 * @thread: the #AgsThread
 *
 * Get children thread.
 * 
 * Returns: the children #AgsThread
 * 
 * Since: 3.0.0
 */
AgsThread*
ags_thread_children(AgsThread *thread)
{
  AgsThread *children;

  GRecMutex *thread_mutex;
  
  if(!AGS_IS_THREAD(thread)){
    return(NULL);
  }

  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  g_rec_mutex_lock(thread_mutex);

  children = thread->children;

  if(children != NULL){
    g_object_ref(children);
  }
  
  g_rec_mutex_unlock(thread_mutex);

  return(children);
}

/**
 * ags_thread_get_toplevel:
 * @thread: the #AgsThread
 *
 * Retrieve toplevel thread.
 *
 * Returns: the toplevevel #AgsThread
 *
 * Since: 3.0.0
 */
AgsThread*
ags_thread_get_toplevel(AgsThread *thread)
{
  AgsThread *current, *current_parent;
  
  if(!AGS_IS_THREAD(thread)){
    return(NULL);
  }

  current = thread;
  g_object_ref(current);
  
  while((current_parent = ags_thread_parent(current)) != NULL){
    /* iterate */
    g_object_unref(current);
    
    current = current_parent;
  }

  return(current);
}

/**
 * ags_thread_first:
 * @thread: the #AgsThread
 *
 * Retrieve first sibling.
 *
 * Returns: the very first #AgsThread within same tree level
 *
 * Since: 3.0.0
 */
AgsThread*
ags_thread_first(AgsThread *thread)
{
  AgsThread *current, *current_prev;
  
  if(!AGS_IS_THREAD(thread)){
    return(NULL);
  }

  current = thread;
  g_object_ref(current);
  
  while((current_prev = ags_thread_prev(current)) != NULL){
    /* iterate */
    g_object_unref(current);
    
    current = current_prev;
  }

  return(current);
}

/**
 * ags_thread_last:
 * @thread: the #AgsThread
 * 
 * Retrieve last sibling.
 *
 * Returns: the very last @AgsThread within same tree level
 *
 * Since: 3.0.0
 */
AgsThread*
ags_thread_last(AgsThread *thread)
{
  AgsThread *current, *current_next;
  
  if(!AGS_IS_THREAD(thread)){
    return(NULL);
  }

  current = thread;
  g_object_ref(current);
  
  while((current_next = ags_thread_next(current)) != NULL){
    /* iterate */
    g_object_unref(current);

    current = current_next;
  }

  return(current);
}

/**
 * ags_thread_set_sync:
 * @thread: the #AgsThread
 * @tic: the tic as sync occured.
 * 
 * Unsets AGS_THREAD_WAIT_0, AGS_THREAD_WAIT_1 or AGS_THREAD_WAIT_2.
 * Additionaly the thread is woken up by this function if waiting.
 *
 * Since: 3.0.0
 */
void
ags_thread_set_sync(AgsThread *thread, guint tic)
{
  guint next_tic;
  guint status_flags;
  gboolean broadcast;
  gboolean waiting;

  GMutex *tic_mutex;

  if(!AGS_IS_THREAD(thread)){
    return;
  }

  tic_mutex = AGS_THREAD_GET_TIC_MUTEX(thread);
  
  broadcast = FALSE;
  waiting = FALSE;

  if(tic > 2){
    tic = tic % 3;
  }

  g_mutex_lock(tic_mutex);

  status_flags = g_atomic_int_get(&(thread->status_flags));

  /* return if no immediate sync and initial run */
  if(!ags_thread_test_flags(thread, AGS_THREAD_IMMEDIATE_SYNC)){
    if(ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_INITIAL_RUN)){
      g_mutex_unlock(tic_mutex);
      
      return;
    }
  }

  /* get next tic and check waiting */
  next_tic = 0;
  
  switch(tic){
  case 0:
    {
      if((AGS_THREAD_STATUS_WAIT_0 & status_flags) != 0){
	next_tic = 1;
	waiting = TRUE;
      }
    }
    break;
  case 1:
    {
      if((AGS_THREAD_STATUS_WAIT_1 & status_flags) != 0){
	next_tic = 2;
	waiting = TRUE;
      }
    }
    break;
  case 2:
    {
      if((AGS_THREAD_STATUS_WAIT_2 & status_flags) != 0){
	next_tic = 0;
	waiting = TRUE;
      }
    }
    break;
  }

  /* unset wait flag */
  switch(tic){
  case 0:
    {
      if((AGS_THREAD_STATUS_WAIT_0 & status_flags) != 0){
	ags_thread_unset_status_flags(thread, AGS_THREAD_STATUS_WAIT_0);
      }
    }
    break;
  case 1:
    {
      if((AGS_THREAD_STATUS_WAIT_1 & status_flags) != 0){
	ags_thread_unset_status_flags(thread, AGS_THREAD_STATUS_WAIT_1);
      }
    }
    break;
  case 2:
    {
      if((AGS_THREAD_STATUS_WAIT_2 & status_flags) != 0){
	ags_thread_unset_status_flags(thread, AGS_THREAD_STATUS_WAIT_2);
      }
    }
    break;
  }

  /* signal waiting thread */
  if(ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_WAITING)){
    ags_thread_unset_status_flags(thread, AGS_THREAD_STATUS_WAITING);

    if(broadcast){
      g_cond_broadcast(AGS_THREAD_GET_TIC_COND(thread));
    }else{
      g_cond_signal(AGS_THREAD_GET_TIC_COND(thread));
    }
  }    

  thread->current_tic = next_tic;

  ags_thread_unset_status_flags(thread, AGS_THREAD_STATUS_WAITING);

  /*  */
  g_mutex_unlock(tic_mutex);
}

void
ags_thread_set_sync_all_reset(AgsThread *thread)
{
  AgsThread *children, *current_child, *next_child;

  GRecMutex *thread_mutex;

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  /* reset chaos tree */
  ags_thread_unset_status_flags(thread, AGS_THREAD_STATUS_IS_CHAOS_TREE);

  if(ags_thread_test_flags(thread, AGS_THREAD_MARK_SYNCED)){
    ags_thread_set_status_flags(thread, AGS_THREAD_STATUS_SYNCED);
  }
    
  /* descend */
  children = ags_thread_children(thread);

  if(children != NULL){
    current_child = children;
    g_object_ref(current_child);
  
    while(current_child != NULL){
      ags_thread_set_sync_all_reset(current_child);

      /* iterate */
      next_child = ags_thread_next(current_child);

      g_object_unref(current_child);

      current_child = next_child;
    }

    g_object_unref(children);
  }
  
  /* increment delay counter and set run per cycle */
  g_rec_mutex_lock(thread_mutex);

  if(thread->freq >= thread->max_precision){
    thread->tic_delay = 0;
  }else{
    if(thread->tic_delay < thread->delay){
      thread->tic_delay++;
    }else{
      thread->tic_delay = 0;
    }
  }

  g_rec_mutex_unlock(thread_mutex);
}
  
void
ags_thread_set_sync_all_recursive(AgsThread *thread, guint tic)
{
  AgsThread *children, *current_child, *next_child;

  ags_thread_set_sync(thread, tic);

  /* recursive children */
  children = ags_thread_children(thread);

  if(children != NULL){
    current_child = children;
    g_object_ref(current_child);    

    while(current_child != NULL){
      ags_thread_set_sync_all_recursive(child, tic);

      /* iterate */
      next_child = ags_thread_next(current_child);

      g_object_unref(current_child);

      current_child = next_child;
    }

    g_object_unref(children);
  }
}

/**
 * ags_thread_set_sync_all:
 * @thread: the #AgsThread
 * @tic: the tic as sync occured.
 * 
 * Calls ags_thread_set_sync() on all threads.
 *
 * Since: 3.0.0
 */
void
ags_thread_set_sync_all(AgsThread *thread, guint tic)
{
  AgsThread *main_loop;

  if(!AGS_IS_THREAD(thread)){
    return;
  }
  
  /* entry point */
  main_loop = ags_thread_get_toplevel(thread);

  ags_thread_set_sync_all_reset(main_loop);
  ags_thread_set_sync_all_recursive(main_loop, tic);

  g_object_unref(main_loop);
}

/**
 * ags_thread_lock:
 * @thread: the #AgsThread
 * 
 * Locks the threads own mutex and sets the appropriate flag.
 *
 * Since: 3.0.0
 */
void
ags_thread_lock(AgsThread *thread)
{
  GRecMutex *mutex;
  
  if(!AGS_IS_THREAD(thread)){
    return;
  }

  /* mutex */
  mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  /* lock */
  g_rec_mutex_lock(mutex);

  ags_thread_set_status_flags(thread, AGS_THREAD_STATUS_LOCKED);
}

/**
 * ags_thread_trylock:
 * @thread: the #AgsThread
 * 
 * Locks the threads own mutex if available and sets the
 * appropriate flag and returning %TRUE. Otherwise return %FALSE
 * without lock.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_thread_trylock(AgsThread *thread)
{
  GRecMutex *mutex;
  
  if(!AGS_IS_THREAD(thread)){
    return(FALSE);
  }

  /* lookup mutices */
  mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  /* lock */
  if(!g_rec_mutex_trylock(mutex)){      
    return(FALSE);
  }

  ags_thread_set_status_flags(thread, AGS_THREAD_STATUS_LOCKED);
  
  return(TRUE);
}

/**
 * ags_thread_unlock:
 * @thread: the #AgsThread
 *
 * Unlocks the threads own mutex and unsets the appropriate flag.
 *
 * Since: 3.0.0
 */
void
ags_thread_unlock(AgsThread *thread)
{
  GRecMutex *mutex;
  
  if(!AGS_IS_THREAD(thread)){
    return;
  }

  /* mutex */
  mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  /* unlock */
  ags_thread_unset_status_flags(thread, AGS_THREAD_STATUS_LOCKED);
  
  g_rec_mutex_unlock(mutex);
}

/**
 * ags_thread_remove_child:
 * @thread: the #AgsThread
 * @child: the child to remove
 * 
 * Remove child of thread.
 *
 * Since: 3.0.0
 */
void
ags_thread_remove_child(AgsThread *thread, AgsThread *child)
{
  AgsThread *children, *current_child, *next_child, *prev_child;

  gboolean found_child;
  
  if(!AGS_IS_THREAD(thread) ||
     !AGS_IS_THREAD(child)){
    return;
  }
  
  children = ags_thread_children(thread);

  if(children == NULL){
    return;
  }

  current_child = children;
  g_object_ref(current_child);
  
  found_child = FALSE;
  
  while(current_child != NULL){
    if(current_child == child){
      found_child = TRUE;
      
      break;
    }

    /* iterate */
    next_child = ags_thread_next(current_child);

    g_object_unref(current_child);

    current_child = next_child;
  }

  g_object_unref(children);
  
  if(found_child){
    gboolean is_first, is_last;
    
    g_rec_mutex_lock(AGS_THREAD_GET_OBJ_MUTEX(child));

    is_first = (child->prev == NULL) ? TRUE: FALSE;
    is_last = (child->next == NULL) ? TRUE: FALSE;

    child->parent = NULL;
    
    g_rec_mutex_unlock(AGS_THREAD_GET_OBJ_MUTEX(child));

    if(is_first){
      next_child = ags_thread_next(child);

      g_rec_mutex_lock(AGS_THREAD_GET_OBJ_MUTEX(thread));
      
      thread->children = next_child;

      g_rec_mutex_unlock(AGS_THREAD_GET_OBJ_MUTEX(thread));
    }    
    
    next_child = ags_thread_next(child);
    prev_child = ags_thread_prev(child);

    /* reset child */
    g_rec_mutex_lock(AGS_THREAD_GET_OBJ_MUTEX(child));

    child->next = NULL;
    child->prev = NULL;

    g_rec_mutex_unlock(AGS_THREAD_GET_OBJ_MUTEX(child));

    if(next_child != NULL){
      g_object_unref(next_child);
    }

    if(prev_child != NULL){
      g_object_unref(prev_child);
    }
    
    /* relink */
    if(next_child != NULL){
      g_rec_mutex_lock(AGS_THREAD_GET_OBJ_MUTEX(next_child));
      
      next_child->prev = prev_child;
      
      g_rec_mutex_unlock(AGS_THREAD_GET_OBJ_MUTEX(next_child));
    }

    if(prev_child != NULL){
      g_rec_mutex_lock(AGS_THREAD_GET_OBJ_MUTEX(prev_child));
      
      prev_child->next = next_child;
      
      g_rec_mutex_unlock(AGS_THREAD_GET_OBJ_MUTEX(prev_child));
    }
    
    g_object_unref(thread);
  }
}

/**
 * ags_thread_add_child:
 * @thread: the #AgsThread
 * @child: the child to remove
 * 
 * Add child to thread.
 *
 * Since: 3.0.0
 */
void
ags_thread_add_child(AgsThread *thread, AgsThread *child)
{
  ags_thread_add_child_extended(thread, child,
				FALSE, TRUE);
}

/**
 * ags_thread_add_child_extended:
 * @thread: the #AgsThread
 * @child: the child to remove
 * @no_start: don't start thread
 * @no_wait: don't wait until started
 * 
 * Add child to thread.
 *
 * Since: 3.0.0
 */
void
ags_thread_add_child_extended(AgsThread *thread, AgsThread *child,
			      gboolean no_start, gboolean no_wait)
{
  AgsThread *main_loop;
  AgsThread *current_parent;
  AgsThread *children;
  AgsThread *last_child;
   
  GRecMutex *tree_lock;
  
  if(!AGS_IS_THREAD(thread) ||
     !AGS_IS_THREAD(child)){
    return;
  }

  /* check current parent */
  current_parent = ags_thread_parent(child);

  if(current_parent == thread){
    g_object_unref(current_parent);
    
    return;
  }  

  main_loop = ags_thread_toplevel(thread);
  tree_lock = ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop));
  
  g_rec_mutex_lock(tree_lock);

  g_object_ref(thread);
  g_object_ref(child);

  g_rec_mutex_lock(AGS_THREAD_GET_OBJ_MUTEX(child));

  child->parent = thread;
  
  g_rec_mutex_unlock(AGS_THREAD_GET_OBJ_MUTEX(child));

  children = ags_thread_children(thread);

  if(children == NULL){
    g_rec_mutex_lock(AGS_THREAD_GET_OBJ_MUTEX(thread));

    thread->children = child;
    
    g_rec_mutex_unlock(AGS_THREAD_GET_OBJ_MUTEX(thread)); 
  }else{
    last_child = ags_thread_last(children);

    /* next */
    g_rec_mutex_lock(AGS_THREAD_GET_OBJ_MUTEX(last_child));

    last_child->next = child;
    
    g_rec_mutex_unlock(AGS_THREAD_GET_OBJ_MUTEX(last_child));

    /* prev */
    g_rec_mutex_lock(AGS_THREAD_GET_OBJ_MUTEX(child));

    child->prev = last_child;

    g_rec_mutex_unlock(AGS_THREAD_GET_OBJ_MUTEX(child));

    /* unref */
    g_object_unref(children);
  }

  g_rec_mutex_unlock(tree_lock);

  g_object_unref(main_loop);
  
  if(!no_start){
    if(!ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_RUNNING)){
      /* start child */
      ags_thread_start(child);

      if(!no_wait){
	guint val;

	/* wait child */
	g_mutex_lock(AGS_THREAD_GET_START_MUTEX(child));

	ags_thread_set_status_flags(child, AGS_THREAD_STATUS_START_WAIT);
	
	if(ags_thread_test_status_flags(child, AGS_THREAD_STATUS_START_WAIT) &&
	   !ags_thread_test_status_flags(child, AGS_THREAD_STATUS_START_DONE)){
	  while(ags_thread_test_status_flags(child, AGS_THREAD_STATUS_START_WAIT) &&
		!ags_thread_test_status_flags(child, AGS_THREAD_STATUS_START_DONE)){
	    g_cond_wait(AGS_THREAD_GET_START_COND(child),
			AGS_THREAD_GET_START_MUTEX(child));
	  }
	}
	
	g_mutex_unlock(AGS_THREAD_GET_START_MUTEX(child));
      }
    }
  }
}

/**
 * ags_thread_parental_is_locked:
 * @thread: the #AgsThread
 * @parent: where to stop iteration
 *
 * Check the AGS_THREAD_STATUS_LOCKED flag in parental levels.
 *
 * Returns: %TRUE if locked otherwise %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_thread_parental_is_locked(AgsThread *thread, AgsThread *parent)
{
  AgsThread *current_thread, *current_parent;

  gboolean is_locked;
  
  if(!AGS_IS_THREAD(thread)){
    return(FALSE);
  }

  current_parent = ags_thread_parent(thread);

  is_locked = FALSE;
  
  if(current_parent != NULL){
    current_thread = current_parent;
    
    while(current_thread != parent){
      if(ags_thread_test_status_flags(current_thread, AGS_THREAD_STATUS_LOCKED)){
	is_locked = TRUE;

	break;
      }

      /* ascend */
      current_parent = ags_thread_parent(thread);
      
      g_object_unref(current_thread);
      
      current_thread = current_parent;
    }

    if(current_thread != NULL){
      g_object_unref(current_thread);
    }
  }

  return(is_locked);
}

/**
 * ags_thread_sibling_is_locked:
 * @thread: the #AgsThread
 *
 * Check the AGS_THREAD_STATUS_LOCKED flag within sibling.
 *
 * Returns: %TRUE if locked otherwise %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_thread_sibling_is_locked(AgsThread *thread)
{
  AgsThread *first_thread, *current_thread, *next_thread;

  gboolean is_locked;
  
  if(!AGS_IS_THREAD(thread)){
    return(FALSE);
  }

  first_thread = ags_thread_first(thread);

  is_locked = FALSE;
  
  if(first_thread != NULL){
    current_thread = first_thread;
    g_object_ref(current_thread);
    
    while(current_thread != NULL){
      if(current_thread != thread){
	if(ags_thread_test_status_flags(current_thread, AGS_THREAD_STATUS_LOCKED)){
	  is_locked = TRUE;

	  break;
	}
      }

      /* iterate */
      next_thread = ags_thread_next(current_thread);

      g_object_unref(current_thread);

      current_thread = next_thread;
    }

    if(current_thread != NULL){
      g_object_unref(current_thread);
    }

    g_object_unref(first_thread);
  }
  
  return(is_locked);
}

gboolean
ags_thread_children_is_locked_recursive(AgsThread *thread)
{
  AgsThread *children, *current_child, *next_child;

  gboolean is_locked;
  
  if(!AGS_IS_THREAD(thread)){
    return(FALSE);
  }

  if(ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_LOCKED)){
    return(TRUE);
  }

  children = ags_thread_children(thread);

  is_locked = FALSE;

  if(children != NULL){
    current_child = children;
    g_object_unref(current_child);
    
    while(current_child != NULL){
      if(ags_thread_children_is_locked_recursive(current_child)){
	is_locked = TRUE;

	break;
      }

      /* iterate */
      next_child = ags_thread_next(current_child);

      g_object_unref(current_child);

      current_child = next_child;
    }

    if(current_child != NULL){
      g_object_unref(current_child);
    }

    g_object_unref(children);
  }
  
  return(is_locked);
}

/**
 * ags_thread_children_is_locked:
 * @thread: the #AgsThread
 *
 * Check the AGS_THREAD_STATUS_LOCKED flag within children.
 *
 * Returns: %TRUE if locked otherwise %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_thread_children_is_locked(AgsThread *thread)
{
  AgsThread *children, *current_child, *next_child;

  gboolean is_locked;

  if(!AGS_IS_THREAD(thread)){
    return(FALSE);
  }

  children = ags_thread_children(thread);

  is_locked = FALSE;

  if(children != NULL){
    current_child = children;
    g_object_unref(current_child);
    
    while(current_child != NULL){
      if(ags_thread_children_is_locked_recursive(current_child)){
	is_locked = TRUE;

	break;
      }

      /* iterate */
      next_child = ags_thread_next(current_child);

      g_object_unref(current_child);

      current_child = next_child;
    }

    if(current_child != NULL){
      g_object_unref(current_child);
    }

    g_object_unref(children);
  }

  return(is_locked);
}

/**
 * ags_thread_is_current_ready:
 * @current: the #AgsThread
 * @tic: the tic to check
 *
 * Check @tic is ready.
 *
 * Returns: %TRUE if ready, otherwise %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_thread_is_current_ready(AgsThread *current,
			    guint tic)
{

  AgsThread *toplevel;

  guint status_flags;
  gboolean retval;

  GRecMutex *current_mutex;

  if(!AGS_IS_THREAD(current)){
    return(TRUE);
  }
  
  toplevel = ags_thread_get_toplevel(current);

  current_mutex = AGS_THREAD_GET_OBJ_MUTEX(current);

  g_rec_mutex_lock(current_mutex);

  status_flags = g_atomic_int_get(&(current->status_flags));

  retval = FALSE;

  if(!ags_thread_test_status_flags(current, AGS_THREAD_RUNNING)){
    retval = TRUE;
  }

  if(!ags_thread_test_flags(current, AGS_THREAD_IMMEDIATE_SYNC)){
    if(ags_thread_test_status_flags(current, AGS_THREAD_INITIAL_RUN)){
      g_rec_mutex_unlock(current_mutex);

      return(FALSE);
    }
  }
  
  if((AGS_THREAD_STATUS_READY & status_flags) != 0){
    retval = TRUE;
  }

  if(retval){
    g_rec_mutex_unlock(current_mutex);

    return(TRUE);
  }

  if(tic > 2){
    tic = tic % 3;
  }

  switch(tic){
  case 0:
    {
      if((AGS_THREAD_STATUS_WAIT_0 & status_flags) == 0){
	retval = TRUE;
      }
    }
    break;
  case 1:
    {
      if((AGS_THREAD_STATUS_WAIT_1 & status_flags) == 0){
	retval = TRUE;
      }
    }
    break;
  case 2:
    {
      if((AGS_THREAD_STATUS_WAIT_2 & status_flags) == 0){
	retval = TRUE;
      }
    }
    break;
  }

  g_rec_mutex_unlock(current_mutex);

  return(retval);
}

gboolean
ags_thread_is_tree_ready_current_tic(AgsThread *current,
				     guint tic)
{
  guint status_flags;
  gboolean retval;

  GMutex *tic_mutex;

  if(!AGS_IS_THREAD(current)){
    return(TRUE);
  }

  tic_mutex = AGS_THREAD_GET_TIC_MUTEX(current);
  
  g_mutex_lock(tic_mutex);

  status_flags = g_atomic_int_get(&(current->status_flags));
    
  retval = FALSE;

  if((AGS_THREAD_STATUS_RUNNING & status_flags) == 0){
    retval = TRUE;
  }

  if(!ags_thread_test_flags(thread, AGS_THREAD_IMMEDIATE_SYNC)){
    if((AGS_THREAD_STATUS_INITIAL_RUN & status_flags) != 0){
      g_mutex_unlock(tic_mutex);
	
      return(FALSE);
    }
  }
    
  if((AGS_THREAD_STATUS_READY & status_flags) != 0){
    retval = TRUE;
  }

  if(retval){
    g_mutex_unlock(tic_mutex);

    return(TRUE);
  }

  if(tic > 2){
    tic = tic % 3;
  }

  switch(tic){
  case 0:
  {
    if((AGS_THREAD_STATUS_WAIT_0 & status_flags) != 0){
      retval = TRUE;
    }
  }
  break;
  case 1:
  {
    if((AGS_THREAD_STATUS_WAIT_1 & status_flags) != 0){
      retval = TRUE;
    }
  }
  break;
  case 2:
  {
    if((AGS_THREAD_STATUS_WAIT_2 & status_flags) != 0){
      retval = TRUE;
    }
  }
  break;
  }

  g_mutex_unlock(tic_mutex);

  return(retval);
}

gboolean
ags_thread_is_tree_ready_recursive(AgsThread *current,
				   guint tic)
{
  AgsThread *children, *current_child, *next_child;

  gboolean is_tree_ready;
  
  if(!AGS_IS_THREAD(current)){
    return(TRUE);
  }

  if(!ags_thread_is_tree_ready_current_tic(current,
					   tic)){
    return(FALSE);
  }

  children = ags_thread_children(current);
  
  is_tree_ready = TRUE;
  
  if(children != NULL){
    current_child = children;
    g_object_ref(current_child);
    
    while(current_child != NULL){
      if(!ags_thread_is_tree_ready_recursive(current_child,
					     tic)){
	is_tree_ready = FALSE;

	break;
      }

      /* iterate */
      next_child = ags_thread_next(current_child);

      g_object_unref(current_child);

      current_child = next_child;
    }

    g_object_unref(children);
  }
  
  return(is_tree_ready);
}

/**
 * ags_thread_is_tree_ready:
 * @thread: the #AgsThread
 * @tic: the tic to check
 *
 * Check @tic is ready.
 *
 * Returns: %TRUE if ready, otherwise %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_thread_is_tree_ready(AgsThread *thread,
			 guint tic)
{
  AgsThread *main_loop;
  
  gboolean retval;

  if(!AGS_IS_THREAD(thread)){
    return(FALSE);
  }

  main_loop = ags_thread_get_toplevel(thread);

  if(ags_thread_global_get_use_sync_counter()){    
    retval = ags_main_loop_sync_counter_test(AGS_MAIN_LOOP(main_loop), tic);
  }else{
    retval = ags_thread_is_tree_ready_recursive(main_loop);
  }

  g_object_unref(main_loop);
  
  return(retval);
}

/**
 * ags_thread_next_parent_locked:
 * @thread: the #AgsThread
 * @parent: the parent #AgsThread where to stop
 * 
 * Retrieve next locked thread above @thread.
 *
 * Returns: next matching #AgsThread
 *
 * Since: 3.0.0
 */
AgsThread*
ags_thread_next_parent_locked(AgsThread *thread, AgsThread *parent)
{
  AgsThread *current_parent, *current;
  AgsThread *retval;
  
  if(!AGS_IS_THREAD(thread)){
    return(NULL);
  }

  current_parent = ags_thread_parent(thread);

  retval = NULL;
  
  if(current_parent != NULL){
    current = current_parent;
    
    while(current != parent){
      if(ags_thread_test_status_flags(current, AGS_THREAD_STATUS_WAITING_FOR_CHILDREN)){
	retval = current;

	break;
      }

      /* ascend */
      current_parent = ags_thread_parent(current);

      g_object_unref(current);

      current = current_parent;
    }

    if(current != retval){
      g_object_unref(current);
    }
  }
  
  return(retval);
}

/**
 * ags_thread_next_sibling_locked:
 * @thread: the #AgsThread
 *
 * Retrieve next locked thread neighbooring @thread
 *
 * Returns: next matching #AgsThread
 *
 * Since: 3.0.0
 */
AgsThread*
ags_thread_next_sibling_locked(AgsThread *thread)
{
  AgsThread *first_thread, *current_thread, *next_thread;
  AgsThread *retval;
  
  if(!AGS_IS_THREAD(thread)){
    return(NULL);
  }
  
  first_thread = ags_thread_first(thread);

  retval = NULL;
  
  if(first_thread){
    current_thread = first_thread;
    g_object_ref(current_thread);
    
    while(current_thread != NULL){
      if(current_thread != thread){
	if(ags_thread_test_status_flags(current_thread, AGS_THREAD_STATUS_WAITING_FOR_SIBLING)){
	  retval = current_thread;

	  break;
	}
      }

      /* iterate */
      next_thread = ags_thread_next(current_thread);

      g_object_unref(current_thread);

      current_thread = next_thread;
    }

    g_object_unref(first_thread);
  }
  
  return(retval);
}

AgsThread*
ags_thread_next_children_locked_recursive(AgsThread *child)
{
  AgsThread *last_child, *current_child, *prev_child;
  AgsThread *retval, *tmp_retval;

  last_child = ags_thread_last(child);

  retval = NULL;
  
  if(last_child != NULL){
    current_child = last_child;
    
    while(current_child != NULL){
      tmp_retval = ags_thread_next_children_locked_recursive(current_child);
      
      if(tmp_retval != NULL){
	retval = tmp_retval;
	
	break;
      }

      /* iterate */
      prev_child = ags_thread_prev(current_child);

      g_object_unref(current_child);
      
      current_child = prev_child;
    }

    g_object_unref(last_child);
  }

  if(ags_thread_test_status_flags(child, AGS_THREAD_STATUS_WAITING_FOR_PARENT)){
    retval = child;
  }
  
  return(retval);
}

/**
 * ags_thread_next_children_locked:
 * @thread: the #AgsThread
 * 
 * Retrieve next locked thread following @thread
 *
 * Returns: next matching #AgsThread
 *
 * Since: 3.0.0
 */
AgsThread*
ags_thread_next_children_locked(AgsThread *thread)
{
  AgsThread *children;
  AgsThread *last_child, *current_child, *prev_child;
  AgsThread *retval, *tmp_retval;

  if(!AGS_IS_THREAD(thread)){
    return(NULL);
  }

  children = ags_thread_children(thread);
  
  retval = NULL;

  if(children != NULL){
    last_child = ags_thread_last(children);
  
    current_child = last_child;
    
    while(current_child != NULL){
      tmp_retval = ags_thread_next_children_locked_recursive(current_child);
      
      if(tmp_retval != NULL){
	retval = tmp_retval;
	
	break;
      }

      /* iterate */
      prev_child = ags_thread_prev(current_child);

      g_object_unref(current_child);
      
      current_child = prev_child;
    }

    g_object_unref(last_child);
    g_object_unref(children);
  }
  
  return(retval);
}

/**
 * ags_thread_lock_parent:
 * @thread: the #AgsThread
 * @parent: the parent #AgsThread where to stop
 *
 * Lock parent tree structure.
 *
 * Since: 3.0.0
 */
void
ags_thread_lock_parent(AgsThread *thread, AgsThread *parent)
{
  AgsThread *current_thread, *current_parent;

  if(!AGS_IS_THREAD(thread)){
    return;
  }

  ags_thread_lock(thread);

  current_parent = ags_thread_parent(thread);

  if(current_parent != NULL){
    current_thread = current_parent;
  
    while(current_thread != parent){
      ags_thread_lock(current_thread);

      ags_thread_set_status_flags(current_thread, AGS_THREAD_STATUS_WAITING_FOR_CHILDREN);

      /* ascend */
      current_parent = ags_thread_parent(current_thread);

      g_object_unref(current_thread);

      current_thread = current_parent;
    }

    if(current_thread != NULL){
      g_object_unref(current_thread);
    }
  }
}

/**
 * ags_thread_lock_sibling:
 * @thread: the #AgsThread
 *
 * Lock sibling tree structure.
 *
 * Since: 3.0.0
 */
void
ags_thread_lock_sibling(AgsThread *thread)
{
  AgsThread *first_thread, *current_thread, *next_thread;

  if(!AGS_IS_THREAD(thread)){
    return;
  }

  ags_thread_lock(thread);

  first_thread = ags_thread_first(thread);

  if(first_thread != NULL){
    current_thread = first_thread;
    g_object_ref(current_thread);
    
    while(current_thread != NULL){
      if(current_thread != thread){
	ags_thread_lock(current_thread);

	ags_thread_set_status_flags(current_thread, AGS_THREAD_STATUS_WAITING_FOR_SIBLING);
      }
      
      /* iterate */
      next_thread = ags_thread_next(current_thread);

      g_object_unref(current_thread);

      current_thread = next_thread;
    }

    g_object_unref(first_thread);
  }
}

void
ags_thread_lock_children_recursive(AgsThread *thread)
{
  AgsThread *children, *current_child, *next_child;

  if(!AGS_IS_THREAD(thread)){
    return;
  }

  ags_thread_lock(thread);

  ags_thread_set_status_flags(thread, AGS_THREAD_STATUS_WAITING_FOR_PARENT);

  /* traverse */
  children = ags_thread_children(thread);

  if(children != NULL){
    current_child = children;
    g_object_unref(current_child);
    
    while(current_child != NULL){
      ags_thread_lock_children_recursive(current_child);

      /* iterate */
      next_child = ags_thread_next(current_child);

      g_object_unref(current_child);

      current_child = next_child;
    }

    g_object_unref(children);
  }
}

/**
 * ags_thread_lock_children:
 * @thread: the #AgsThread
 *
 * Lock child tree structure.
 *
 * Since: 3.0.0
 */
void
ags_thread_lock_children(AgsThread *thread)
{
  AgsThread *children, *current_child, *next_child;

  if(!AGS_IS_THREAD(thread)){
    return;
  }

  ags_thread_lock(thread);
  
  children = ags_thread_children(thread);

  if(children != NULL){
    current_child = children;
    g_object_unref(current_child);
    
    while(current_child != NULL){
      ags_thread_lock_children_recursive(g_atomic_pointer_get(current_child);

      /* iterate */
      next_child = ags_thread_next(current_child);

      g_object_unref(current_child);

      current_child = next_child;
    }

    g_object_unref(children);
  }
}

/**
 * ags_thread_lock_all:
 * @thread: the #AgsThread
 *
 * Lock tree structure.
 *
 * Since: 3.0.0
 */
void
ags_thread_lock_all(AgsThread *thread)
{
  if(!AGS_IS_THREAD(thread)){
    return;
  }

  ags_thread_lock_parent(thread, NULL);
  ags_thread_lock_sibling(thread);
  ags_thread_lock_children(thread);
}

/**
 * ags_thread_unlock_parent:
 * @thread: the #AgsThread
 * @parent: the parent #AgsThread where to stop
 *
 * Unlock parent tree structure.
 *
 * Since: 3.0.0
 */
void
ags_thread_unlock_parent(AgsThread *thread, AgsThread *parent)
{
  AgsThread *current_parent, *current_thread;

  if(!AGS_IS_THREAD(thread)){
    return;
  }

  ags_thread_unlock(thread);

  current_parent = ags_thread_parent(thread);

  if(current_parent != NULL){
    current_thread = current_parent;
    
    while(current_thread != parent){
      ags_thread_unset_status_flags(current_thread, AGS_THREAD_STATUS_WAITING_FOR_CHILDREN);

      if(!ags_thread_test_flags(thread, AGS_THREAD_BROADCAST_PARENT)){
	g_cond_signal(AGS_THREAD_GET_OBJ_COND(current_thread));
      }else{
	g_cond_broadcast(AGS_THREAD_GET_OBJ_COND(current_thread));
      }

      ags_thread_unlock(current_thread);

      /* ascend */
      current_parent = ags_thread_parent(current_thread);

      g_object_unref(current_thread);

      current_thread = current_parent;
    }

    if(current_thread != NULL){
      g_object_unref(current_thread);
    }
  }
}

/**
 * ags_thread_unlock_sibling:
 * @thread: the #AgsThread
 *
 * Unlock sibling tree structure.
 *
 * Since: 3.0.0
 */
void
ags_thread_unlock_sibling(AgsThread *thread)
{
  AgsThread *first_thread, *current_thread, *next_thread;

  if(!AGS_IS_THREAD(thread)){
    return;
  }

  ags_thread_unlock(thread);
  
  first_thread = ags_thread_first(thread);

  if(first_thread){
    current_thread = first_thread;
    g_object_ref(current_thread);
    
    while(current_thread != NULL){
      ags_thread_unset_status_flags(current_thread, AGS_THREAD_STATUS_WAITING_FOR_SIBLING);

      if(!ags_thread_test_flags(thread, AGS_THREAD_BROADCAST_SIBLING)){
	g_cond_signal(AGS_THREAD_GET_OBJ_COND(current_thread));
      }else{
	g_cond_broadcast(AGS_THREAD_GET_OBJ_COND(current_thread));
      }

      ags_thread_unlock(current_thread);

      /* iterate */
      next_thread = ags_thread_next(current_thread);

      g_object_unref(current_thread);

      current_thread = next_thread;
    }

    g_object_unref(first_thread);
  }
}

void
ags_thread_unlock_children_recursive(AgsThread *thread, AgsThread *child)
{
  AgsThread *last_child, *current_child, *prev_child;

  /*  */
  last_child = ags_thread_last(child);

  if(last_child != NULL){
    current_child = last_child;
    
    while(current_child != NULL){
      ags_thread_unlock_children_recursive(thread, current_child);

      /* iterate */
      prev_child = ags_thread_prev(current_child);

      g_object_unref(current_child);
      
      current_child = prev_child;
    }

    g_object_unref(last_child);
  }

  ags_thread_unset_status_flags(child, AGS_THREAD_STATUS_WAITING_FOR_PARENT);

  if(!ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_BROADCAST_CHILDREN)){
    g_cond_signal(AGS_THREAD_GET_OBJ_COND(child));
  }else{
    g_cond_broadcast(AGS_THREAD_GET_OBJ_COND(child));
  }

  ags_thread_unlock(child);
}

/**
 * ags_thread_unlock_children:
 * @thread: the #AgsThread
 *
 * Unlock child tree structure.
 *
 * Since: 3.0.0
 */
void
ags_thread_unlock_children(AgsThread *thread)
{  
  AgsThread *children;
  AgsThread *last_child, *current_child, *prev_child;

  if(!AGS_IS_THREAD(thread)){
    return;
  }
  
  ags_thread_unlock(thread);

  children = ags_thread_children(thread);

  if(children != NULL){
    last_child = ags_thread_last(children);
  
    current_child = last_child;
    
    while(current_child != NULL){
      ags_thread_unlock_children_recursive(thread, current_child);

      /* iterate */
      prev_child = ags_thread_prev(current_child);

      g_object_unref(current_child);
      
      current_child = prev_child;
    }

    g_object_unref(last_child);
    g_object_unref(children);
  }
}

/**
 * ags_thread_unlock_all:
 * @thread: the #AgsThread
 *
 * Unlock tree structure.
 *
 * Since: 3.0.0
 */
void
ags_thread_unlock_all(AgsThread *thread)
{
  if(!AGS_IS_THREAD(thread)){
    return;
  }

  ags_thread_unlock_parent(thread, NULL);
  ags_thread_unlock_sibling(thread);
  ags_thread_unlock_children(thread);
}

/**
 * ags_thread_wait_parent:
 * @thread: the #AgsThread
 * @parent: the parent #AgsThread where to stop
 *
 * Wait on parent tree structure.
 *
 * Since: 3.0.0
 */
void
ags_thread_wait_parent(AgsThread *thread, AgsThread *parent)
{
  AgsThread *current_thread, *current_parent;

  if(!AGS_IS_THREAD(thread) ||
     thread == parent){
    return;
  }

  /* wait parent */
  current_parent = ags_thread_parent(thread);

  if(current_parent != NULL){
    current_thread = current_parent;
    
    while(current_thread != parent){
      g_mutex_lock(AGS_THREAD_GET_WAIT_MUTEX(current_thread));
      
      while(ags_thread_test_status_flags(current_thread, AGS_THREAD_STATUS_IDLE) ||
	    !ags_thread_test_status_flags(current_thread, AGS_THREAD_STATUS_WAITING_FOR_CHILDREN)){
	g_cond_wait(AGS_THREAD_GET_WAIT_COND(current_thread),
		    AGS_THREAD_GET_WAIT_MUTEX(current_thread));
      }

      g_mutex_unlock(AGS_THREAD_GET_WAIT_MUTEX(current));

      /* ascend */
      current_parent = ags_thread_parent(current_thread);

      g_object_unref(current_thread);

      current_thread = current_parent;
    }

    if(current_thread != NULL){
      g_object_unref(current_thread);
    }
  }

  /* unset flag */
  ags_thread_unset_status_flags(thread, AGS_THREAD_STATUS_WAITING_FOR_PARENT);
}

/**
 * ags_thread_wait_sibling:
 * @thread: the #AgsThread
 *
 * Wait on sibling tree structure.
 *
 * Since: 3.0.0
 */
void
ags_thread_wait_sibling(AgsThread *thread)
{
  AgsThread *first_thread, *current_thread, *next_thread;

  if(!AGS_IS_THREAD(thread)){
    return;
  }

  /* wait sibling */
  current = ags_thread_first(thread);
  
  if(first_thread){
    current_thread = first_thread;
    g_object_ref(current_thread);
    
    while(current_thread != NULL){
      if(current_thread != thread){
	g_mutex_lock(AGS_THREAD_GET_WAIT_MUTEX(current_thread));

	while(ags_thread_test_status_flags(current_thread, AGS_THREAD_STATUS_IDLE) ||
	      !ags_thread_test_status_flags(current_thread, AGS_THREAD_STATUS_WAITING_FOR_SIBLING)){   
	  g_cond_wait(AGS_THREAD_GET_WAIT_COND(current_thread),
		      AGS_THREAD_GET_WAIT_MUTEX(current_thread));    
	}

	g_mutex_unlock(AGS_THREAD_GET_WAIT_MUTEX(current_thread));
      }

      /* iterate */
      next_thread = ags_thread_next(current_thread);

      g_object_unref(current_thread);

      current_thread = next_thread;
    }

    g_object_unref(first_thread);
  }

  /* unset flags */
  ags_thread_unset_status_flags(thread, AGS_THREAD_STATUS_WAITING_FOR_SIBLING);
}

void
ags_thread_wait_children_recursive(AgsThread *child)
{
  AgsThread *children, *current_child, *next_child;

  if(!AGS_IS_THREAD(child)){
    return;
  }
  
  children = ags_thread_children(child);

  if(children != NULL){
    current_child = children;
    g_object_ref(current_child);
    
    while(current_child != NULL){
      ags_thread_wait_children_recursive(current_child);

      /* iterate */
      next_child = ags_thread_next(current_child);

      g_object_unref(current_child);
      
      current_child = next_child;
    }

    g_object_unref(children);
  }

  g_mutex_lock(AGS_THREAD_GET_WAIT_MUTEX(child));

  while(ags_thread_test_status_flags(child, AGS_THREAD_STATUS_IDLE) ||
	!ags_thread_test_status_flags(child, AGS_THREAD_STATUS_WAITING_FOR_PARENT)){
    g_cond_wait(AGS_THREAD_GET_WAIT_COND(child),
		AGS_THREAD_GET_WAIT_MUTEX(child));
  }

  g_mutex_unlock(AGS_THREAD_GET_WAIT_MUTEX(child));
}

/**
 * ags_thread_wait_children:
 * @thread: the #AgsThread
 *
 * Wait on child tree structure.
 *
 * Since: 3.0.0
 */
void
ags_thread_wait_children(AgsThread *thread)
{
  AgsThread *children, *current_child, *next_child;

  if(!AGS_IS_THREAD(thread)){
    return;
  }

  children = ags_thread_children(child);

  if(children != NULL){
    current_child = children;
    g_object_ref(current_child);
    
    while(current_child != NULL){
      ags_thread_wait_children_recursive(current_child);

      /* iterate */
      next_child = ags_thread_next(current_child);

      g_object_unref(current_child);
      
      current_child = next_child;
    }

    g_object_unref(children);
  }

  /* unset flags */
  ags_thread_unset_status_flags(thread, AGS_THREAD_STATUS_WAITING_FOR_CHILDREN);
}

/**
 * ags_thread_signal_parent:
 * @thread: the #AgsThread
 * @parent: the parent #AgsThread
 * @broadcast: whether to perforam a signal or to broadcast
 *
 * Signals the tree in higher levels.
 *
 * Since: 2.0.0
 */
void
ags_thread_signal_parent(AgsThread *thread, AgsThread *parent,
			 gboolean broadcast)
{
  AgsThread *current;

  current = g_atomic_pointer_get(&(thread->parent));

  while(current != NULL){
    if((AGS_THREAD_WAIT_FOR_CHILDREN & (g_atomic_int_get(&(current->sync_flags)))) != 0){
      if(!broadcast){
	pthread_cond_signal(current->cond);
      }else{
	pthread_cond_broadcast(current->cond);
      }
    }

    current = g_atomic_pointer_get(&(current->parent));
  }
}

/**
 * ags_thread_signal_sibling:
 * @thread: the #AgsThread
 * @broadcast: whether to perforam a signal or to broadcast
 *
 * Signals the tree on same level.
 *
 * Since: 2.0.0
 */
void
ags_thread_signal_sibling(AgsThread *thread, gboolean broadcast)
{
  AgsThread *current;

  current = ags_thread_first(thread);

  while(current != NULL){
    if((AGS_THREAD_WAIT_FOR_SIBLING & (g_atomic_int_get(&(current->sync_flags)))) != 0){
      if(!broadcast){
	pthread_cond_signal(current->cond);
      }else{
	pthread_cond_broadcast(current->cond);
      }
    }

    current = g_atomic_pointer_get(&(current->next));
  }
}

/**
 * ags_thread_signal_children:
 * @thread: the #AgsThread
 * @broadcast: whether to perforam a signal or to broadcast
 *
 * Signals the tree in lower levels.
 *
 * Since: 2.0.0
 */
void
ags_thread_signal_children(AgsThread *thread, gboolean broadcast)
{
  auto void ags_thread_signal_children_recursive(AgsThread *thread, gboolean broadcast);

  void ags_thread_signal_children_recursive(AgsThread *thread, gboolean broadcast){
    AgsThread *current;

    if(thread == NULL){
      return;
    }

    current = thread;

    while(current != NULL){
      if((AGS_THREAD_WAIT_FOR_PARENT & (g_atomic_int_get(&(current->sync_flags)))) != 0){
	if(!broadcast){
	  pthread_cond_signal(current->cond);
	}else{
	  pthread_cond_broadcast(current->cond);
	}
      }
      
      ags_thread_signal_children_recursive(current, broadcast);

      current = g_atomic_pointer_get(&(current->next));
    }
  }

  ags_thread_signal_children(g_atomic_pointer_get(&(thread->children)), broadcast);
}

guint
ags_thread_real_clock(AgsThread *thread)
{
  AgsThread *main_loop, *async_queue;

#ifdef __APPLE__
  clock_serv_t cclock;
  mach_timespec_t mts;
#endif
  
  struct timespec time_now;

  gdouble main_loop_delay;
  gdouble delay_per_hertz;
  guint steps;
  gboolean async_queue_running;
  
  GRecMutex *application_mutex;
  GRecMutex *mutex, *main_loop_mutex;
  pthread_cond_t *run_cond;
  GRecMutex *run_mutex;

  auto void ags_thread_clock_sync(AgsThread *thread);
  auto void ags_thread_clock_wait_async();
    
  void ags_thread_clock_sync(AgsThread *thread){
    guint next_tic, current_tic;
    
    guint sync_tic, next_sync_tic;
    gboolean skip_sync_counter;
    
    /* sync */
    skip_sync_counter = FALSE;

    if(thread != main_loop){
      g_atomic_int_or(&(thread->sync_flags),
		      AGS_THREAD_MARK_SYNCED);
    }
    
    pthread_mutex_lock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));
    
    if((AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(thread->flags)))) != 0){
      if((AGS_THREAD_IMMEDIATE_SYNC & (g_atomic_int_get(&(thread->flags)))) == 0){
	skip_sync_counter = TRUE;
      }
      
      thread->current_tic = ags_main_loop_get_tic(AGS_MAIN_LOOP(main_loop));

      if((AGS_THREAD_START_SYNCED_FREQ & (g_atomic_int_get(&(thread->flags)))) != 0 &&
	 g_atomic_pointer_get(&(thread->parent)) != NULL){
	AgsThread *chaos_tree;
	
	chaos_tree = main_loop;//ags_thread_chaos_tree(thread);

	/* ordinary sync */
	thread->tic_delay = chaos_tree->tic_delay;

	if((AGS_THREAD_INTERMEDIATE_PRE_SYNC & (g_atomic_int_get(&(thread->flags)))) != 0){
	  if(thread->freq >= thread->max_precision){
	    thread->tic_delay = 0;
	  }else{
	    if(thread->tic_delay < thread->delay){
	      thread->tic_delay++;
	    }else{
	      thread->tic_delay = 0;
	    }
	  }
	}
	
	if((AGS_THREAD_INTERMEDIATE_POST_SYNC & (g_atomic_int_get(&(thread->flags)))) != 0){
	  if(thread->freq >= thread->max_precision){
	    thread->tic_delay = 0;
	  }else{
	    if(thread->tic_delay > 0){
	      thread->tic_delay--;
	    }else{
	      thread->tic_delay = thread->delay;
	    }
	  }
	}
	
	g_atomic_int_or(&(thread->flags),
			AGS_THREAD_SYNCED_FREQ);
      }
      
      g_atomic_int_and(&(thread->flags),
		       (~AGS_THREAD_INITIAL_RUN));
    }

    sync_tic = ags_main_loop_get_sync_tic(AGS_MAIN_LOOP(main_loop));
    next_sync_tic = 0;

    if(sync_tic == 5){
      next_sync_tic = 0;
    }else if(sync_tic == 0){
      next_sync_tic = 1;
    }else if(sync_tic == 1){
      next_sync_tic = 2;
    }else if(sync_tic == 2){
      next_sync_tic = 3;
    }else if(sync_tic == 4){
      next_sync_tic = 5;
    }

    /* thread tree */
    current_tic = thread->current_tic;
    next_tic = 0;
    
    if(current_tic == 2){
      next_tic = 0;
    }else if(current_tic == 0){
      next_tic = 1;
    }else if(current_tic == 1){
      next_tic = 2;
    }

    if(ags_thread_global_get_use_sync_counter()){
      ags_main_loop_sync_counter_inc(AGS_MAIN_LOOP(main_loop),
				     next_sync_tic);

      if(!skip_sync_counter){
	ags_main_loop_sync_counter_dec(AGS_MAIN_LOOP(main_loop),
				       sync_tic);
      }
    }else{
      sync_tic = current_tic;
    }

    switch(thread->current_tic){
    case 0:
      {
	g_atomic_int_or(&(thread->sync_flags),
			AGS_THREAD_WAIT_0);
      }
      break;
    case 1:
      {
	g_atomic_int_or(&(thread->sync_flags),
			AGS_THREAD_WAIT_1);
      }
      break;
    case 2:
      {
	g_atomic_int_or(&(thread->sync_flags),
			AGS_THREAD_WAIT_2);
      }
      break;
    }
    
    if(!ags_thread_is_tree_ready(thread,
				 sync_tic)){
      //      ags_thread_hangcheck(main_loop);
      pthread_mutex_unlock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));
      
      if(!ags_thread_is_current_ready(thread,
				      current_tic)){
	pthread_mutex_lock(thread->mutex);

	g_atomic_int_or(&(thread->flags),
			AGS_THREAD_WAITING);
	
	while(!ags_thread_is_current_ready(thread,
					   current_tic) &&
	      (AGS_THREAD_WAITING & (g_atomic_int_get(&(thread->flags)))) != 0){
	  pthread_cond_wait(thread->cond,
			    thread->mutex);
	}

	pthread_mutex_unlock(thread->mutex);
      }
    }else{      
      /* async-queue */
      if(async_queue != NULL){
	if((AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(async_queue->flags)))) == 0){
	  ags_async_queue_set_run(AGS_ASYNC_QUEUE(async_queue),
				  FALSE);
	}
	
	async_queue_running = ((AGS_THREAD_RUNNING & (g_atomic_int_get(&(async_queue->flags)))) != 0) ? TRUE: FALSE;
      }else{
	async_queue_running = FALSE;
      }
      
      ags_main_loop_set_last_sync(AGS_MAIN_LOOP(main_loop), thread->current_tic);
      ags_main_loop_set_tic(AGS_MAIN_LOOP(main_loop), next_tic);
      ags_main_loop_set_sync_tic(AGS_MAIN_LOOP(main_loop), next_sync_tic);

      ags_thread_set_sync_all(main_loop, thread->current_tic);

      ags_thread_reset_all(main_loop);

      pthread_mutex_unlock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));
    }
  }
  
  void ags_thread_clock_wait_async(){
    if(async_queue == NULL){
      return;
    }
    
    /* async-queue */
    if(!AGS_IS_ASYNC_QUEUE(thread)){
      if(async_queue_running){
	pthread_mutex_lock(run_mutex);
	//	g_message("blocked");
	
	if(!ags_async_queue_is_run(AGS_ASYNC_QUEUE(async_queue)) &&
	   (AGS_THREAD_DONE_ASYNC_QUEUE & (g_atomic_int_get(&(thread->sync_flags)))) == 0){
	  ags_async_queue_increment_wait_ref(AGS_ASYNC_QUEUE(async_queue));
	  //	  g_message("wait");
	  
	  g_atomic_int_or(&(thread->sync_flags),
			  AGS_THREAD_WAIT_ASYNC_QUEUE);
	  
	  while(!ags_async_queue_is_run(AGS_ASYNC_QUEUE(async_queue)) &&
		ags_async_queue_get_wait_ref(AGS_ASYNC_QUEUE(async_queue)) != 0 &&
		(AGS_THREAD_WAIT_ASYNC_QUEUE & (g_atomic_int_get(&(thread->sync_flags)))) != 0 &&
		(AGS_THREAD_DONE_ASYNC_QUEUE & (g_atomic_int_get(&(thread->sync_flags)))) == 0){
	    pthread_cond_wait(run_cond,
			      run_mutex);
	  }
	}

	g_atomic_int_and(&(thread->sync_flags),
			 (~(AGS_THREAD_WAIT_ASYNC_QUEUE |
			    AGS_THREAD_DONE_ASYNC_QUEUE)));

	pthread_mutex_unlock(run_mutex);
      }
    }else{
      //      g_message("not blocked");
    }
  }

  main_loop = ags_thread_get_toplevel(thread);

  main_loop_mutex = NULL;
  
  if(!AGS_IS_MAIN_LOOP(main_loop)){
    main_loop = NULL;
  }

  /* mutex */
  if(main_loop != NULL){
    main_loop_mutex = AGS_THREAD_GET_OBJ_MUTEX(main_loop);
  }
  
  mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  if(!AGS_IS_MAIN_LOOP(main_loop)){
    if(thread->tic_delay == 0){
      if(thread->freq >= thread->max_precision){
	steps = 1.0 / thread->delay;
      }else{
	steps = 1;
      }
    }else{
      steps = 0;
    }
    
    return(steps);
  }
  
  steps = 0;

  /* async-queue */
  if(AGS_IS_MAIN_LOOP(main_loop)){
    async_queue = (AgsThread *) ags_main_loop_get_async_queue(AGS_MAIN_LOOP(main_loop));
    async_queue_running = FALSE;

    if(async_queue != NULL){
      run_mutex = ags_async_queue_get_run_mutex(AGS_ASYNC_QUEUE(async_queue));
      run_cond = ags_async_queue_get_run_cond(AGS_ASYNC_QUEUE(async_queue));
    }
  }else{
    async_queue = NULL;
  }
  
  /* calculate main loop delay */
  pthread_mutex_lock(main_loop_mutex);
  
  main_loop_delay = main_loop->max_precision / main_loop->freq / (AGS_THREAD_HERTZ_JIFFIE / main_loop->max_precision);
  
  pthread_mutex_unlock(main_loop_mutex);
  
  /* calculate thread delay */
  pthread_mutex_lock(mutex);

  thread->delay = (guint) ceil((AGS_THREAD_HERTZ_JIFFIE / thread->freq) / (AGS_THREAD_HERTZ_JIFFIE / thread->max_precision));
  delay_per_hertz = AGS_THREAD_HERTZ_JIFFIE / thread->max_precision;

  if(thread->delay < thread->tic_delay){
    thread->tic_delay = thread->delay;
  }
  
  if(g_atomic_pointer_get(&(thread->parent)) == NULL){
    //    g_message("%d %d %f", thread->tic_delay, thread->delay, thread->freq);
  }  
  
  pthread_mutex_unlock(mutex);

  /* check initial sync or immediate sync */
  if((AGS_THREAD_IMMEDIATE_SYNC & (g_atomic_int_get(&(thread->flags)))) == 0){
    if((AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(thread->flags)))) != 0 &&
       (AGS_THREAD_INITIAL_SYNC & (g_atomic_int_get(&(thread->flags)))) != 0){
#ifdef __APPLE__
      host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
      
      clock_get_time(cclock, &mts);
      mach_port_deallocate(mach_task_self(), cclock);
      
      thread->computing_time->tv_sec = mts.tv_sec;
      thread->computing_time->tv_nsec = mts.tv_nsec;
#else
      clock_gettime(CLOCK_MONOTONIC, thread->computing_time);
#endif

      g_atomic_int_and(&(thread->flags),
		       (~AGS_THREAD_INITIAL_SYNC));

      return(1);
    }
  }else{
    if((AGS_THREAD_INITIAL_SYNC & (g_atomic_int_get(&(thread->flags)))) != 0){
      g_atomic_int_and(&(thread->flags),
		      (~AGS_THREAD_INITIAL_SYNC));
    }
  }
  
  /* idle */
#ifdef AGS_USE_TIMER
  if(g_atomic_pointer_get(&(thread->parent)) == NULL){
    pthread_mutex_lock(thread->timer_mutex);
    
    if(!g_atomic_int_get(&(thread->timer_expired))){
      g_atomic_int_set(&(thread->timer_wait),
		       TRUE);
      
      while(!g_atomic_int_get(&(thread->timer_expired))){
	pthread_cond_wait(thread->timer_cond,
			  thread->timer_mutex);
      }
    }

    g_atomic_int_set(&(thread->timer_wait),
		     FALSE);
    g_atomic_int_set(&(thread->timer_expired),
		     FALSE);
	
    pthread_mutex_unlock(thread->timer_mutex);
  }
#else
  if(thread->tic_delay == thread->delay &&
     (AGS_THREAD_TIMING & (g_atomic_int_get(&(thread->flags)))) != 0){
    gdouble time_spent, relative_time_spent;
    gdouble time_cycle;

    static const gdouble nsec_per_jiffie = NSEC_PER_SEC / AGS_THREAD_HERTZ_JIFFIE;
    
    struct timespec timed_sleep = {
      0,
      0,
    };

#ifdef __APPLE__
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    
    time_now.tv_sec = mts.tv_sec;
    time_now.tv_nsec = mts.tv_nsec;
#else
    clock_gettime(CLOCK_MONOTONIC, &time_now);
#endif
      
    if(time_now.tv_sec == thread->computing_time->tv_sec + 1){
      time_spent = (time_now.tv_nsec) + (NSEC_PER_SEC - thread->computing_time->tv_nsec);
    }else if(time_now.tv_sec > thread->computing_time->tv_sec + 1){
      time_spent = (time_now.tv_sec - thread->computing_time->tv_sec) * NSEC_PER_SEC;
      time_spent += (time_now.tv_nsec - thread->computing_time->tv_nsec);
    }else{
      time_spent = time_now.tv_nsec - thread->computing_time->tv_nsec;
    }

    time_cycle = NSEC_PER_SEC / thread->freq;
      
    relative_time_spent = time_cycle - time_spent - AGS_THREAD_TOLERANCE;

    if(relative_time_spent - (gdouble) g_atomic_int_get(&(thread->time_late)) < 0.0){
      if(g_atomic_int_get(&(thread->time_late)) - (gint) ceil(1.25 * relative_time_spent) < 0){
	g_atomic_int_set(&(thread->time_late),
			 0);
      }else{
	g_atomic_int_set(&(thread->time_late),
			 g_atomic_int_get(&(thread->time_late)) - (gint) ceil(1.25 * relative_time_spent));
      }
    }else if(relative_time_spent > 0.0 &&
	     relative_time_spent - (gdouble) g_atomic_int_get(&(thread->time_late)) < (44.0 / 45.0) * time_cycle){
      g_atomic_int_set(&(thread->time_late),
		       0);
      timed_sleep.tv_nsec = (long) relative_time_spent - (gdouble) g_atomic_int_get(&(thread->time_late)) - (1.0 / 45.0) * time_cycle;
      
      nanosleep(&timed_sleep, NULL);
    }else{
      g_atomic_int_set(&(thread->time_late),
		       0);
    }

#ifdef __APPLE__
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    
    thread->computing_time->tv_sec = mts.tv_sec;
    thread->computing_time->tv_nsec = mts.tv_nsec;
#else
    clock_gettime(CLOCK_MONOTONIC, thread->computing_time);
#endif
  }
#endif
  
  //  g_message("%s", G_OBJECT_TYPE_NAME(thread));

  /* sync */  
  /* run in hierarchy */
  //  pthread_mutex_lock(thread->mutex);
  ags_thread_clock_sync(thread);

  ags_thread_clock_wait_async();
    
  //  pthread_mutex_unlock(thread->mutex);
  /* increment delay counter and set run per cycle */
  if(thread->tic_delay == 0){
    if(thread->freq >= thread->max_precision){
      steps = 1.0 / thread->delay;
    }else{
      steps = 1;
    }
  }else{
    steps = 0;
  }

  /* nth cycle */
  if(thread->cycle_iteration != thread->max_precision){
    thread->cycle_iteration++;
  }else{
    thread->cycle_iteration = 0;
  }
  
  return(steps);
}

/**
 * ags_thread_clock:
 * @thread: the #AgsThread instance
 *
 * Clock the thread.
 *
 * Returns: the cycles to be performed
 * 
 * Since: 2.0.0
 */
guint
ags_thread_clock(AgsThread *thread)
{
  guint thread_signal;
  guint cycles;
  
  thread_signal = thread_signals[CLOCK];  
  g_return_val_if_fail(AGS_IS_THREAD(thread), 0);

  g_object_ref(thread);
  g_signal_emit(thread,
		thread_signal, 0,
		&cycles);
  g_object_unref(thread);

  return(cycles);
}

void
ags_thread_real_start(AgsThread *thread)
{
  AgsMainLoop *main_loop;
  guint val;

  main_loop = ags_thread_get_toplevel(thread);

  if(!AGS_IS_MAIN_LOOP(main_loop)){
    main_loop = NULL;
  }
  
  /* set/unset run flags */
  g_atomic_int_or(&(thread->flags),
		  (AGS_THREAD_RUNNING |
		   AGS_THREAD_INITIAL_RUN |
		   AGS_THREAD_INITIAL_SYNC));

  g_atomic_int_and(&(thread->flags),
		   (~(AGS_THREAD_RT_SETUP |
		      AGS_THREAD_WAITING)));

  g_atomic_int_and(&(thread->sync_flags),
		   (~(AGS_THREAD_WAIT_0 |
		      AGS_THREAD_WAIT_1 |
		      AGS_THREAD_WAIT_2)));

  g_atomic_int_and(&(thread->sync_flags),
		   (~(AGS_THREAD_MARK_SYNCED |
		      AGS_THREAD_SYNCED)));

  g_atomic_int_and(&(thread->sync_flags),
		   (~(AGS_THREAD_SYNCED_FREQ)));

#ifdef AGS_DEBUG
  g_message("thread start: %s", G_OBJECT_TYPE_NAME(thread));
#endif

  /* */
  val = g_atomic_int_get(&(thread->sync_flags));
  
  if((AGS_THREAD_TIMELOCK_RUN & val) != 0){
    pthread_create(thread->timelock_thread, NULL,
    		   ags_thread_timelock_loop, thread);
  }

  /*  */
  pthread_create(thread->thread, thread->thread_attr,
		 ags_thread_loop, thread);
}

/**
 * ags_thread_add_start_queue:
 * @thread: the #AgsThread
 * @child: the child #AgsThread to start
 *
 * Add @child to @thread's start queue.
 *
 * Since: 2.0.0
 */
void
ags_thread_add_start_queue(AgsThread *thread,
			   AgsThread *child)
{
  GRecMutex *mutex;

  if(!AGS_IS_THREAD(thread) ||
     !AGS_IS_THREAD(child)){
    return;
  }

  /* mutex */
  mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  /* add */
  pthread_mutex_lock(mutex);
  
  g_atomic_pointer_set(&(thread->start_queue),
		       g_list_prepend(g_atomic_pointer_get(&(thread->start_queue)),
				      child));
  
  pthread_mutex_unlock(mutex);
}

/**
 * ags_thread_add_start_queue_all:
 * @thread: the #AgsThread
 * @child: the children as #GList-struct containing #AgsThread to start
 *
 * Add @child to @thread's start queue.
 *
 * Since: 2.0.0
 */
void
ags_thread_add_start_queue_all(AgsThread *thread,
			       GList *child)
{
  GList *start_queue;
  
  GRecMutex *mutex;

  if(!AGS_IS_THREAD(thread) ||
     child == NULL){
    return;
  }

  /* mutex */
  mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  /* add all */
  pthread_mutex_lock(mutex);
  
  start_queue = g_atomic_pointer_get(&(thread->start_queue));

  if(start_queue == NULL){
    g_atomic_pointer_set(&(thread->start_queue),
			 g_list_copy(child));
  }else{
    start_queue = g_list_concat(start_queue,
				g_list_copy(child));
    
    g_atomic_pointer_set(&(thread->start_queue),
			 start_queue);
  }
    
  pthread_mutex_unlock(mutex);
}

/**
 * ags_thread_is_running:
 * @thread: the #AgsThread instance
 *
 * Query running flag.
 *
 * Since: 2.0.0
 */
gboolean
ags_thread_is_running(AgsThread *thread)
{
  if(!AGS_IS_THREAD(thread)){
    return(FALSE);
  }
  
  return(((AGS_THREAD_RUNNING & (g_atomic_int_get(&(thread->flags)))) ? TRUE: FALSE));
}

/**
 * ags_thread_start:
 * @thread: the #AgsThread instance
 *
 * Start the thread.
 *
 * Since: 2.0.0
 */
void
ags_thread_start(AgsThread *thread)
{
  guint thread_signal;
  
  thread_signal = thread_signals[START];
  g_return_if_fail(AGS_IS_THREAD(thread));

  if((AGS_THREAD_RUNNING & (g_atomic_int_get(&(thread->flags)))) != 0){
    return;
  }
  
  g_object_ref(thread);
  g_signal_emit(thread,
		thread_signal, 0);
  g_object_unref(thread);
}

void*
ags_thread_timer(void *ptr)
{
  //TODO:JK: implement me

  return(NULL);
}

void*
ags_thread_loop(void *ptr)
{
  AgsThread *thread, *main_loop;
  AgsThread *async_queue;
  AgsThread *queued_thread;
  
  GList *start_start_queue, *start_queue, *start_queue_next;

  guint val, running, locked_greedy;
  guint i, i_stop;
  gboolean wait_for_parent, wait_for_sibling, wait_for_children;
  guint sync_tic, next_sync_tic;
  gboolean skip_sync_counter;
  
  GRecMutex *mutex, *main_loop_mutex;
  
  thread = (AgsThread *) ptr;
  g_object_ref(thread);
  
  pthread_once(&ags_thread_key_once, ags_thread_self_create);
  pthread_setspecific(ags_thread_key, thread);
  
  main_loop = ags_thread_get_toplevel(thread);
  main_loop_mutex = NULL;
  
  if(!AGS_IS_MAIN_LOOP(main_loop)){
    main_loop = NULL;
  }

  /* async queue and mutex */
  async_queue = NULL;
  
  if(main_loop != NULL){
    main_loop_mutex = AGS_THREAD_GET_OBJ_MUTEX(main_loop);

    async_queue = (AgsThread *) ags_main_loop_get_async_queue(AGS_MAIN_LOOP(main_loop));
  }
  
  mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);
  
  if(main_loop != NULL){
    pthread_mutex_lock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));
    
    thread->current_tic = ags_main_loop_get_tic(AGS_MAIN_LOOP(main_loop));
    
    pthread_mutex_unlock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));  
  }else{
    thread->current_tic = 0;
  }
  
  running = g_atomic_int_get(&(thread->flags));
  
  /* get start computing time */
#ifndef AGS_USE_TIMER
  if(g_atomic_pointer_get(&(thread->parent)) == NULL){
#ifdef __APPLE__
    clock_serv_t cclock;
    mach_timespec_t mts;
#endif

    thread->delay = 
      thread->tic_delay = (AGS_THREAD_HERTZ_JIFFIE / thread->freq) / (AGS_THREAD_HERTZ_JIFFIE / thread->max_precision);
#ifdef __APPLE__
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);

    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);

    thread->computing_time->tv_sec = mts.tv_sec;
    thread->computing_time->tv_nsec = mts.tv_nsec;
#else
    clock_gettime(CLOCK_MONOTONIC, thread->computing_time);
#endif
  }
#endif
  
  while((AGS_THREAD_RUNNING & running) != 0){
    /* start queue */
    pthread_mutex_lock(mutex);

    start_start_queue = 
      start_queue = g_atomic_pointer_get(&(thread->start_queue));
    g_atomic_pointer_set(&(thread->start_queue),
			 NULL);

    pthread_mutex_unlock(mutex);
    
    while(start_queue != NULL){
      start_queue_next = start_queue->next;
      
      queued_thread = (AgsThread *) start_queue->data;
      g_object_ref(queued_thread);
      
      /*  */
      ags_thread_start(queued_thread);

      pthread_mutex_lock(queued_thread->start_mutex);

      if(g_atomic_int_get(&(queued_thread->start_done)) == FALSE){

	if(main_loop != NULL){
	  pthread_mutex_lock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));
	}
	
	/* ordinary sync */
	queued_thread->tic_delay = thread->tic_delay;

	if((AGS_THREAD_INTERMEDIATE_PRE_SYNC & (g_atomic_int_get(&(queued_thread->flags)))) != 0){
	  if(queued_thread->freq >= queued_thread->max_precision){
	    queued_thread->tic_delay = 0;
	  }else{
	    if(queued_thread->tic_delay < queued_thread->delay){
	      queued_thread->tic_delay++;
	    }else{
	      queued_thread->tic_delay = 0;
	    }
	  }
	}
	
	if((AGS_THREAD_INTERMEDIATE_POST_SYNC & (g_atomic_int_get(&(queued_thread->flags)))) != 0){
	  if(queued_thread->freq >= queued_thread->max_precision){
	    queued_thread->tic_delay = 0;
	  }else{
	    if(queued_thread->tic_delay > 0){
	      queued_thread->tic_delay--;
	    }else{
	      queued_thread->tic_delay = queued_thread->delay;
	    }
	  }
	}

	/*  */
	if(main_loop != NULL){
	  pthread_mutex_unlock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));
	}
	
	g_atomic_int_set(&(queued_thread->start_wait),
			 TRUE);
      
	while(g_atomic_int_get(&(queued_thread->start_wait)) == TRUE &&
	      g_atomic_int_get(&(queued_thread->start_done)) == FALSE){
	  pthread_cond_wait(queued_thread->start_cond,
			    queued_thread->start_mutex);
	}
      }
      
      pthread_mutex_unlock(queued_thread->start_mutex);
      g_object_unref(queued_thread);
      
      start_queue = start_queue_next;
    }

    g_list_free(start_start_queue);
    
    /* notify start */
    /* signal AgsMainLoop */
    pthread_mutex_lock(thread->start_mutex);
      
    g_atomic_int_set(&(thread->start_done),
		     TRUE);    
      
    if(g_atomic_int_get(&(thread->start_wait)) == TRUE){
      pthread_cond_broadcast(thread->start_cond);
    }
      
    pthread_mutex_unlock(thread->start_mutex);

    /* barrier */
    if((AGS_THREAD_WAITING_FOR_BARRIER & (g_atomic_int_get(&(thread->sync_flags)))) != 0){
      int wait_count;

      if(thread->first_barrier){
	/* retrieve wait count */
	ags_thread_lock(thread);

	wait_count = thread->wait_count[0];

	ags_thread_unlock(thread);

	/* init and wait */
#ifndef __APPLE__
	pthread_barrier_init(thread->barrier[0], NULL, wait_count);
	pthread_barrier_wait(thread->barrier[0]);
#endif
      }else{
	/* retrieve wait count */
	ags_thread_lock(thread);

	wait_count = thread->wait_count[1];

	ags_thread_unlock(thread);

	/* init and wait */
#ifndef __APPLE__
	pthread_barrier_init(thread->barrier[1], NULL, wait_count);
	pthread_barrier_wait(thread->barrier[1]);
#endif
      }
    }
      
    /* run clock synchronization */
    i_stop = ags_thread_clock(thread);

    /* if still running */    
    running = g_atomic_int_get(&(thread->flags));

    for(i = 0; i < i_stop && (AGS_THREAD_RUNNING & running) != 0; i++){
      /* do cycles */
      running = g_atomic_int_get(&(thread->flags));

      /* set idle flag */
      g_atomic_int_or(&(thread->flags),
		      AGS_THREAD_IDLE);

      if((AGS_THREAD_WAIT_FOR_PARENT & (g_atomic_int_get(&(thread->sync_flags)))) != 0){
	wait_for_parent = TRUE;

	g_atomic_int_or(&(thread->sync_flags),
			AGS_THREAD_WAITING_FOR_PARENT);
	ags_thread_lock_parent(thread, NULL);
      }else{
	wait_for_parent = FALSE;
      }

      /* lock sibling */
      if((AGS_THREAD_WAIT_FOR_SIBLING & (g_atomic_int_get(&(thread->sync_flags)))) != 0){
	wait_for_sibling = TRUE;

	g_atomic_int_or(&(thread->sync_flags),
			AGS_THREAD_WAITING_FOR_SIBLING);
	ags_thread_lock_sibling(thread);
      }else{
	wait_for_sibling = FALSE;
      }

      /* lock_children */
      if((AGS_THREAD_WAIT_FOR_CHILDREN & (g_atomic_int_get(&(thread->sync_flags)))) != 0){
	wait_for_children = TRUE;

	g_atomic_int_or(&(thread->sync_flags),
			AGS_THREAD_WAITING_FOR_CHILDREN);
	ags_thread_lock_children(thread);
      }else{
	wait_for_children = FALSE;
      }

      /* skip very first sync of AgsAudioLoop */
      if((AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(thread->flags)))) == 0 &&
	 !AGS_IS_MAIN_LOOP(thread)){

	/* wait parent */
	if(wait_for_parent){
	  ags_thread_wait_parent(thread, NULL);
	}

	/* wait sibling */
	if(wait_for_sibling){
	  ags_thread_wait_sibling(thread);
	}

	/* wait children */
	if(wait_for_children){
	  ags_thread_wait_children(thread);
	}
      }

      /* check for greedy to announce */
      if(thread->greedy_locks != NULL){
	GList *greedy_locks;

	greedy_locks = thread->greedy_locks;
      
	while(greedy_locks != NULL){
	  pthread_mutex_lock(AGS_THREAD(greedy_locks->data)->greedy_mutex);

	  locked_greedy = g_atomic_int_get(&(AGS_THREAD(greedy_locks->data)->locked_greedy));
	  locked_greedy++;

	  g_atomic_int_set(&(AGS_THREAD(greedy_locks->data)->locked_greedy),
			   locked_greedy);

	  pthread_mutex_unlock(AGS_THREAD(greedy_locks->data)->greedy_mutex);

	  greedy_locks = greedy_locks->next;
	}
      }

      /* greedy work around */
      pthread_mutex_lock(thread->greedy_mutex);

      locked_greedy = g_atomic_int_get(&(thread->locked_greedy));

      if(locked_greedy != 0){
	while(locked_greedy != 0){
	  pthread_cond_wait(thread->greedy_cond,
			    thread->greedy_mutex);
	
	  locked_greedy = g_atomic_int_get(&(thread->locked_greedy));
	}
      }

      pthread_mutex_unlock(thread->greedy_mutex);
      
      /* run */
      if((AGS_THREAD_INTERRUPTED & (g_atomic_int_get(&(thread->sync_flags)))) == 0 &&
	 (AGS_THREAD_MONITORING & (g_atomic_int_get(&(thread->sync_flags)))) == 0){
	/* timed lock (suspend) */
	pthread_mutex_lock(thread->timelock_mutex);

	val = g_atomic_int_get(&(thread->sync_flags));

	if((AGS_THREAD_TIMELOCK_RUN & val) != 0){
	  guint locked;

	  locked = g_atomic_int_get(&(thread->sync_flags));
				
	  g_atomic_int_and(&(thread->sync_flags),
			   (~AGS_THREAD_TIMELOCK_WAIT));

	  if((AGS_THREAD_TIMELOCK_WAIT & locked) != 0){	
	    pthread_cond_signal(thread->timelock_cond);
	  }
	}

	pthread_mutex_unlock(thread->timelock_mutex);
	
	ags_thread_run(thread);
      }
      //    g_printf("%s\n", G_OBJECT_TYPE_NAME(thread));

      /*  */
      running = g_atomic_int_get(&(thread->flags));

      /* check for greedy to release */
      if(thread->greedy_locks != NULL){
	GList *greedy_locks;

	greedy_locks = thread->greedy_locks;
      
	while(greedy_locks != NULL){
	  pthread_mutex_lock(AGS_THREAD(greedy_locks->data)->greedy_mutex);

	  locked_greedy = g_atomic_int_get(&(AGS_THREAD(greedy_locks->data)->locked_greedy));
	  locked_greedy--;

	  g_atomic_int_set(&(AGS_THREAD(greedy_locks->data)->locked_greedy),
			   locked_greedy);

	  pthread_cond_signal(AGS_THREAD(greedy_locks->data)->greedy_cond);

	  pthread_mutex_unlock(AGS_THREAD(greedy_locks->data)->greedy_mutex);

	  greedy_locks = greedy_locks->next;
	}
      }

      /* unset idle flag */
      g_atomic_int_and(&(thread->flags),
		       (~AGS_THREAD_IDLE));

      /* unlock parent */
      if(wait_for_parent){
	ags_thread_unlock_parent(thread, NULL);
      }

      /* unlock sibling */
      if(wait_for_sibling){
	ags_thread_unlock_sibling(thread);
      }

      /* unlock children */
      if(wait_for_children){
	ags_thread_unlock_children(thread);
      }
    }
    
    /* yield */
#if defined __APPLE__ || AGS_W32API
#else
    if(main_loop != NULL &&
       main_loop->tic_delay != main_loop->delay &&
       thread->cycle_iteration % (guint) floor(AGS_THREAD_HERTZ_JIFFIE / AGS_THREAD_YIELD_JIFFIE) == 0){
      pthread_yield();
    }
#endif
  }

  //  g_object_ref(thread);
  
  if(main_loop != NULL){
    pthread_mutex_lock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));
  }

  skip_sync_counter = FALSE;  
	
  if((AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(thread->flags)))) != 0){
    if((AGS_THREAD_IMMEDIATE_SYNC & (g_atomic_int_get(&(thread->flags)))) == 0){
      skip_sync_counter = TRUE;
    }
  }
  
  if(main_loop != NULL &&
     (AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(thread->flags)))) != 0  ||
     (AGS_THREAD_INITIAL_SYNC & (g_atomic_int_get(&(thread->flags)))) != 0){
    AgsThread *chaos_tree;
    
    chaos_tree = main_loop;//ags_thread_chaos_tree(thread);

    /* ordinary sync */
    thread->tic_delay = chaos_tree->tic_delay;

    if((AGS_THREAD_INTERMEDIATE_PRE_SYNC & (g_atomic_int_get(&(thread->flags)))) != 0){
      if(thread->freq >= thread->max_precision){
	thread->tic_delay = 0;
      }else{
	if(thread->tic_delay > 0){
	  thread->tic_delay--;
	}else{
	  thread->tic_delay = thread->delay;
	}
      }
    }
	
    if((AGS_THREAD_INTERMEDIATE_POST_SYNC & (g_atomic_int_get(&(thread->flags)))) != 0){
      if(thread->freq >= thread->max_precision){
	thread->tic_delay = 0;
      }else{
	if(thread->tic_delay < thread->delay){
	  thread->tic_delay++;
	}else{
	  thread->tic_delay = 0;
	}
      }
    }
	
    g_atomic_int_and(&(thread->flags),
		     (~AGS_THREAD_INITIAL_RUN));

  }

  sync_tic = ags_main_loop_get_sync_tic(AGS_MAIN_LOOP(main_loop));

  next_sync_tic = 0;

  if(sync_tic == 5){
    next_sync_tic = 0;
  }else if(sync_tic == 0){
    next_sync_tic = 1;
  }else if(sync_tic == 1){
    next_sync_tic = 2;
  }else if(sync_tic == 2){
    next_sync_tic = 3;
  }else if(sync_tic == 4){
    next_sync_tic = 5;
  }

  if(ags_thread_global_get_use_sync_counter()){
    if(!skip_sync_counter){
      ags_main_loop_sync_counter_dec(AGS_MAIN_LOOP(main_loop),
				     sync_tic);
    }
  }

  switch(thread->current_tic){
  case 0:
    {
      g_atomic_int_or(&(thread->sync_flags),
		      AGS_THREAD_WAIT_0);
    }
    break;
  case 1:
    {
      g_atomic_int_or(&(thread->sync_flags),
		      AGS_THREAD_WAIT_1);
    }
    break;
  case 2:
    {
      g_atomic_int_or(&(thread->sync_flags),
		      AGS_THREAD_WAIT_2);
    }
    break;
  }
    
  if(ags_thread_is_tree_ready(thread,
			      sync_tic)){
    guint next_tic, current_tic;

    current_tic = thread->current_tic;
    
    /* thread tree */
    next_tic = 0;
    
    if(current_tic == 2){
      next_tic = 0;
    }else if(current_tic == 0){
      next_tic = 1;
    }else if(current_tic == 1){
      next_tic = 2;
    }
    
    /* async-queue */
    if(async_queue != NULL){
      ags_async_queue_set_run(AGS_ASYNC_QUEUE(async_queue),
			      FALSE);
    }

    if(main_loop != NULL){
      ags_main_loop_set_last_sync(AGS_MAIN_LOOP(main_loop), thread->current_tic);
      ags_main_loop_set_tic(AGS_MAIN_LOOP(main_loop), next_tic);
      ags_main_loop_set_sync_tic(AGS_MAIN_LOOP(main_loop), next_sync_tic);
      
      ags_thread_set_sync_all(main_loop, thread->current_tic);
    }
  }
  
  if((AGS_THREAD_UNREF_ON_EXIT & (g_atomic_int_get(&(thread->flags)))) != 0){
    ags_thread_remove_child(g_atomic_pointer_get(&(thread->parent)),
			    thread);
    g_object_unref(thread);
  }

  if(main_loop != NULL){
    pthread_mutex_unlock(ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop)));
  }
  
#ifdef AGS_DEBUG
  g_message("thread finished");
#endif  
  
  g_object_unref(thread);
  //  g_object_ref(thread);
  
  /* exit thread */
  pthread_exit(NULL);

#ifdef AGS_W32API
  return(NULL);
#endif
}

/**
 * ags_thread_run:
 * @thread: the #AgsThread instance
 * 
 * Only for internal use of ags_thread_loop but you may want to set the your very own
 * class function namely your thread's routine.
 *
 * Since: 2.0.0
 */
void
ags_thread_run(AgsThread *thread)
{
  guint thread_signal;
  
  thread_signal = thread_signals[RUN];
  g_return_if_fail(AGS_IS_THREAD(thread));

  g_object_ref(thread);
  g_signal_emit(thread,
		thread_signal, 0);
  g_object_unref(thread);
}

void
ags_thread_suspend(AgsThread *thread)
{
  g_return_if_fail(AGS_IS_THREAD(thread));

  g_object_ref(G_OBJECT(thread));
  g_signal_emit(G_OBJECT(thread),
		thread_signals[SUSPEND], 0);
  g_object_unref(G_OBJECT(thread));
}

void
ags_thread_resume(AgsThread *thread)
{
  g_return_if_fail(AGS_IS_THREAD(thread));

  g_object_ref(G_OBJECT(thread));
  g_signal_emit(G_OBJECT(thread),
		thread_signals[RESUME], 0);
  g_object_unref(G_OBJECT(thread));
}

void*
ags_thread_timelock_loop(void *ptr)
{
  AgsThread *thread;
  int retval;
  guint flags, sync_flags;

  thread = AGS_THREAD(ptr);

  //FIXME:JK: not thread safe
  
  flags = g_atomic_int_get(&(thread->flags));
  
  sync_flags = g_atomic_int_get(&(thread->sync_flags));
  
  pthread_mutex_lock(thread->timelock_mutex);

  g_atomic_int_or(&(thread->sync_flags),
		  AGS_THREAD_TIMELOCK_WAIT);

  while((AGS_THREAD_RUNNING & (flags)) != 0){

    g_atomic_int_or(&(thread->sync_flags),
		    AGS_THREAD_TIMELOCK_WAIT);

    sync_flags = g_atomic_int_get(&(thread->sync_flags));

    while((AGS_THREAD_TIMELOCK_WAIT & (sync_flags)) != 0){
      retval = pthread_cond_wait(thread->timelock_cond,
				 thread->timelock_mutex);

      sync_flags = g_atomic_int_get(&(thread->sync_flags));
    }

    nanosleep(&(thread->timelock), NULL);
    // ndelay(thread->timelock.tv_nsec);

    sync_flags = g_atomic_int_get(&(thread->sync_flags));

    if((AGS_THREAD_WAIT_0 & sync_flags ||
	AGS_THREAD_WAIT_1 & sync_flags ||
	AGS_THREAD_WAIT_2 & sync_flags) != 0){
#ifdef AGS_DEBUG
      g_message("thread in realtime");
#endif
    }else{
#ifdef AGS_DEBUG
      g_message("thread timelock");
#endif
      ags_thread_timelock(thread);
    }

    flags = g_atomic_int_get(&(thread->flags));
  }

  pthread_mutex_unlock(thread->timelock_mutex);

  return(NULL);
}

void
ags_thread_real_timelock(AgsThread *thread)
{
  g_atomic_int_or(&(thread->sync_flags),
		  AGS_THREAD_INTERRUPTED);
  
  /* throughput is mandatory */
#ifdef AGS_PTHREAD_SUSPEND
    pthread_suspend(thread->thread);
#else
    pthread_kill(*(thread->thread), AGS_THREAD_SUSPEND_SIG);
#endif
}

void
ags_thread_timelock(AgsThread *thread)
{
  g_return_if_fail(AGS_IS_THREAD(thread));

  g_object_ref(G_OBJECT(thread));
  g_signal_emit(G_OBJECT(thread),
		thread_signals[TIMELOCK], 0);
  g_object_unref(G_OBJECT(thread));
}

void
ags_thread_real_stop(AgsThread *thread)
{
  g_atomic_int_and(&(thread->flags),
		   (~AGS_THREAD_RUNNING));

  pthread_detach(*(thread->thread));
}

/**
 * ags_thread_stop:
 * @thread: the #AgsThread instance
 * 
 * Stop the threads loop by unsetting AGS_THREAD_RUNNING flag.
 *
 * Since: 2.0.0
 */
void
ags_thread_stop(AgsThread *thread)
{
  guint thread_signal;

  thread_signal = thread_signals[STOP];
  g_return_if_fail(AGS_IS_THREAD(thread));

  if((AGS_THREAD_RUNNING & (g_atomic_int_get(&(thread->flags)))) == 0){
    return;
  }
  
  g_object_ref(G_OBJECT(thread));
  g_signal_emit(G_OBJECT(thread),
		thread_signal, 0);
  g_object_unref(G_OBJECT(thread));
}

/**
 * ags_thread_interrupted:
 * @thread: the #AgsThread
 * @sig: signal number
 * @time_cycle: duration of the time cycle
 * @time_spent: time spent since last cycle
 *
 * Notify to resume interrupted thread.
 * 
 * Returns: the time spent
 *
 * Since: 2.0.0
 */
guint
ags_thread_interrupted(AgsThread *thread,
		       int sig,
		       guint time_cycle, guint *time_spent)
{
  guint retval;
  guint thread_signal;

  return(0);
  
  thread_signal = thread_signals[INTERRUPTED];
  g_return_val_if_fail(AGS_IS_THREAD(thread),
		       0);

  if((AGS_THREAD_RUNNING & (g_atomic_int_get(&(thread->flags)))) == 0){
    return(0);
  }

  g_object_ref(G_OBJECT(thread));
  g_signal_emit(G_OBJECT(thread),
		thread_signal,
		0,
		sig,
		time_cycle,
		time_spent,
		&retval);
  g_object_unref(G_OBJECT(thread));

  return(retval);
}

/**
 * ags_thread_hangcheck:
 * @thread: the #AgsThread instance
 *
 * Performs hangcheck of thread.
 *
 * Since: 2.0.0
 */
void
ags_thread_hangcheck(AgsThread *thread)
{
  AgsThread *toplevel;
  gboolean synced[3];

  auto void ags_thread_hangcheck_recursive(AgsThread *thread);
  auto void ags_thread_hangcheck_unsync_all(AgsThread *thread, gboolean broadcast);

  void ags_thread_hangcheck_recursive(AgsThread *thread){
    AgsThread *child;
    guint sync_flags;

    sync_flags = g_atomic_int_get(&(thread->sync_flags));

    if((AGS_THREAD_WAIT_0 & sync_flags) != 0){
      synced[0] = TRUE;
    }

    if((AGS_THREAD_WAIT_1 & sync_flags) != 0){
      synced[1] = TRUE;
    }

    if((AGS_THREAD_WAIT_2 & sync_flags) != 0){
      synced[2] = TRUE;
    }

    /* iterate tree */
    child = g_atomic_pointer_get(&(thread->children));

    while(child != NULL){
      ags_thread_hangcheck_recursive(child);

      child = g_atomic_pointer_get(&(child->next));
    }
  }
  void ags_thread_hangcheck_unsync_all(AgsThread *thread, gboolean broadcast){
    AgsThread *child;
    guint sync_flags;

    sync_flags = g_atomic_int_get(&(thread->sync_flags));
    g_atomic_int_and(&(thread->sync_flags),
		     (~(AGS_THREAD_WAIT_0 |
			AGS_THREAD_WAIT_1 |
			AGS_THREAD_WAIT_2)));

    if(AGS_THREAD_WAIT_0 & sync_flags){
      if(broadcast){
	pthread_cond_broadcast(thread->cond);
      }else{
	pthread_cond_signal(thread->cond);
      }
    }

    if(AGS_THREAD_WAIT_1 & sync_flags){
      if(broadcast){
	pthread_cond_broadcast(thread->cond);
      }else{
	pthread_cond_signal(thread->cond);
      }
    }

    if(AGS_THREAD_WAIT_2 & sync_flags){
      if(broadcast){
	pthread_cond_broadcast(thread->cond);
      }else{
	pthread_cond_signal(thread->cond);
      }
    }

    /* iterate tree */
    child = g_atomic_pointer_get(&(thread->children));

    while(child != NULL){
      ags_thread_hangcheck_unsync_all(child, broadcast);

      child = g_atomic_pointer_get(&(child->next));
    }
  }

  /* detect memory corruption */
  synced[0] = FALSE;
  synced[1] = FALSE;
  synced[2] = FALSE;

  /* fill synced array */
  toplevel = ags_thread_get_toplevel(thread);
  ags_thread_hangcheck_recursive(toplevel);
  
  /*  */
  if(!((synced[0] && !synced[1] && !synced[2]) ||
       (!synced[0] && synced[1] && !synced[2]) ||
       (!synced[0] && !synced[1] && synced[2]))){
    g_warning("thread tree hung up");

    ags_thread_hangcheck_unsync_all(toplevel, FALSE);
  }
}

AgsThread*
ags_thread_find_type(AgsThread *thread, GType type)
{
  AgsThread *current, *retval;

  if(thread == NULL || type == G_TYPE_NONE){
    return(NULL);
  }

  if(g_type_is_a(G_OBJECT_TYPE(thread), type)){
    return(thread);
  }
  
  current = g_atomic_pointer_get(&(thread->children));

  while(current != NULL){
    if((retval = ags_thread_find_type(current, type)) != NULL){
      return(retval);
    }
    
    current = g_atomic_pointer_get(&(current->next));
  }
  
  return(NULL);
}

AgsThread*
ags_thread_self(void)
{
  return((AgsThread *) g_private_get(&ags_thread_key));
}

/**
 * ags_thread_chaos_tree:
 * @thread: the #AgsThread
 *
 * Escape chaos tree, get syncing point.
 *
 * Returns: the first ordered thread
 *
 * Since: 2.0.0
 */
AgsThread*
ags_thread_chaos_tree(AgsThread *thread)
{
  while((AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(thread->flags)))) != 0 &&
	g_atomic_pointer_get(&(thread->parent)) != NULL){
    thread = g_atomic_pointer_get(&(thread->parent));
  }

  return(thread);
}

/**
 * ags_thread_is_chaos_tree:
 * @thread: the #AgsThread
 * @tic_delay: the tic to sync with
 * @is_chaos_tree: if %TRUE AGS_THREAD_IS_CHAOS_TREE flag is set 
 *
 * Check chaos tree and flag threads.
 *
 * Returns: %TRUE if tree is in chaos, otherwise %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_thread_is_chaos_tree(AgsThread *thread,
			 guint tic_delay,
			 gboolean is_chaos_tree)
{
  AgsThread *child;
  
  gboolean retval;

  ags_thread_lock(thread);
  
  retval = is_chaos_tree;

  if(!is_chaos_tree){
    if((AGS_THREAD_INITIAL_SYNC & (g_atomic_int_get(&(thread->flags)))) != 0 ||
       (AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(thread->flags)))) != 0){
      retval = TRUE;
    }
    
    if(!retval &&
       (AGS_THREAD_START_SYNCED_FREQ & (g_atomic_int_get(&(thread->flags)))) != 0 &&
       (AGS_THREAD_SYNCED_FREQ & (g_atomic_int_get(&(thread->flags)))) == 0){
      retval = TRUE;
    }

    if(!retval){
      child = g_atomic_pointer_get(&(thread->children));
      
      while(!retval &&
	    child != NULL){
	retval = ags_thread_is_chaos_tree(child,
					  tic_delay,
					  FALSE);

	child = g_atomic_pointer_get(&(child->next));
      }
    }
  }

  if(is_chaos_tree ||
     retval){
    /* flag thread */
    g_atomic_int_or(&(thread->flags),
		    AGS_THREAD_IS_CHAOS_TREE);
    
    /* apply recursive */
    child = g_atomic_pointer_get(&(thread->children));
      
    while(child != NULL){
      ags_thread_is_chaos_tree(child,
			       tic_delay,
			       TRUE);

      child = g_atomic_pointer_get(&(child->next));
    }    
  }
  
  ags_thread_unlock(thread);

  return(retval);
}

/**
 * ags_thread_new:
 * @data: an #GObject
 *
 * Create a new instance of #AgsThread you may provide a #gpointer as @data
 * to your thread routine.
 *
 * Returns: the new #AgsThread
 * 
 * Since: 2.0.0
 */
AgsThread*
ags_thread_new(gpointer data)
{
  AgsThread *thread;

  thread = (AgsThread *) g_object_new(AGS_TYPE_THREAD,
				      NULL);

  thread->data = data;

  return(thread);
}
