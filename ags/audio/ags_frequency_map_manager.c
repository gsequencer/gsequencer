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

#include <ags/audio/ags_frequency_map_manager.h>

#include <ags/libags.h>

void ags_frequency_map_manager_class_init(AgsFrequencyMapManagerClass *frequency_map_manager);
void ags_frequency_map_manager_init (AgsFrequencyMapManager *frequency_map_manager);
void ags_frequency_map_manager_dispose(GObject *gobject);
void ags_frequency_map_manager_finalize(GObject *gobject);

static gpointer ags_frequency_map_manager_parent_class = NULL;

static pthread_mutex_t ags_frequency_map_manager_class_mutex = PTHREAD_MUTEX_INITIALIZER;

guint ags_frequency_map_manager_default_samplerate_count = 4;
guint ags_frequency_map_manager_default_samplerate[] = {
  44100,
  48000,
  96000,
  192000,
};

guint ags_frequency_map_manager_default_buffer_size_count = 6;
guint ags_frequency_map_manager_default_buffer_size[] = {
  128,
  256,
  512,
  1024,
  2048,
  4096,
};

GType
ags_frequency_map_manager_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_frequency_map_manager = 0;

    static const GTypeInfo ags_frequency_map_manager_info = {
      sizeof (AgsFrequencyMapManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_frequency_map_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsFrequencyMapManager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_frequency_map_manager_init,
    };

    ags_type_frequency_map_manager = g_type_register_static(G_TYPE_OBJECT,
							    "AgsFrequencyMapManager",
							    &ags_frequency_map_manager_info,
							    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_frequency_map_manager);
  }

  return g_define_type_id__volatile;
}

void
ags_frequency_map_manager_class_init(AgsFrequencyMapManagerClass *frequency_map_manager)
{
  GObjectClass *gobject;

  ags_frequency_map_manager_parent_class = g_type_class_peek_parent(frequency_map_manager);

  /* GObjectClass */
  gobject = (GObjectClass *) frequency_map_manager;

  gobject->dispose = ags_frequency_map_manager_dispose;
  gobject->finalize = ags_frequency_map_manager_finalize;
}

