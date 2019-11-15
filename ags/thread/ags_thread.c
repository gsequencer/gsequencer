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

#include <ags/thread/ags_thread.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_main_loop.h>
#include <ags/object/ags_config.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_marshal.h>

#include <ags/thread/ags_concurrency_provider.h>
#include <ags/thread/ags_task_launcher.h>

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

gboolean ags_thread_is_tree_ready_current_tic(AgsThread *current,
					      guint tic);
gboolean ags_thread_is_tree_ready_recursive(AgsThread *current,
					    guint tic);

void ags_thread_clock_sync(AgsThread *thread);

guint ags_thread_real_clock(AgsThread *thread);

void ags_thread_real_start(AgsThread *thread);
void ags_thread_real_stop(AgsThread *thread);

void* ags_thread_loop(void *ptr);

//TODO:JK: migrate these functions
AgsThread* ags_thread_chaos_tree(AgsThread *thread);
gboolean ags_thread_is_chaos_tree(AgsThread *thread,
				  guint tic_delay,
				  gboolean is_chaos_tree);
void ags_thread_hangcheck(AgsThread *thread);

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
  PROP_TIC_DELAY,
  PROP_DELAY,
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

  /**
   * AgsThread:delay:
   *
   * The delay until next tic.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("delay",
				 i18n_pspec("delay"),
				 i18n_pspec("The delay until next tic"),
				 1,
				 (guint) AGS_THREAD_DEFAULT_MAX_PRECISION,
				 1,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DELAY,
				  param_spec);

  /**
   * AgsThread:tic-delay:
   *
   * The tic delay is current tic related to delay.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("tic-delay",
				 i18n_pspec("tic delay"),
				 i18n_pspec("The current tic of delay"),
				 0,
				 (guint) AGS_THREAD_DEFAULT_MAX_PRECISION,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TIC_DELAY,
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
  thread->my_flags = 0;
  
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

  /* tree */
  thread->parent = NULL;
  
  thread->next = NULL;
  thread->prev = NULL;

  thread->children = NULL;
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
  case PROP_DELAY:
  {
    g_rec_mutex_lock(thread_mutex);

    thread->delay = g_value_get_uint(value);

    g_rec_mutex_unlock(thread_mutex);
  }
  break;
  case PROP_TIC_DELAY:
  {
    g_rec_mutex_lock(thread_mutex);

    thread->tic_delay = g_value_get_uint(value);

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
  case PROP_DELAY:
    {
      g_rec_mutex_lock(thread_mutex);

      g_value_set_uint(value, thread->delay);

      g_rec_mutex_unlock(thread_mutex);
    }
    break;
  case PROP_TIC_DELAY:
    {
      g_rec_mutex_lock(thread_mutex);

      g_value_set_uint(value, thread->tic_delay);

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
      ags_thread_set_sync_all_recursive(current_child, tic);

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

  AgsApplicationContext *application_context;
  
  if(!AGS_IS_THREAD(thread)){
    return;
  }
  
  /* get main loop */
  application_context = ags_application_context_get_instance();
  
  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  /* sync all */
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

  main_loop = ags_thread_get_toplevel(thread);
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
  guint status_flags;
  gboolean retval;

  GRecMutex *current_mutex;

  if(!AGS_IS_THREAD(current)){
    return(TRUE);
  }
  
  /* get thread mutex */
  current_mutex = AGS_THREAD_GET_OBJ_MUTEX(current);

  g_rec_mutex_lock(current_mutex);

  status_flags = g_atomic_int_get(&(current->status_flags));

  retval = FALSE;

  if(!ags_thread_test_status_flags(current, AGS_THREAD_STATUS_RUNNING)){
    retval = TRUE;
  }

  if(!ags_thread_test_flags(current, AGS_THREAD_IMMEDIATE_SYNC)){
    if(ags_thread_test_status_flags(current, AGS_THREAD_STATUS_INITIAL_RUN)){
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

  if(!ags_thread_test_flags(current, AGS_THREAD_IMMEDIATE_SYNC)){
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
  
  AgsApplicationContext *application_context;
  
  gboolean retval;

  if(!AGS_IS_THREAD(thread)){
    return(FALSE);
  }

  /* get main loop */
  application_context = ags_application_context_get_instance();
  
  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  /* check is tree ready */
  if(ags_thread_global_get_use_sync_counter()){    
    retval = ags_main_loop_sync_counter_test(AGS_MAIN_LOOP(main_loop), tic);
  }else{
    retval = ags_thread_is_tree_ready_recursive(main_loop,
						tic);
  }

  g_object_unref(main_loop);
  
  return(retval);
}

void
ags_thread_clock_sync(AgsThread *thread)
{
  AgsThread *main_loop;
  
  AgsApplicationContext *application_context;

  guint next_tic, current_tic;
    
  guint sync_tic, next_sync_tic;
  gboolean skip_sync_counter;

  GRecMutex *tree_lock;
  
  /* get main loop */
  application_context = ags_application_context_get_instance();
  
  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  tree_lock = ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop));
  
  /* sync */
  skip_sync_counter = FALSE;

  if(thread != main_loop){
    ags_thread_set_flags(thread, AGS_THREAD_MARK_SYNCED);
  }
    
  g_rec_mutex_lock(tree_lock);
    
  if(ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_INITIAL_RUN)){
    AgsThread *parent_thread;
    
    if(!ags_thread_test_flags(thread, AGS_THREAD_IMMEDIATE_SYNC)){
      skip_sync_counter = TRUE;
    }
      
    thread->current_tic = ags_main_loop_get_tic(AGS_MAIN_LOOP(main_loop));

    parent_thread = ags_thread_parent(thread);
    
    if(ags_thread_test_flags(thread, AGS_THREAD_START_SYNCED_FREQ) &&
       parent_thread != NULL){
      AgsThread *chaos_tree;
	
      chaos_tree = main_loop;//ags_thread_chaos_tree(thread);

      /* ordinary sync */
      thread->tic_delay = chaos_tree->tic_delay;

      if(ags_thread_test_flags(thread, AGS_THREAD_INTERMEDIATE_PRE_SYNC)){
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
	
      if(ags_thread_test_flags(thread, AGS_THREAD_INTERMEDIATE_POST_SYNC)){
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
	
      ags_thread_set_status_flags(thread, AGS_THREAD_STATUS_SYNCED_FREQ);
    }

    ags_thread_unset_status_flags(thread, AGS_THREAD_STATUS_INITIAL_RUN);

    g_object_unref(parent_thread);
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
    ags_thread_set_status_flags(thread, AGS_THREAD_STATUS_WAIT_0);
  }
  break;
  case 1:
  {
    ags_thread_set_status_flags(thread, AGS_THREAD_STATUS_WAIT_1);
  }
  break;
  case 2:
  {
    ags_thread_set_status_flags(thread, AGS_THREAD_STATUS_WAIT_2);
  }
  break;
  }
    
  if(!ags_thread_is_tree_ready(thread,
			       sync_tic)){
    //      ags_thread_hangcheck(main_loop);
    g_rec_mutex_unlock(tree_lock);
      
    if(!ags_thread_is_current_ready(thread,
				    current_tic)){
      g_mutex_lock(AGS_THREAD_GET_WAIT_MUTEX(thread));

      ags_thread_set_status_flags(thread, AGS_THREAD_STATUS_WAITING);
	
      while(!ags_thread_is_current_ready(thread,
					 current_tic) &&
	    ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_WAITING)){
	g_cond_wait(AGS_THREAD_GET_WAIT_COND(thread),
		    AGS_THREAD_GET_WAIT_MUTEX(thread));
      }

      g_mutex_unlock(AGS_THREAD_GET_WAIT_MUTEX(thread));
    }
  }else{
    AgsTaskLauncher *task_launcher;
    
    task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

    ags_task_launcher_sync_run(task_launcher);
    
    /* sync all */
    ags_main_loop_set_last_sync(AGS_MAIN_LOOP(main_loop), thread->current_tic);
    ags_main_loop_set_tic(AGS_MAIN_LOOP(main_loop), next_tic);
    ags_main_loop_set_sync_tic(AGS_MAIN_LOOP(main_loop), next_sync_tic);

    ags_thread_set_sync_all(main_loop, thread->current_tic);

    g_rec_mutex_unlock(tree_lock);
  }

  g_object_unref(main_loop);
}

guint
ags_thread_real_clock(AgsThread *thread)
{
  AgsThread *toplevel;

#ifdef __APPLE__
  clock_serv_t cclock;
  mach_timespec_t mts;
#endif
  
  struct timespec time_now;

  guint delay, tic_delay;
  gdouble delay_per_hertz;
  guint steps;

  GRecMutex *thread_mutex;
  
  /* get thread mutex */  
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  /* check toplevel to be main loop */
  toplevel = ags_thread_get_toplevel(thread);

  if(!AGS_IS_MAIN_LOOP(toplevel)){
    g_rec_mutex_lock(thread_mutex);
    
    if(thread->tic_delay == 0){
      if(thread->freq >= thread->max_precision){
	steps = 1.0 / thread->delay;
      }else{
	steps = 1;
      }
    }else{
      steps = 0;
    }

    g_rec_mutex_unlock(thread_mutex);

    g_object_unref(toplevel);
    
    return(steps);
  }
  
  g_object_unref(toplevel);

  toplevel = NULL;
  
  steps = 0;
  
  /* calculate thread delay */
  g_rec_mutex_lock(thread_mutex);

  thread->delay = (guint) ceil((AGS_THREAD_HERTZ_JIFFIE / thread->freq) / (AGS_THREAD_HERTZ_JIFFIE / thread->max_precision));
  delay_per_hertz = AGS_THREAD_HERTZ_JIFFIE / thread->max_precision;

  if(thread->delay < thread->tic_delay){
    thread->tic_delay = thread->delay;
  }
  
  g_rec_mutex_unlock(thread_mutex);

  /* check initial sync or immediate sync */
  if(!ags_thread_test_flags(thread, AGS_THREAD_IMMEDIATE_SYNC)){
    if(ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_INITIAL_RUN) &&
       ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_INITIAL_SYNC)){
#ifdef __APPLE__
      host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
      
      clock_get_time(cclock, &mts);
      mach_port_deallocate(mach_task_self(), cclock);

      /* apply computing time */
      g_rec_mutex_lock(thread_mutex);
      
      thread->computing_time->tv_sec = mts.tv_sec;
      thread->computing_time->tv_nsec = mts.tv_nsec;

      g_rec_mutex_unlock(thread_mutex);
#else
      /* apply computing time */
      g_rec_mutex_lock(thread_mutex);

      clock_gettime(CLOCK_MONOTONIC, thread->computing_time);

      g_rec_mutex_unlock(thread_mutex);
#endif

      ags_thread_unset_status_flags(thread, AGS_THREAD_STATUS_INITIAL_SYNC);

      return(1);
    }
  }else{
    if(ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_INITIAL_SYNC)){
      ags_thread_unset_status_flags(thread, AGS_THREAD_STATUS_INITIAL_SYNC);
    }
  }
  
  /* idle */
  g_object_get(thread,
	       "delay", &delay,
	       "tic-delay", &tic_delay,
	       NULL);
  
  if(tic_delay == delay &&
     ags_thread_test_flags(thread, AGS_THREAD_TIME_ACCOUNTING)){
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
      
    g_rec_mutex_lock(thread_mutex);
    
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

    if(relative_time_spent - (gdouble) thread->time_late < 0.0){
      if(thread->time_late - (gint) ceil(1.25 * relative_time_spent) < 0){
	thread->time_late = 0;
      }else{
	thread->time_late = thread->time_late - (gint) ceil(1.25 * relative_time_spent);
      }
    }else if(relative_time_spent > 0.0 &&
	     relative_time_spent - (gdouble) thread->time_late < (44.0 / 45.0) * time_cycle){
      thread->time_late = 0;
      timed_sleep.tv_nsec = (long) relative_time_spent - (gdouble) thread->time_late - (1.0 / 45.0) * time_cycle;
    }else{
      thread->time_late = 0;
    }

    g_rec_mutex_unlock(thread_mutex);

    if(timed_sleep.tv_sec != 0 ||
       timed_sleep.tv_nsec != 0){
      nanosleep(&timed_sleep, NULL);
    }
    
#ifdef __APPLE__
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    
    g_rec_mutex_lock(thread_mutex);

    thread->computing_time->tv_sec = mts.tv_sec;
    thread->computing_time->tv_nsec = mts.tv_nsec;

    g_rec_mutex_unlock(thread_mutex);
#else
    g_rec_mutex_lock(thread_mutex);

    clock_gettime(CLOCK_MONOTONIC, thread->computing_time);

    g_rec_mutex_unlock(thread_mutex);
#endif
  }
  
  //  g_message("%s", G_OBJECT_TYPE_NAME(thread));

  /* sync - run in hierarchy */
  ags_thread_clock_sync(thread);

  /* increment delay counter and set run per cycle */
  g_rec_mutex_lock(thread_mutex);

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
  
  g_rec_mutex_unlock(thread_mutex);
  
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
 * Since: 3.0.0
 */
