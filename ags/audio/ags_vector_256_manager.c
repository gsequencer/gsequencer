/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/audio/ags_vector_256_manager.h>

void ags_vector_256_manager_class_init(AgsVector256ManagerClass *vector_256_manager);
void ags_vector_256_manager_init (AgsVector256Manager *vector_256_manager);
void ags_vector_256_manager_dispose(GObject *gobject);
void ags_vector_256_manager_finalize(GObject *gobject);

/**
 * SECTION:ags_vector_256_manager
 * @short_description: Singleton pattern to organize prefaulted vector memory
 * @title: AgsVector256Manager
 * @section_id:
 * @include: ags/plugin/ags_vector_256_manager.h
 *
 * The #AgsVector256Manager contains prefaulted memory for vector types.
 */

static gpointer ags_vector_256_manager_parent_class = NULL;

AgsVector256Manager *ags_vector_256_manager = NULL;

GType
ags_vector_256_manager_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_vector_256_manager = 0;

    static const GTypeInfo ags_vector_256_manager_info = {
      sizeof (AgsVector256ManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_vector_256_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsVector256Manager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_vector_256_manager_init,
    };

    ags_type_vector_256_manager = g_type_register_static(G_TYPE_OBJECT,
							 "AgsVector256Manager",
							 &ags_vector_256_manager_info,
							 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_vector_256_manager);
  }

  return g_define_type_id__volatile;
}

void
ags_vector_256_manager_class_init(AgsVector256ManagerClass *vector_256_manager)
{
  GObjectClass *gobject;

  ags_vector_256_manager_parent_class = g_type_class_peek_parent(vector_256_manager);

  /* GObjectClass */
  gobject = (GObjectClass *) vector_256_manager;

  gobject->dispose = ags_vector_256_manager_dispose;
  gobject->finalize = ags_vector_256_manager_finalize;
}

void
ags_vector_256_manager_init(AgsVector256Manager *vector_256_manager)
{
  /* vector_256 manager mutex */
  g_rec_mutex_init(&(vector_256_manager->obj_mutex));

  vector_256_manager->vector_count = AGS_VECTOR_256_MANAGER_DEFAULT_VECTOR_COUNT;

  vector_256_manager->v8s8_arr = NULL;
  vector_256_manager->v8s16_arr = NULL;
  vector_256_manager->v8s32_arr = NULL;
  vector_256_manager->v8s64_arr = NULL;
  vector_256_manager->v8float_arr = NULL;
  vector_256_manager->v8double_arr = NULL;
}

void
ags_vector_256_manager_dispose(GObject *gobject)
{
  AgsVector256Manager *vector_256_manager;

  vector_256_manager = AGS_VECTOR_256_MANAGER(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_vector_256_manager_parent_class)->dispose(gobject);
}

void
ags_vector_256_manager_finalize(GObject *gobject)
{
  AgsVector256Manager *vector_256_manager;
  
  vector_256_manager = AGS_VECTOR_256_MANAGER(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_vector_256_manager_parent_class)->finalize(gobject);
}

/**
 * ags_vector_arr_alloc:
 * @vector_type: the #AgsVector256Types-enum
 *
 * Allocate vector array of @vector_type.
 *
 * Returns: (transfer full): the newly allocated vector
 *
 * Since: 7.0.0
 */
