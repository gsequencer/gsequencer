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
void ags_generic_main_loop_set_syncing(AgsMainLoop *main_loop, gboolean is_syncing);
gboolean ags_generic_main_loop_is_syncing(AgsMainLoop *main_loop);
void ags_generic_main_loop_set_critical_region(AgsMainLoop *main_loop, gboolean is_critical_region);
gboolean ags_generic_main_loop_is_critical_region(AgsMainLoop *main_loop);
void ags_generic_main_loop_inc_queued_critical_region(AgsMainLoop *main_loop);
void ags_generic_main_loop_dec_queued_critical_region(AgsMainLoop *main_loop);
guint ags_generic_main_loop_test_queued_critical_region(AgsMainLoop *main_loop);
void ags_generic_main_loop_change_frequency(AgsMainLoop *main_loop,
					    gdouble frequency);

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

  main_loop->set_syncing = ags_generic_main_loop_set_syncing;
  main_loop->is_syncing = ags_generic_main_loop_is_syncing;

  main_loop->set_critical_region = ags_generic_main_loop_set_critical_region;
  main_loop->is_critical_region = ags_generic_main_loop_is_critical_region;

  main_loop->inc_queued_critical_region = ags_generic_main_loop_inc_queued_critical_region;
  main_loop->dec_queued_critical_region = ags_generic_main_loop_dec_queued_critical_region;
  main_loop->test_queued_critical_region = ags_generic_main_loop_test_queued_critical_region;

  main_loop->change_frequency = ags_generic_main_loop_change_frequency;
}

void
ags_generic_main_loop_init(AgsGenericMainLoop *generic_main_loop)
{
  AgsThread *thread;

  guint i;
  
  /* calculate frequency */
  thread = (AgsThread *) generic_main_loop;
  
  ags_thread_set_flags(thread, AGS_THREAD_TIME_ACCOUNTING);

  g_object_set(thread,
	       "frequency", AGS_GENERIC_MAIN_LOOP_DEFAULT_JIFFIE,
	       NULL);
  
  /* tree lock mutex */
  g_rec_mutex_init(&(generic_main_loop->tree_lock));

  ags_main_loop_set_syncing(AGS_MAIN_LOOP(generic_main_loop), FALSE);

  ags_main_loop_set_critical_region(AGS_MAIN_LOOP(generic_main_loop), FALSE);
  g_atomic_int_set(&(generic_main_loop->critical_region_ref), 0);
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
ags_generic_main_loop_set_syncing(AgsMainLoop *main_loop, gboolean is_syncing)
{
  AgsGenericMainLoop *generic_main_loop;

  generic_main_loop = AGS_GENERIC_MAIN_LOOP(main_loop);

  /* set syncing */
  g_atomic_int_set(&(generic_main_loop->is_syncing), is_syncing);
}

gboolean
ags_generic_main_loop_is_syncing(AgsMainLoop *main_loop)
{
  AgsGenericMainLoop *generic_main_loop;

  gboolean is_syncing;

  generic_main_loop = AGS_GENERIC_MAIN_LOOP(main_loop);

  /* is syncing */
  is_syncing = g_atomic_int_get(&(generic_main_loop->is_syncing));

  return(is_syncing);
}

void
ags_generic_main_loop_set_critical_region(AgsMainLoop *main_loop, gboolean is_critical_region)
{
  AgsGenericMainLoop *generic_main_loop;

  generic_main_loop = AGS_GENERIC_MAIN_LOOP(main_loop);

  /* set critical region */
  g_atomic_int_set(&(generic_main_loop->is_critical_region), is_critical_region);
}

gboolean
ags_generic_main_loop_is_critical_region(AgsMainLoop *main_loop)
{
  AgsGenericMainLoop *generic_main_loop;

  gboolean is_critical_region;

  generic_main_loop = AGS_GENERIC_MAIN_LOOP(main_loop);

  /* is critical region */
  is_critical_region = g_atomic_int_get(&(generic_main_loop->is_critical_region));

  return(is_critical_region);
}

void
ags_generic_main_loop_inc_queued_critical_region(AgsMainLoop *main_loop)
{
  AgsGenericMainLoop *generic_main_loop;

  generic_main_loop = AGS_GENERIC_MAIN_LOOP(main_loop);

  /* increment critical region */
  g_atomic_int_inc(&(generic_main_loop->critical_region_ref));
}

void
ags_generic_main_loop_dec_queued_critical_region(AgsMainLoop *main_loop)
{
  AgsGenericMainLoop *generic_main_loop;

  generic_main_loop = AGS_GENERIC_MAIN_LOOP(main_loop);

  /* decrement critical region */
  g_atomic_int_dec_and_test(&(generic_main_loop->critical_region_ref));
}

guint
ags_generic_main_loop_test_queued_critical_region(AgsMainLoop *main_loop)
{
  AgsGenericMainLoop *generic_main_loop;

  guint critical_region_ref;
  
  generic_main_loop = AGS_GENERIC_MAIN_LOOP(main_loop);

  /* set critical region */
  critical_region_ref = g_atomic_int_get(&(generic_main_loop->is_critical_region));

  return(critical_region_ref);
}

void
ags_generic_main_loop_change_frequency(AgsMainLoop *main_loop,
				       gdouble frequency)
{
  //empty
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
 * Since: 3.0.0
 */
AgsGenericMainLoop*
ags_generic_main_loop_new()
{
  AgsGenericMainLoop *generic_main_loop;

  generic_main_loop = (AgsGenericMainLoop *) g_object_new(AGS_TYPE_GENERIC_MAIN_LOOP,
							  NULL);

  return(generic_main_loop);
}
