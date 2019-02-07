/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/audio/ags_recycling_context.h>

#include <ags/libags.h>

#include <ags/audio/ags_recall_id.h>

#include <stdlib.h>
#include <string.h>

#include <ags/i18n.h>

void ags_recycling_context_class_init(AgsRecyclingContextClass *recycling_context_class);
void ags_recycling_context_init(AgsRecyclingContext *recycling_context);
void ags_recycling_context_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_recycling_context_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_recycling_context_dispose(GObject *gobject);
void ags_recycling_context_finalize(GObject *gobject);

/**
 * SECTION:ags_recycling_context
 * @short_description: A context of recycling acting as dynamic context.
 * @title: AgsRecyclingContext
 * @section_id:
 * @include: ags/audio/ags_recycling_context.h
 *
 * #AgsRecyclingContext organizes #AgsRecycling objects as dynamic context
 * within nested tree.
 */

enum{
  PROP_0,
  PROP_RECALL_ID,
  PROP_PARENT,
  PROP_CHILD,
  PROP_LENGTH,
};

static gpointer ags_recycling_context_parent_class = NULL;

static pthread_mutex_t ags_recycling_context_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_recycling_context_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_recycling_context = 0;

    static const GTypeInfo ags_recycling_context_info = {
      sizeof (AgsRecyclingContextClass),
      (GBaseInitFunc) NULL, /* base_init */
      (GBaseFinalizeFunc) NULL, /* base_finalize */
      (GClassInitFunc) ags_recycling_context_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecyclingContext),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recycling_context_init,
    };

    ags_type_recycling_context = g_type_register_static(G_TYPE_OBJECT,
							"AgsRecyclingContext",
							&ags_recycling_context_info,
							0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_recycling_context);
  }

  return g_define_type_id__volatile;
}

void
ags_recycling_context_class_init(AgsRecyclingContextClass *recycling_context)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_recycling_context_parent_class = g_type_class_peek_parent(recycling_context);
  
  gobject = (GObjectClass *) recycling_context;

  gobject->set_property = ags_recycling_context_set_property;
  gobject->get_property = ags_recycling_context_get_property;

  gobject->dispose = ags_recycling_context_dispose;
  gobject->finalize = ags_recycling_context_finalize;

  /* properties */
  /**
   * AgsRecyclingContext:recall-id:
   *
   * The assigned #AgsRecallID.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("recall-id",
				   i18n_pspec("the default recall id"),
				   i18n_pspec("The recall id located in audio object as destiny"),
				   AGS_TYPE_RECALL_ID,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_ID,
				  param_spec);

  /**
   * AgsRecyclingContext:parent:
   *
   * The parent recycling context within tree.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("parent",
				   i18n_pspec("parent context"),
				   i18n_pspec("The context this one is packed into"),
				   AGS_TYPE_RECYCLING_CONTEXT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PARENT,
				  param_spec);

  /**
   * AgsRecyclingContext:child:
   *
   * The child recycling contexts.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("child",
				    i18n_pspec("child context"),
				    i18n_pspec("The child context"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHILD,
				  param_spec);
  
  /**
   * AgsRecyclingContext:length:
   *
   * Boundary length.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint64("length",
				   i18n_pspec("length of the array of assigned recycling"),
				   i18n_pspec("The recycling array length"),
				   0, G_MAXUINT64,
				   0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LENGTH,
				  param_spec);
}

void
ags_recycling_context_init(AgsRecyclingContext *recycling_context)
{
  recycling_context->flags = 0;
  recycling_context->sound_scope = 0;

  /* object mutex */
  recycling_context->obj_mutexattr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));

  pthread_mutexattr_init(recycling_context->obj_mutexattr);
  pthread_mutexattr_settype(recycling_context->obj_mutexattr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(recycling_context->obj_mutexattr,
				PTHREAD_PRIO_INHERIT);
#endif

  recycling_context->obj_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(recycling_context->obj_mutex,
		     recycling_context->obj_mutexattr);

  /* recall id */
  recycling_context->recall_id = NULL;

  /* parent and child */
  recycling_context->parent = NULL;
  recycling_context->children = NULL;

  /* context */
  recycling_context->recycling = NULL;
  recycling_context->length = 0;
}