AgsVectorArr*
ags_vector_arr_alloc(AgsVector256Types vector_type)
{
  AgsVectorArr *vector_arr;

  vector_arr = (AgsVectorArr *) g_malloc(sizeof(AgsVectorArr));

  vector_arr->vector_type = vector_type;
  
  g_atomic_int_set(&(vector_arr->locked),
		   FALSE);

  switch(vector_type){
  case AGS_VECTOR_256_SIGNED_8_BIT:
    {
      vector_arr->vector.vec_s8 = (AgsVectorMemS8 *) malloc(sizeof(AgsVectorMemS8));
    }
    break;
  case AGS_VECTOR_256_SIGNED_16_BIT:
    {
      vector_arr->vector.vec_s16 = (AgsVectorMemS16 *) malloc(sizeof(AgsVectorMemS16));
    }
    break;
  case AGS_VECTOR_256_SIGNED_32_BIT:
    {
      vector_arr->vector.vec_s32 = (AgsVectorMemS32 *) malloc(sizeof(AgsVectorMemS32));
    }
    break;
  case AGS_VECTOR_256_SIGNED_64_BIT:
    {
      vector_arr->vector.vec_s64 = (AgsVectorMemS64 *) malloc(sizeof(AgsVectorMemS64));
    }
    break;
  case AGS_VECTOR_256_FLOAT:
    {
      vector_arr->vector.vec_float = (AgsVectorMemFloat *) malloc(sizeof(AgsVectorMemFloat));
    }
    break;
  case AGS_VECTOR_256_DOUBLE:
    {
      vector_arr->vector.vec_double = (AgsVectorMemDouble *) malloc(sizeof(AgsVectorMemDouble));
    }
    break;
  default:
    {
      memset(&(vector_arr->vector.vec_double), 0, sizeof(AgsVectorMemDouble));
    }
  };
  
  return(vector_arr);
}

/**
 * ags_vector_arr_free:
 * @vector_arr: (transfer full): the #AgsVectorArr-struct
 *
 * Free @vector_arr.
 *
 * Since: 7.0.0
 */
void
ags_vector_arr_free(AgsVectorArr *vector_arr)
{
  g_return_if_fail(vector_arr);
  
  switch(vector_arr->vector_type){
  case AGS_VECTOR_256_SIGNED_8_BIT:
    {
      free(vector_arr->vector.vec_s8);
    }
    break;
  case AGS_VECTOR_256_SIGNED_16_BIT:
    {
      free(vector_arr->vector.vec_s16);
    }
    break;
  case AGS_VECTOR_256_SIGNED_32_BIT:
    {
      free(vector_arr->vector.vec_s32);
    }
    break;
  case AGS_VECTOR_256_SIGNED_64_BIT:
    {
      free(vector_arr->vector.vec_s64);
    }
    break;
  case AGS_VECTOR_256_FLOAT:
    {
      free(vector_arr->vector.vec_float);
    }
    break;
  case AGS_VECTOR_256_DOUBLE:
    {
      free(vector_arr->vector.vec_double);
    }
    break;
  };

  g_free(vector_arr);
}

/**
 * ags_vector_256_manager_reserve_all:
 * @vector_256_manager: the #AgsVector256Manager
 *
 * Reserve all vector types.
 *
 * Since: 7.0.0
 */
void
ags_vector_256_manager_reserve_all(AgsVector256Manager *vector_256_manager)
{
  guint i;

  g_return_if_fail(vector_256_manager != NULL);

  for(i = 0; i < vector_256_manager->vector_count; i++){
    AgsVectorArr *vector_arr;

    vector_arr = ags_vector_arr_alloc(AGS_VECTOR_256_SIGNED_8_BIT);

    vector_256_manager->v8s8_arr = g_list_prepend(vector_256_manager->v8s8_arr,
						  vector_arr);
  }
  
  for(i = 0; i < vector_256_manager->vector_count; i++){
    AgsVectorArr *vector_arr;

    vector_arr = ags_vector_arr_alloc(AGS_VECTOR_256_SIGNED_16_BIT);

    vector_256_manager->v8s16_arr = g_list_prepend(vector_256_manager->v8s16_arr,
						   vector_arr);
  }
  
  for(i = 0; i < vector_256_manager->vector_count; i++){
    AgsVectorArr *vector_arr;

    vector_arr = ags_vector_arr_alloc(AGS_VECTOR_256_SIGNED_32_BIT);

    vector_256_manager->v8s32_arr = g_list_prepend(vector_256_manager->v8s32_arr,
						   vector_arr);
  }
  
  for(i = 0; i < vector_256_manager->vector_count; i++){
    AgsVectorArr *vector_arr;

    vector_arr = ags_vector_arr_alloc(AGS_VECTOR_256_SIGNED_64_BIT);

    vector_256_manager->v8s64_arr = g_list_prepend(vector_256_manager->v8s64_arr,
						   vector_arr);
  }
  
  for(i = 0; i < vector_256_manager->vector_count; i++){
    AgsVectorArr *vector_arr;

    vector_arr = ags_vector_arr_alloc(AGS_VECTOR_256_FLOAT);

    vector_256_manager->v8float_arr = g_list_prepend(vector_256_manager->v8float_arr,
						     vector_arr);
  }
  
  for(i = 0; i < vector_256_manager->vector_count; i++){
    AgsVectorArr *vector_arr;

    vector_arr = ags_vector_arr_alloc(AGS_VECTOR_256_DOUBLE);

    vector_256_manager->v8double_arr = g_list_prepend(vector_256_manager->v8double_arr,
						      vector_arr);
  }
}

