/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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
  PROP_PARENT,
  PROP_LENGTH,
  PROP_RECALL_ID,
};

static gpointer ags_recycling_context_parent_class = NULL;

GType
ags_recycling_context_get_type (void)
{
  static GType ags_type_recycling_context = 0;

  if(!ags_type_recycling_context){
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
  }

  return (ags_type_recycling_context);
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
   * AgsRecyclingContext:parent:
   *
   * The parent recycling context within tree.
   * 
   * Since: 1.0.0
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
   * AgsRecyclingContext:length:
   *
   * Boundary length.
   * 
   * Since: 1.0.0
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

  /**
   * AgsRecyclingContext:recall-id:
   *
   * The assigned #AgsRecallID.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("recall-id",
				   i18n_pspec("the default recall id"),
				   i18n_pspec("The recall id located in audio object as destiny"),
				   AGS_TYPE_RECALL_ID,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_ID,
				  param_spec);
}

void
ags_recycling_context_init(AgsRecyclingContext *recycling_context)
{
  recycling_context->mutexattr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));

  pthread_mutexattr_init(recycling_context->attr);
  pthread_mutexattr_settype(recycling_context->attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(recycling_context->attr,
				PTHREAD_PRIO_INHERIT);
#endif

  recycling_context->mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(recycling_context->mutex,
		     recycling_context->attr);
  
  recycling_context->recall_id = NULL;

  recycling_context->recycling = NULL;
  recycling_context->length = 0;

  recycling_context->parent = NULL;
  recycling_context->children = NULL;
}

void
ags_recycling_context_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsRecyclingContext *recycling_context;
  guint i;

  recycling_context = AGS_RECYCLING_CONTEXT(gobject);

  switch(prop_id){
  case PROP_PARENT:
    {
      AgsRecyclingContext *parent;

      parent = (AgsRecyclingContext *) g_value_get_object(value);

      if(recycling_context->parent == parent){
	return;
      }
      
      if(recycling_context->parent != NULL){
	ags_recycling_context_remove_child(recycling_context->parent,
					   recycling_context);
      }

      if(parent != NULL){
	ags_recycling_context_add_child(parent,
					recycling_context);
      }

      recycling_context->parent = parent;
    }
    break;
  case PROP_LENGTH:
    {
      guint64 length;

      length = g_value_get_uint64(value);

      recycling_context->recycling = (AgsRecycling **) malloc(length * sizeof(AgsRecycling *));
      recycling_context->length = length;

      for(i = 0; i < length; i++){
	recycling_context->recycling[i] = NULL;
      }
    }
    break;
  case PROP_RECALL_ID:
    {
      AgsRecallID *recall_id;

      recall_id = (AgsRecallID *) g_value_get_object(value);

      if(recycling_context->recall_id == (GObject *) recall_id){
	return;
      }

      if(recycling_context->recall_id != NULL){
	g_object_unref(G_OBJECT(recycling_context->recall_id));
      }

      if(recall_id != NULL){
	g_object_ref(G_OBJECT(recall_id));
      }

      recycling_context->recall_id = (GObject *) recall_id;
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

  recycling_context = AGS_RECYCLING_CONTEXT(gobject);

  switch(prop_id){
  case PROP_PARENT:
    {
      g_value_set_object(value, recycling_context->parent);
    }
    break;
  case PROP_LENGTH:
    {
      g_value_set_uint64(value, recycling_context->length);
    }
    break;
  case PROP_RECALL_ID:
    {
      g_value_set_object(value, recycling_context->recall_id);
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

  GList *list, *list_next;

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
    
    recycling_context->recycling = NULL;
    recycling_context->length = 0;
  }
  
  /* children */
  list = recycling_context->children;

  while(list != NULL){
    list_next = list->next;
    
    ags_recycling_context_remove_child(recycling_context,
				       list->data);
    
    list = list_next;
  }
  
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
  g_list_free(recycling_context->children);

  /* call parent */
  G_OBJECT_CLASS(ags_recycling_context_parent_class)->finalize(gobject);
}

/**
 * ags_recycling_context_replace:
 * @recycling_context: the #AgsRecyclingContext
 * @recycling: the #AgsRecycling to add
 * @position: the index of @recycling
 *
 * Replaces one recycling entry in a context.
 *
 * Since: 1.0.0
 */