void
ags_recycling_context_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsRecyclingContext *recycling_context;

  pthread_mutex_t *recycling_context_mutex;

  recycling_context = AGS_RECYCLING_CONTEXT(gobject);

  /* get recycling context mutex */
  pthread_mutex_lock(ags_recycling_context_get_class_mutex());
  
  recycling_context_mutex = recycling_context->obj_mutex;
  
  pthread_mutex_unlock(ags_recycling_context_get_class_mutex());

  switch(prop_id){
  case PROP_RECALL_ID:
    {
      AgsRecallID *recall_id;

      recall_id = (AgsRecallID *) g_value_get_object(value);

      pthread_mutex_lock(recycling_context_mutex);
      
      if(recycling_context->recall_id == (GObject *) recall_id){
	pthread_mutex_unlock(recycling_context_mutex);
      
	return;
      }

      if(recycling_context->recall_id != NULL){
	g_object_unref(G_OBJECT(recycling_context->recall_id));
      }

      if(recall_id != NULL){
	g_object_ref(G_OBJECT(recall_id));
      }

      recycling_context->recall_id = (GObject *) recall_id;
      
      pthread_mutex_unlock(recycling_context_mutex);      
    }
    break;
  case PROP_PARENT:
    {
      AgsRecyclingContext *parent;

      parent = (AgsRecyclingContext *) g_value_get_object(value);

      pthread_mutex_lock(recycling_context_mutex);

      if(recycling_context->parent == parent){
	pthread_mutex_unlock(recycling_context_mutex);

	return;
      }

      if(recycling_context->parent != NULL){
	g_object_unref(recycling_context->parent);
      }
      
      if(parent != NULL){
	g_object_ref(parent);
      }

      recycling_context->parent = parent;

      pthread_mutex_unlock(recycling_context_mutex);
    }
    break;
  case PROP_CHILD:
    {
      AgsRecyclingContext *child;

      child = (AgsRecyclingContext *) g_value_get_pointer(value);

      pthread_mutex_lock(recycling_context_mutex);
      
      if(!AGS_IS_RECYCLING_CONTEXT(child) ||
	 g_list_find(recycling_context->children, child) != NULL){
	pthread_mutex_unlock(recycling_context_mutex);

	return;
      }

      pthread_mutex_unlock(recycling_context_mutex);

      ags_recycling_context_add_child(recycling_context,
				      child);
    }
    break;
  case PROP_LENGTH:
    {
      guint64 length, old_length;
      guint64 i;

      length = g_value_get_uint64(value);

      pthread_mutex_lock(recycling_context_mutex);
      
      if(length == 0){
	if(recycling_context->recycling != NULL){
	  free(recycling_context->recycling);
	}
	
	recycling_context->recycling = NULL;
	recycling_context->length = 0;

	pthread_mutex_unlock(recycling_context_mutex);

	return;
      }
	  
      if(recycling_context->recycling == NULL){
	recycling_context->recycling = (AgsRecycling **) malloc(length * sizeof(AgsRecycling *));
	old_length = 0;
      }else{
	recycling_context->recycling = (AgsRecycling **) realloc(recycling_context->recycling,
								 length * sizeof(AgsRecycling *));
	old_length = recycling_context->length;
      }
      
      recycling_context->length = length;

      for(i = old_length; i < length; i++){
	recycling_context->recycling[i] = NULL;
      }

      pthread_mutex_unlock(recycling_context_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recycling_context_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsRecyclingContext *recycling_context;

  pthread_mutex_t *recycling_context_mutex;

  recycling_context = AGS_RECYCLING_CONTEXT(gobject);

  /* get recycling context mutex */
  pthread_mutex_lock(ags_recycling_context_get_class_mutex());
  
  recycling_context_mutex = recycling_context->obj_mutex;
  
  pthread_mutex_unlock(ags_recycling_context_get_class_mutex());

  switch(prop_id){
  case PROP_RECALL_ID:
    {
      pthread_mutex_lock(recycling_context_mutex);
      
      g_value_set_object(value, recycling_context->recall_id);

      pthread_mutex_unlock(recycling_context_mutex);
    }
    break;
  case PROP_PARENT:
    {
      pthread_mutex_lock(recycling_context_mutex);
      
      g_value_set_object(value, recycling_context->parent);

      pthread_mutex_unlock(recycling_context_mutex);
    }
    break;
  case PROP_CHILD:
    {
      pthread_mutex_lock(recycling_context_mutex);
      
      g_value_set_pointer(value, g_list_copy_deep(recycling_context->children,
						  (GCopyFunc) g_object_ref,
						  NULL));

      pthread_mutex_unlock(recycling_context_mutex);
    }
    break;
  case PROP_LENGTH:
    {
      pthread_mutex_lock(recycling_context_mutex);
      
      g_value_set_uint64(value, recycling_context->length);

      pthread_mutex_unlock(recycling_context_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recycling_context_dispose(GObject *gobject)
{
  AgsRecyclingContext *recycling_context;

  GList *list_start, *list;

  guint i;
  
  recycling_context = AGS_RECYCLING_CONTEXT(gobject);

  /* parent */
  if(recycling_context->parent != NULL){
    g_object_unref(recycling_context->parent);

    recycling_context->parent = NULL;
  }
  
  /* recall id */
  if(recycling_context->recall_id != NULL){
    g_object_unref(recycling_context->recall_id);

    recycling_context->recall_id = NULL;
  }

  /* recycling */
  if(recycling_context->recycling != NULL){
    for(i = 0; i < recycling_context->length; i++){
      g_object_unref(recycling_context->recycling[i]);
    }

    free(recycling_context->recycling);
    
    recycling_context->recycling = NULL;
    recycling_context->length = 0;
  }
  
  /* children */
  list = 
    list_start = g_list_copy(recycling_context->children);

  while(list != NULL){
    ags_recycling_context_remove_child(recycling_context,
				       list->data);
    
    list = list->next;
  }

  g_list_free(list_start);
  
  /* call parent */
  G_OBJECT_CLASS(ags_recycling_context_parent_class)->dispose(gobject);
}

void
ags_recycling_context_finalize(GObject *gobject)
{
  AgsRecyclingContext *recycling_context;

  GList *list;

  guint i;
    
  recycling_context = AGS_RECYCLING_CONTEXT(gobject);

  /* parent */
  if(recycling_context->parent != NULL){
    g_object_unref(recycling_context->parent);
  }
  
  /* recall id */
  if(recycling_context->recall_id != NULL){
    g_object_unref(recycling_context->recall_id);
  }

  /* recycling */
  if(recycling_context->recycling != NULL){
    for(i = 0; i < recycling_context->length; i++){
      g_object_unref(recycling_context->recycling[i]);
    }
  
    free(recycling_context->recycling);
  }
  
  /* children */
  g_list_free_full(recycling_context->children,
		   g_object_unref);

  /* call parent */
  G_OBJECT_CLASS(ags_recycling_context_parent_class)->finalize(gobject);
}

/**
 * ags_recycling_context_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_recycling_context_get_class_mutex()
{
  return(&ags_recycling_context_class_mutex);
}

/**
 * ags_recycling_context_find_scope:
 * @recycling_context: the #GList-struct containing #AgsRecyclingContext
 * @sound_scope: the sound scope
 * 
 * Find matching @sound_scope in @recycling_context.
 *
 * Returns: the matching #GList-struct or %NULL if not found
 * 
 * Since: 2.0.0
 */
GList*
ags_recycling_context_find_scope(GList *recycling_context, gint sound_scope)
{
  gboolean success;

  pthread_mutex_t *recycling_context_mutex;
  
  while(recycling_context != NULL){
    AgsRecyclingContext *current;

    current = AGS_RECYCLING_CONTEXT(recycling_context->data);

    /* get recycling context mutex */
    pthread_mutex_lock(ags_recycling_context_get_class_mutex());

    recycling_context_mutex = current->obj_mutex;
    
    pthread_mutex_unlock(ags_recycling_context_get_class_mutex());

    /* check success */
    pthread_mutex_lock(recycling_context_mutex);

    success = (current->sound_scope == sound_scope) ? TRUE: FALSE;
    
    pthread_mutex_unlock(recycling_context_mutex);
    
    if(success){
      break;
    }

    recycling_context = recycling_context->next;
  }

  return(recycling_context);
}

/**
 * ags_recycling_context_replace:
 * @recycling_context: the #AgsRecyclingContext
 * @recycling: the #AgsRecycling to add
 * @position: the index of @recycling
 *
 * Replaces one recycling entry in a context.
 *
 * Since: 2.0.0
 */
void
ags_recycling_context_replace(AgsRecyclingContext *recycling_context,
			      AgsRecycling *recycling,
			      gint position)
{
  AgsRecycling *old_recycling;
  
  guint64 length;
  
  pthread_mutex_t *recycling_context_mutex;
  
  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return;
  }

  /* get recycling context mutex */
  pthread_mutex_lock(ags_recycling_context_get_class_mutex());
  
  recycling_context_mutex = recycling_context->obj_mutex;
  
  pthread_mutex_unlock(ags_recycling_context_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(recycling_context_mutex);

  length = recycling_context->length;

  pthread_mutex_unlock(recycling_context_mutex);
  
  if(position >= length){    
    return;
  }

  /* replace */
  pthread_mutex_lock(recycling_context_mutex);
  
  old_recycling = recycling_context->recycling[position];
  recycling_context->recycling[position] = recycling;
  
  pthread_mutex_unlock(recycling_context_mutex);

  /* ref count */
  if(old_recycling != NULL){
    g_object_unref(old_recycling);
  }

  if(recycling != NULL){
    g_object_ref(recycling);
  }
}

/**
 * ags_recycling_context_add:
 * @recycling_context: the #AgsRecyclingContext
 * @recycling: the #AgsRecycling to add
 *
 * Adds a recycling to a context.
 *
 * Since: 2.0.0
 */
void
ags_recycling_context_add(AgsRecyclingContext *recycling_context,
			  AgsRecycling *recycling)
{
  gint new_length;

  pthread_mutex_t *recycling_context_mutex;
  
  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context) ||
     !AGS_IS_RECYCLING(recycling)){
    return;
  }

  /* get recycling context mutex */
  pthread_mutex_lock(ags_recycling_context_get_class_mutex());
  
  recycling_context_mutex = recycling_context->obj_mutex;
  
  pthread_mutex_unlock(ags_recycling_context_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(recycling_context_mutex);

  new_length = recycling_context->length + 1;

  pthread_mutex_unlock(recycling_context_mutex);

  /* resize */
  g_object_set(recycling_context,
	       "length", (guint64) new_length,
	       NULL);
  
  /*  */
  recycling_context->recycling[new_length - 1] = recycling;

  /* ref count */
  g_object_ref(recycling);
}

/**
 * ags_recycling_context_remove:
 * @recycling_context: the #AgsRecyclingContext
 * @recycling: the #AgsRecycling to remove
 *
 * Removes a recycling in a context.
 *
 * Since: 2.0.0
 */
void
ags_recycling_context_remove(AgsRecyclingContext *recycling_context,
			     AgsRecycling *recycling)
{
  AgsRecycling **old_context;

  gint new_length;
  guint i, j;

  pthread_mutex_t *recycling_context_mutex;

  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context) ||
     !AGS_IS_RECYCLING(recycling)){
    return;
  }

  /* get recycling context mutex */
  pthread_mutex_lock(ags_recycling_context_get_class_mutex());
  
  recycling_context_mutex = recycling_context->obj_mutex;
  
  pthread_mutex_unlock(ags_recycling_context_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(recycling_context_mutex);

  if(recycling_context->length > 0){
    old_context = (AgsRecycling **) malloc(recycling_context->length * sizeof(AgsRecycling *));
    memcpy(old_context, recycling_context->recycling, recycling_context->length * sizeof(AgsRecycling *));
  
    new_length = recycling_context->length - 1;
  }else{
    old_context = NULL;
    
    new_length = 0;
  }
  
  pthread_mutex_unlock(recycling_context_mutex);

  /* resize */
  g_object_set(recycling_context,
	       "length", (guint64) new_length,
	       NULL);

  /* reset */
  pthread_mutex_lock(recycling_context_mutex);

  for(i = 0, j = 0; i < new_length; i++){
    if(old_context[i] != recycling){
      recycling_context->recycling[j] = old_context[i];
      j++;
    }
  }
  
  pthread_mutex_unlock(recycling_context_mutex);

  /* ref count */
  g_object_unref(recycling);

  /* free old context */
  g_free(old_context);
}

/**
 * ags_recycling_context_insert:
 * @recycling_context: the #AgsRecyclingContext
 * @recycling: the #AgsRecycling to insert
 * @position: the index to insert at
 *
 * Inserts a recycling to a context.
 *
 * Since: 2.0.0
 */
void
ags_recycling_context_insert(AgsRecyclingContext *recycling_context,
			     AgsRecycling *recycling,
			     gint position)
{
  AgsRecycling **old_context;

  gint new_length;
  guint i, j;

  pthread_mutex_t *recycling_context_mutex;

  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context) ||
     !AGS_IS_RECYCLING(recycling)){
    return;
  }

  /* get recycling context mutex */
  pthread_mutex_lock(ags_recycling_context_get_class_mutex());
  
  recycling_context_mutex = recycling_context->obj_mutex;
  
  pthread_mutex_unlock(ags_recycling_context_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(recycling_context_mutex);

  if(recycling_context->length > 0){
    old_context = (AgsRecycling **) malloc(recycling_context->length * sizeof(AgsRecycling *));
    memcpy(old_context, recycling_context->recycling, recycling_context->length * sizeof(AgsRecycling *));
  }else{
    old_context = NULL;
  }
  
  new_length = recycling_context->length + 1;
  
  pthread_mutex_unlock(recycling_context_mutex);

  /* resize */
  g_object_set(recycling_context,
	       "length", (guint64) new_length,
	       NULL);

  /* reset */
  pthread_mutex_lock(recycling_context_mutex);

  for(i = 0, j = 0; i < new_length - 1; i++){
    if(i != position){
      recycling_context->recycling[i] = old_context[j];
      j++;
    }
  }

  recycling_context->recycling[position] = recycling;
  
  pthread_mutex_unlock(recycling_context_mutex);

  /* ref count */
  g_object_ref(recycling);

  if(old_context != NULL){
    free(old_context);
  }
}

/**
 * ags_recycling_context_get_toplevel:
 * @recycling_context: the #AgsRecyclingContext
 *
 * Iterates the tree up to highest level.
 *
 * Returns: the topmost recycling context
 *
 * Since: 2.0.0
 */
AgsRecyclingContext*
ags_recycling_context_get_toplevel(AgsRecyclingContext *recycling_context)
{
  AgsRecyclingContext *parent;
  
  pthread_mutex_t *recycling_context_mutex;

  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return(NULL);
  }

  do{
    /* get recycling context mutex */
    pthread_mutex_lock(ags_recycling_context_get_class_mutex());
  
    recycling_context_mutex = recycling_context->obj_mutex;
  
    pthread_mutex_unlock(ags_recycling_context_get_class_mutex());

    /* get some fields */
    pthread_mutex_lock(recycling_context_mutex);

    parent = recycling_context->parent;
    
    pthread_mutex_unlock(recycling_context_mutex);
  }while(parent != NULL && (recycling_context = parent) != NULL);
  
  return(recycling_context);
}

/**
 * ags_recycling_context_find:
 * @recycling_context: the #AgsRecyclingContext
 * @recycling: the #AgsRecycling to look up
 * 
 * Find position of recycling within array.
 *
 * Returns: recycling array index
 *
 * Since: 2.0.0
 */
gint
ags_recycling_context_find(AgsRecyclingContext *recycling_context,
			   AgsRecycling *recycling)
{
  gint i;

  pthread_mutex_t *recycling_context_mutex;

  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context) ||
     !AGS_IS_RECYCLING(recycling)){
    return(-1);
  }
  
  /* get recycling context mutex */
  pthread_mutex_lock(ags_recycling_context_get_class_mutex());
  
  recycling_context_mutex = recycling_context->obj_mutex;
  
  pthread_mutex_unlock(ags_recycling_context_get_class_mutex());

  /* find */
  pthread_mutex_lock(recycling_context_mutex);

  for(i = 0; i < recycling_context->length; i++){
    if(recycling_context->recycling[i] == recycling){
      pthread_mutex_unlock(recycling_context_mutex);
      
      return(i);
    }
  }

  pthread_mutex_unlock(recycling_context_mutex);

  return(-1);
}