/**
 * ags_vector_256_manager_try_acquire:
 * @vector_256_manager: the #AgsVector256Manager
 * @vector_type: the vector type
 *
 * Try acquire vector of @vector_type and lock it.
 *
 * Returns: (transfer none): the vector array or %NULL if none is available yet
 * 
 * Since: 7.0.0
 */
AgsVectorArr*
ags_vector_256_manager_try_acquire(AgsVector256Manager *vector_256_manager,
				   AgsVector256Types vector_type)
{
  AgsVectorArr *vector_arr;

  GList *vector;
  
  GRecMutex *mutex;

  g_return_val_if_fail(vector_256_manager != NULL, NULL);

  mutex = AGS_VECTOR_256_MANAGER_GET_OBJ_MUTEX(vector_256_manager);
  
  vector_arr = NULL;

  g_rec_mutex_lock(mutex);
  
  switch(vector_type){
  case AGS_VECTOR_256_SIGNED_8_BIT:
    {
      vector = vector_256_manager->v8s8_arr;
      
      while(vector != NULL){
	if(g_atomic_int_get(&(AGS_VECTOR_ARR(vector->data)->locked)) == FALSE){
	  vector_arr = (AgsVectorArr *) vector->data;
	  
	  break;
	}
	
	vector = vector->next;
      }
    }
    break;
  case AGS_VECTOR_256_SIGNED_16_BIT:
    {
      vector = vector_256_manager->v8s16_arr;
      
      while(vector != NULL){
	if(g_atomic_int_get(&(AGS_VECTOR_ARR(vector->data)->locked)) == FALSE){
	  vector_arr = (AgsVectorArr *) vector->data;
	  
	  break;
	}
	
	vector = vector->next;
      }
    }
    break;
  case AGS_VECTOR_256_SIGNED_32_BIT:
    {
      vector = vector_256_manager->v8s32_arr;
      
      while(vector != NULL){
	if(g_atomic_int_get(&(AGS_VECTOR_ARR(vector->data)->locked)) == FALSE){
	  vector_arr = (AgsVectorArr *) vector->data;
	  
	  break;
	}
	
	vector = vector->next;
      }
    }
    break;
  case AGS_VECTOR_256_SIGNED_64_BIT:
    {
      vector = vector_256_manager->v8s64_arr;
      
      while(vector != NULL){
	if(g_atomic_int_get(&(AGS_VECTOR_ARR(vector->data)->locked)) == FALSE){
	  vector_arr = (AgsVectorArr *) vector->data;
	  
	  break;
	}
	
	vector = vector->next;
      }
    }
    break;
  case AGS_VECTOR_256_FLOAT:
    {
      vector = vector_256_manager->v8float_arr;
      
      while(vector != NULL){
	if(g_atomic_int_get(&(AGS_VECTOR_ARR(vector->data)->locked)) == FALSE){
	  vector_arr = (AgsVectorArr *) vector->data;
	  
	  break;
	}
	
	vector = vector->next;
      }
    }
    break;
  case AGS_VECTOR_256_DOUBLE:
    {
      vector = vector_256_manager->v8double_arr;
      
      while(vector != NULL){
	if(g_atomic_int_get(&(AGS_VECTOR_ARR(vector->data)->locked)) == FALSE){
	  vector_arr = (AgsVectorArr *) vector->data;
	  
	  break;
	}
	
	vector = vector->next;
      }
    }
    break;
  };

  if(vector_arr != NULL){
    g_atomic_int_set(&(vector_arr->locked),
		     TRUE);
  }

  g_rec_mutex_unlock(mutex);
  
  return(vector_arr);
}