guint
ags_thread_clock(AgsThread *thread)
{
  guint cycles;
  
  g_return_val_if_fail(AGS_IS_THREAD(thread), 0);

  g_object_ref(thread);
  g_signal_emit(thread,
		thread_signals[CLOCK], 0,
		&cycles);
  g_object_unref(thread);

  return(cycles);
}

void
ags_thread_real_start(AgsThread *thread)
{
  guint val;
  
  /* set/unset run flags */
  ags_thread_set_status_flags(thread,
			      (AGS_THREAD_STATUS_RUNNING |
			       AGS_THREAD_STATUS_INITIAL_RUN |
			       AGS_THREAD_STATUS_INITIAL_SYNC));

  ags_thread_unset_status_flags(thread,
				(AGS_THREAD_STATUS_RT_SETUP |
				 AGS_THREAD_STATUS_WAITING |
				 AGS_THREAD_STATUS_WAIT_0 |
				 AGS_THREAD_STATUS_WAIT_1 |
				 AGS_THREAD_STATUS_WAIT_2 |
				 AGS_THREAD_STATUS_SYNCED |
				 AGS_THREAD_STATUS_SYNCED_FREQ));

  ags_thread_unset_flags(thread,
			 (AGS_THREAD_MARK_SYNCED));

#ifdef AGS_DEBUG
  g_message("thread start: %s", G_OBJECT_TYPE_NAME(thread));
#endif

  /*  */
  thread->thread = g_thread_new("ags-thread",
				ags_thread_loop,
				thread);
}

