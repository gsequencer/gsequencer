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

#include "ags_expander_set.h"

#include <stdlib.h>

void ags_expander_set_class_init(AgsExpanderSetClass *expander_set);
void ags_expander_set_init(AgsExpanderSet *expander_set);
void ags_expander_set_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_expander_set_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_expander_set_finalize(GObject *gobject);
void ags_expander_set_show(GtkWidget *widget);

void ags_expander_set_draw(AgsExpanderSet *expander_set);
void ags_expander_set_insert_child(AgsExpanderSet *expander_set,
				   AgsExpanderSetChild *child,
				   gboolean ghost);
void ags_expander_set_remove_child(AgsExpanderSet *expander_set,
				   AgsExpanderSetChild *child,
				   gboolean ghost);

/**
 * SECTION:ags_expander_set
 * @short_description: A container
 * @title: AgsExpanderSet
 * @section_id:
 * @include: ags/widget/ags_expander_set.h
 *
 * #AgsExpanderSet is a composite widget containing a #GtkGrid.
 */

enum{
  PROP_0,
  PROP_GHOST,
  PROP_LOCATION,
};

static gpointer ags_expander_set_parent_class = NULL;

GType
ags_expander_set_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_expander_set = 0;

    static const GTypeInfo ags_expander_set_info = {
      sizeof(AgsExpanderSetClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_expander_set_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsExpanderSet),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_expander_set_init,
    };

    ags_type_expander_set = g_type_register_static(GTK_TYPE_GRID,
						   "AgsExpanderSet", &ags_expander_set_info,
						   0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_expander_set);
  }

  return g_define_type_id__volatile;
}

void
ags_expander_set_class_init(AgsExpanderSetClass *expander_set)
{
  GObjectClass *gobject;
  //  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_expander_set_parent_class = g_type_class_peek_parent(expander_set);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(expander_set);

  gobject->set_property = ags_expander_set_set_property;
  gobject->get_property = ags_expander_set_get_property;

  gobject->finalize = ags_expander_set_finalize;

  /* GtkWidgetClass */
  //  widget = (GtkWidgetClass *) expander_set;
}

void
ags_expander_set_init(AgsExpanderSet *expander_set)
{
  g_object_set(G_OBJECT(expander_set),
	       "row-homogeneous", FALSE,
	       "column-homogeneous", FALSE,
	       NULL);

  expander_set->flags = 0;

  expander_set->ghost = NULL;
  expander_set->location = NULL;
}