/**
 * ags_recycling_context_find_child:
 * @recycling_context: the #AgsRecyclingContext
 * @recycling: the #AgsRecycling to look up
 * 
 * Find position of recycling within arrays.
 *
 * Returns: recycling array index
 *
 * Since: 2.0.0
 */
gint
ags_recycling_context_find_child(AgsRecyclingContext *recycling_context,
				 AgsRecycling *recycling)
{
  GList *child_start, *child;

  gint i;

  pthread_mutex_t *recycling_context_mutex;

  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context) ||
     !AGS_IS_RECYCLING(recycling)){
    return(-1);
  }

  /* get recycling context mutex */
  pthread_mutex_lock(ags_recycling_context_get_class_mutex());
  
  recycling_context_mutex = recycling_context->obj_mutex;
  
  pthread_mutex_unlock(ags_recycling_context_get_class_mutex());

  /* get child */
  pthread_mutex_lock(recycling_context_mutex);

  child =
    child_start = g_list_copy(recycling_context->children);

  pthread_mutex_unlock(recycling_context_mutex);

  for(i = 0; child != NULL; i++){
    if(ags_recycling_context_find(AGS_RECYCLING_CONTEXT(child->data),
				  recycling) != -1){
      g_list_free(child_start);
      
      return(i);
    }

    child = child->next;
  }

  g_list_free(child_start);

  return(-1);
}

