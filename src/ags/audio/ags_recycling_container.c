/* This file is part of GSequencer.
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

#include <ags/audio/ags_recycling_container.h>

#include <ags/audio/ags_recall_id.h>

#include <stdlib.h>
#include <string.h>

void ags_recycling_container_class_init(AgsRecyclingContainerClass *recycling_container_class);
void ags_recycling_container_init(AgsRecyclingContainer *recycling_container);
void ags_recycling_container_set_property(GObject *gobject,
					  guint prop_id,
					  const GValue *value,
					  GParamSpec *param_spec);
void ags_recycling_container_get_property(GObject *gobject,
					  guint prop_id,
					  GValue *value,
					  GParamSpec *param_spec);
void ags_recycling_container_finalize(GObject *gobject);

/**
 * SECTION:ags_recycling_container
 * @short_description: A container of recycling acting as dynamic context.
 * @title: AgsRecyclingContainer
 * @section_id:
 * @include: ags/audio/ags_recycling_container.h
 *
 * #AgsRecyclingContainer organizes #AgsRecycling objects as dynamic context
 * within nested tree.
 */

enum{
  PROP_0,
  PROP_PARENT,
  PROP_LENGTH,
  PROP_RECALL_ID,
};

static gpointer ags_recycling_container_parent_class = NULL;

GType
ags_recycling_container_get_type (void)
{
  static GType ags_type_recycling_container = 0;

  if(!ags_type_recycling_container){
    static const GTypeInfo ags_recycling_container_info = {
      sizeof (AgsRecyclingContainerClass),
      (GBaseInitFunc) NULL, /* base_init */
      (GBaseFinalizeFunc) NULL, /* base_finalize */
      (GClassInitFunc) ags_recycling_container_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecyclingContainer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recycling_container_init,
    };

    ags_type_recycling_container = g_type_register_static(G_TYPE_OBJECT,
							  "AgsRecyclingContainer\0",
							  &ags_recycling_container_info,
							  0);
  }

  return (ags_type_recycling_container);
}

void
ags_recycling_container_class_init(AgsRecyclingContainerClass *recycling_container)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_recycling_container_parent_class = g_type_class_peek_parent(recycling_container);
  
  gobject = (GObjectClass *) recycling_container;

  gobject->set_property = ags_recycling_container_set_property;
  gobject->get_property = ags_recycling_container_get_property;

  gobject->finalize = ags_recycling_container_finalize;

  /* properties */
  /**
   * AgsRecyclingContainer:parent:
   *
   * The parent recycling container within tree.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_object("parent\0",
				   "parent container\0",
				   "The container this one is packed into\0",
				   AGS_TYPE_RECYCLING_CONTAINER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PARENT,
				  param_spec);

  /**
   * AgsRecyclingContainer:length:
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
   * AgsRecyclingContainer:recall-id:
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
ags_recycling_container_init(AgsRecyclingContainer *recycling_container)
{
  recycling_container->recall_id = NULL;

  recycling_container->recycling = NULL;
  recycling_container->length = 0;

  recycling_container->parent = NULL;
  recycling_container->children = NULL;
}

void
ags_recycling_container_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec)
{
  AgsRecyclingContainer *recycling_container;
  guint i;

  recycling_container = AGS_RECYCLING_CONTAINER(gobject);

  switch(prop_id){
  case PROP_PARENT:
    {
      AgsRecyclingContainer *parent;

      parent = (AgsRecyclingContainer *) g_value_get_object(value);

      if(recycling_container->parent == parent){
	return;
      }
      
      if(recycling_container->parent != NULL){
	ags_recycling_container_remove_child(recycling_container->parent,
					     recycling_container);
      }

      if(parent != NULL){
	ags_recycling_container_add_child(parent,
					  recycling_container);
      }

      recycling_container->parent = parent;
    }
    break;
  case PROP_LENGTH:
    {
      guint64 length;

      length = g_value_get_uint64(value);

      recycling_container->recycling = (AgsRecycling **) malloc(length * sizeof(AgsRecycling *));
      recycling_container->length = length;

      for(i = 0; i < length; i++){
	recycling_container->recycling[i] = NULL;
      }
    }
    break;
  case PROP_RECALL_ID:
    {
      AgsRecallID *recall_id;

      recall_id = (AgsRecallID *) g_value_get_object(value);

      if((AgsRecallID *) recall_id == recycling_container->recall_id){
	return;
      }

      if(recycling_container->recall_id != NULL){
	g_object_unref(G_OBJECT(recycling_container->recall_id));
      }

      if(recall_id != NULL){
	g_object_ref(G_OBJECT(recall_id));
      }

      recycling_container->recall_id = recall_id;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recycling_container_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec)
{
  AgsRecyclingContainer *recycling_container;

  recycling_container = AGS_RECYCLING_CONTAINER(gobject);

  switch(prop_id){
  case PROP_PARENT:
    {
      g_value_set_object(value, recycling_container->parent);
    }
    break;
  case PROP_LENGTH:
    {
      g_value_set_uint64(value, recycling_container->length);
    }
    break;
  case PROP_RECALL_ID:
    {
      g_value_set_object(value, recycling_container->recall_id);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recycling_container_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_recycling_container_parent_class)->finalize(gobject);
}

/**
 * ags_recycling_container_replace:
 * @recycling_container: the #AgsRecyclingContainer
 * @recycling: the #AgsRecycling to add
 * @position: the index of @recycling
 *
 * Replaces one recycling entry in a container.
 *
 * Returns: the new recycling container
 *
 * Since: 0.4
 */