/**
 * ags_thread_start:
 * @thread: the #AgsThread instance
 *
 * Start the thread.
 *
 * Since: 3.0.0
 */
void
ags_thread_start(AgsThread *thread)
{
  g_return_if_fail((AGS_IS_THREAD(thread) ||
		    thread->thread == NULL ||
		    !ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_RUNNING)));
  
  g_object_ref(thread);
  g_signal_emit(thread,
		thread_signals[START], 0);
  g_object_unref(thread);
}

/**
 * ags_thread_add_start_queue:
 * @thread: the #AgsThread
 * @child: the child #AgsThread to start
 *
 * Add @child to @thread's start queue.
 *
 * Since: 3.0.0
 */
void
ags_thread_add_start_queue(AgsThread *thread,
			   AgsThread *child)
{
  GRecMutex *thread_mutex;

  if(!AGS_IS_THREAD(thread) ||
     !AGS_IS_THREAD(child)){
    return;
  }

  /* thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  /* add */
  g_rec_mutex_lock(thread_mutex);
  
  thread->start_queue = g_list_prepend(thread->start_queue,
				       child);
  g_object_ref(child);
  
  g_rec_mutex_unlock(thread_mutex);
}

/**
 * ags_thread_add_start_queue_all:
 * @thread: the #AgsThread
 * @child: the children as #GList-struct containing #AgsThread to start
 *
 * Add @child to @thread's start queue.
 *
 * Since: 3.0.0
 */
