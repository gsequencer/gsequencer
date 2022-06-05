/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

guint ags_thread_real_clock(AgsThread *thread);

void ags_thread_real_start(AgsThread *thread);
void ags_thread_real_stop(AgsThread *thread);

void* ags_thread_loop(void *ptr);

/**
 * SECTION:ags_thread
 * @short_description: Thread class with base frequency
 * @title: AgsThread
 * @section_id:
 * @include: ags/thread/ags_thread.h
 *
 * The #AgsThread base class. It supports organizing them within a tree,
 * perform syncing and frequencies.
 *
 * The thread class synchronizes within tree at `max-precision` property
 * JIFFIE. AgsThread::run() is called at `frequency` property JIFFIE.
 *
 * The AgsThread::run() function can sync to the very same tic with
 * AGS_THREAD_IMMEDIATE_SYNC flag, sync to the previous or next tic with
 * AGS_THREAD_INTERMEDIATE_PRE_SYNC or AGS_THREAD_INTERMEDIATE_POST_SYNC
 * flag set.
 *
 * The AgsTread::clock() function is called at `max-precision` property
 * rate per second.
 * 
 * Note you usually have a default source giving you timing hints i.e.
 * poll is used. If not you can still set the AGS_THREAD_TIME_ACCOUNTING
 * flags, causing a sleep for every tic.
 * 
 * Further there is no time promise within one second. But you should
 * make sure you hit the real-time dead-line or use a worker thread
 * instead for time expensive work or tasks.
 *
 * ags_thread_self() returns you your own #AgsThread you are running
 * in or %NULL if not applicable.
 */

enum{
  PROP_0,
  PROP_FREQUENCY,
  PROP_MAX_PRECISION,
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

GType
ags_thread_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_THREAD_UNREF_ON_EXIT, "AGS_THREAD_UNREF_ON_EXIT", "thread-unref-on-exit" },
      { AGS_THREAD_IMMEDIATE_SYNC, "AGS_THREAD_IMMEDIATE_SYNC", "thread-immediate-sync" },
      { AGS_THREAD_INTERMEDIATE_PRE_SYNC, "AGS_THREAD_INTERMEDIATE_PRE_SYNC", "thread-intermediate-pre-sync" },
      { AGS_THREAD_INTERMEDIATE_POST_SYNC, "AGS_THREAD_INTERMEDIATE_POST_SYNC", "thread-intermediate-post-sync" },
      { AGS_THREAD_START_SYNCED_FREQ, "AGS_THREAD_START_SYNCED_FREQ", "thread-start-synced-freq" },
      { AGS_THREAD_MARK_SYNCED, "AGS_THREAD_MARK_SYNCED", "thread-mark-synced" },
      { AGS_THREAD_TIME_ACCOUNTING, "AGS_THREAD_TIME_ACCOUNTING", "thread-time-accounting" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsThreadFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
}

GType
ags_thread_status_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_THREAD_STATUS_RT_SETUP, "AGS_THREAD_STATUS_RT_SETUP", "thread-status-rt-setup" },
      { AGS_THREAD_STATUS_INITIAL_SYNC, "AGS_THREAD_STATUS_INITIAL_SYNC", "thread-status-initial-sync" },
      { AGS_THREAD_STATUS_INITIAL_RUN, "AGS_THREAD_STATUS_INITIAL_RUN", "thread-status-initial-run" },
      { AGS_THREAD_STATUS_IS_CHAOS_TREE, "AGS_THREAD_STATUS_IS_CHAOS_TREE", "thread-status-is-chaos-tree" },
      { AGS_THREAD_STATUS_START_WAIT, "AGS_THREAD_STATUS_START_WAIT", "thread-status-start-wait" },
      { AGS_THREAD_STATUS_START_DONE, "AGS_THREAD_STATUS_START_DONE", "thread-status-start-done" },
      { AGS_THREAD_STATUS_READY, "AGS_THREAD_STATUS_READY", "thread-status-ready" },
      { AGS_THREAD_STATUS_WAITING, "AGS_THREAD_STATUS_WAITING", "thread-status-waiting" },
      { AGS_THREAD_STATUS_RUNNING, "AGS_THREAD_STATUS_RUNNING", "thread-status-running" },
      { AGS_THREAD_STATUS_LOCKED, "AGS_THREAD_STATUS_LOCKED", "thread-status-locked" },
      { AGS_THREAD_STATUS_BUSY, "AGS_THREAD_STATUS_BUSY", "thread-status-busy" },
      { AGS_THREAD_STATUS_SYNCED, "AGS_THREAD_STATUS_SYNCED", "thread-status-synced" },
      { AGS_THREAD_STATUS_SYNCED_FREQ, "AGS_THREAD_STATUS_SYNCED_FREQ", "thread-status-synced-freq" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsThreadStatusFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
}