void
ags_recycling_container_replace(AgsRecyclingContainer *recycling_container,
				AgsRecycling *recycling,
				gint position)
{
  if(recycling_container == NULL ||
     position >= recycling_container->length){
    return;
  }

  recycling_container->recycling[position] = recycling;
}

/**
 * ags_recycling_container_add:
 * @recycling_container: the #AgsRecyclingContainer
 * @recycling: the #AgsRecycling to add
 *
 * Adds a recycling to a container.
 *
 * Returns: the new recycling container
 *
 * Since: 0.4
 */
AgsRecyclingContainer*
ags_recycling_container_add(AgsRecyclingContainer *recycling_container,
			    AgsRecycling *recycling)
{
  AgsRecyclingContainer *new_recycling_container;
  gint new_length;

  if(recycling_container == NULL){
    return;
  }

  new_length = recycling_container->length + 1;
  new_recycling_container = (AgsRecyclingContainer *) g_object_new(AGS_TYPE_RECYCLING_CONTAINER,
								   "length\0", new_length,
								   NULL);

  memcpy(new_recycling_container->recycling, recycling_container->recycling, new_length * sizeof(AgsRecycling *));
  new_recycling_container->recycling[new_length] = recycling;

  return(new_recycling_container);
}

/**
 * ags_recycling_container_remove:
 * @recycling_container: the #AgsRecyclingContainer
 * @recycling: the #AgsRecycling to remove
 *
 * Removes a recycling in a container.
 *
 * Returns: the new recycling container
 *
 * Since: 0.4
 */
AgsRecyclingContainer*
ags_recycling_container_remove(AgsRecyclingContainer *recycling_container,
			       AgsRecycling *recycling)
{
  AgsRecyclingContainer *new_recycling_container;
  gint new_length;
  gint position;

  new_recycling_container = (AgsRecyclingContainer *) g_object_new(AGS_TYPE_RECYCLING_CONTAINER,
								   NULL);

  new_length = recycling_container->length - 1;

  new_recycling_container->recycling = (AgsRecycling **) malloc(new_length * sizeof(AgsRecycling *));

  for(position = 0; recycling_container->recycling[position] != recycling; position++);

  memcpy(new_recycling_container->recycling, recycling_container->recycling, (new_length - position) * sizeof(AgsRecycling *));
  memcpy(&(new_recycling_container->recycling[position + 1]), recycling_container->recycling, (-1 * (position - new_length)) * sizeof(AgsRecycling *));

  return(new_recycling_container);
}

/**
 * ags_recycling_container_insert:
 * @recycling_container: the #AgsRecyclingContainer
 * @recycling: the #AgsRecycling to insert
 * @position: the index to insert at
 *
 * Inserts a recycling to a container.
 *
 * Returns: the new recycling container
 *
 * Since: 0.4
 */