void
ags_thread_add_start_queue_all(AgsThread *thread,
			       GList *child)
{
  GRecMutex *thread_mutex;

  if(!AGS_IS_THREAD(thread) ||
     child == NULL){
    return;
  }

  /* thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  /* add all */
  g_rec_mutex_lock(thread_mutex);
  
  if(thread->start_queue == NULL){
    thread->start_queue = g_list_copy_deep(child,
					   (GCopyFunc) g_object_ref,
					   NULL);
  }else{
    thread->start_queue = g_list_concat(thread->start_queue,
					g_list_copy_deep(child,
							 (GCopyFunc) g_object_ref,
							 NULL));
  }
    
  g_rec_mutex_unlock(thread_mutex);
}

/**
 * ags_thread_is_running:
 * @thread: the #AgsThread instance
 *
 * Query running flag.
 *
 * Since: 3.0.0
 */
gboolean
ags_thread_is_running(AgsThread *thread)
{
  if(!AGS_IS_THREAD(thread)){
    return(FALSE);
  }
  
  return(ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_RUNNING));
}

void*
ags_thread_loop(void *ptr)
{
  AgsThread *main_loop;
  AgsThread *parent;
  AgsThread *thread;
  AgsThread *queued_thread;
  
  AgsApplicationContext *application_context;
  
  GList *start_start_queue, *start_queue, *start_queue_next;
  
  guint val;
  guint i, i_stop;
  gboolean wait_for_parent, wait_for_sibling, wait_for_children;
  guint sync_tic, next_sync_tic;
  gboolean is_running;
  gboolean skip_sync_counter;

  GRecMutex *tree_lock;
  GRecMutex *main_loop_mutex;
  GRecMutex *thread_mutex;
  
  thread = (AgsThread *) ptr;
  g_object_ref(thread);

  /* set private for ags_thread_self() */
  g_private_set(&ags_thread_key,
		thread);

  /* get main loop */
  application_context = ags_application_context_get_instance();
  
  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  main_loop_mutex = NULL;
  tree_lock = NULL;
  
  /* main loop mutex */  
  if(main_loop != NULL){
    main_loop_mutex = AGS_THREAD_GET_OBJ_MUTEX(main_loop);

    tree_lock = ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop));
  }

  /* thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  /* retrieve current tic */
  g_rec_mutex_lock(tree_lock);
  
  thread->current_tic = ags_main_loop_get_tic(AGS_MAIN_LOOP(main_loop));
  
  g_rec_mutex_unlock(tree_lock);

  /* check is running */
  is_running = ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_RUNNING);
  
  /* get start computing time */
  parent = ags_thread_parent(thread);
  
  if(parent == NULL){
#ifdef __APPLE__
    clock_serv_t cclock;
    mach_timespec_t mts;
#endif

    g_rec_mutex_lock(thread_mutex);
    
    thread->delay = 
      thread->tic_delay = (AGS_THREAD_HERTZ_JIFFIE / thread->freq) / (AGS_THREAD_HERTZ_JIFFIE / thread->max_precision);

    g_rec_mutex_unlock(thread_mutex);

#ifdef __APPLE__
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);

    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);

    g_rec_mutex_lock(thread_mutex);

    thread->computing_time->tv_sec = mts.tv_sec;
    thread->computing_time->tv_nsec = mts.tv_nsec;

    g_rec_mutex_unlock(thread_mutex);