void
ags_expander_set_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsExpanderSet *expander_set;

  expander_set = AGS_EXPANDER_SET(gobject);

  switch(prop_id){
  case PROP_GHOST:
    {
      AgsExpanderSetChild *ghost;

      ghost = (AgsExpanderSetChild *) g_value_get_pointer(value);

      ags_expander_set_insert_child(expander_set,
				    ghost,
				    TRUE);
    }
    break;
  case PROP_LOCATION:
    {
      AgsExpanderSetChild *location;

      location = (AgsExpanderSetChild *) g_value_get_pointer(value);

      ags_expander_set_insert_child(expander_set,
				    location,
				    FALSE);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_expander_set_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsExpanderSet *expander_set;

  expander_set = AGS_EXPANDER_SET(gobject);

  switch(prop_id){
  case PROP_GHOST:
    g_value_set_pointer(value, expander_set->ghost);
    break;
  case PROP_LOCATION:
    g_value_set_pointer(value, expander_set->location);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_expander_set_finalize(GObject *gobject)
{
  AgsExpanderSet *expander_set;

  expander_set = AGS_EXPANDER_SET(gobject);

  if(expander_set->ghost != NULL){
    g_list_free_full(expander_set->ghost,
		     g_free);
  }

  if(expander_set->location != NULL){
    g_list_free_full(expander_set->location,
		     g_free);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_expander_set_parent_class)->finalize(gobject);
}

/**
 * ags_expander_set_child_alloc:
 * @x: the x position
 * @y: the y position
 * @width: the width
 * @height: the height
 * 
 * Allocate #AgsExpanderSetChild-struct.
 * 
 * Returns: (type gpointer) (transfer full): the new #AgsExpanderSetChild-struct
 * 
 * Since: 3.0.0
 */
AgsExpanderSetChild*
ags_expander_set_child_alloc(guint x, guint y,
			     guint width, guint height)
{
  AgsExpanderSetChild *child;

  child = (AgsExpanderSetChild *) g_malloc(sizeof(AgsExpanderSetChild));

  child->x = x;
  child->y = y;
  child->width = width;
  child->height = height;

  return(child);
}

/**
 * ags_expander_set_child_free:
 * @expander_set_child: (type gpointer) (transfer full): the #AgsExpanderSetChild-struct
 * 
 * Free @expander_set_child.
 * 
 * Since: 3.2.2
 */
void
ags_expander_set_child_free(AgsExpanderSetChild *expander_set_child)
{
  if(expander_set_child == NULL){
    return;
  }

  g_free(expander_set_child);
}

/**
 * ags_expander_set_child_find:
 * @expander_set: the #AgsExpanderSet
 * @child: the #GtkWidget
 * 
 * Find @child of @expander_set.
 * 
 * Returns: (type gpointer) (transfer none): the matching #AgsExpanderSetChild-struct
 * 
 * Since: 3.0.0
 */
AgsExpanderSetChild*
ags_expander_set_child_find(AgsExpanderSet *expander_set,
			    GtkWidget *child)
{
  GList *list;

  if(!AGS_IS_EXPANDER_SET(expander_set) ||
     !GTK_IS_WIDGET(child)){
    return(NULL);
  }
  
  list = expander_set->location;

  while(list != NULL){
    if(AGS_EXPANDER_SET_CHILD(list->data)->child == child){
      return((AgsExpanderSetChild *) list->data);
    }

    list = list->next;
  }

  return(NULL);
}

void
ags_expander_set_draw(AgsExpanderSet *expander_set)
{
  //TODO:JK: implement me
}

void
ags_expander_set_insert_child(AgsExpanderSet *expander_set,
			      AgsExpanderSetChild *child,
			      gboolean ghost)
{
  GList *list;

  guint i;

  if(!AGS_IS_EXPANDER_SET(expander_set) ||
     child == NULL){
    return;
  }
  
  if(ghost){
    list = expander_set->ghost;
  }else{
    list = expander_set->location;
  }

  i = 0;
  
  while(list != NULL){
    if(AGS_EXPANDER_SET_CHILD(list->data)->x > child->x){
      break;
    }

    if(AGS_EXPANDER_SET_CHILD(list->data)->x == child->x){
      while(list != NULL){
	if(AGS_EXPANDER_SET_CHILD(list->data)->x > child->x){
	  break;
	}

	if(AGS_EXPANDER_SET_CHILD(list->data)->x == child->x &&
	   AGS_EXPANDER_SET_CHILD(list->data)->y >= child->y){
	  break;
	}
	
	i++;
	list = list->next;
      }

      break;
    }

    i++;
    list = list->next;
  }

  gtk_grid_attach((GtkGrid *) expander_set,
		  child->child,
		  child->x, child->y,
		  child->width, child->height);
  
  gtk_widget_set_vexpand(child->child,
			 TRUE);
  gtk_widget_set_hexpand(child->child,
			 TRUE);

  gtk_widget_set_valign(child->child,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign(child->child,
			GTK_ALIGN_FILL);
  
  if(ghost){
    expander_set->ghost = g_list_insert(expander_set->ghost,
					child,
					i);
  }else{
    expander_set->location = g_list_insert(expander_set->location,
					   child,
					   i);
  }
}

void
ags_expander_set_remove_child(AgsExpanderSet *expander_set,
			      AgsExpanderSetChild *child,
			      gboolean ghost)
{
  if(!AGS_IS_EXPANDER_SET(expander_set) ||
     child == NULL){
    return;
  }

  gtk_grid_remove((GtkGrid *) expander_set,
		  child->child);

  if(ghost){
    expander_set->ghost = g_list_remove(expander_set->ghost,
					child);
  }else{
    expander_set->location = g_list_remove(expander_set->location,
					   child);
  }

  ags_expander_set_child_free(child);
}

/**
 * ags_expander_set_add:
 * @expander_set: the #AgsExpanderSet
 * @widget: the child #GtkWidget
 * @x: x-position
 * @y: y-position
 * @width: allocation width
 * @height: allocation height
 *
 * Adds a #GtkWidget to #AgsExpanderSet
 *
 * Since: 3.0.0
 */
void
ags_expander_set_add(AgsExpanderSet *expander_set,
		     GtkWidget *widget,
		     guint x, guint y,
		     guint width, guint height)
{
  AgsExpanderSetChild *child;

  if(!AGS_IS_EXPANDER_SET(expander_set) ||
     !GTK_IS_WIDGET(widget)){
    return;
  }

  child = ags_expander_set_child_alloc(x, y,
				       width, height);
  child->child = widget;
    
  if(widget != NULL){
    ags_expander_set_insert_child(expander_set,
				  child,
				  FALSE);
  }else{
    ags_expander_set_insert_child(expander_set,
				  child,
				  TRUE);
  }
}

/**
 * ags_expander_set_remove:
 * @expander_set: the #AgsExpanderSet
 * @widget: the child #GtkWidget
 *
 * Removes a #GtkWidget of #AgsExpanderSet
 *
 * Since: 3.0.0
 */
void
ags_expander_set_remove(AgsExpanderSet *expander_set,
			GtkWidget *widget)
{
  if(!AGS_IS_EXPANDER_SET(expander_set) ||
     !GTK_IS_WIDGET(widget)){
    return;
  }

  ags_expander_set_remove_child(expander_set,
				ags_expander_set_child_find(expander_set,
							    widget),
				FALSE);
}

/**
 * ags_expander_set_new:
 *
 * Creates an #AgsExpanderSet
 *
 * Returns: a new #AgsExpanderSet
 *
 * Since: 3.0.0
 */
AgsExpanderSet*
ags_expander_set_new()
{
  AgsExpanderSet *expander_set;

  expander_set = (AgsExpanderSet *) g_object_new(AGS_TYPE_EXPANDER_SET,
						 NULL);
  
  return(expander_set);
}