/**
 * ags_vector_256_manager_try_acquire_dual:
 * @vector_256_manager: the #AgsVector256Manager
 * @vector_type_a: the first vector type
 * @vector_type_b: the second vector type
 * @vector_arr_a: (transfer none) (out): the vector array or %NULL if none is available yet
 * @vector_arr_b: (transfer none) (out): the vector array or %NULL if none is available yet
 *
 * Try acquire 2 vectors of vector type and lock it.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 7.0.0
 */
gboolean
ags_vector_256_manager_try_acquire_dual(AgsVector256Manager *vector_256_manager,
					AgsVector256Types vector_type_a, AgsVector256Types vector_type_b,
					AgsVectorArr **vector_arr_a, AgsVectorArr **vector_arr_b)
{
  AgsVectorArr *arr_a, *arr_b;

  guint i;
  gboolean success;
  
  GRecMutex *mutex;

  g_return_val_if_fail(vector_256_manager != NULL, FALSE);
  g_return_val_if_fail(vector_arr_a != NULL, FALSE);
  g_return_val_if_fail(vector_arr_b != NULL, FALSE);

  mutex = AGS_VECTOR_256_MANAGER_GET_OBJ_MUTEX(vector_256_manager);

  arr_a = NULL;
  arr_b = NULL;
  
  success = FALSE;
  
  g_rec_mutex_lock(mutex);

  for(i = 0; i < 2; i++){
    AgsVectorArr *vector_arr;
    
    GList *vector;

    AgsVector256Types vector_type;

    if(i == 0){
      vector_type = vector_type_a;
    }else{
      vector_type = vector_type_b;
    }
    
    vector_arr = NULL;
  
    switch(vector_type){
    case AGS_VECTOR_256_SIGNED_8_BIT:
      {
	vector = vector_256_manager->v8s8_arr;
      
	while(vector != NULL){
	  if(g_atomic_int_get(&(AGS_VECTOR_ARR(vector->data)->locked)) == FALSE){
	    vector_arr = (AgsVectorArr *) vector->data;
	  
	    break;
	  }
	
	  vector = vector->next;
	}
      }
      break;
    case AGS_VECTOR_256_SIGNED_16_BIT:
      {
	vector = vector_256_manager->v8s16_arr;
      
	while(vector != NULL){
	  if(g_atomic_int_get(&(AGS_VECTOR_ARR(vector->data)->locked)) == FALSE){
	    vector_arr = (AgsVectorArr *) vector->data;
	  
	    break;
	  }
	
	  vector = vector->next;
	}
      }
      break;
    case AGS_VECTOR_256_SIGNED_32_BIT:
      {
	vector = vector_256_manager->v8s32_arr;
      
	while(vector != NULL){
	  if(g_atomic_int_get(&(AGS_VECTOR_ARR(vector->data)->locked)) == FALSE){
	    vector_arr = (AgsVectorArr *) vector->data;
	  
	    break;
	  }
	
	  vector = vector->next;
	}
      }
      break;
    case AGS_VECTOR_256_SIGNED_64_BIT:
      {
	vector = vector_256_manager->v8s64_arr;
      
	while(vector != NULL){
	  if(g_atomic_int_get(&(AGS_VECTOR_ARR(vector->data)->locked)) == FALSE){
	    vector_arr = (AgsVectorArr *) vector->data;
	  
	    break;
	  }
	
	  vector = vector->next;
	}
      }
      break;
    case AGS_VECTOR_256_FLOAT:
      {
	vector = vector_256_manager->v8float_arr;
      
	while(vector != NULL){
	  if(g_atomic_int_get(&(AGS_VECTOR_ARR(vector->data)->locked)) == FALSE){
	    vector_arr = (AgsVectorArr *) vector->data;
	  
	    break;
	  }
	
	  vector = vector->next;
	}
      }
      break;
    case AGS_VECTOR_256_DOUBLE:
      {
	vector = vector_256_manager->v8double_arr;
      
	while(vector != NULL){
	  if(g_atomic_int_get(&(AGS_VECTOR_ARR(vector->data)->locked)) == FALSE){
	    vector_arr = (AgsVectorArr *) vector->data;
	  
	    break;
	  }
	
	  vector = vector->next;
	}
      }
      break;
    };

    if(i == 0){
      arr_a = vector_arr;
    }else{
      arr_b = vector_arr;
    }
  }
  
  if(arr_a != NULL &&
     arr_b != NULL){
    success = TRUE;
  }
  
  if(success){
    vector_arr_a[0] = arr_a;
    vector_arr_b[0] = arr_b;
    
    g_atomic_int_set(&(arr_a->locked),
		     TRUE);

    g_atomic_int_set(&(arr_b->locked),
		     TRUE);
  }

  g_rec_mutex_unlock(mutex);
  
  return(success);
}