#else
    g_rec_mutex_lock(thread_mutex);

    clock_gettime(CLOCK_MONOTONIC, thread->computing_time);

    g_rec_mutex_unlock(thread_mutex);
#endif
  }

  g_object_unref(parent);

  parent = NULL;
  
  while(is_running){
    guint tic_delay;
    
    /* start queue */
    g_rec_mutex_lock(thread_mutex);

    tic_delay = thread->tic_delay;
    
    start_queue = 
      start_start_queue = thread->start_queue;
    thread->start_queue = NULL;

    g_rec_mutex_unlock(thread_mutex);
    
    while(start_queue != NULL){
      start_queue_next = start_queue->next;
      
      queued_thread = (AgsThread *) start_queue->data;
      g_object_ref(queued_thread);
      
      /*  */
      ags_thread_start(queued_thread);

      g_mutex_lock(AGS_THREAD_GET_START_MUTEX(queued_thread));

      if(!ags_thread_test_status_flags(queued_thread, AGS_THREAD_STATUS_START_DONE)){
	g_rec_mutex_lock(tree_lock);
	
	/* ordinary sync */
	g_rec_mutex_lock(AGS_THREAD_GET_OBJ_MUTEX(queued_thread));
	
	queued_thread->tic_delay = tic_delay;

	if(ags_thread_test_flags(queued_thread, AGS_THREAD_INTERMEDIATE_PRE_SYNC)){
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
	
	if(ags_thread_test_flags(queued_thread, AGS_THREAD_INTERMEDIATE_POST_SYNC)){
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

	g_rec_mutex_unlock(AGS_THREAD_GET_OBJ_MUTEX(queued_thread));

	/*  */
	g_rec_mutex_unlock(tree_lock);

	ags_thread_set_status_flags(queued_thread, AGS_THREAD_STATUS_START_WAIT);
      
	while(ags_thread_test_status_flags(queued_thread, AGS_THREAD_STATUS_START_WAIT) &&
	      !ags_thread_test_status_flags(queued_thread, AGS_THREAD_STATUS_START_DONE)){
	  g_cond_wait(AGS_THREAD_GET_START_COND(queued_thread),
		      AGS_THREAD_GET_START_MUTEX(queued_thread));
	}
      }
      
      g_mutex_unlock(AGS_THREAD_GET_START_MUTEX(queued_thread));
      
      g_object_unref(queued_thread);
      
      start_queue = start_queue_next;
    }

    g_list_free_full(start_start_queue,
		     g_object_unref);
    
    /* notify start */
    g_mutex_lock(AGS_THREAD_GET_START_MUTEX(thread));
      
    ags_thread_set_status_flags(thread, AGS_THREAD_STATUS_START_DONE);
      
    if(ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_START_WAIT)){
      g_cond_broadcast(AGS_THREAD_GET_START_COND(thread));
    }
      
    g_mutex_unlock(AGS_THREAD_GET_START_MUTEX(thread));
      
    /* run clock synchronization */
    i_stop = ags_thread_clock(thread);

    /* do cycles if still running */    
    is_running = ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_RUNNING);

    for(i = 0; i < i_stop && is_running; i++){
      /* set busy flag */
      ags_thread_set_status_flags(thread, AGS_THREAD_STATUS_BUSY);
      
      /* run */
      ags_thread_run(thread);
      //    g_printf("%s\n", G_OBJECT_TYPE_NAME(thread));

      /*  */
      is_running = ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_RUNNING);

      /* unset busy flag */
      ags_thread_unset_status_flags(thread, AGS_THREAD_STATUS_BUSY);
    }    
  }

  //  g_object_ref(thread);
  g_rec_mutex_lock(tree_lock);  

  skip_sync_counter = FALSE;  
	
  if(ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_INITIAL_RUN)){
    if(!ags_thread_test_flags(thread, AGS_THREAD_IMMEDIATE_SYNC)){
      skip_sync_counter = TRUE;
    }
  }
  
  if(main_loop != NULL &&
     ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_INITIAL_RUN) ||
     ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_INITIAL_SYNC)){
    AgsThread *chaos_tree;

    guint tic_delay;
    
    chaos_tree = main_loop;//ags_thread_chaos_tree(thread);

    g_rec_mutex_lock(main_loop_mutex);
    
    tic_delay = chaos_tree->tic_delay;
    
    g_rec_mutex_unlock(main_loop_mutex);
    
    /* ordinary sync */
    g_rec_mutex_lock(thread_mutex);
    
    thread->tic_delay = tic_delay;

    if(ags_thread_test_flags(thread, AGS_THREAD_INTERMEDIATE_PRE_SYNC)){
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
	
    if(ags_thread_test_flags(thread, AGS_THREAD_INTERMEDIATE_POST_SYNC)){
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
	
    g_rec_mutex_unlock(thread_mutex);
    
    ags_thread_unset_status_flags(thread, AGS_THREAD_STATUS_INITIAL_RUN);
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
    ags_thread_set_status_flags(thread, AGS_THREAD_STATUS_WAIT_0);
  }
  break;
  case 1:
  {
    ags_thread_set_status_flags(thread, AGS_THREAD_STATUS_WAIT_1);
  }
  break;
  case 2:
  {
    ags_thread_set_status_flags(thread, AGS_THREAD_STATUS_WAIT_2);
  }
  break;
  }
    
  if(ags_thread_is_tree_ready(thread,
			      sync_tic)){
    AgsTaskLauncher *task_launcher;
    
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
    
    task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

    ags_task_launcher_sync_run(task_launcher);

    /* sync all */
    ags_main_loop_set_last_sync(AGS_MAIN_LOOP(main_loop), thread->current_tic);
    ags_main_loop_set_tic(AGS_MAIN_LOOP(main_loop), next_tic);
    ags_main_loop_set_sync_tic(AGS_MAIN_LOOP(main_loop), next_sync_tic);
    
    ags_thread_set_sync_all(main_loop, thread->current_tic);
  }
  
  if(ags_thread_test_flags(thread, AGS_THREAD_UNREF_ON_EXIT)){
    parent = ags_thread_parent(thread);
    
    ags_thread_remove_child(parent,
			    thread);

    g_object_unref(parent);
    g_object_unref(thread);

    parent = NULL;
  }
  
  g_rec_mutex_unlock(tree_lock);  
  
#ifdef AGS_DEBUG
  g_message("thread finished");
#endif  

  thread->thread = NULL;
  
  g_object_unref(thread);
  //  g_object_ref(thread);
  
  /* exit thread */
  g_thread_exit(NULL);

  return(NULL);
}