/**
 * ags_recycling_context_find_parent:
 * @recycling_context: the #AgsRecyclingContext
 * @recycling: the #AgsRecycling to look up
 * 
 * Find position of recycling within array.
 *
 * Returns: recycling array index
 *
 * Since: 2.0.0
 */
gint
ags_recycling_context_find_parent(AgsRecyclingContext *recycling_context,
				  AgsRecycling *recycling)
{
  AgsRecyclingContext *parent;
  
  gint i;

  pthread_mutex_t *recycling_context_mutex;
  pthread_mutex_t *parent_mutex;

  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context) ||
     !AGS_IS_RECYCLING(recycling)){
    return(-1);
  }
  
  /* get recycling context mutex */
  pthread_mutex_lock(ags_recycling_context_get_class_mutex());
  
  recycling_context_mutex = recycling_context->obj_mutex;
  
  pthread_mutex_unlock(ags_recycling_context_get_class_mutex());

  /* get parent */
  pthread_mutex_lock(recycling_context_mutex);

  parent = recycling_context->parent;
  
  pthread_mutex_unlock(recycling_context_mutex);

  if(parent != NULL){
    /* get parent mutex */
    pthread_mutex_lock(ags_recycling_context_get_class_mutex());
  
    parent_mutex = parent->obj_mutex;
  
    pthread_mutex_unlock(ags_recycling_context_get_class_mutex());

    /* find */
    pthread_mutex_lock(parent_mutex);

    for(i = 0; i < parent->length; i++){
      if(parent->recycling[i] == recycling){
	pthread_mutex_unlock(parent_mutex);
      
	return(i);
      }
    }

    pthread_mutex_unlock(parent_mutex);
  }
  
  return(-1);
}

