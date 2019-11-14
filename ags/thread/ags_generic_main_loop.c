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

#include <ags/thread/ags_generic_main_loop.h>

#include <ags/object/ags_main_loop.h>

#include <ags/i18n.h>

void ags_generic_main_loop_class_init(AgsGenericMainLoopClass *generic_main_loop);
void ags_generic_main_loop_main_loop_interface_init(AgsMainLoopInterface *main_loop);
void ags_generic_main_loop_init(AgsGenericMainLoop *generic_main_loop);
void ags_generic_main_loop_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_generic_main_loop_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_generic_main_loop_finalize(GObject *gobject);

GRecMutex* ags_generic_main_loop_get_tree_lock(AgsMainLoop *main_loop);
void ags_generic_main_loop_set_tic(AgsMainLoop *main_loop, guint tic);
guint ags_generic_main_loop_get_tic(AgsMainLoop *main_loop);
void ags_generic_main_loop_set_last_sync(AgsMainLoop *main_loop, guint last_sync);
guint ags_generic_main_loop_get_last_sync(AgsMainLoop *main_loop);
void ags_generic_main_loop_change_frequency(AgsMainLoop *main_loop,
				     gdouble frequency);
void ags_generic_main_loop_sync_counter_inc(AgsMainLoop *main_loop, guint tic);
void ags_generic_main_loop_sync_counter_dec(AgsMainLoop *main_loop, guint tic);
gboolean ags_generic_main_loop_sync_counter_test(AgsMainLoop *main_loop, guint tic);
void ags_generic_main_loop_set_sync_tic(AgsMainLoop *main_loop, guint tic);
guint ags_generic_main_loop_get_sync_tic(AgsMainLoop *main_loop);

void ags_generic_main_loop_start(AgsThread *thread);

/**
 * SECTION:ags_generic_main_loop
 * @short_description: generic loop
 * @title: AgsGenericMainLoop
 * @section_id:
 * @include: ags/thread/ags_generic_main_loop.h
 *
 * The #AgsGenericMainLoop is suitable as #AgsMainLoop and does
 * generic processing.
 */

static gpointer ags_generic_main_loop_parent_class = NULL;

enum{
  PROP_0,
};

GType
ags_generic_main_loop_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_generic_main_loop = 0;

    static const GTypeInfo ags_generic_main_loop_info = {
      sizeof (AgsGenericMainLoopClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_generic_main_loop_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsGenericMainLoop),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_generic_main_loop_init,
    };

    static const GInterfaceInfo ags_main_loop_interface_info = {
      (GInterfaceInitFunc) ags_generic_main_loop_main_loop_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_generic_main_loop = g_type_register_static(AGS_TYPE_THREAD,
							"AgsGenericMainLoop",
							&ags_generic_main_loop_info,
							0);
    
    g_type_add_interface_static(ags_type_generic_main_loop,
				AGS_TYPE_MAIN_LOOP,
				&ags_main_loop_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_generic_main_loop);
  }

  return g_define_type_id__volatile;
}

void
ags_generic_main_loop_class_init(AgsGenericMainLoopClass *generic_main_loop)
{
  GObjectClass *gobject;

  AgsThreadClass *thread;

  GParamSpec *param_spec;
  
  ags_generic_main_loop_parent_class = g_type_class_peek_parent(generic_main_loop);

  /* GObject */
  gobject = (GObjectClass *) generic_main_loop;

  gobject->set_property = ags_generic_main_loop_set_property;
  gobject->get_property = ags_generic_main_loop_get_property;

  gobject->finalize = ags_generic_main_loop_finalize;

  /* properties */

  /* AgsThread */
  thread = (AgsThreadClass *) generic_main_loop;
  
  thread->start = ags_generic_main_loop_start;

  /* AgsGenericMainLoop */
}