AgsRecyclingContainer*
ags_recycling_container_insert(AgsRecyclingContainer *recycling_container,
			       AgsRecycling *recycling,
			       gint position)
{
  AgsRecyclingContainer *new_recycling_container;
  gint new_length;

  new_recycling_container = (AgsRecyclingContainer *) g_object_new(AGS_TYPE_RECYCLING_CONTAINER,
								   NULL);

  new_length = recycling_container->length + 1;

  new_recycling_container->recycling = (AgsRecycling **) malloc(new_length * sizeof(AgsRecycling *));

  memcpy(new_recycling_container->recycling, recycling_container->recycling, (new_length - position) * sizeof(AgsRecycling *));
  new_recycling_container->recycling[position] = recycling;
  memcpy(&(new_recycling_container->recycling[position + 1]), recycling_container->recycling, (-1 * (position - new_length)) * sizeof(AgsRecycling *));

  return(new_recycling_container);
}

/**
 * ags_recycling_container_get_toplevel:
 * @recycling_container: the #AgsRecyclingContainer
 *
 * Iterates the tree up to highest level.
 *
 * Returns: the topmost recycling container
 *
 * Since: 0.4
 */
AgsRecyclingContainer*
ags_recycling_container_get_toplevel(AgsRecyclingContainer *recycling_container)
{
  if(recycling_container == NULL){
    return(NULL);
  }

  while(recycling_container->parent != NULL){
    recycling_container = recycling_container->parent;
  }

  return(recycling_container);
}

/**
 * ags_recycling_container_find:
 * @recycling_container: the #AgsRecyclingContainer
 * @recycling: the #AgsRecycling to look up
 * 
 * Find position of recycling within array.
 *
 * Returns: recycling array index
 *
 * Since: 0.4
 */
gint
ags_recycling_container_find(AgsRecyclingContainer *recycling_container,
			     AgsRecycling *recycling)
{
  gint i;

  for(i = 0; i < recycling_container->length; i++){
    if(recycling_container->recycling[i] == recycling){
      return(i);
    }
  }

  return(-1);
}

/**
 * ags_recycling_container_find_child:
 * @recycling_container: the #AgsRecyclingContainer
 * @recycling: the #AgsRecycling to look up
 * 
 * Find position of recycling within arrays.
 *
 * Returns: recycling array index
 *
 * Since: 0.4
 */
gint
ags_recycling_container_find_child(AgsRecyclingContainer *recycling_container,
				   AgsRecycling *recycling)
{
  GList *child;
  gint i;

  child = recycling_container->children;

  for(i = 0; child != NULL; i++){
    if(ags_recycling_container_find(AGS_RECYCLING_CONTAINER(child->data),
				    recycling) != -1){
      return(i);
    }

    child = child->next;
  }

  return(-1);
}

/**
 * ags_recycling_container_find_parent:
 * @recycling_container: the #AgsRecyclingContainer
 * @recycling: the #AgsRecycling to look up
 * 
 * Find position of recycling within array.
 *
 * Returns: recycling array index
 *
 * Since: 0.4
 */
gint
ags_recycling_container_find_parent(AgsRecyclingContainer *recycling_container,
				    AgsRecycling *recycling)
{
  gint i;

  for(i = 0; i < recycling_container->parent->length; i++){
    if(recycling_container->parent->recycling[i] == recycling){
      return(i);
    }
  }

  return(-1);
}

/**
 * ags_recycling_container_add_child:
 * @parent: the parental #AgsRecyclingContainer
 * @child: the child
 *
 * Adds a recycling container as child.
 *
 * Since: 0.4
 */
void
ags_recycling_container_add_child(AgsRecyclingContainer *parent,
				  AgsRecyclingContainer *child)
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
 * ags_recycling_container_remove_child:
 * @parent: the #AgsRecyclingContainer
 * @child: the child to remove
 *
 * Removes a recycling container of its parent.
 *
 * Since: 0.4
 */
void
ags_recycling_container_remove_child(AgsRecyclingContainer *parent,
				     AgsRecyclingContainer *child)
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
 * ags_recycling_container_get_child_recall_id:
 * @recycling_container: the #AgsRecyclingContainer
 * 
 * Retrieve all child recall ids.
 *
 * Returns: the #AgsRecallID as #GList
 *
 * Since: 0.4
 */
GList*
ags_recycling_container_get_child_recall_id(AgsRecyclingContainer *recycling_container)
{
  GList *child;
  GList *recall_id_list;

  if(recycling_container == NULL){
    return(NULL);
  }

  child = recycling_container->children;
  recall_id_list = NULL;
  
  while(child != NULL){
    if(AGS_RECYCLING_CONTAINER(child->data)->recall_id != NULL){
      recall_id_list = g_list_append(recall_id_list,
				     AGS_RECYCLING_CONTAINER(child->data)->recall_id);
    }
    
    child = child->next;
  }

  return(recall_id_list);
}

