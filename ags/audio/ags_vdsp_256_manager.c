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

#include <ags/audio/ags_vdsp_256_manager.h>

void ags_vdsp_256_manager_class_init(AgsVDSP256ManagerClass *vdsp_256_manager);
void ags_vdsp_256_manager_init (AgsVDSP256Manager *vdsp_256_manager);
void ags_vdsp_256_manager_dispose(GObject *gobject);
void ags_vdsp_256_manager_finalize(GObject *gobject);

/**
 * SECTION:ags_vdsp_256_manager
 * @short_description: Singleton pattern to organize memory
 * @title: AgsVDSP256Manager
 * @section_id:
 * @include: ags/plugin/ags_vdsp_256_manager.h
 *
 * The #AgsVDSP256Manager contains memory for vdsp types.
 */

static gpointer ags_vdsp_256_manager_parent_class = NULL;

AgsVDSP256Manager *ags_vdsp_256_manager = NULL;

GType
ags_vdsp_256_manager_get_type (void)
{
  static gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_vdsp_256_manager = 0;

    static const GTypeInfo ags_vdsp_256_manager_info = {
      sizeof (AgsVDSP256ManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_vdsp_256_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsVDSP256Manager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_vdsp_256_manager_init,
    };

    ags_type_vdsp_256_manager = g_type_register_static(G_TYPE_OBJECT,
						       "AgsVDSP256Manager",
						       &ags_vdsp_256_manager_info,
						       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_vdsp_256_manager);
  }

  return g_define_type_id__volatile;
}

void
ags_vdsp_256_manager_class_init(AgsVDSP256ManagerClass *vdsp_256_manager)
{
  GObjectClass *gobject;

  ags_vdsp_256_manager_parent_class = g_type_class_peek_parent(vdsp_256_manager);

  /* GObjectClass */
  gobject = (GObjectClass *) vdsp_256_manager;

  gobject->dispose = ags_vdsp_256_manager_dispose;
  gobject->finalize = ags_vdsp_256_manager_finalize;
}

void
ags_vdsp_256_manager_init(AgsVDSP256Manager *vdsp_256_manager)
{
  /* vdsp_256 manager mutex */
  g_rec_mutex_init(&(vdsp_256_manager->obj_mutex));

  vdsp_256_manager->vdsp_count = AGS_VDSP_256_MANAGER_DEFAULT_VDSP_COUNT;

  vdsp_256_manager->int_arr = NULL;
  vdsp_256_manager->float_arr = NULL;
  vdsp_256_manager->double_arr = NULL;
}

void
ags_vdsp_256_manager_dispose(GObject *gobject)
{
  AgsVDSP256Manager *vdsp_256_manager;

  vdsp_256_manager = AGS_VDSP_256_MANAGER(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_vdsp_256_manager_parent_class)->dispose(gobject);
}

void
ags_vdsp_256_manager_finalize(GObject *gobject)
{
  AgsVDSP256Manager *vdsp_256_manager;
  
  vdsp_256_manager = AGS_VDSP_256_MANAGER(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_vdsp_256_manager_parent_class)->finalize(gobject);
}

/**
 * ags_vdsp_arr_alloc:
 * @vdsp_type: the #AgsVDSP256Types-enum
 *
 * Allocate vdsp array of @vdsp_type.
 *
 * Returns: (transfer full): the newly allocated vdsp
 *
 * Since: 7.0.0
 */
AgsVDSPArr*
ags_vdsp_arr_alloc(AgsVDSP256Types vdsp_type)
{
  AgsVDSPArr *vdsp_arr;

  vdsp_arr = (AgsVDSPArr *) g_malloc(sizeof(AgsVDSPArr));

  vdsp_arr->vdsp_type = vdsp_type;
  
  ags_atomic_int_set(&(vdsp_arr->locked),
		   FALSE);

  switch(vdsp_type){
  case AGS_VDSP_256_INT:
    {
      vdsp_arr->vdsp.vec_int = (AgsVDSPMemInt *) malloc(sizeof(AgsVDSPMemInt));
    }
    break;
  case AGS_VDSP_256_FLOAT:
    {
      vdsp_arr->vdsp.vec_float = (AgsVDSPMemFloat *) malloc(sizeof(AgsVDSPMemFloat));
    }
    break;
  case AGS_VDSP_256_DOUBLE:
    {
      vdsp_arr->vdsp.vec_double = (AgsVDSPMemDouble *) malloc(sizeof(AgsVDSPMemDouble));
    }
    break;
  default:
    {
      memset(&(vdsp_arr->vdsp.vec_double), 0, sizeof(AgsVDSPMemDouble));
    }
  };
  
  return(vdsp_arr);
}