/**
 * ags_recycling_context_add_child:
 * @parent: the parental #AgsRecyclingContext
 * @child: the child
 *
 * Adds a recycling context as child.
 *
 * Since: 2.0.0
 */
void
ags_recycling_context_add_child(AgsRecyclingContext *parent,
				AgsRecyclingContext *child)
{
  pthread_mutex_t *parent_mutex;

  if(!AGS_IS_RECYCLING_CONTEXT(parent) ||
     !AGS_IS_RECYCLING_CONTEXT(child)){
    return;
  }

  g_object_set(child,
	       "parent", parent,
	       NULL);

  /* get parent mutex */
  pthread_mutex_lock(ags_recycling_context_get_class_mutex());

  parent_mutex = parent->obj_mutex;
    
  pthread_mutex_unlock(ags_recycling_context_get_class_mutex());
  
  /* add child */
  pthread_mutex_lock(parent_mutex);

  if(g_list_find(parent->children, child) == NULL){
    parent->children = g_list_append(parent->children,
				     child);
    g_object_ref(child);
  }
  
  pthread_mutex_unlock(parent_mutex);
}

/**
 * ags_recycling_context_remove_child:
 * @parent: the #AgsRecyclingContext
 * @child: the child to remove
 *
 * Removes a recycling context of its parent.
 *
 * Since: 2.0.0
 */