/**
 * ags_vector_256_manager_try_acquire_triple:
 * @vector_256_manager: the #AgsVector256Manager
 * @vector_type_a: the first vector type
 * @vector_type_b: the second vector type
 * @vector_type_c: the third vector type
 * @vector_arr_a: (transfer none) (out): the vector array or %NULL if none is available yet
 * @vector_arr_b: (transfer none) (out): the vector array or %NULL if none is available yet
 * @vector_arr_c: (transfer none) (out): the vector array or %NULL if none is available yet
 *
 * Try acquire 3 vectors of vector type and lock it.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 7.0.0
 */
gboolean
ags_vector_256_manager_try_acquire_triple(AgsVector256Manager *vector_256_manager,
					  AgsVector256Types vector_type_a, AgsVector256Types vector_type_b, AgsVector256Types vector_type_c,
					  AgsVectorArr **vector_arr_a, AgsVectorArr **vector_arr_b, AgsVectorArr **vector_arr_c)
{
  AgsVectorArr *arr_a, *arr_b, *arr_c;

  guint i;
  gboolean success;
  
  GRecMutex *mutex;

  g_return_val_if_fail(vector_256_manager != NULL, FALSE);
  g_return_val_if_fail(vector_arr_a != NULL, FALSE);
  g_return_val_if_fail(vector_arr_b != NULL, FALSE);
  g_return_val_if_fail(vector_arr_c != NULL, FALSE);

  mutex = AGS_VECTOR_256_MANAGER_GET_OBJ_MUTEX(vector_256_manager);

  arr_a = NULL;
  arr_b = NULL;
  arr_c = NULL;
  
  success = FALSE;
  
  g_rec_mutex_lock(mutex);

  for(i = 0; i < 3; i++){
    AgsVectorArr *vector_arr;
    
    GList *vector;

    AgsVector256Types vector_type;

    if(i == 0){
      vector_type = vector_type_a;
    }else if(i == 1){
      vector_type = vector_type_b;
    }else{
      vector_type = vector_type_c;
    }
    
    vector_arr = NULL;
  
    switch(vector_type){
    case AGS_VECTOR_256_SIGNED_8_BIT:
      {
	vector = vector_256_manager->v8s8_arr;
      
	while(vector != NULL){
	  if(g_atomic_int_get(&(AGS_VECTOR_ARR(vector->data)->locked)) == FALSE){
	    vector_arr = (AgsVectorArr *) vector->data;
	  
	    break;
	  }
	
	  vector = vector->next;
	}
      }
      break;
    case AGS_VECTOR_256_SIGNED_16_BIT:
      {
	vector = vector_256_manager->v8s16_arr;
      
	while(vector != NULL){
	  if(g_atomic_int_get(&(AGS_VECTOR_ARR(vector->data)->locked)) == FALSE){
	    vector_arr = (AgsVectorArr *) vector->data;
	  
	    break;
	  }
	
	  vector = vector->next;
	}
      }
      break;
    case AGS_VECTOR_256_SIGNED_32_BIT:
      {
	vector = vector_256_manager->v8s32_arr;
      
	while(vector != NULL){
	  if(g_atomic_int_get(&(AGS_VECTOR_ARR(vector->data)->locked)) == FALSE){
	    vector_arr = (AgsVectorArr *) vector->data;
	  
	    break;
	  }
	
	  vector = vector->next;
	}
      }
      break;
    case AGS_VECTOR_256_SIGNED_64_BIT:
      {
	vector = vector_256_manager->v8s64_arr;
      
	while(vector != NULL){
	  if(g_atomic_int_get(&(AGS_VECTOR_ARR(vector->data)->locked)) == FALSE){
	    vector_arr = (AgsVectorArr *) vector->data;
	  
	    break;
	  }
	
	  vector = vector->next;
	}
      }
      break;
    case AGS_VECTOR_256_FLOAT:
      {
	vector = vector_256_manager->v8float_arr;
      
	while(vector != NULL){
	  if(g_atomic_int_get(&(AGS_VECTOR_ARR(vector->data)->locked)) == FALSE){
	    vector_arr = (AgsVectorArr *) vector->data;
	  
	    break;
	  }
	
	  vector = vector->next;
	}
      }
      break;
    case AGS_VECTOR_256_DOUBLE:
      {
	vector = vector_256_manager->v8double_arr;
      
	while(vector != NULL){
	  if(g_atomic_int_get(&(AGS_VECTOR_ARR(vector->data)->locked)) == FALSE){
	    vector_arr = (AgsVectorArr *) vector->data;
	  
	    break;
	  }
	
	  vector = vector->next;
	}
      }
      break;
    };

    if(i == 0){
      arr_a = vector_arr;
    }else if(i == 1){
      arr_b = vector_arr;
    }else{
      arr_c = vector_arr;
    }
  }
  
  if(arr_a != NULL &&
     arr_b != NULL &&
     arr_c != NULL){
    success = TRUE;
  }
  
  if(success){
    vector_arr_a[0] = arr_a;
    vector_arr_b[0] = arr_b;
    vector_arr_c[0] = arr_c;
    
    g_atomic_int_set(&(arr_a->locked),
		     TRUE);

    g_atomic_int_set(&(arr_b->locked),
		     TRUE);

    g_atomic_int_set(&(arr_c->locked),
		     TRUE);
  }

  g_rec_mutex_unlock(mutex);
  
  return(success);
}