void
ags_recycling_context_replace(AgsRecyclingContext *recycling_context,
			      AgsRecycling *recycling,
			      gint position)
{
  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return;
  }

  pthread_mutex_lock(recycling_context->mutex);
  
  if(position >= recycling_context->length){
    pthread_mutex_unlock(recycling_context->mutex);
    
    return;
  }

  recycling_context->recycling[position] = recycling;
  
  pthread_mutex_unlock(recycling_context->mutex);
}

/**
 * ags_recycling_context_add:
 * @recycling_context: the #AgsRecyclingContext
 * @recycling: the #AgsRecycling to add
 *
 * Adds a recycling to a context.
 *
 * Returns: the new recycling context
 *
 * Since: 1.0.0
 */
AgsRecyclingContext*
ags_recycling_context_add(AgsRecyclingContext *recycling_context,
			  AgsRecycling *recycling)
{
  AgsRecyclingContext *new_recycling_context;

  gint new_length;
  guint i;
  
  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return(NULL);
  }

  pthread_mutex_lock(recycling_context->mutex);

  new_length = recycling_context->length + 1;
  new_recycling_context = (AgsRecyclingContext *) g_object_new(AGS_TYPE_RECYCLING_CONTEXT,
							       "length", new_length,
							       NULL);

  memcpy(new_recycling_context->recycling, recycling_context->recycling, new_length * sizeof(AgsRecycling *));
  new_recycling_context->recycling[new_length] = recycling;

  /* ref count */
  for(i = 0; i < new_recycling_context->length; i++){
    g_object_ref(new_recycling_context->recycling[i]);
  }

  pthread_mutex_unlock(recycling_context->mutex);

  return(new_recycling_context);
}

/**
 * ags_recycling_context_remove:
 * @recycling_context: the #AgsRecyclingContext
 * @recycling: the #AgsRecycling to remove
 *
 * Removes a recycling in a context.
 *
 * Returns: the new recycling context
 *
 * Since: 1.0.0
 */
AgsRecyclingContext*
ags_recycling_context_remove(AgsRecyclingContext *recycling_context,
			     AgsRecycling *recycling)
{
  AgsRecyclingContext *new_recycling_context;

  gint new_length;
  gint position;
  guint i;

  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return(NULL);
  }

  pthread_mutex_lock(recycling_context->mutex);
  
  new_recycling_context = (AgsRecyclingContext *) g_object_new(AGS_TYPE_RECYCLING_CONTEXT,
							       NULL);

  new_length = recycling_context->length - 1;

  new_recycling_context->recycling = (AgsRecycling **) malloc(new_length * sizeof(AgsRecycling *));

  for(position = 0; recycling_context->recycling[position] != recycling; position++);

  memcpy(new_recycling_context->recycling, recycling_context->recycling, (new_length - position) * sizeof(AgsRecycling *));
  memcpy(&(new_recycling_context->recycling[position + 1]), recycling_context->recycling, (-1 * (position - new_length)) * sizeof(AgsRecycling *));

  /* ref count */
  for(i = 0; i < new_recycling_context->length; i++){
    g_object_ref(new_recycling_context->recycling[i]);
  }

  pthread_mutex_unlock(recycling_context->mutex);

  return(new_recycling_context);
}

/**
 * ags_recycling_context_insert:
 * @recycling_context: the #AgsRecyclingContext
 * @recycling: the #AgsRecycling to insert
 * @position: the index to insert at
 *
 * Inserts a recycling to a context.
 *
 * Returns: the new recycling context
 *
 * Since: 1.0.0
 */
AgsRecyclingContext*
ags_recycling_context_insert(AgsRecyclingContext *recycling_context,
			     AgsRecycling *recycling,
			     gint position)
{
  AgsRecyclingContext *new_recycling_context;

  gint new_length;
  guint i;

  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return(NULL);
  }

  pthread_mutex_lock(recycling_context->mutex);

  new_recycling_context = (AgsRecyclingContext *) g_object_new(AGS_TYPE_RECYCLING_CONTEXT,
							       NULL);

  new_length = recycling_context->length + 1;

  new_recycling_context->recycling = (AgsRecycling **) malloc(new_length * sizeof(AgsRecycling *));

  memcpy(new_recycling_context->recycling, recycling_context->recycling, (new_length - position) * sizeof(AgsRecycling *));
  new_recycling_context->recycling[position] = recycling;
  memcpy(&(new_recycling_context->recycling[position + 1]), recycling_context->recycling, (-1 * (position - new_length)) * sizeof(AgsRecycling *));
  
  /* ref count */
  for(i = 0; i < new_recycling_context->length; i++){
    g_object_ref(new_recycling_context->recycling[i]);
  }

  pthread_mutex_unlock(recycling_context->mutex);

  return(new_recycling_context);
}