void
ags_recycling_context_remove_child(AgsRecyclingContext *parent,
				   AgsRecyclingContext *child)
{
  pthread_mutex_t *parent_mutex;

  if(!AGS_IS_RECYCLING_CONTEXT(parent) ||
     !AGS_IS_RECYCLING_CONTEXT(child)){
    return;
  }

  g_object_set(child,
	       "parent", NULL,
	       NULL);

  /* get parent mutex */
  pthread_mutex_lock(ags_recycling_context_get_class_mutex());

  parent_mutex = parent->obj_mutex;
    
  pthread_mutex_unlock(ags_recycling_context_get_class_mutex());

  /* remove child */
  pthread_mutex_lock(parent_mutex);

  if(g_list_find(parent->children, child) != NULL){
    parent->children = g_list_remove(parent->children,
				     child);
    g_object_unref(child);
  }
  
  pthread_mutex_unlock(parent_mutex);
}

/**
 * ags_recycling_context_get_child_recall_id:
 * @recycling_context: the #AgsRecyclingContext
 * 
 * Retrieve all child recall ids.
 *
 * Returns: the #AgsRecallID as #GList-struct
 *
 * Since: 2.0.0
 */
GList*
ags_recycling_context_get_child_recall_id(AgsRecyclingContext *recycling_context)
{
  GList *child_start, *child;

  GList *recall_id_list;

  pthread_mutex_t *recycling_context_mutex;
  pthread_mutex_t *child_mutex;

  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return(NULL);
  }

  /* get recycling context mutex */
  pthread_mutex_lock(ags_recycling_context_get_class_mutex());
  
  recycling_context_mutex = recycling_context->obj_mutex;
  
  pthread_mutex_unlock(ags_recycling_context_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(recycling_context_mutex);
  
  child =
    child_start = g_list_copy(recycling_context->children);
  
  pthread_mutex_unlock(recycling_context_mutex);

  recall_id_list = NULL;
  
  while(child != NULL){
    /* get recycling context mutex */
    pthread_mutex_lock(ags_recycling_context_get_class_mutex());
  
    child_mutex = AGS_RECYCLING_CONTEXT(child->data)->obj_mutex;
  
    pthread_mutex_unlock(ags_recycling_context_get_class_mutex());

    /*  */
    pthread_mutex_lock(child_mutex);
    
    if(AGS_RECYCLING_CONTEXT(child->data)->recall_id != NULL){
      recall_id_list = g_list_prepend(recall_id_list,
				      AGS_RECYCLING_CONTEXT(child->data)->recall_id);
    }

    pthread_mutex_unlock(child_mutex);
    
    child = child->next;
  }

  g_list_free(child_start);

  /* reverse the result */
  recall_id_list = g_list_reverse(recall_id_list);
  
  return(recall_id_list);
}