/**
 * ags_vector_256_manager_release:
 * @vector_256_manager: the #AgsVector256Manager
 * @vector_arr: the #AgsVectorArr-struct
 *
 * Release @vector_arr.
 * 
 * Since: 7.0.0
 */
void
ags_vector_256_manager_release(AgsVector256Manager *vector_256_manager,
			       AgsVectorArr *vector_arr)
{
  g_return_if_fail(vector_arr != NULL);
  
  g_atomic_int_set(&(vector_arr->locked),
		   FALSE);
}

/**
 * ags_vector_256_manager_get_instance:
 *
 * Get instance.
 *
 * Returns: (transfer none): the #AgsVector256Manager
 *
 * Since: 7.0.0
 */
AgsVector256Manager*
ags_vector_256_manager_get_instance()
{
  static GMutex mutex;

  g_mutex_lock(&mutex);

  if(ags_vector_256_manager == NULL){
    ags_vector_256_manager = ags_vector_256_manager_new();

    ags_vector_256_manager_reserve_all(ags_vector_256_manager);
  }

  g_mutex_unlock(&mutex);

  return(ags_vector_256_manager);
}

/**
 * ags_vector_256_manager_new:
 *
 * Create a new instance of #AgsVector256Manager
 *
 * Returns: the new #AgsVector256Manager
 *
 * Since: 7.0.0
 */
AgsVector256Manager*
ags_vector_256_manager_new()
{
  AgsVector256Manager *vector_256_manager;

  vector_256_manager = (AgsVector256Manager *) g_object_new(AGS_TYPE_VECTOR_256_MANAGER,
							    NULL);

  return(vector_256_manager);
}