void
ags_generic_main_loop_main_loop_interface_init(AgsMainLoopInterface *main_loop)
{
  main_loop->get_tree_lock = ags_generic_main_loop_get_tree_lock;

  main_loop->set_tic = ags_generic_main_loop_set_tic;
  main_loop->get_tic = ags_generic_main_loop_get_tic;

  main_loop->set_last_sync = ags_generic_main_loop_set_last_sync;
  main_loop->get_last_sync = ags_generic_main_loop_get_last_sync;

  main_loop->change_frequency = ags_generic_main_loop_change_frequency;

  main_loop->sync_counter_inc = ags_generic_main_loop_sync_counter_inc;
  main_loop->sync_counter_dec = ags_generic_main_loop_sync_counter_dec;
  main_loop->sync_counter_test = ags_generic_main_loop_sync_counter_test;

  main_loop->set_sync_tic = ags_generic_main_loop_set_sync_tic;
  main_loop->get_sync_tic = ags_generic_main_loop_get_sync_tic;
}

void
ags_generic_main_loop_init(AgsGenericMainLoop *generic_main_loop)
{
  AgsThread *thread;

  guint i;
  
  /* calculate frequency */
  thread = (AgsThread *) generic_main_loop;
  
  ags_thread_set_flags(thread, AGS_THREAD_TIME_ACCOUNTING);

  thread->freq = AGS_GENERIC_MAIN_LOOP_DEFAULT_JIFFIE;

  ags_main_loop_set_tic(AGS_MAIN_LOOP(generic_main_loop), 0);
  ags_main_loop_set_last_sync(AGS_MAIN_LOOP(generic_main_loop), 0);

  generic_main_loop->time_cycle = NSEC_PER_SEC / thread->freq;
  generic_main_loop->time_spent = 0;

  generic_main_loop->sync_tic = 0;
  
  for(i = 0; i < 6; i++){
    generic_main_loop->sync_counter[i] = 0;
  }
  
  /* tree lock mutex */
  g_rec_mutex_init(&(generic_main_loop->tree_lock));
}

void
ags_generic_main_loop_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsGenericMainLoop *generic_main_loop;

  GRecMutex *thread_mutex;

  generic_main_loop = AGS_GENERIC_MAIN_LOOP(gobject);

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(generic_main_loop);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_generic_main_loop_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsGenericMainLoop *generic_main_loop;

  GRecMutex *thread_mutex;

  generic_main_loop = AGS_GENERIC_MAIN_LOOP(gobject);

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(generic_main_loop);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_generic_main_loop_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_generic_main_loop_parent_class)->finalize(gobject);
}

GRecMutex*
ags_generic_main_loop_get_tree_lock(AgsMainLoop *main_loop)
{
  GRecMutex *tree_lock;

  /* get tree lock mutex */
  tree_lock = &(AGS_GENERIC_MAIN_LOOP(main_loop)->tree_lock);
  
  return(tree_lock);
}

void
ags_generic_main_loop_set_tic(AgsMainLoop *main_loop, guint tic)
{
  AgsGenericMainLoop *generic_main_loop;
  
  GRecMutex *thread_mutex;

  generic_main_loop = AGS_GENERIC_MAIN_LOOP(main_loop);

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(generic_main_loop);

  /* set tic */
  g_rec_mutex_lock(thread_mutex);

  generic_main_loop->tic = tic;

  g_rec_mutex_unlock(thread_mutex);
}

guint
ags_generic_main_loop_get_tic(AgsMainLoop *main_loop)
{
  AgsGenericMainLoop *generic_main_loop;

  guint tic;
  
  GRecMutex *thread_mutex;

  generic_main_loop = AGS_GENERIC_MAIN_LOOP(main_loop);

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(generic_main_loop);

  /* set tic */
  g_rec_mutex_lock(thread_mutex);

  tic = generic_main_loop->tic;

  g_rec_mutex_unlock(thread_mutex);

  return(tic);
}

void
ags_generic_main_loop_set_last_sync(AgsMainLoop *main_loop, guint last_sync)
{
  AgsGenericMainLoop *generic_main_loop;
  
  GRecMutex *thread_mutex;

  generic_main_loop = AGS_GENERIC_MAIN_LOOP(main_loop);

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(generic_main_loop);

  /* set tic */
  g_rec_mutex_lock(thread_mutex);

  generic_main_loop->last_sync = last_sync;

  g_rec_mutex_unlock(thread_mutex);
}