GType
ags_thread_sync_tic_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_THREAD_SYNC_TIC_WAIT_0, "AGS_THREAD_SYNC_TIC_WAIT_0", "thread-sync-tic-wait-0" },
      { AGS_THREAD_SYNC_TIC_DONE_0, "AGS_THREAD_SYNC_TIC_DONE_0", "thread-sync-tic-done-0" },
      { AGS_THREAD_SYNC_TIC_WAIT_1, "AGS_THREAD_SYNC_TIC_WAIT_1", "thread-sync-tic-wait-1" },
      { AGS_THREAD_SYNC_TIC_DONE_1, "AGS_THREAD_SYNC_TIC_DONE_1", "thread-sync-tic-done-1" },
      { AGS_THREAD_SYNC_TIC_WAIT_2, "AGS_THREAD_SYNC_TIC_WAIT_2", "thread-sync-tic-wait-2" },
      { AGS_THREAD_SYNC_TIC_DONE_2, "AGS_THREAD_SYNC_TIC_DONE_2", "thread-sync-tic-done-2" },
      { AGS_THREAD_SYNC_TIC_WAIT_3, "AGS_THREAD_SYNC_TIC_WAIT_3", "thread-sync-tic-wait-3" },
      { AGS_THREAD_SYNC_TIC_DONE_3, "AGS_THREAD_SYNC_TIC_DONE_3", "thread-sync-tic-done-3" },
      { AGS_THREAD_SYNC_TIC_WAIT_4, "AGS_THREAD_SYNC_TIC_WAIT_4", "thread-sync-tic-wait-4" },
      { AGS_THREAD_SYNC_TIC_DONE_4, "AGS_THREAD_SYNC_TIC_DONE_4", "thread-sync-tic-done-4" },
      { AGS_THREAD_SYNC_TIC_WAIT_5, "AGS_THREAD_SYNC_TIC_WAIT_5", "thread-sync-tic-wait-5" },
      { AGS_THREAD_SYNC_TIC_DONE_5, "AGS_THREAD_SYNC_TIC_DONE_5", "thread-sync-tic-done-5" },
      { AGS_THREAD_SYNC_TIC_WAIT_6, "AGS_THREAD_SYNC_TIC_WAIT_6", "thread-sync-tic-wait-6" },
      { AGS_THREAD_SYNC_TIC_DONE_6, "AGS_THREAD_SYNC_TIC_DONE_6", "thread-sync-tic-done-6" },
      { AGS_THREAD_SYNC_TIC_WAIT_7, "AGS_THREAD_SYNC_TIC_WAIT_7", "thread-sync-tic-wait-7" },
      { AGS_THREAD_SYNC_TIC_DONE_7, "AGS_THREAD_SYNC_TIC_DONE_7", "thread-sync-tic-done-7" },
      { AGS_THREAD_SYNC_TIC_WAIT_8, "AGS_THREAD_SYNC_TIC_WAIT_8", "thread-sync-tic-wait-8" },
      { AGS_THREAD_SYNC_TIC_DONE_8, "AGS_THREAD_SYNC_TIC_DONE_8", "thread-sync-tic-done-8" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsThreadSyncTicFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
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
  param_spec = g_param_spec_double("delay",
				   i18n_pspec("delay"),
				   i18n_pspec("The delay until next tic"),
				   1.0,
				   AGS_THREAD_DEFAULT_MAX_PRECISION,
				   1.0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DELAY,
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
  thread->connectable_flags = 0;
  
  g_atomic_int_set(&(thread->status_flags),
		   0);

  g_atomic_int_set(&(thread->sync_tic_flags),
		   0);

  /* uuid */
  thread->uuid = ags_uuid_alloc();
  ags_uuid_generate(thread->uuid);
    
  /* clock */
  g_atomic_int_set(&(thread->current_sync_tic), 0);

  thread->delay = 1.0;

  thread->tic_delay = 0.0;

  thread->frequency = AGS_THREAD_DEFAULT_JIFFIE;

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

  /* the wait mutex and cond */
  g_mutex_init(&(thread->wait_mutex));

  g_cond_init(&(thread->wait_cond));
  
  /* thread, tic mutex and cond */
  thread->thread = NULL;

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
      gdouble frequency;

      frequency = g_value_get_double(value);

      ags_thread_set_frequency(thread,
			       frequency);
    }
    break;
  case PROP_MAX_PRECISION:
    {
      gdouble max_precision;

      max_precision = g_value_get_double(value);

      ags_thread_set_max_precision(thread,
				   max_precision);
    }
    break;
  case PROP_DELAY:
  {
    gdouble delay;

    delay = g_value_get_double(value);

    ags_thread_set_delay(thread, delay);
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

      g_value_set_double(value, thread->frequency);

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

      g_value_set_double(value, thread->delay);

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

#ifdef AGS_DEBUG
  g_message("fin");
#endif
  
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

  GRecMutex *thread_mutex;

  thread = AGS_THREAD(connectable);

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  /* check is ready */
  g_rec_mutex_lock(thread_mutex);

  is_ready = ((AGS_CONNECTABLE_ADDED_TO_REGISTRY & (thread->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(thread_mutex);
  
  return(is_ready);
}

void
ags_thread_add_to_registry(AgsConnectable *connectable)
{
  AgsThread *thread;

  GRecMutex *thread_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  thread = AGS_THREAD(connectable);

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  g_rec_mutex_lock(thread_mutex);

  thread->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;
  
  g_rec_mutex_unlock(thread_mutex);
}

void
ags_thread_remove_from_registry(AgsConnectable *connectable)
{
  AgsThread *thread;

  GRecMutex *thread_mutex;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  thread = AGS_THREAD(connectable);

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  g_rec_mutex_lock(thread_mutex);

  thread->connectable_flags &= (~AGS_CONNECTABLE_ADDED_TO_REGISTRY);
  
  g_rec_mutex_unlock(thread_mutex);
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

  GRecMutex *thread_mutex;

  thread = AGS_THREAD(connectable);

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  /* check is connected */
  g_rec_mutex_lock(thread_mutex);

  is_connected = ((AGS_CONNECTABLE_CONNECTED & (thread->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(thread_mutex);
  
  return(is_connected);
}

void
ags_thread_connect(AgsConnectable *connectable)
{
  AgsThread *thread;
  AgsThread *children, *current_child, *next_child;

  GRecMutex *thread_mutex;

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  thread = AGS_THREAD(connectable);

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  g_rec_mutex_lock(thread_mutex);

  thread->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_rec_mutex_unlock(thread_mutex);

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

  GRecMutex *thread_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  thread = AGS_THREAD(connectable);

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  g_rec_mutex_lock(thread_mutex);

  thread->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_rec_mutex_unlock(thread_mutex);

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
 * @status_flags: the status flags
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
 * ags_thread_clear_status_flags:
 * @thread: the #AgsThread
 *
 * Clear status flags.
 * 
 * Since: 3.0.0
 */
void
ags_thread_clear_status_flags(AgsThread *thread)
{
  if(!AGS_IS_THREAD(thread)){
    return;
  }

  g_atomic_int_set(&(thread->status_flags), 0);
}

/**
 * ags_thread_test_sync_tic_flags:
 * @thread: the #AgsThread
 * @sync_tic_flags: the sync-tic flags
 *
 * Test @sync_tic_flags to be set on @thread.
 * 
 * Returns: %TRUE if sync-tic flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_thread_test_sync_tic_flags(AgsThread *thread, guint sync_tic_flags)
{
  gboolean retval;  

  if(!AGS_IS_THREAD(thread)){
    return(FALSE);
  }
  
  retval = ((sync_tic_flags & (g_atomic_int_get(&(thread->sync_tic_flags)))) != 0) ? TRUE: FALSE;
    
  return(retval);
}

/**
 * ags_thread_set_sync_tic_flags:
 * @thread: the #AgsThread
 * @sync_tic_flags: the sync-tic flags
 *
 * Set sync-tic flags.
 * 
 * Since: 3.0.0
 */
void
ags_thread_set_sync_tic_flags(AgsThread *thread, guint sync_tic_flags)
{
  if(!AGS_IS_THREAD(thread)){
    return;
  }

  g_atomic_int_or(&(thread->sync_tic_flags), sync_tic_flags);
}

/**
 * ags_thread_unset_sync_tic_flags:
 * @thread: the #AgsThread
 * @sync_tic_flags: the sync-tic flags
 *
 * Unset sync-tic flags.
 * 
 * Since: 3.0.0
 */
void
ags_thread_unset_sync_tic_flags(AgsThread *thread, guint sync_tic_flags)
{
  if(!AGS_IS_THREAD(thread)){
    return;
  }

  g_atomic_int_and(&(thread->sync_tic_flags), (~sync_tic_flags));
}

/**
 * ags_thread_clear_sync_tic_flags:
 * @thread: the #AgsThread
 *
 * Clear sync-tic flags.
 * 
 * Since: 3.0.0
 */
void
ags_thread_clear_sync_tic_flags(AgsThread *thread)
{
  if(!AGS_IS_THREAD(thread)){
    return;
  }
  
  g_atomic_int_set(&(thread->sync_tic_flags), 0);
}

/**
 * ags_thread_set_current_sync_tic:
 * @thread: the #AgsThread
 * @current_sync_tic: the current sync-tic
 *
 * Set current sync-tic.
 * 
 * Since: 3.0.0
 */
void
ags_thread_set_current_sync_tic(AgsThread *thread, guint current_sync_tic)
{
  if(!AGS_IS_THREAD(thread)){
    return;
  }

  g_atomic_int_set(&(thread->current_sync_tic), current_sync_tic);
}

/**
 * ags_thread_get_current_sync_tic:
 * @thread: the #AgsThread
 *
 * Get current sync-tic.
 * 
 * Returns: the current sync-tic
 * 
 * Since: 3.0.0
 */
guint
ags_thread_get_current_sync_tic(AgsThread *thread)
{
  guint current_sync_tic;
  
  if(!AGS_IS_THREAD(thread)){
    return(G_MAXUINT);
  }

  current_sync_tic = g_atomic_int_get(&(thread->current_sync_tic));

  return(current_sync_tic);
}

/**
 * ags_thread_set_delay:
 * @thread: the #AgsThread
 * @delay: the delay
 *
 * Set delay.
 * 
 * Since: 3.0.0
 */
void
ags_thread_set_delay(AgsThread *thread, gdouble delay)
{
  GRecMutex *thread_mutex;
  
  if(!AGS_IS_THREAD(thread)){
    return;
  }

  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  g_rec_mutex_lock(thread_mutex);

  thread->delay = delay;
  
  g_rec_mutex_unlock(thread_mutex);
}

/**
 * ags_thread_get_delay:
 * @thread: the #AgsThread
 *
 * Get delay.
 * 
 * Returns: the delay
 * 
 * Since: 3.0.0
 */
gdouble
ags_thread_get_delay(AgsThread *thread)
{
  gdouble delay;
  
  GRecMutex *thread_mutex;
  
  if(!AGS_IS_THREAD(thread)){
    return(0.0);
  }

  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  g_rec_mutex_lock(thread_mutex);

  delay = thread->delay;
  
  g_rec_mutex_unlock(thread_mutex);

  return(delay);
}

/**
 * ags_thread_set_frequency:
 * @thread: the #AgsThread
 * @frequency: the frequency
 *
 * Set frequency.
 * 
 * Since: 3.0.0
 */
void
ags_thread_set_frequency(AgsThread *thread, gdouble frequency)
{
  GRecMutex *thread_mutex;

  if(!AGS_IS_THREAD(thread)){
    return;
  }

  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  g_rec_mutex_lock(thread_mutex);

  if(frequency == thread->frequency){
    g_rec_mutex_unlock(thread_mutex);

    return;
  }

  thread->frequency = frequency;

  thread->delay = (guint) ceil((AGS_THREAD_HERTZ_JIFFIE / thread->frequency) / (AGS_THREAD_HERTZ_JIFFIE / thread->max_precision));
  thread->tic_delay = 0.0;

  if(ags_thread_test_flags(thread, AGS_THREAD_INTERMEDIATE_POST_SYNC)){
    thread->tic_delay = thread->delay;
  }else if(ags_thread_test_flags(thread, AGS_THREAD_INTERMEDIATE_PRE_SYNC)){
    thread->tic_delay = 1.0;
  }else{
    thread->tic_delay = 0.0;
  }

  g_rec_mutex_unlock(thread_mutex);
}

/**
 * ags_thread_get_frequency:
 * @thread: the #AgsThread
 *
 * Get frequency.
 * 
 * Returns: the frequency
 * 
 * Since: 3.0.0
 */
gdouble
ags_thread_get_frequency(AgsThread *thread)
{
  gdouble frequency;

  GRecMutex *thread_mutex;
  
  if(!AGS_IS_THREAD(thread)){
    return(-1.0);
  }

  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  g_rec_mutex_lock(thread_mutex);

  frequency = thread->frequency;
  
  g_rec_mutex_unlock(thread_mutex);

  return(frequency);
}

/**
 * ags_thread_set_max_precision:
 * @thread: the #AgsThread
 * @max_precision: the max-precision
 *
 * Set max-precision.
 * 
 * Since: 3.0.0
 */
void
ags_thread_set_max_precision(AgsThread *thread, gdouble max_precision)
{
  gdouble old_max_precision;
  
  GRecMutex *thread_mutex;

  if(!AGS_IS_THREAD(thread)){
    return;
  }

  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  g_rec_mutex_lock(thread_mutex);

  old_max_precision = thread->max_precision;
  
  if(max_precision == old_max_precision){
    g_rec_mutex_unlock(thread_mutex);
    
    return;
  }
      
  thread->max_precision = max_precision;

  thread->frequency = thread->frequency / old_max_precision * max_precision;

  thread->delay = (AGS_THREAD_HERTZ_JIFFIE / thread->frequency) / (AGS_THREAD_HERTZ_JIFFIE / thread->max_precision);
  thread->tic_delay = 0.0;
  
  if(ags_thread_test_flags(thread, AGS_THREAD_INTERMEDIATE_POST_SYNC)){
    thread->tic_delay = floor(thread->delay); // thread->delay;
  }else if(ags_thread_test_flags(thread, AGS_THREAD_INTERMEDIATE_PRE_SYNC)){
    thread->tic_delay = 1.0;
      }else{
    thread->tic_delay = 0.0;
  }
  
  g_rec_mutex_unlock(thread_mutex);
}

/**
 * ags_thread_get_max_precision:
 * @thread: the #AgsThread
 *
 * Get max-precision.
 * 
 * Returns: the max-precision
 * 
 * Since: 3.0.0
 */
gdouble
ags_thread_get_max_precision(AgsThread *thread)
{
  gdouble max_precision;
  
  GRecMutex *thread_mutex;

  if(!AGS_IS_THREAD(thread)){
    return(-1.0);
  }

  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  g_rec_mutex_lock(thread_mutex);

  max_precision = thread->max_precision;
  
  g_rec_mutex_unlock(thread_mutex);

  return(max_precision);
}

/**
 * ags_thread_parent:
 * @thread: the #AgsThread
 *
 * Get parent thread.
 * 
 * Returns: (transfer full): the parent #AgsThread
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
  
  g_rec_mutex_unlock(thread_mutex);

  if(parent != NULL){
    g_object_ref(parent);
  }
  
  return(parent);
}

/**
 * ags_thread_next:
 * @thread: the #AgsThread
 *
 * Get next thread.
 * 
 * Returns: (transfer full): the next #AgsThread
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
 * Returns: (transfer full): the prev #AgsThread
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
 * Returns: (transfer full): the children #AgsThread
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
 * Returns: (transfer full): the toplevevel #AgsThread
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
 * Returns: (transfer full): the very first #AgsThread within same tree level
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
 * Returns: (transfer full): the very last @AgsThread within same tree level
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
#if 0
    if(current_parent != NULL){
      g_object_unref(current_parent);
    }
#endif
    
    return;
  }  

  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(ags_application_context_get_instance()));
  
  tree_lock = NULL;

  if(main_loop != NULL){
    tree_lock = ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop));
  }
  
  if(tree_lock != NULL){
    g_rec_mutex_lock(tree_lock);
  }

  if(current_parent != NULL){
    g_object_unref(current_parent);
  }
  
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
    g_object_ref(child);
    
    g_rec_mutex_unlock(AGS_THREAD_GET_OBJ_MUTEX(last_child));

    /* prev */
    g_rec_mutex_lock(AGS_THREAD_GET_OBJ_MUTEX(child));

    child->prev = last_child;

    g_rec_mutex_unlock(AGS_THREAD_GET_OBJ_MUTEX(child));

    /* unref */
    g_object_unref(children);
  }

  if(tree_lock != NULL){
    g_rec_mutex_unlock(tree_lock);
  }

  if(main_loop != NULL){
    g_object_unref(main_loop);
  }
  
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

gboolean
ags_thread_is_current_ready(AgsThread *current, guint current_sync_tic)
{
  guint sync_tic;
  gboolean is_current_ready;

  if(!AGS_IS_THREAD(current)){
    return(TRUE);
  }
  
  if((!ags_thread_test_status_flags(current, AGS_THREAD_STATUS_RUNNING) &&
      !ags_thread_test_status_flags(current, AGS_THREAD_STATUS_READY)) ||
     ags_thread_test_status_flags(current, AGS_THREAD_STATUS_IS_CHAOS_TREE)){
    return(TRUE);
  }

  is_current_ready = TRUE;

  /* check current sync tic */
  sync_tic = ags_thread_get_current_sync_tic(current);
  
  if(current_sync_tic == sync_tic){
    if(!ags_thread_test_status_flags(current, AGS_THREAD_STATUS_WAITING)){
      is_current_ready = FALSE;
    }
  }

  return(is_current_ready);
}

gboolean
ags_thread_is_tree_ready_recursive(AgsThread *thread, guint current_sync_tic)
{
  AgsThread *child, *next_child;
  
  gboolean is_tree_ready;
  
  if(!AGS_IS_THREAD(thread)){
    return(TRUE);
  }

  is_tree_ready = ags_thread_is_current_ready(thread, current_sync_tic);

  if(!is_tree_ready){
    return(FALSE);
  }
  
  /* set tree sync recursive */
  child = ags_thread_children(thread);

  while(child != NULL){
    is_tree_ready = ags_thread_is_tree_ready_recursive(child, current_sync_tic);

    if(!is_tree_ready){
      g_object_unref(child);
      
      return(FALSE);
    }

    /* iterate */
    next_child = ags_thread_next(child);

    g_object_unref(child);

    child = next_child;
  }

  return(TRUE);
}

void
ags_thread_prepare_current_sync(AgsThread *current, guint current_sync_tic)
{
  if(!AGS_IS_THREAD(current)){
    return;
  }
  
  /* get current mutex */  
  if((!ags_thread_test_status_flags(current, AGS_THREAD_STATUS_RUNNING) &&
      !ags_thread_test_status_flags(current, AGS_THREAD_STATUS_READY)) ||
     ags_thread_test_status_flags(current, AGS_THREAD_STATUS_IS_CHAOS_TREE)){
    return;
  }

  if(ags_thread_get_current_sync_tic(current) != current_sync_tic){
    g_critical("out-of-sync - main sync-tic != current sync-tic");
    
    return;
  }

  ags_thread_set_status_flags(current, AGS_THREAD_STATUS_SYNCED);
}

void
ags_thread_prepare_tree_sync_recursive(AgsThread *thread, guint current_sync_tic)
{
  AgsThread *child, *next_child;
  
  if(!AGS_IS_THREAD(thread)){
    return;
  }

  ags_thread_prepare_current_sync(thread, current_sync_tic);

  /* set tree sync recursive */
  child = ags_thread_children(thread);

  while(child != NULL){
    ags_thread_prepare_tree_sync_recursive(child, current_sync_tic);

    /* iterate */
    next_child = ags_thread_next(child);

    g_object_unref(child);

    child = next_child;
  }
}

void
ags_thread_set_current_sync(AgsThread *current, guint current_sync_tic)
{
  guint next_current_sync_tic;
  guint sync_tic_wait, sync_tic_done;

  GMutex *wait_mutex;
  GCond *wait_cond;

  if(!AGS_IS_THREAD(current)){
    return;
  }
  
  /* get current mutex */  
  if((!ags_thread_test_status_flags(current, AGS_THREAD_STATUS_RUNNING) &&
      !ags_thread_test_status_flags(current, AGS_THREAD_STATUS_READY)) ||
     ags_thread_test_status_flags(current, AGS_THREAD_STATUS_IS_CHAOS_TREE)){
    return;
  }

  if(ags_thread_get_current_sync_tic(current) != current_sync_tic){
    g_critical("out-of-sync - main sync-tic != current sync-tic");
    
    return;
  }
  
  wait_mutex = AGS_THREAD_GET_WAIT_MUTEX(current);
  wait_cond = AGS_THREAD_GET_WAIT_COND(current);

  next_current_sync_tic = G_MAXUINT;
  
  sync_tic_wait = 0;
  sync_tic_done = 0;
  
  switch(current_sync_tic){
  case 0:
  {
    next_current_sync_tic = 1;
    
    sync_tic_wait = AGS_THREAD_SYNC_TIC_WAIT_0;
    sync_tic_done = AGS_THREAD_SYNC_TIC_DONE_0;
  }
  break;
  case 1:
  {
    next_current_sync_tic = 2;

    sync_tic_wait = AGS_THREAD_SYNC_TIC_WAIT_1;
    sync_tic_done = AGS_THREAD_SYNC_TIC_DONE_1;
  }
  break;
  case 2:
  {
    next_current_sync_tic = 3;

    sync_tic_wait = AGS_THREAD_SYNC_TIC_WAIT_2;
    sync_tic_done = AGS_THREAD_SYNC_TIC_DONE_2;
  }
  break;
  case 3:
  {
    next_current_sync_tic = 4;

    sync_tic_wait = AGS_THREAD_SYNC_TIC_WAIT_3;
    sync_tic_done = AGS_THREAD_SYNC_TIC_DONE_3;
  }
  break;
  case 4:
  {
    next_current_sync_tic = 5;

    sync_tic_wait = AGS_THREAD_SYNC_TIC_WAIT_4;
    sync_tic_done = AGS_THREAD_SYNC_TIC_DONE_4;
  }
  break;
  case 5:
  {
    next_current_sync_tic = 6;

    sync_tic_wait = AGS_THREAD_SYNC_TIC_WAIT_5;
    sync_tic_done = AGS_THREAD_SYNC_TIC_DONE_5;
  }
  break;
  case 6:
  {
    next_current_sync_tic = 7;

    sync_tic_wait = AGS_THREAD_SYNC_TIC_WAIT_6;
    sync_tic_done = AGS_THREAD_SYNC_TIC_DONE_6;
  }
  break;
  case 7:
  {
    next_current_sync_tic = 8;

    sync_tic_wait = AGS_THREAD_SYNC_TIC_WAIT_7;
    sync_tic_done = AGS_THREAD_SYNC_TIC_DONE_7;
  }
  break;
  case 8:
  {
    next_current_sync_tic = 0;

    sync_tic_wait = AGS_THREAD_SYNC_TIC_WAIT_8;
    sync_tic_done = AGS_THREAD_SYNC_TIC_DONE_8;
  }
  break;
  default:
    g_critical("invalid current sync-tic");
  }

  ags_thread_unset_status_flags(current, AGS_THREAD_STATUS_WAITING);

  /* apply next current sync tic */
  ags_thread_set_current_sync_tic(current, next_current_sync_tic);
  
  g_mutex_lock(wait_mutex);

  ags_thread_set_sync_tic_flags(current, sync_tic_done);

  if(ags_thread_test_sync_tic_flags(current, sync_tic_wait)){
    g_cond_signal(wait_cond);
  }

  g_mutex_unlock(wait_mutex);
}

void
ags_thread_set_tree_sync_recursive(AgsThread *thread, guint current_sync_tic)
{
  AgsThread *child, *next_child;
  
  if(!AGS_IS_THREAD(thread)){
    return;
  }

  ags_thread_set_current_sync(thread, current_sync_tic);

  /* set tree sync recursive */
  child = ags_thread_children(thread);

  while(child != NULL){
    ags_thread_set_tree_sync_recursive(child, current_sync_tic);

    /* iterate */
    next_child = ags_thread_next(child);

    g_object_unref(child);

    child = next_child;
  }
}

guint
ags_thread_real_clock(AgsThread *thread)
{
  AgsThread *main_loop;
  AgsTaskLauncher *task_launcher;
  
  AgsApplicationContext *application_context;

  guint main_sync_tic, current_sync_tic, next_main_sync_tic;
  gdouble next_tic_delay, prev_tic_delay;
  guint sync_tic_wait, sync_tic_done;
  guint next_sync_tic_wait, next_sync_tic_done;    
  guint clocked_steps;
  gboolean initial_sync;
  
  GRecMutex *thread_mutex;
  GRecMutex *main_loop_mutex;
  GRecMutex *tree_mutex;
  GMutex *thread_start_mutex;
  GCond *thread_start_cond;
  GMutex *wait_mutex;
  GCond *wait_cond;
  
  application_context = ags_application_context_get_instance();
  
  /* get thread mutex */  
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  /* get main loop */
  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  main_loop_mutex = AGS_THREAD_GET_OBJ_MUTEX(main_loop);

  tree_mutex = ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop));
  
  /* wait */
  wait_mutex = AGS_THREAD_GET_WAIT_MUTEX(thread);
  wait_cond = AGS_THREAD_GET_WAIT_COND(thread);

  /* check initial sync */
  initial_sync = FALSE;
  
  if(ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_INITIAL_SYNC)){
    initial_sync = TRUE;

    while(ags_main_loop_is_critical_region(AGS_MAIN_LOOP(main_loop))){
#if !defined(AGS_WITH_RT)
      g_usleep(4);
#endif
    }
    
    /* increment queued critical region */
    ags_main_loop_inc_queued_critical_region(AGS_MAIN_LOOP(main_loop));
  }

  thread_start_mutex = AGS_THREAD_GET_START_MUTEX(thread);
  thread_start_cond = AGS_THREAD_GET_START_COND(thread);

  /* notify start */
  g_mutex_lock(thread_start_mutex);
      
  ags_thread_set_status_flags(thread, AGS_THREAD_STATUS_START_DONE);
  
  if(ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_START_WAIT)){
    ags_thread_unset_status_flags(thread, AGS_THREAD_STATUS_START_WAIT);
    
    g_cond_broadcast(thread_start_cond);
  }
      
  g_mutex_unlock(thread_start_mutex);

  g_rec_mutex_lock(tree_mutex);

  if(thread == main_loop){
    while(ags_main_loop_test_queued_critical_region(AGS_MAIN_LOOP(main_loop)) != 0){
      g_rec_mutex_unlock(tree_mutex);
    
      g_rec_mutex_lock(tree_mutex);
    }

    ags_main_loop_set_critical_region(AGS_MAIN_LOOP(main_loop), TRUE);
  }  
  
  if(ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_IS_CHAOS_TREE)){
    while(ags_main_loop_is_syncing(AGS_MAIN_LOOP(main_loop))){
      g_rec_mutex_unlock(tree_mutex);

#if !defined(AGS_WITH_RT)
      g_usleep(4);
#endif
      
      g_rec_mutex_lock(tree_mutex);
    }

    ags_thread_unset_status_flags(thread, AGS_THREAD_STATUS_IS_CHAOS_TREE);
  }
  
  /* sync tic */
  main_sync_tic = ags_thread_get_current_sync_tic(main_loop);

  current_sync_tic = ags_thread_get_current_sync_tic(thread);
  
  if(initial_sync){
    current_sync_tic = main_sync_tic;
    ags_thread_set_current_sync_tic(thread, main_sync_tic);
  }
  
  sync_tic_wait = 0;
  sync_tic_done = 0;
  
  switch(current_sync_tic){
  case 0:
  {
    sync_tic_wait = AGS_THREAD_SYNC_TIC_WAIT_0;
    sync_tic_done = AGS_THREAD_SYNC_TIC_DONE_0;
  }
  break;
  case 1:
  {
    sync_tic_wait = AGS_THREAD_SYNC_TIC_WAIT_1;
    sync_tic_done = AGS_THREAD_SYNC_TIC_DONE_1;
  }
  break;
  case 2:
  {
    sync_tic_wait = AGS_THREAD_SYNC_TIC_WAIT_2;
    sync_tic_done = AGS_THREAD_SYNC_TIC_DONE_2;
  }
  break;
  case 3:
  {
    sync_tic_wait = AGS_THREAD_SYNC_TIC_WAIT_3;
    sync_tic_done = AGS_THREAD_SYNC_TIC_DONE_3;
  }
  break;
  case 4:
  {
    sync_tic_wait = AGS_THREAD_SYNC_TIC_WAIT_4;
    sync_tic_done = AGS_THREAD_SYNC_TIC_DONE_4;
  }
  break;
  case 5:
  {
    sync_tic_wait = AGS_THREAD_SYNC_TIC_WAIT_5;
    sync_tic_done = AGS_THREAD_SYNC_TIC_DONE_5;
  }
  break;
  case 6:
  {
    sync_tic_wait = AGS_THREAD_SYNC_TIC_WAIT_6;
    sync_tic_done = AGS_THREAD_SYNC_TIC_DONE_6;
  }
  break;
  case 7:
  {
    sync_tic_wait = AGS_THREAD_SYNC_TIC_WAIT_7;
    sync_tic_done = AGS_THREAD_SYNC_TIC_DONE_7;
  }
  break;
  case 8:
  {
    sync_tic_wait = AGS_THREAD_SYNC_TIC_WAIT_8;
    sync_tic_done = AGS_THREAD_SYNC_TIC_DONE_8;
  }
  break;
  default:
    g_critical("invalid current sync-tic");
  }

  next_main_sync_tic = G_MAXUINT;
  
  next_sync_tic_wait = 0;
  next_sync_tic_done = 0;
  
  switch(main_sync_tic){
  case 0:
  {
    next_main_sync_tic = 1;
    
    next_sync_tic_wait = AGS_THREAD_SYNC_TIC_WAIT_1;
    next_sync_tic_done = AGS_THREAD_SYNC_TIC_DONE_1;
  }
  break;
  case 1:
  {
    next_main_sync_tic = 2;
    
    next_sync_tic_wait = AGS_THREAD_SYNC_TIC_WAIT_2;
    next_sync_tic_done = AGS_THREAD_SYNC_TIC_DONE_2;
  }
  break;
  case 2:
  {
    next_main_sync_tic = 3;
    
    next_sync_tic_wait = AGS_THREAD_SYNC_TIC_WAIT_3;
    next_sync_tic_done = AGS_THREAD_SYNC_TIC_DONE_3;
  }
  break;
  case 3:
  {
    next_main_sync_tic = 4;

    next_sync_tic_wait = AGS_THREAD_SYNC_TIC_WAIT_4;
    next_sync_tic_done = AGS_THREAD_SYNC_TIC_DONE_4;
  }
  break;
  case 4:
  {
    next_main_sync_tic = 5;
    
    next_sync_tic_wait = AGS_THREAD_SYNC_TIC_WAIT_5;
    next_sync_tic_done = AGS_THREAD_SYNC_TIC_DONE_5;
  }
  break;
  case 5:
  {
    next_main_sync_tic = 6;
    
    next_sync_tic_wait = AGS_THREAD_SYNC_TIC_WAIT_6;
    next_sync_tic_done = AGS_THREAD_SYNC_TIC_DONE_6;
  }
  break;
  case 6:
  {
    next_main_sync_tic = 7;
    
    next_sync_tic_wait = AGS_THREAD_SYNC_TIC_WAIT_7;
    next_sync_tic_done = AGS_THREAD_SYNC_TIC_DONE_7;
  }
  break;
  case 7:
  {
    next_main_sync_tic = 8;
    
    next_sync_tic_wait = AGS_THREAD_SYNC_TIC_WAIT_8;
    next_sync_tic_done = AGS_THREAD_SYNC_TIC_DONE_8;
  }
  break;
  case 8:
  {
    next_main_sync_tic = 0;
    
    next_sync_tic_wait = AGS_THREAD_SYNC_TIC_WAIT_0;
    next_sync_tic_done = AGS_THREAD_SYNC_TIC_DONE_0;
  }
  break;
  default:
    g_critical("invalid main sync-tic");
  }

  /* do initial sync */
  if(initial_sync){
    gdouble main_delay;
    gdouble main_tic_delay, next_main_tic_delay, prev_main_tic_delay;
    
    g_rec_mutex_lock(main_loop_mutex);
      
    main_delay = main_loop->delay;
    main_tic_delay = main_loop->tic_delay;

    g_rec_mutex_unlock(main_loop_mutex);
    
    if(main_tic_delay + 1.0 < main_delay){
      next_main_tic_delay = main_tic_delay + 1.0;
    }else{
      next_main_tic_delay =  0.0; //(main_tic_delay + 1.0) - floor(main_delay); // (main_tic_delay + 1.0) - main_delay;
    }

    if(main_tic_delay - 1.0 > 0.0){
      prev_main_tic_delay = main_tic_delay - 1.0;
    }else{
      prev_main_tic_delay = main_delay - 1.0; // (floor(main_delay) + 1.0) - (floor(main_delay) + main_tic_delay); // (main_delay + 1.0) - (main_delay + main_tic_delay);
    }
  
    ags_thread_set_sync_tic_flags(thread, sync_tic_wait);
    ags_thread_unset_sync_tic_flags(thread, sync_tic_done);	
      
    /* mark synced */
    if(ags_thread_test_flags(thread, AGS_THREAD_IMMEDIATE_SYNC)){
      thread->tic_delay = main_tic_delay;
    }else if(ags_thread_test_flags(thread, AGS_THREAD_INTERMEDIATE_PRE_SYNC)){
      thread->tic_delay = next_main_tic_delay;
    }else if(ags_thread_test_flags(thread, AGS_THREAD_INTERMEDIATE_POST_SYNC)){
      thread->tic_delay = prev_main_tic_delay;
    }else{
      thread->tic_delay = 0.0;
    }
      
    ags_thread_set_flags(thread, AGS_THREAD_MARK_SYNCED);
    ags_thread_set_status_flags(thread, (AGS_THREAD_STATUS_SYNCED_FREQ));
    
    /* unset status flags */
    ags_thread_unset_status_flags(thread, AGS_THREAD_STATUS_INITIAL_SYNC);

    /* decrement queued critical region */
    ags_main_loop_dec_queued_critical_region(AGS_MAIN_LOOP(main_loop));
  }

  /* synchronize */
  ags_thread_set_status_flags(thread, AGS_THREAD_STATUS_WAITING);
    
  if(current_sync_tic != main_sync_tic ||
     !ags_thread_is_tree_ready_recursive(main_loop, current_sync_tic)){
    gboolean unlock_tree;

    unlock_tree = TRUE;
          
    g_mutex_lock(wait_mutex);
      
    if(ags_thread_test_sync_tic_flags(thread, sync_tic_wait) &&
       !ags_thread_test_sync_tic_flags(thread, sync_tic_done)){
      ags_thread_set_sync_tic_flags(thread, sync_tic_wait);

      while(ags_thread_test_sync_tic_flags(thread, sync_tic_wait) &&
	    !ags_thread_test_sync_tic_flags(thread, sync_tic_done)){
	if(unlock_tree){
	  unlock_tree = FALSE;
	  
	  g_rec_mutex_unlock(tree_mutex);
	}
	
	g_cond_wait(wait_cond,
		    wait_mutex);
      }
    }

    ags_thread_unset_status_flags(thread, AGS_THREAD_STATUS_WAITING);

    if(unlock_tree){
      unlock_tree = FALSE;
	  
      g_rec_mutex_unlock(tree_mutex);
    }

    ags_thread_unset_sync_tic_flags(thread, sync_tic_wait);
    ags_thread_unset_sync_tic_flags(thread, sync_tic_done);

    ags_thread_set_sync_tic_flags(thread, next_sync_tic_wait);
    ags_thread_unset_sync_tic_flags(thread, next_sync_tic_done);
      
    g_mutex_unlock(wait_mutex);
  }else{
    ags_main_loop_set_syncing(AGS_MAIN_LOOP(main_loop), TRUE);

    ags_thread_unset_status_flags(thread, AGS_THREAD_STATUS_WAITING);

    /* get task launcher */
    task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));
      
    /* run task launcher */
    ags_task_launcher_sync_run(task_launcher);

    if(task_launcher != NULL){
      g_object_unref(task_launcher);
    }
    
    /* signal */
#if 1
    if(main_sync_tic == current_sync_tic){
    }else{
      g_critical("out-of-sync - main sync-tic != current sync-tic");
    }

    ags_thread_prepare_tree_sync_recursive(main_loop,
					   main_sync_tic);

    ags_thread_set_tree_sync_recursive(main_loop,
				       main_sync_tic);
#else
    ags_thread_prepare_tree_sync_recursive(main_loop,
					   main_sync_tic);

    ags_thread_set_tree_sync_recursive(main_loop,
				       main_sync_tic);
#endif
    
    ags_main_loop_set_syncing(AGS_MAIN_LOOP(main_loop), FALSE);

    ags_thread_unset_sync_tic_flags(thread, sync_tic_wait);
    ags_thread_unset_sync_tic_flags(thread, sync_tic_done);

    ags_thread_set_sync_tic_flags(thread, next_sync_tic_wait);
    ags_thread_unset_sync_tic_flags(thread, next_sync_tic_done);

    g_rec_mutex_unlock(tree_mutex);
  }

  /* get task launcher */
  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

  if(task_launcher != NULL){
    g_mutex_lock(&(task_launcher->wait_mutex));
  
    if(g_atomic_int_get(&(task_launcher->is_running))){
      g_atomic_int_inc(&(task_launcher->wait_count));

      while(g_atomic_int_get(&(task_launcher->is_running)) &&
	    g_atomic_int_get(&(task_launcher->wait_count)) != 0){
	g_cond_wait(&(task_launcher->wait_cond),
		    &(task_launcher->wait_mutex));
      }
    }

    g_mutex_unlock(&(task_launcher->wait_mutex));

    g_object_unref(task_launcher);
  }

  /* compute clocked steps */
  clocked_steps = 0;
  
  g_rec_mutex_lock(thread_mutex);
  
  if(thread->delay >= 1.0){
    thread->tic_delay += 1.0;
    
    if(thread->tic_delay >= thread->delay){
      clocked_steps = 1;

      thread->tic_delay = 0.0; // thread->delay;
    }

    if(initial_sync){
      /* prepare initial run */
      ags_thread_set_status_flags(thread, AGS_THREAD_STATUS_INITIAL_RUN);	
    }
  }else{
    clocked_steps = (guint) floor((1.0 + thread->tic_delay) / thread->delay);

    thread->tic_delay = 0.0; // (1.0 / thread->delay) - floor(1.0 / thread->delay);
  }

  g_rec_mutex_unlock(thread_mutex);

  if(thread == main_loop){
    ags_main_loop_set_critical_region(AGS_MAIN_LOOP(main_loop), FALSE);
  }

  if(main_loop != NULL){
    g_object_unref(main_loop);
  }
  
  return(clocked_steps);
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

void*
ags_thread_loop(void *ptr)
{
  AgsThread *main_loop;
  AgsThread *thread;
  AgsTaskLauncher *task_launcher;
  
  AgsApplicationContext *application_context;
  
  GList *start_start_queue, *start_queue;
  
  guint main_sync_tic, current_sync_tic;
  guint i, i_stop;

  GRecMutex *tree_mutex;
  GRecMutex *thread_mutex;
  
  thread = (AgsThread *) ptr;

  application_context = ags_application_context_get_instance();

  /* set private for ags_thread_self() */
  g_object_ref(thread);

  g_private_set(&ags_thread_key,
		thread);

  if(!AGS_IS_MAIN_LOOP(thread)){
    ags_thread_set_status_flags(thread, AGS_THREAD_STATUS_IS_CHAOS_TREE);
  }

  /* set/unset run flags */
  ags_thread_unset_status_flags(thread,
				(AGS_THREAD_STATUS_RT_SETUP |
				 AGS_THREAD_STATUS_WAITING |
				 AGS_THREAD_STATUS_SYNCED |
				 AGS_THREAD_STATUS_SYNCED_FREQ));

  ags_thread_unset_sync_tic_flags(thread, (AGS_THREAD_SYNC_TIC_WAIT_0 |
					   AGS_THREAD_SYNC_TIC_DONE_0 |
					   AGS_THREAD_SYNC_TIC_WAIT_1 |
					   AGS_THREAD_SYNC_TIC_DONE_1 |
					   AGS_THREAD_SYNC_TIC_WAIT_2 |
					   AGS_THREAD_SYNC_TIC_DONE_2 |
					   AGS_THREAD_SYNC_TIC_WAIT_3 |
					   AGS_THREAD_SYNC_TIC_DONE_3 |
					   AGS_THREAD_SYNC_TIC_WAIT_4 |
					   AGS_THREAD_SYNC_TIC_DONE_4 |
					   AGS_THREAD_SYNC_TIC_WAIT_5 |
					   AGS_THREAD_SYNC_TIC_DONE_5 |
					   AGS_THREAD_SYNC_TIC_WAIT_6 |
					   AGS_THREAD_SYNC_TIC_DONE_6 |
					   AGS_THREAD_SYNC_TIC_WAIT_7 |
					   AGS_THREAD_SYNC_TIC_DONE_7 |
					   AGS_THREAD_SYNC_TIC_WAIT_8 |
					   AGS_THREAD_SYNC_TIC_DONE_8));

  ags_thread_unset_flags(thread,
			 (AGS_THREAD_MARK_SYNCED));

  ags_thread_set_status_flags(thread,
			      (AGS_THREAD_STATUS_RUNNING |
			       AGS_THREAD_STATUS_READY |
			       AGS_THREAD_STATUS_INITIAL_RUN |
			       AGS_THREAD_STATUS_INITIAL_SYNC));

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

  while(ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_RUNNING)){
    /* start queue */
    g_rec_mutex_lock(thread_mutex);
    
    start_queue = 
      start_start_queue = thread->start_queue;

    thread->start_queue = NULL;

    g_rec_mutex_unlock(thread_mutex);
    
    while(start_queue != NULL){
      AgsThread *queued_thread;
      
      GMutex *queued_thread_start_mutex;
      GCond *queued_thread_start_cond;
      
      queued_thread = (AgsThread *) start_queue->data;
      g_object_ref(queued_thread);
      
      /*  */
      ags_thread_start(queued_thread);

      queued_thread_start_mutex = AGS_THREAD_GET_START_MUTEX(queued_thread);
      queued_thread_start_cond = AGS_THREAD_GET_START_COND(queued_thread);
      
      g_mutex_lock(queued_thread_start_mutex);

      if(ags_thread_test_status_flags(queued_thread, AGS_THREAD_STATUS_START_WAIT) &&
	 !ags_thread_test_status_flags(queued_thread, AGS_THREAD_STATUS_START_DONE)){
	ags_thread_set_status_flags(queued_thread, AGS_THREAD_STATUS_START_WAIT);
      
	while(ags_thread_test_status_flags(queued_thread, AGS_THREAD_STATUS_START_WAIT) &&
	      !ags_thread_test_status_flags(queued_thread, AGS_THREAD_STATUS_START_DONE)){
	  g_cond_wait(queued_thread_start_cond,
		      queued_thread_start_mutex);
	}
      }
      
      g_mutex_unlock(queued_thread_start_mutex);
      
      g_object_unref(queued_thread);

      /* iterate */
      start_queue = start_queue->next;
    }

    start_queue = start_start_queue;

    while(start_queue != NULL){
      while(!ags_thread_test_status_flags(start_queue->data, AGS_THREAD_STATUS_SYNCED_FREQ));
      
      /* iterate */
      start_queue = start_queue->next;
    }
    
    g_list_free_full(start_start_queue,
		     g_object_unref);
    
    /* run with clock synchronization */
    i_stop = ags_thread_clock(thread);

    /* set busy flag */
    if(i_stop > 0){
      ags_thread_set_status_flags(thread, AGS_THREAD_STATUS_BUSY);
    }
    
    for(i = 0; i < i_stop && ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_RUNNING); i++){
#if 0
      g_message("run thread 0x%x", thread);
#endif
      
      /* run */
      ags_thread_run(thread);
    }    

    /* unset busy flag */
    if(i_stop > 0){
      if(ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_INITIAL_RUN)){
	ags_thread_unset_status_flags(thread, AGS_THREAD_STATUS_INITIAL_RUN);
      }
      
      ags_thread_unset_status_flags(thread, AGS_THREAD_STATUS_BUSY);
    }
  }

  /* get main loop */
  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  tree_mutex = ags_main_loop_get_tree_lock(AGS_MAIN_LOOP(main_loop));  
  
  g_rec_mutex_lock(tree_mutex);

  /* sync tic */
  main_sync_tic = ags_thread_get_current_sync_tic(main_loop);

  current_sync_tic = ags_thread_get_current_sync_tic(thread);