/**
 * ags_thread_run:
 * @thread: the #AgsThread
 * 
 * Only for internal use of ags_thread_loop but you may want to set the your very own
 * class function namely your thread's routine.
 *
 * Since: 3.0.0
 */
void
ags_thread_run(AgsThread *thread)
{
  g_return_if_fail(AGS_IS_THREAD(thread));

  g_object_ref(thread);
  g_signal_emit(thread,
		thread_signals[RUN], 0);
  g_object_unref(thread);
}


void
ags_thread_real_stop(AgsThread *thread)
{
  ags_thread_unset_status_flags(thread, AGS_THREAD_STATUS_RUNNING);
}

/**
 * ags_thread_stop:
 * @thread: the #AgsThread
 * 
 * Stop the threads loop by unsetting AGS_THREAD_STATUS_RUNNING flag.
 *
 * Since: 3.0.0
 */
void
ags_thread_stop(AgsThread *thread)
{
  g_return_if_fail(AGS_IS_THREAD(thread) ||
		   thread->thread != NULL ||
		   !ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_RUNNING));
  
  g_object_ref(G_OBJECT(thread));
  g_signal_emit(G_OBJECT(thread),
		thread_signals[STOP], 0);
  g_object_unref(G_OBJECT(thread));
}

#if 0
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
	g_cond_broadcast(&(thread->cond));
      }else{
	g_cond_signal(&(thread->cond));
      }
    }

    if(AGS_THREAD_WAIT_1 & sync_flags){
      if(broadcast){
	g_cond_broadcast(&(thread->cond));
      }else{
	g_cond_signal(&(thread->cond));
      }
    }

    if(AGS_THREAD_WAIT_2 & sync_flags){
      if(broadcast){
	g_cond_broadcast(&(thread->cond));
      }else{
	g_cond_signal(&(thread->cond));
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
#endif

/**
 * ags_thread_find_type:
 * @thread: the #AgsThread
 * @gtype: the #GType-struct
 * 
 * Find @gtype as descendant of @thread. If its a descendant thread,
 * the ref-count is increased.
 * 
 * Since: 3.0.0
 */
AgsThread*
ags_thread_find_type(AgsThread *thread, GType gtype)
{
  AgsThread *child, *next_child;
  AgsThread *retval;

  if(!AGS_IS_THREAD(thread) ||
     gtype == G_TYPE_NONE){
    return(NULL);
  }

  if(g_type_is_a(G_OBJECT_TYPE(thread), gtype)){
    return(thread);
  }
  
  child = ags_thread_children(thread);

  while(child != NULL){
    if((retval = ags_thread_find_type(child, gtype)) != NULL){
      return(retval);
    }

    /* iterate */
    next_child = ags_thread_next(child);

    g_object_unref(child);

    child = next_child;
  }
  
  return(NULL);
}

AgsThread*
ags_thread_self(void)
{
  return((AgsThread *) g_private_get(&ags_thread_key));
}

#if 0
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
#endif

/**
 * ags_thread_new:
 *
 * Create a new instance of #AgsThread.
 *
 * Returns: the new #AgsThread
 * 
 * Since: 3.0.0
 */
AgsThread*
ags_thread_new()
{
  AgsThread *thread;

  thread = (AgsThread *) g_object_new(AGS_TYPE_THREAD,
				      NULL);

  return(thread);
}