/**
 * ags_recycling_context_get_toplevel:
 * @recycling_context: the #AgsRecyclingContext
 *
 * Iterates the tree up to highest level.
 *
 * Returns: the topmost recycling context
 *
 * Since: 1.0.0
 */
AgsRecyclingContext*
ags_recycling_context_get_toplevel(AgsRecyclingContext *recycling_context)
{
  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return(NULL);
  }

  pthread_mutex_lock(recycling_context->mutex);

  while(recycling_context->parent != NULL){
    recycling_context = recycling_context->parent;

    pthread_mutex_unlock(recycling_context->mutex);

    pthread_mutex_lock(recycling_context->mutex);
  }

  pthread_mutex_unlock(recycling_context->mutex);
  
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
 * Since: 1.0.0
 */
gint
ags_recycling_context_find(AgsRecyclingContext *recycling_context,
			   AgsRecycling *recycling)
{
  gint i;

  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return(-1);
  }

  pthread_mutex_lock(recycling_context->mutex);

  for(i = 0; i < recycling_context->length; i++){
    if(recycling_context->recycling[i] == recycling){
      pthread_mutex_unlock(recycling_context->mutex);
      
      return(i);
    }
  }

  pthread_mutex_unlock(recycling_context->mutex);

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
 * Since: 1.0.0
 */
gint
ags_recycling_context_find_child(AgsRecyclingContext *recycling_context,
				 AgsRecycling *recycling)
{
  GList *child_start, *child;

  gint i;

  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return(-1);
  }

  pthread_mutex_lock(recycling_context->mutex);

  child =
    child_start = g_list_copy(recycling_context->children);

  pthread_mutex_unlock(recycling_context->mutex);

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
 * Since: 1.0.0
 */
gint
ags_recycling_context_find_parent(AgsRecyclingContext *recycling_context,
				  AgsRecycling *recycling)
{
  AgsRecyclingContext *parent;
  
  gint i;

  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return(-1);
  }

  pthread_mutex_lock(recycling_context->mutex);

  parent = recycling_context->parent;
  
  pthread_mutex_unlock(recycling_context->mutex);

  pthread_mutex_lock(parent->mutex);

  for(i = 0; i < parent->length; i++){
    if(parent->recycling[i] == recycling){
      pthread_mutex_unlock(parent->mutex);
      
      return(i);
    }
  }

  pthread_mutex_unlock(parent->mutex);

  return(-1);
}

/**
 * ags_recycling_context_add_child:
 * @parent: the parental #AgsRecyclingContext
 * @child: the child
 *
 * Adds a recycling context as child.
 *
 * Since: 1.0.0
 */
void
ags_recycling_context_add_child(AgsRecyclingContext *parent,
				AgsRecyclingContext *child)
{
  if(!AGS_IS_RECYCLING_CONTEXT(parent) ||
     !AGS_IS_RECYCLING_CONTEXT(child)){
    return;
  }

  g_object_ref(G_OBJECT(parent));
  g_object_ref(G_OBJECT(child));

  /* set parent */
  pthread_mutex_lock(child->mutex);

  child->parent = parent;

  pthread_mutex_unlock(child->mutex);

  /* add child */
  pthread_mutex_lock(parent->mutex);

  parent->children = g_list_append(parent->children,
				   child);

  pthread_mutex_unlock(parent->mutex);
}

/**
 * ags_recycling_context_remove_child:
 * @parent: the #AgsRecyclingContext
 * @child: the child to remove
 *
 * Removes a recycling context of its parent.
 *
 * Since: 1.0.0
 */
void
ags_recycling_context_remove_child(AgsRecyclingContext *parent,
				   AgsRecyclingContext *child)
{
  if(!AGS_IS_RECYCLING_CONTEXT(parent) ||
     !AGS_IS_RECYCLING_CONTEXT(child)){
    return;
  }

  /* unset parent */
  pthread_mutex_lock(child->mutex);

  child->parent = NULL;

  pthread_mutex_unlock(child->mutex);

  /* remove child */
  pthread_mutex_lock(parent->mutex);

  parent->children = g_list_remove(parent->children,
				   child);

  pthread_mutex_unlock(parent->mutex);

  g_object_unref(G_OBJECT(parent));
  g_object_unref(G_OBJECT(child));
}

