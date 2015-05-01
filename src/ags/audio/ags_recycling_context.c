/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/audio/ags_recycling_context.h>

#include <ags/audio/ags_recall_id.h>

#include <stdlib.h>

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
							"AgsRecyclingContext\0",
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

  gobject->finalize = ags_recycling_context_finalize;

  /* properties */
  /**
   * AgsRecyclingContext:parent:
   *
   * The parent recycling context within tree.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_object("parent\0",
				   "parent context\0",
				   "The context this one is packed into\0",
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
   * Since: 0.4.0
   */
  param_spec = g_param_spec_uint64("length\0",
				   "length of the array of assigned recycling\0",
				   "The recycling array length\0",
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
   * Since: 0.4.0
   */
  param_spec = g_param_spec_object("recall-id\0",
				   "the default recall id\0",
				   "The recall id located in audio object as destiny\0",
				   AGS_TYPE_RECALL_ID,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_ID,
				  param_spec);
}

void
ags_recycling_context_init(AgsRecyclingContext *recycling_context)
{
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

      if(recall_id == recycling_context->recall_id){
	return;
      }

      if(recycling_context->recall_id != NULL){
	g_object_unref(G_OBJECT(recycling_context->recall_id));
      }

      if(recall_id != NULL){
	g_object_ref(G_OBJECT(recall_id));
      }

      recycling_context->recall_id = recall_id;
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
ags_recycling_context_finalize(GObject *gobject)
{
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
 * Returns: the new recycling context
 *
 * Since: 0.4
 */
void
ags_recycling_context_replace(AgsRecyclingContext *recycling_context,
			      AgsRecycling *recycling,
			      gint position)
{
  if(recycling_context == NULL){
    return;
  }

  recycling_context->recycling[position] = recycling;
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
 * Since: 0.4
 */
AgsRecyclingContext*
ags_recycling_context_add(AgsRecyclingContext *recycling_context,
			  AgsRecycling *recycling)
{
  AgsRecyclingContext *new_recycling_context;
  gint new_length;

  if(recycling_context == NULL){
    return;
  }

  new_length = recycling_context->length + 1;
  new_recycling_context = (AgsRecyclingContext *) g_object_new(AGS_TYPE_RECYCLING_CONTEXT,
							       "length\0", new_length,
							       NULL);

  memcpy(new_recycling_context->recycling, recycling_context->recycling, new_length * sizeof(AgsRecycling *));
  new_recycling_context->recycling[new_length] = recycling;

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
 * Since: 0.4
 */
AgsRecyclingContext*
ags_recycling_context_remove(AgsRecyclingContext *recycling_context,
			     AgsRecycling *recycling)
{
  AgsRecyclingContext *new_recycling_context;
  gint new_length;
  gint position;

  new_recycling_context = (AgsRecyclingContext *) g_object_new(AGS_TYPE_RECYCLING_CONTEXT,
							       NULL);

  new_length = recycling_context->length - 1;

  new_recycling_context->recycling = (AgsRecycling **) malloc(new_length * sizeof(AgsRecycling *));

  for(position = 0; recycling_context->recycling[position] != recycling; position++);

  memcpy(new_recycling_context->recycling, recycling_context->recycling, (new_length - position) * sizeof(AgsRecycling *));
  memcpy(&(new_recycling_context->recycling[position + 1]), recycling_context->recycling, (-1 * (position - new_length)) * sizeof(AgsRecycling *));

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
 * Since: 0.4
 */
AgsRecyclingContext*
ags_recycling_context_insert(AgsRecyclingContext *recycling_context,
			     AgsRecycling *recycling,
			     gint position)
{
  AgsRecyclingContext *new_recycling_context;
  gint new_length;

  new_recycling_context = (AgsRecyclingContext *) g_object_new(AGS_TYPE_RECYCLING_CONTEXT,
							       NULL);

  new_length = recycling_context->length + 1;

  new_recycling_context->recycling = (AgsRecycling **) malloc(new_length * sizeof(AgsRecycling *));

  memcpy(new_recycling_context->recycling, recycling_context->recycling, (new_length - position) * sizeof(AgsRecycling *));
  new_recycling_context->recycling[position] = recycling;
  memcpy(&(new_recycling_context->recycling[position + 1]), recycling_context->recycling, (-1 * (position - new_length)) * sizeof(AgsRecycling *));

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
 * Since: 0.4
 */
AgsRecyclingContext*
ags_recycling_context_get_toplevel(AgsRecyclingContext *recycling_context)
{
  if(recycling_context == NULL){
    return(NULL);
  }

  while(recycling_context->parent != NULL){
    recycling_context = recycling_context->parent;
  }

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
 * Since: 0.4
 */
gint
ags_recycling_context_find(AgsRecyclingContext *recycling_context,
			   AgsRecycling *recycling)
{
  gint i;

  for(i = 0; i < recycling_context->length; i++){
    if(recycling_context->recycling[i] == recycling){
      return(i);
    }
  }

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
 * Since: 0.4
 */
gint
ags_recycling_context_find_child(AgsRecyclingContext *recycling_context,
				 AgsRecycling *recycling)
{
  GList *child;
  gint i;

  child = recycling_context->children;

  for(i = 0; child != NULL; i++){
    if(ags_recycling_context_find(AGS_RECYCLING_CONTEXT(child->data),
				  recycling) != -1){
      return(i);
    }

    child = child->next;
  }

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
 * Since: 0.4
 */
gint
ags_recycling_context_find_parent(AgsRecyclingContext *recycling_context,
				  AgsRecycling *recycling)
{
  gint i;

  for(i = 0; i < recycling_context->parent->length; i++){
    if(recycling_context->parent->recycling[i] == recycling){
      return(i);
    }
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
 * Since: 0.4
 */
void
ags_recycling_context_add_child(AgsRecyclingContext *parent,
				AgsRecyclingContext *child)
{
  if(parent == NULL || child == NULL){
    return;
  }

  g_object_ref(G_OBJECT(parent));
  g_object_ref(G_OBJECT(child));

  child->parent = parent;
  parent->children = g_list_append(parent->children,
				   child);
}

/**
 * ags_recycling_context_remove_child:
 * @parent: the #AgsRecyclingContext
 * @child: the child to remove
 *
 * Removes a recycling context of its parent.
 *
 * Since: 0.4
 */
void
ags_recycling_context_remove_child(AgsRecyclingContext *parent,
				   AgsRecyclingContext *child)
{
  if(parent == NULL || child == NULL){
    return;
  }

  child->parent = NULL;
  parent->children = g_list_remove(parent->children,
				   child);

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
 * Since: 0.4
 */
GList*
ags_recycling_context_get_child_recall_id(AgsRecyclingContext *recycling_context)
{
  GList *child;
  GList *recall_id_list;

  if(recycling_context == NULL){
    return(NULL);
  }

  child = recycling_context->children;
  recall_id_list = NULL;
  
  while(child != NULL){
    recall_id_list = g_list_append(recall_id_list,
				   AGS_RECYCLING_CONTEXT(child->data)->recall_id);

    child = child->next;
  }

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
 * Since: 0.4
 */
AgsRecyclingContext*
ags_recycling_context_reset_recycling(AgsRecyclingContext *recycling_context,
				      AgsRecycling *old_first_recycling, AgsRecycling *old_last_recycling,
				      AgsRecycling *new_first_recycling, AgsRecycling *new_last_recycling)
{
  AgsRecyclingContext *new_recycling_context;
  AgsRecycling *recycling;
  guint new_length;
  gint first_index, last_index;
  guint i;
  
  /* retrieve new length of recycling array */
  new_length = ags_recycling_position(new_first_recycling, new_last_recycling->next,
				      new_last_recycling);
  new_length++;
  
  /* retrieve indices to replace */
  if(old_first_recycling != NULL){
    first_index = ags_recycling_context_find(recycling_context,
					     old_first_recycling);

    last_index = ags_recycling_context_find(recycling_context,
					    old_last_recycling);
  }else{
    if(recycling_context->recycling == NULL ||
       recycling_context->length == 0 ||
       recycling_context->recycling[0]->prev == new_first_recycling){
      first_index = 0;
      last_index = 0;
    }else{
      first_index = ags_recycling_position(recycling_context->recycling[0], recycling_context->recycling[recycling_context->length - 1]->next,
					   new_first_recycling);

      last_index = first_index;
    }
  }
  
  /* instantiate */
  new_recycling_context = g_object_new(AGS_TYPE_RECYCLING_CONTEXT,
				       "length\0", (recycling_context->length -
						    (last_index - first_index) +
						    new_length),
				       NULL);

  new_recycling_context->children = g_list_copy(recycling_context->children);
  g_object_set(new_recycling_context,
	       "parent\0", recycling_context->parent,
	       NULL);
  g_object_set(recycling_context,
	       "parent\0", NULL,
	       NULL);

  
  /* copy heading */
  if(first_index > 0){
    memcpy(new_recycling_context->recycling,
	   recycling_context->recycling,
	   first_index * sizeof(AgsRecycling *));
  }
  
  /* insert new */
  recycling = new_first_recycling;

  for(i = 0; i < new_length; i++){
    ags_recycling_context_replace(new_recycling_context,
				  recycling,
				  first_index + i);
    recycling = recycling->next;
  }

  /* copy trailing */
  if(last_index + 1 != recycling_context->length){
    memcpy(&(new_recycling_context->recycling[first_index + new_length]),
	   recycling_context->recycling,
	   (new_recycling_context->length - first_index - new_length) * sizeof(AgsRecycling *));
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
 * Since: 0.4
 */
AgsRecyclingContext*
ags_recycling_context_new(gint length)
{
  AgsRecyclingContext *recycling_context;
  gint i;

  recycling_context = g_object_new(AGS_TYPE_RECYCLING_CONTEXT,
				   "length\0", length,
				   NULL);

  return(recycling_context);
}