#ifdef AGS_DEBUG
  g_message("thread finish %d %d", main_sync_tic, current_sync_tic);
#endif
  
  ags_thread_clear_status_flags(thread);
  ags_thread_clear_sync_tic_flags(thread);
  
  if(!ags_thread_is_tree_ready_recursive(main_loop, current_sync_tic) ||
     current_sync_tic != main_sync_tic){
    if(main_sync_tic != current_sync_tic){
      g_critical("out-of-sync - main sync-tic != current sync-tic");
    }
    
    g_rec_mutex_unlock(tree_mutex);
  }else{
    AgsTaskLauncher *task_launcher;

    ags_main_loop_set_syncing(AGS_MAIN_LOOP(main_loop), TRUE);

    /* get task launcher */
    task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

    /* run task launcher */
    ags_task_launcher_sync_run(task_launcher);

    if(task_launcher != NULL){
      g_object_unref(task_launcher);
    }
    
    /* signal */
#if 1
    if(main_sync_tic == current_sync_tic){
    }else{
      g_critical("out-of-sync - main sync-tic != current sync-tic");
    }

    ags_thread_prepare_tree_sync_recursive(main_loop,
					   main_sync_tic);

    ags_thread_set_tree_sync_recursive(main_loop,
				       main_sync_tic);
#else
    ags_thread_prepare_tree_sync_recursive(main_loop,
					   main_sync_tic);

    ags_thread_set_tree_sync_recursive(main_loop,
				       main_sync_tic);
#endif

    ags_main_loop_set_syncing(AGS_MAIN_LOOP(main_loop), FALSE);
      
    g_rec_mutex_unlock(tree_mutex);
  }

  if(main_loop != NULL){
    g_object_unref(main_loop);
  }
  
  /* exit thread */
  ags_thread_unset_flags(thread, AGS_THREAD_MARK_SYNCED);

  thread->thread = NULL;
  
  g_thread_exit(NULL);

  return(NULL);
}