void
ags_frequency_map_manager_init(AgsFrequencyMapManager *frequency_map_manager)
{
  /* frequency_map manager mutex */
  frequency_map_manager->obj_mutexattr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(frequency_map_manager->obj_mutexattr);
  pthread_mutexattr_settype(frequency_map_manager->obj_mutexattr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(frequency_map_manager->obj_mutexattr,
				PTHREAD_PRIO_INHERIT);
#endif

  frequency_map_manager->obj_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(frequency_map_manager->obj_mutex,
		     frequency_map_manager->obj_mutexattr);

  /* frequency map */
  frequency_map_manager->frequency_map = NULL;
}

void
ags_frequency_map_manager_dispose(GObject *gobject)
{
  AgsFrequencyMapManager *frequency_map_manager;

  frequency_map_manager = AGS_FREQUENCY_MAP_MANAGER(gobject);

  if(frequency_map_manager->frequency_map != NULL){
    g_list_free_full(frequency_map_manager->frequency_map,
		     (GDestroyNotify) g_object_unref);

    frequency_map_manager->frequency_map = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_frequency_map_manager_parent_class)->dispose(gobject);
}

void
ags_frequency_map_manager_finalize(GObject *gobject)
{
  AgsFrequencyMapManager *frequency_map_manager;
  
  GList *frequency_map;

  frequency_map_manager = AGS_FREQUENCY_MAP_MANAGER(gobject);

  pthread_mutex_destroy(frequency_map_manager->obj_mutex);
  free(frequency_map_manager->obj_mutex);

  pthread_mutexattr_destroy(frequency_map_manager->obj_mutexattr);
  free(frequency_map_manager->obj_mutexattr);

  frequency_map = frequency_map_manager->frequency_map;

  g_list_free_full(frequency_map,
		   (GDestroyNotify) g_object_unref);

  if(frequency_map_manager == ags_frequency_map_manager){
    ags_frequency_map_manager = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_frequency_map_manager_parent_class)->finalize(gobject);
}

/**
 * ags_frequency_map_manager_add_frequency_map:
 * @frequency_map_manager: the #AgsFrequencyMapManager
 * @frequency_map: the #AgsFrequencyMap
 * 
 * Add @frequency_map to @frequency_map_manager.
 * 
 * Since: 2.3.0
 */
void
ags_frequency_map_manager_add_frequency_map(AgsFrequencyMapManager *frequency_map_manager,
					    AgsFrequencyMap *frequency_map)
{
  pthread_mutex_t *frequency_map_manager_mutex;
  
  if(!AGS_IS_FREQUENCY_MAP_MANAGER(frequency_map_manager) ||
     !AGS_IS_FREQUENCY_MAP(frequency_map)){
    return;
  }

  frequency_map_manager_mutex = AGS_FREQUENCY_MAP_MANAGER_GET_OBJ_MUTEX(frequency_map_manager);

  pthread_mutex_lock(frequency_map_manager_mutex);
  
  if(g_list_find(frequency_map_manager->frequency_map, frequency_map) == NULL){
    g_object_ref(frequency_map);
    
    frequency_map_manager->frequency_map = g_list_insert_sorted(frequency_map_manager->frequency_map,
								frequency_map,
								(GCompareFunc) ags_frequency_map_sort_func);
  }

  pthread_mutex_unlock(frequency_map_manager_mutex);
}

/**
 * ags_frequency_map_manager_find_frequency_map:
 * @frequency_map_manager: the #AgsFrequencyMapManager
 * @samplerate: the samplerate to match
 * @buffer_size: the buffer size to match
 * @freq: the frequency to match
 * 
 * Find #AgsFrequencyMap by @samplerate, @buffer_size and @freq.
 * 
 * Returns: the matching #AgsFrequencyMap if found, otherwise %NULL
 * 
 * Since: 2.3.0
 */
AgsFrequencyMap*
ags_frequency_map_manager_find_frequency_map(AgsFrequencyMapManager *frequency_map_manager,
					     guint samplerate, guint buffer_size,
					     gdouble freq)
{
  AgsFrequencyMap *frequency_map;

  GList *start_list, *list;
  
  pthread_mutex_t *frequency_map_manager_mutex;

  if(!AGS_IS_FREQUENCY_MAP_MANAGER(frequency_map_manager)){
    return(NULL);
  }

  frequency_map_manager_mutex = AGS_FREQUENCY_MAP_MANAGER_GET_OBJ_MUTEX(frequency_map_manager);
  
  pthread_mutex_lock(frequency_map_manager_mutex);

  list =
    start_list = g_list_copy_deep(frequency_map_manager->frequency_map,
				  (GCopyFunc) g_object_ref,
				  NULL);
  
  pthread_mutex_unlock(frequency_map_manager_mutex);
  
  frequency_map = NULL;

  while(list != NULL){
    guint current_samplerate;
    guint current_buffer_size;
    gdouble current_freq;

    g_object_get(list->data,
		 "samplerate", &current_samplerate,
		 "buffer-size", &current_buffer_size,
		 NULL);

    if(current_samplerate != samplerate){
      list = g_list_nth(list,
			current_buffer_size);

      continue;
    }

    if(current_buffer_size != buffer_size){
      list = g_list_nth(list,
			current_buffer_size);

      continue;
    }
    
    g_object_get(list->data,
		 "freq", &current_freq,
		 NULL);

    if(current_freq == freq){
      frequency_map = list->data;
      
      g_object_ref(frequency_map);

      break;
    }
    
    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
  
  return(frequency_map);
}

/**
 * ags_frequency_map_manager_load_default:
 * @frequency_map_manager: the #AgsFrequencyMapManager
 * 
 * Load default #AgsFrequencyMap.
 * 
 * Since: 2.3.0
 */
void
ags_frequency_map_manager_load_default(AgsFrequencyMapManager *frequency_map_manager)
{
  guint samplerate;
  guint buffer_size;
  guint i_stop, j_stop, k_stop;
  guint i, j, k;

  i_stop = ags_frequency_map_manager_default_samplerate_count;
  j_stop = ags_frequency_map_manager_default_buffer_size_count;
  
  for(i = 0; i < i_stop; i++){
    samplerate = ags_frequency_map_manager_default_samplerate[i];
    
    for(j = 0; j < j_stop; j++){
      buffer_size = ags_frequency_map_manager_default_buffer_size[j];
      
      k_stop = buffer_size;
      
      for(k = 0; k < k_stop; k++){
	AgsFrequencyMap *frequency_map;

	gdouble freq;
	guint frame_count;
	guint attack;

	freq = (gdouble) AGS_FREQUENCY_MAP_MANAGER_EQUINOX * ((gdouble) k / (gdouble) buffer_size);

	frame_count = buffer_size / 2;

	attack = k;
	
	frequency_map = (AgsFrequencyMap *) g_object_new(AGS_TYPE_FREQUENCY_MAP,
							 "z-index", k,
							 "window-count", buffer_size,
							 "samplerate", samplerate,
							 "buffer-size", buffer_size,
							 "freq", freq,
							 "frame-count", frame_count,
							 "attack", attack,
							 NULL);
	ags_frequency_map_process(frequency_map);
	ags_frequency_map_manager_add_frequency_map(frequency_map_manager,
						    frequency_map);
      }
    }
  }
}

/**
 * ags_frequency_map_manager_get_class_mutex:
 * 
 * Get class mutex.
 * 
 * Returns: the class mutex of #AgsFrequencyMapManager
 * 
 * Since: 2.3.0
 */
pthread_mutex_t*
ags_frequency_map_manager_get_class_mutex()
{
  return(&ags_frequency_map_manager_class_mutex);
}

/**
 * ags_frequency_map_manager_get_instance:
 *
 * Get instance.
 *
 * Returns: the #AgsFrequencyMapManager
 *
 * Since: 2.3.0
 */
AgsFrequencyMapManager*
ags_frequency_map_manager_get_instance()
{
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&mutex);

  if(ags_frequency_map_manager == NULL){
    ags_frequency_map_manager = ags_frequency_map_manager_new();
  }

  pthread_mutex_unlock(&mutex);

  return(ags_frequency_map_manager);
}

/**
 * ags_frequency_map_manager_new:
 *
 * Create a new instance of #AgsFrequencyMapManager
 *
 * Returns: the new #AgsFrequencyMapManager
 *
 * Since: 2.3.0
 */
AgsFrequencyMapManager*
ags_frequency_map_manager_new()
{
  AgsFrequencyMapManager *frequency_map_manager;

  frequency_map_manager = (AgsFrequencyMapManager *) g_object_new(AGS_TYPE_FREQUENCY_MAP_MANAGER,
								  NULL);

  return(frequency_map_manager);
}