/**
 * ags_vdsp_arr_free:
 * @vdsp_arr: (transfer full): the #AgsVDSPArr-struct
 *
 * Free @vdsp_arr.
 *
 * Since: 7.0.0
 */
void
ags_vdsp_arr_free(AgsVDSPArr *vdsp_arr)
{
  g_return_if_fail(vdsp_arr);
  
  switch(vdsp_arr->vdsp_type){
  case AGS_VDSP_256_INT:
    {
      free(vdsp_arr->vdsp.vec_int);
    }
    break;
  case AGS_VDSP_256_FLOAT:
    {
      free(vdsp_arr->vdsp.vec_float);
    }
    break;
  case AGS_VDSP_256_DOUBLE:
    {
      free(vdsp_arr->vdsp.vec_double);
    }
    break;
  };

  g_free(vdsp_arr);
}

/**
 * ags_vdsp_256_manager_reserve_all:
 * @vdsp_256_manager: the #AgsVDSP256Manager
 *
 * Reserve all vdsp types.
 *
 * Since: 7.0.0
 */
void
ags_vdsp_256_manager_reserve_all(AgsVDSP256Manager *vdsp_256_manager)
{
  guint i;

  g_return_if_fail(vdsp_256_manager != NULL);
  
  for(i = 0; i < vdsp_256_manager->vdsp_count; i++){
    AgsVDSPArr *vdsp_arr;

    vdsp_arr = ags_vdsp_arr_alloc(AGS_VDSP_256_INT);

    vdsp_256_manager->int_arr = g_list_prepend(vdsp_256_manager->int_arr,
					       vdsp_arr);
  }
  
  for(i = 0; i < vdsp_256_manager->vdsp_count; i++){
    AgsVDSPArr *vdsp_arr;

    vdsp_arr = ags_vdsp_arr_alloc(AGS_VDSP_256_FLOAT);

    vdsp_256_manager->float_arr = g_list_prepend(vdsp_256_manager->float_arr,
						 vdsp_arr);
  }
  
  for(i = 0; i < vdsp_256_manager->vdsp_count; i++){
    AgsVDSPArr *vdsp_arr;

    vdsp_arr = ags_vdsp_arr_alloc(AGS_VDSP_256_DOUBLE);

    vdsp_256_manager->double_arr = g_list_prepend(vdsp_256_manager->double_arr,
						  vdsp_arr);
  }
}

/**
 * ags_vdsp_256_manager_try_acquire:
 * @vdsp_256_manager: the #AgsVDSP256Manager
 * @vdsp_type: the vdsp type
 *
 * Try acquire vdsp of @vdsp_type and lock it.
 *
 * Returns: (transfer none): the vdsp array or %NULL if none is available yet
 * 
 * Since: 7.0.0
 */
AgsVDSPArr*
ags_vdsp_256_manager_try_acquire(AgsVDSP256Manager *vdsp_256_manager,
				 AgsVDSP256Types vdsp_type)
{
  AgsVDSPArr *vdsp_arr;

  GList *vdsp;
  
  GRecMutex *mutex;

  g_return_val_if_fail(vdsp_256_manager != NULL, NULL);

  mutex = AGS_VDSP_256_MANAGER_GET_OBJ_MUTEX(vdsp_256_manager);
  
  vdsp_arr = NULL;

  g_rec_mutex_lock(mutex);
  
  switch(vdsp_type){
  case AGS_VDSP_256_INT:
    {
      vdsp = vdsp_256_manager->int_arr;
      
      while(vdsp != NULL){
	if(ags_atomic_int_get(&(AGS_VDSP_ARR(vdsp->data)->locked)) == FALSE){
	  vdsp_arr = (AgsVDSPArr *) vdsp->data;
	  
	  break;
	}
	
	vdsp = vdsp->next;
      }
    }
    break;
  case AGS_VDSP_256_FLOAT:
    {
      vdsp = vdsp_256_manager->float_arr;
      
      while(vdsp != NULL){
	if(ags_atomic_int_get(&(AGS_VDSP_ARR(vdsp->data)->locked)) == FALSE){
	  vdsp_arr = (AgsVDSPArr *) vdsp->data;
	  
	  break;
	}
	
	vdsp = vdsp->next;
      }
    }
    break;
  case AGS_VDSP_256_DOUBLE:
    {
      vdsp = vdsp_256_manager->double_arr;
      
      while(vdsp != NULL){
	if(ags_atomic_int_get(&(AGS_VDSP_ARR(vdsp->data)->locked)) == FALSE){
	  vdsp_arr = (AgsVDSPArr *) vdsp->data;
	  
	  break;
	}
	
	vdsp = vdsp->next;
      }
    }
    break;
  };

  if(vdsp_arr != NULL){
    ags_atomic_int_set(&(vdsp_arr->locked),
		     TRUE);
  }

  g_rec_mutex_unlock(mutex);
  
  return(vdsp_arr);
}