void
ags_thread_real_start(AgsThread *thread)
{
  if(thread->thread != NULL ||
     ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_RUNNING)){
    return;
  }
  
#ifdef AGS_DEBUG
  g_message("thread start: %s", G_OBJECT_TYPE_NAME(thread));
#endif
  
  /*  */
  thread->thread = g_thread_new("Advanced Gtk+ Sequencer - clock",
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
  g_return_if_fail(AGS_IS_THREAD(thread));
  
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
 * @child: (element-type Ags.Thread): the children as #GList-struct containing #AgsThread to start
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
  if(thread->thread == NULL ||
     !ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_RUNNING)){
    return;
  }
  
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
  g_return_if_fail(AGS_IS_THREAD(thread));
  
  g_object_ref(G_OBJECT(thread));
  g_signal_emit(G_OBJECT(thread),
		thread_signals[STOP], 0);
  g_object_unref(G_OBJECT(thread));
}

/**
 * ags_thread_find_type:
 * @thread: the #AgsThread
 * @gtype: the #GType-struct
 * 
 * Find @gtype as descendant of @thread. If its a descendant thread,
 * the ref-count is increased.
 * 
 * Returns: (transfer full): the matching #AgsThread
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

/**
 * ags_thread_self:
 * 
 * Thread self.
 * 
 * Returns: (transfer full): the running #AgsThread
 * 
 * Since: 3.0.0
 */
AgsThread*
ags_thread_self(void)
{
  AgsThread *self;

  self = (AgsThread *) g_private_get(&ags_thread_key);
  g_object_ref(self);
  
  return(self);
}

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