/**
 * ags_recycling_context_get_child_recall_id:
 * @recycling_context: the #AgsRecyclingContext
 * 
 * Retrieve all child recall ids.
 *
 * Returns: the #AgsRecallID as #GList
 *
 * Since: 1.0.0
 */
GList*
ags_recycling_context_get_child_recall_id(AgsRecyclingContext *recycling_context)
{
  GList *child_start, *child;

  GList *recall_id_list;

  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return(NULL);
  }

  pthread_mutex_lock(recycling_context->mutex);
  
  child =
    child_start = g_list_copy(recycling_context->children);
  
  pthread_mutex_unlock(recycling_context->mutex);

  recall_id_list = NULL;
  
  while(child != NULL){
    pthread_mutex_lock(AGS_RECYCLING_CONTEXT(child->data)->mutex);
    
    if(AGS_RECYCLING_CONTEXT(child->data)->recall_id != NULL){
      recall_id_list = g_list_append(recall_id_list,
				     AGS_RECYCLING_CONTEXT(child->data)->recall_id);
    }

    pthread_mutex_unlock(AGS_RECYCLING_CONTEXT(child->data)->mutex);
    
    child = child->next;
  }

  g_list_free(child_start);
  
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
 * Since: 1.0.0
 */
AgsRecyclingContext*
ags_recycling_context_reset_recycling(AgsRecyclingContext *recycling_context,
				      AgsRecycling *old_first_recycling, AgsRecycling *old_last_recycling,
				      AgsRecycling *new_first_recycling, AgsRecycling *new_last_recycling)
{
  AgsRecyclingContext *new_recycling_context, *parent;
  AgsRecycling *recycling;
  AgsRecycling *next, *prev;

  pthread_mutex_t *recycling_mutex;

  gint new_length;
  gint first_index, last_index;
  guint i;
  gboolean new_context;

  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return(NULL);
  }
  
  if(old_first_recycling != NULL){
    /* get recycling mutex */
    pthread_mutex_lock(application_mutex);

    recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) old_last_recycling);
  
    pthread_mutex_unlock(application_mutex);

    /* get some fields */
    pthread_mutex_lock(recycling_mutex);

    next = old_last_recycling->next;

    pthread_mutex_unlock(recycling_mutex);
    
    if(ags_recycling_position(old_first_recycling, next,
			      new_first_recycling) == -1){
      /* get recycling mutex */
      pthread_mutex_lock(application_mutex);

      recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
						 (GObject *) old_first_recycling);
  
      pthread_mutex_unlock(application_mutex);

      /* get some fields */
      pthread_mutex_lock(recycling_mutex);

      prev = old_first_recycling->prev;
      next = old_first_recycling->next;
      
      pthread_mutex_unlock(recycling_mutex);
      
      if(prev == new_last_recycling){
	new_context = FALSE;
      }else if(next == new_first_recycling){
	new_context = FALSE;
      }else{
	new_context = TRUE;
      }
    }else{
      new_context = FALSE;
    }
  }else{
    new_context = FALSE;
  }
  
  /* retrieve new length of recycling array */
  if(new_first_recycling != NULL){
    /* get recycling mutex */
    pthread_mutex_lock(application_mutex);

    recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) new_last_recycling);
  
    pthread_mutex_unlock(application_mutex);

    /* get some fields */
    pthread_mutex_lock(recycling_mutex);

    next = new_last_recycling->next;

    pthread_mutex_unlock(recycling_mutex);

    new_length = ags_recycling_position(new_first_recycling, next,
					new_last_recycling);
    new_length++;
  }else{
    return(NULL);
  }
  
  /* retrieve indices to replace */
  if(new_context){
    first_index = 0;
    last_index = 0;
  }else if(old_first_recycling != NULL){
    first_index = ags_recycling_context_find(recycling_context,
					     old_first_recycling);
    
    last_index = ags_recycling_context_find(recycling_context,
					    old_last_recycling);
  }else{
    AgsRecycling **recycling;
    AgsRecycling *start_recycling;
    guint64 length;
    
    pthread_mutex_lock(recycling_context->mutex);

    recycling = recycling_context->recycling;

    if(recycling != NULL){
      start_recycling = recycling[0];
    }else{
      start_recycling = NULL;
    }
    
    length = recycling_context->length;
    
    pthread_mutex_unlock(recycling_context->mutex);

    if(start_recycling != NULL){
      /* get recycling mutex */
      pthread_mutex_lock(application_mutex);

      recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
						 (GObject *) start_recycling);
  
      pthread_mutex_unlock(application_mutex);

      /* get some fields */
      pthread_mutex_lock(recycling_mutex);

      prev = start_recycling->prev;

      pthread_mutex_unlock(recycling_mutex);
    }else{
      prev = NULL;
    }
    
    if(recycling == NULL ||
       length == 0 ||
       prev == new_first_recycling){
      first_index = 0;
      last_index = 0;
    }else{
      AgsRecycling *end_recycling;
      
      pthread_mutex_lock(recycling_context->mutex);

      end_recycling = recycling[length - 1];
      
      pthread_mutex_unlock(recycling_context->mutex);

      /* get recycling mutex */
      pthread_mutex_lock(application_mutex);

      recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
						 (GObject *) end_recycling);
  
      pthread_mutex_unlock(application_mutex);

      /* get some fields */
      pthread_mutex_lock(recycling_mutex);

      next = end_recycling->next;

      pthread_mutex_unlock(recycling_mutex);

      first_index = ags_recycling_position(start_recycling, next,
					   new_first_recycling);

      last_index = first_index;
    }
  }
  
  /* instantiate */
  if(new_context){
    new_recycling_context = g_object_new(AGS_TYPE_RECYCLING_CONTEXT,
					 "length", new_length,
					 NULL);
  }else{
    guint64 length;
    
    pthread_mutex_lock(recycling_context->mutex);

    length = recycling_context->length;
    
    pthread_mutex_unlock(recycling_context->mutex);
    
    new_recycling_context = g_object_new(AGS_TYPE_RECYCLING_CONTEXT,
					 "length", (length -
						      (last_index - first_index) +
						      new_length),
					 NULL);
  }

  pthread_mutex_lock(recycling_context->mutex);

  parent = recycling_context->parent;
  
  pthread_mutex_unlock(recycling_context->mutex);

  //FIXME:JK: critical part - may be move at end
  if(parent != NULL){
    GList *list;
    
    pthread_mutex_lock(parent->mutex);
    
    list = g_list_find(parent->children,
		       recycling_context);
    list->data = new_recycling_context;

    pthread_mutex_unlock(parent->mutex);

    g_object_ref(new_recycling_context);
    
    
    pthread_mutex_lock(recycling_context->mutex);
    
    new_recycling_context->parent = recycling_context->parent;
    g_object_ref(new_recycling_context->parent);
    
    new_recycling_context->recall_id = recycling_context->recall_id;
    g_object_ref(recycling_context->recall_id);

    pthread_mutex_unlock(recycling_context->mutex);
  }
  
  pthread_mutex_lock(recycling_context->mutex);
  
  new_recycling_context->children = g_list_copy(recycling_context->children);
  
  pthread_mutex_unlock(recycling_context->mutex);
  
  /* copy heading */
  pthread_mutex_lock(recycling_context->mutex);

  if(!new_context){
    if(first_index > 0){
      memcpy(new_recycling_context->recycling,
	     recycling_context->recycling,
	     first_index * sizeof(AgsRecycling *));
    }
  }
  
  pthread_mutex_unlock(recycling_context->mutex);

  /* insert new */
  recycling = new_first_recycling;

  for(i = 0; i < new_length; i++){
    /* get recycling mutex */
    pthread_mutex_lock(application_mutex);

    recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) recycling);
  
    pthread_mutex_unlock(application_mutex);

    /* insert */
    ags_recycling_context_replace(new_recycling_context,
				  recycling,
				  first_index + i);

    /* iterate */
    pthread_mutex_lock(recycling_mutex);

    recycling = recycling->next;

    pthread_mutex_unlock(recycling_mutex);
  }

  /* copy trailing */
  pthread_mutex_lock(recycling_context->mutex);

  if(!new_context){
    if(new_recycling_context->length - first_index > 0){
      memcpy(&(new_recycling_context->recycling[first_index + new_length]),
	     &(recycling_context->recycling[first_index]),
	     (new_recycling_context->length - first_index - new_length) * sizeof(AgsRecycling *));
    }
  }

  pthread_mutex_unlock(recycling_context->mutex);

  /* ref count */
  for(i = 0; i < new_recycling_context->length; i++){
    g_object_ref(new_recycling_context->recycling[i]);
  }

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
 * Since: 1.0.0
 */
AgsRecyclingContext*
ags_recycling_context_new(guint64 length)
{
  AgsRecyclingContext *recycling_context;
  gint i;

  recycling_context = g_object_new(AGS_TYPE_RECYCLING_CONTEXT,
				   "length", length,
				   NULL);

  return(recycling_context);
}