gboolean
ags_vdsp_256_manager_try_acquire_dual(AgsVDSP256Manager *vdsp_256_manager,
				      AgsVDSP256Types vdsp_type_a, AgsVDSP256Types vdsp_type_b,
				      AgsVDSPArr **vdsp_arr_a, AgsVDSPArr **vdsp_arr_b)
{
  AgsVDSPArr *arr_a, *arr_b;

  guint i;
  gboolean success;
  
  GRecMutex *mutex;

  g_return_val_if_fail(vdsp_256_manager != NULL, FALSE);
  g_return_val_if_fail(vdsp_arr_a != NULL, FALSE);
  g_return_val_if_fail(vdsp_arr_b != NULL, FALSE);

  mutex = AGS_VDSP_256_MANAGER_GET_OBJ_MUTEX(vdsp_256_manager);
  
  arr_a = NULL;
  arr_b = NULL;

  success = FALSE;
  
  g_rec_mutex_lock(mutex);

  for(i = 0; i < 2; i++){
    AgsVDSPArr *vdsp_arr;
    
    GList *vdsp;

    AgsVDSP256Types vdsp_type;

    if(i == 0){
      vdsp_type = vdsp_type_a;
    }else{
      vdsp_type = vdsp_type_b;
    }
    
    vdsp_arr = NULL;
  
    switch(vdsp_type){
    case AGS_VDSP_256_INT:
      {
	vdsp = vdsp_256_manager->int_arr;
      
	while(vdsp != NULL){
	  if(ags_atomic_int_get(&(AGS_VDSP_ARR(vdsp->data)->locked)) == FALSE){
	    vdsp_arr = (AgsVDSPArr *) vdsp->data;
	  
	    break;
	  }
	
	  vdsp = vdsp->next;
	}
      }
      break;
    case AGS_VDSP_256_FLOAT:
      {
	vdsp = vdsp_256_manager->float_arr;
      
	while(vdsp != NULL){
	  if(ags_atomic_int_get(&(AGS_VDSP_ARR(vdsp->data)->locked)) == FALSE){
	    vdsp_arr = (AgsVDSPArr *) vdsp->data;
	  
	    break;
	  }
	
	  vdsp = vdsp->next;
	}
      }
      break;
    case AGS_VDSP_256_DOUBLE:
      {
	vdsp = vdsp_256_manager->double_arr;
      
	while(vdsp != NULL){
	  if(ags_atomic_int_get(&(AGS_VDSP_ARR(vdsp->data)->locked)) == FALSE){
	    vdsp_arr = (AgsVDSPArr *) vdsp->data;
	  
	    break;
	  }
	
	  vdsp = vdsp->next;
	}
      }
      break;
    };

    if(i == 0){
      arr_a = vdsp_arr;
    }else{
      arr_b = vdsp_arr;
    }
  }
  
  if(arr_a != NULL &&
     arr_b != NULL){
    success = TRUE;
  }
  
  if(success){
    vdsp_arr_a[0] = arr_a;
    vdsp_arr_b[0] = arr_b;
    
    ags_atomic_int_set(&(arr_a->locked),
		     TRUE);

    ags_atomic_int_set(&(arr_b->locked),
		     TRUE);
  }

  g_rec_mutex_unlock(mutex);
  
  return(success);
}