/**
 * ags_recycling_context_reset_recycling:
 * @recycling_context: the #AgsRecyclingContext
 * @old_first_recycling: the first recycling to replace
 * @old_last_recycling: the last recycling to replace
 * @new_first_recycling: the first recycling to insert
 * @new_last_recycling: the last recycling to insert
 *
 * Modify recycling of context.
 *
 * Returns: the new #AgsRecyclingContext
 *
 * Since: 2.0.0
 */
AgsRecyclingContext*
ags_recycling_context_reset_recycling(AgsRecyclingContext *recycling_context,
				      AgsRecycling *old_first_recycling, AgsRecycling *old_last_recycling,
				      AgsRecycling *new_first_recycling, AgsRecycling *new_last_recycling)
{
  AgsRecycling *recycling;
  AgsRecyclingContext *new_recycling_context;
  AgsRecyclingContext *parent;

  GList *child_start, *child;
  
  gint new_length;
  guint i;

  pthread_mutex_t *recycling_mutex;
  pthread_mutex_t *recycling_context_mutex;

  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return(NULL);
  }

  parent = NULL;
  
  /* retrieve new length */
  new_length = ags_recycling_position(new_first_recycling, old_last_recycling,
				      old_last_recycling);
  new_length++;

  /* instantiate */
  new_recycling_context = g_object_new(AGS_TYPE_RECYCLING_CONTEXT,
				       "length", (guint64) new_length,
				       NULL);
  
  recycling = new_first_recycling;
  
  for(i = 0; i < new_length; i++){
    /* get parent mutex */
    pthread_mutex_lock(ags_recycling_get_class_mutex());

    recycling_mutex = recycling->obj_mutex;
    
    pthread_mutex_unlock(ags_recycling_get_class_mutex());

    /* set context */
    new_recycling_context->recycling[i] = recycling;
    g_object_ref(recycling);
    
    /* iterate */
    pthread_mutex_lock(recycling_mutex);

    recycling = recycling->next;
    
    pthread_mutex_unlock(recycling_mutex);
  }

  /* get recycling context mutex */
  pthread_mutex_lock(ags_recycling_context_get_class_mutex());
  
  recycling_context_mutex = recycling_context->obj_mutex;
  
  pthread_mutex_unlock(ags_recycling_context_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(recycling_context_mutex);
  
  child =
    child_start = g_list_copy(recycling_context->children);
  
  pthread_mutex_unlock(recycling_context_mutex);
  
  while(child != NULL){
    g_object_set(new_recycling_context,
		 "child", child->data,
		 NULL);

    child = child->next;
  }

  if(parent != NULL){
    g_object_set(parent,
		 "child", new_recycling_context,
		 NULL);
  }
  
  /* dispose old recycling context */
  g_object_run_dispose((GObject *) recycling_context);
    
  return(new_recycling_context);
}

/**
 * ags_recycling_context_new:
 * @length: array dimension of context
 *
 * Creates a #AgsRecyclingContext, boundaries are specified by @length
 *
 * Returns: a new #AgsRecyclingContext
 *
 * Since: 2.0.0
 */
AgsRecyclingContext*
ags_recycling_context_new(guint64 length)
{
  AgsRecyclingContext *recycling_context;

  recycling_context = g_object_new(AGS_TYPE_RECYCLING_CONTEXT,
				   "length", length,
				   NULL);

  return(recycling_context);
}