/**
 * ags_recycling_container_reset_recycling:
 * @recycling_container: the #AgsRecyclingContainer
 * @old_first_recycling: the first recycling to replace
 * @old_last_recycling: the last recycling to replace
 * @new_first_recycling: the first recycling to insert
 * @new_last_recycling: the last recycling to insert
 *
 * Modify recycling of container.
 *
 * Since: 0.4
 */
AgsRecyclingContainer*
ags_recycling_container_reset_recycling(AgsRecyclingContainer *recycling_container,
					AgsRecycling *old_first_recycling, AgsRecycling *old_last_recycling,
					AgsRecycling *new_first_recycling, AgsRecycling *new_last_recycling)
{
  AgsRecyclingContainer *new_recycling_container;
  AgsRecycling *recycling;
  gint new_length;
  gint first_index, last_index;
  guint i;
  gboolean new_context;

  if(old_first_recycling != NULL){
    if(ags_recycling_position(old_first_recycling, old_last_recycling->next,
			      new_first_recycling) == -1){
      if(old_first_recycling->prev == new_last_recycling){
	new_context = FALSE;
      }else if(old_first_recycling->next == new_first_recycling){
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
  new_length = ags_recycling_position(new_first_recycling, new_last_recycling->next,
				      new_last_recycling);
  new_length++;
  
  /* retrieve indices to replace */
  if(new_context){
    first_index = 0;
    last_index = 0;
  }else if(old_first_recycling != NULL){
    first_index = ags_recycling_container_find(recycling_container,
					       old_first_recycling);
    
    last_index = ags_recycling_container_find(recycling_container,
					      old_last_recycling);
  }else{
    if(recycling_container->recycling == NULL ||
       recycling_container->length == 0 ||
       recycling_container->recycling[0]->prev == new_first_recycling){
      first_index = 0;
      last_index = 0;
    }else{
      first_index = ags_recycling_position(recycling_container->recycling[0], recycling_container->recycling[recycling_container->length - 1]->next,
					   new_first_recycling);

      last_index = first_index;
    }
  }
  
  /* instantiate */
  if(new_context){
    new_recycling_container = g_object_new(AGS_TYPE_RECYCLING_CONTAINER,
					   "length\0", new_length,
					   NULL);
  }else{
    new_recycling_container = g_object_new(AGS_TYPE_RECYCLING_CONTAINER,
					   "length\0", (recycling_container->length -
							(last_index - first_index) +
							new_length),
					   NULL);
  }

  if(recycling_container->parent != NULL){
    GList *list;
    
    list = g_list_find(recycling_container->parent->children,
		       recycling_container);
    list->data = new_recycling_container;
    g_object_ref(new_recycling_container);
    
    new_recycling_container->parent = recycling_container->parent;
    g_object_ref(new_recycling_container->parent);
    
    new_recycling_container->recall_id = recycling_container->recall_id;
    g_object_ref(recycling_container->recall_id);
  }
  
  new_recycling_container->children = g_list_copy(recycling_container->children);
  
  /* copy heading */
  if(!new_context){
    if(first_index > 0){
      memcpy(new_recycling_container->recycling,
	     recycling_container->recycling,
	     first_index * sizeof(AgsRecycling *));
    }
  }
  
  /* insert new */
  recycling = new_first_recycling;

  for(i = 0; i < new_length; i++){
    ags_recycling_container_replace(new_recycling_container,
				    recycling,
				    first_index + i);
    recycling = recycling->next;
  }

  /* copy trailing */
  if(!new_context){
    if(new_recycling_container->length - first_index > 0){
      memcpy(&(new_recycling_container->recycling[first_index + new_length]),
	     &(recycling_container->recycling[first_index]),
	     (new_recycling_container->length - first_index - new_length) * sizeof(AgsRecycling *));
    }
  }

  return(new_recycling_container);
}

/**
 * ags_recycling_container_new:
 * @length: array dimension of context
 *
 * Creates a #AgsRecyclingContainer, boundaries are specified by @length
 *
 * Returns: a new #AgsRecyclingContainer
 *
 * Since: 0.4
 */
AgsRecyclingContainer*
ags_recycling_container_new(gint length)
{
  AgsRecyclingContainer *recycling_container;
  gint i;

  recycling_container = g_object_new(AGS_TYPE_RECYCLING_CONTAINER,
				     "length\0", length,
				     NULL);

  return(recycling_container);
}