gboolean
ags_vdsp_256_manager_try_acquire_triple(AgsVDSP256Manager *vdsp_256_manager,
					AgsVDSP256Types vdsp_type_a, AgsVDSP256Types vdsp_type_b, AgsVDSP256Types vdsp_type_c,
					AgsVDSPArr **vdsp_arr_a, AgsVDSPArr **vdsp_arr_b, AgsVDSPArr **vdsp_arr_c)
{
  AgsVDSPArr *arr_a, *arr_b, *arr_c;

  guint i;
  gboolean success;
  
  GRecMutex *mutex;

  g_return_val_if_fail(vdsp_256_manager != NULL, FALSE);
  g_return_val_if_fail(vdsp_arr_a != NULL, FALSE);
  g_return_val_if_fail(vdsp_arr_b != NULL, FALSE);
  g_return_val_if_fail(vdsp_arr_c != NULL, FALSE);

  mutex = AGS_VDSP_256_MANAGER_GET_OBJ_MUTEX(vdsp_256_manager);
  
  success = FALSE;

  arr_a = NULL;
  arr_b = NULL;
  arr_c = NULL;
  
  g_rec_mutex_lock(mutex);

  for(i = 0; i < 3; i++){
    AgsVDSPArr *vdsp_arr;
    
    GList *vdsp;

    AgsVDSP256Types vdsp_type;

    if(i == 0){
      vdsp_type = vdsp_type_a;
    }else if(i == 1){
      vdsp_type = vdsp_type_b;
    }else{
      vdsp_type = vdsp_type_c;
    }
    
    vdsp_arr = NULL;
  
    switch(vdsp_type){
    case AGS_VDSP_256_INT:
      {
	vdsp = vdsp_256_manager->int_arr;
      
	while(vdsp != NULL){
	  if(ags_atomic_int_get(&(AGS_VDSP_ARR(vdsp->data)->locked)) == FALSE){
	    vdsp_arr = (AgsVDSPArr *) vdsp->data;
	  
	    break;
	  }
	
	  vdsp = vdsp->next;
	}
      }
      break;
    case AGS_VDSP_256_FLOAT:
      {
	vdsp = vdsp_256_manager->float_arr;
      
	while(vdsp != NULL){
	  if(ags_atomic_int_get(&(AGS_VDSP_ARR(vdsp->data)->locked)) == FALSE){
	    vdsp_arr = (AgsVDSPArr *) vdsp->data;
	  
	    break;
	  }
	
	  vdsp = vdsp->next;
	}
      }
      break;
    case AGS_VDSP_256_DOUBLE:
      {
	vdsp = vdsp_256_manager->double_arr;
      
	while(vdsp != NULL){
	  if(ags_atomic_int_get(&(AGS_VDSP_ARR(vdsp->data)->locked)) == FALSE){
	    vdsp_arr = (AgsVDSPArr *) vdsp->data;
	  
	    break;
	  }
	
	  vdsp = vdsp->next;
	}
      }
      break;
    };

    if(i == 0){
      arr_a = vdsp_arr;
    }else if(i == 1){
      arr_b = vdsp_arr;
    }else{
      arr_c = vdsp_arr;
    }
  }
  
  if(arr_a != NULL &&
     arr_b != NULL &&
     arr_c != NULL){
    success = TRUE;
  }
  
  if(success){
    vdsp_arr_a[0] = arr_a;
    vdsp_arr_b[0] = arr_b;
    vdsp_arr_c[0] = arr_c;
    
    ags_atomic_int_set(&(arr_a->locked),
		     TRUE);

    ags_atomic_int_set(&(arr_b->locked),
		     TRUE);

    ags_atomic_int_set(&(arr_c->locked),
		     TRUE);
  }

  g_rec_mutex_unlock(mutex);
  
  return(success);
}