guint
ags_generic_main_loop_get_last_sync(AgsMainLoop *main_loop)
{
  AgsGenericMainLoop *generic_main_loop;

  guint last_sync;
  
  GRecMutex *thread_mutex;

  generic_main_loop = AGS_GENERIC_MAIN_LOOP(main_loop);

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(generic_main_loop);

  /* set tic */
  g_rec_mutex_lock(thread_mutex);

  last_sync = generic_main_loop->last_sync;

  g_rec_mutex_unlock(thread_mutex);

  return(last_sync);
}

void
ags_generic_main_loop_change_frequency(AgsMainLoop *main_loop,
				gdouble frequency)
{
}

void
ags_generic_main_loop_sync_counter_inc(AgsMainLoop *main_loop, guint tic)
{
  AgsGenericMainLoop *generic_main_loop;

  GRecMutex *thread_mutex;

  if(tic >= 6){
    return;
  }
  
  generic_main_loop = AGS_GENERIC_MAIN_LOOP(main_loop);

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(generic_main_loop);

  /* increment */
  g_rec_mutex_lock(thread_mutex);
  
  generic_main_loop->sync_counter[tic] += 1;

  g_rec_mutex_unlock(thread_mutex);
}

void
ags_generic_main_loop_sync_counter_dec(AgsMainLoop *main_loop, guint tic)
{
  AgsGenericMainLoop *generic_main_loop;

  GRecMutex *thread_mutex;

  if(tic >= 6){
    return;
  }
  
  generic_main_loop = AGS_GENERIC_MAIN_LOOP(main_loop);

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(generic_main_loop);

  /* increment */
  g_rec_mutex_lock(thread_mutex);

  if(generic_main_loop->sync_counter[tic] > 0){
    generic_main_loop->sync_counter[tic] -= 1;
  }else{
    g_critical("sync counter already equals 0");
  }
  
  g_rec_mutex_unlock(thread_mutex);
}

gboolean
ags_generic_main_loop_sync_counter_test(AgsMainLoop *main_loop, guint tic)
{
  AgsGenericMainLoop *generic_main_loop;

  gboolean success;
  
  GRecMutex *thread_mutex;

  if(tic >= 6){
    return(FALSE);
  }
  
  generic_main_loop = AGS_GENERIC_MAIN_LOOP(main_loop);

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(generic_main_loop);

  /* test */
  success = FALSE;
  
  g_rec_mutex_lock(thread_mutex);

  if(generic_main_loop->sync_counter[tic] == 0){
    success = TRUE;
  }
  
  g_rec_mutex_unlock(thread_mutex);

  return(success);
}

void
ags_generic_main_loop_set_sync_tic(AgsMainLoop *main_loop, guint sync_tic)
{
  AgsGenericMainLoop *generic_main_loop;
  
  GRecMutex *thread_mutex;
  
  generic_main_loop = AGS_GENERIC_MAIN_LOOP(main_loop);

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(generic_main_loop);

  /* test */
  g_rec_mutex_lock(thread_mutex);

  generic_main_loop->sync_tic = sync_tic;
  
  g_rec_mutex_unlock(thread_mutex);
}

guint
ags_generic_main_loop_get_sync_tic(AgsMainLoop *main_loop)
{
  AgsGenericMainLoop *generic_main_loop;

  guint sync_tic;
  
  GRecMutex *thread_mutex;
  
  generic_main_loop = AGS_GENERIC_MAIN_LOOP(main_loop);

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(generic_main_loop);

  /* test */
  g_rec_mutex_lock(thread_mutex);

  sync_tic = generic_main_loop->sync_tic;
  
  g_rec_mutex_unlock(thread_mutex);

  return(sync_tic);
}

void
ags_generic_main_loop_start(AgsThread *thread)
{
  AGS_THREAD_CLASS(ags_generic_main_loop_parent_class)->start(thread);
}
 
/**
 * ags_generic_main_loop_new:
 *
 * Create a new #AgsGenericMainLoop.
 *
 * Returns: the new #AgsGenericMainLoop
 *
 * Since: 2.0.0
 */
AgsGenericMainLoop*
ags_generic_main_loop_new()
{
  AgsGenericMainLoop *generic_main_loop;

  generic_main_loop = (AgsGenericMainLoop *) g_object_new(AGS_TYPE_GENERIC_MAIN_LOOP,
							  NULL);

  return(generic_main_loop);
}