gboolean
ags_vdsp_256_manager_try_acquire_quad(AgsVDSP256Manager *vdsp_256_manager,
				      AgsVDSP256Types vdsp_type_a, AgsVDSP256Types vdsp_type_b, AgsVDSP256Types vdsp_type_c, AgsVDSP256Types vdsp_type_d,
				      AgsVDSPArr **vdsp_arr_a, AgsVDSPArr **vdsp_arr_b, AgsVDSPArr **vdsp_arr_c, AgsVDSPArr **vdsp_arr_d)
{
  AgsVDSPArr *arr_a, *arr_b, *arr_c, *arr_d;

  guint i;
  gboolean success;
  
  GRecMutex *mutex;

  g_return_val_if_fail(vdsp_256_manager != NULL, FALSE);
  g_return_val_if_fail(vdsp_arr_a != NULL, FALSE);
  g_return_val_if_fail(vdsp_arr_b != NULL, FALSE);
  g_return_val_if_fail(vdsp_arr_c != NULL, FALSE);
  g_return_val_if_fail(vdsp_arr_d != NULL, FALSE);

  mutex = AGS_VDSP_256_MANAGER_GET_OBJ_MUTEX(vdsp_256_manager);
  
  success = FALSE;

  arr_a = NULL;
  arr_b = NULL;
  arr_c = NULL;
  arr_d = NULL;
  
  g_rec_mutex_lock(mutex);

  for(i = 0; i < 3; i++){
    AgsVDSPArr *vdsp_arr;
    
    GList *vdsp;

    AgsVDSP256Types vdsp_type;

    if(i == 0){
      vdsp_type = vdsp_type_a;
    }else if(i == 1){
      vdsp_type = vdsp_type_b;
    }else if(i == 2){
      vdsp_type = vdsp_type_c;
    }else{
      vdsp_type = vdsp_type_d;
    }
    
    vdsp_arr = NULL;
  
    switch(vdsp_type){
    case AGS_VDSP_256_INT:
      {
	vdsp = vdsp_256_manager->int_arr;
      
	while(vdsp != NULL){
	  if(ags_atomic_int_get(&(AGS_VDSP_ARR(vdsp->data)->locked)) == FALSE){
	    vdsp_arr = (AgsVDSPArr *) vdsp->data;
	  
	    break;
	  }
	
	  vdsp = vdsp->next;
	}
      }
      break;
    case AGS_VDSP_256_FLOAT:
      {
	vdsp = vdsp_256_manager->float_arr;
      
	while(vdsp != NULL){
	  if(ags_atomic_int_get(&(AGS_VDSP_ARR(vdsp->data)->locked)) == FALSE){
	    vdsp_arr = (AgsVDSPArr *) vdsp->data;
	  
	    break;
	  }
	
	  vdsp = vdsp->next;
	}
      }
      break;
    case AGS_VDSP_256_DOUBLE:
      {
	vdsp = vdsp_256_manager->double_arr;
      
	while(vdsp != NULL){
	  if(ags_atomic_int_get(&(AGS_VDSP_ARR(vdsp->data)->locked)) == FALSE){
	    vdsp_arr = (AgsVDSPArr *) vdsp->data;
	  
	    break;
	  }
	
	  vdsp = vdsp->next;
	}
      }
      break;
    };

    if(i == 0){
      arr_a = vdsp_arr;
    }else if(i == 1){
      arr_b = vdsp_arr;
    }else if(i == 2){
      arr_c = vdsp_arr;
    }else{
      arr_d = vdsp_arr;
    }
  }
  
  if(arr_a != NULL &&
     arr_b != NULL &&
     arr_c != NULL &&
     arr_d != NULL){
    success = TRUE;
  }
  
  if(success){
    vdsp_arr_a[0] = arr_a;
    vdsp_arr_b[0] = arr_b;
    vdsp_arr_c[0] = arr_c;
    vdsp_arr_d[0] = arr_d;
    
    ags_atomic_int_set(&(arr_a->locked),
		     TRUE);

    ags_atomic_int_set(&(arr_b->locked),
		     TRUE);

    ags_atomic_int_set(&(arr_c->locked),
		     TRUE);

    ags_atomic_int_set(&(arr_d->locked),
		     TRUE);
  }

  g_rec_mutex_unlock(mutex);
  
  return(success);
}

/**
 * ags_vdsp_256_manager_release:
 * @vdsp_256_manager: the #AgsVDSP256Manager
 * @vdsp_arr: the #AgsVDSPArr-struct
 *
 * Release @vdsp_arr.
 * 
 * Since: 7.0.0
 */
void
ags_vdsp_256_manager_release(AgsVDSP256Manager *vdsp_256_manager,
			     AgsVDSPArr *vdsp_arr)
{
  g_return_if_fail(vdsp_arr != NULL);
  
  ags_atomic_int_set(&(vdsp_arr->locked),
		   FALSE);
}

/**
 * ags_vdsp_256_manager_get_instance:
 *
 * Get instance.
 *
 * Returns: (transfer none): the #AgsVDSP256Manager
 *
 * Since: 7.0.0
 */
AgsVDSP256Manager*
ags_vdsp_256_manager_get_instance()
{
  static GMutex mutex;

  g_mutex_lock(&mutex);

  if(ags_vdsp_256_manager == NULL){
    ags_vdsp_256_manager = ags_vdsp_256_manager_new();

    ags_vdsp_256_manager_reserve_all(ags_vdsp_256_manager);
  }

  g_mutex_unlock(&mutex);

  return(ags_vdsp_256_manager);
}

/**
 * ags_vdsp_256_manager_new:
 *
 * Create a new instance of #AgsVDSP256Manager
 *
 * Returns: the new #AgsVDSP256Manager
 *
 * Since: 7.0.0
 */
AgsVDSP256Manager*
ags_vdsp_256_manager_new()
{
  AgsVDSP256Manager *vdsp_256_manager;

  vdsp_256_manager = (AgsVDSP256Manager *) g_object_new(AGS_TYPE_VDSP_256_MANAGER,
							NULL);

  return(vdsp_256_manager);
}
