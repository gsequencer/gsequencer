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

#include "ags_expander.h"

#include <stdlib.h>

void ags_expander_class_init(AgsExpanderClass *expander);
void ags_expander_init(AgsExpander *expander);
void ags_expander_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec);
void ags_expander_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec);
void ags_expander_finalize(GObject *gobject);
void ags_expander_show(GtkWidget *widget);

void ags_expander_activate_callback(GtkExpander *expander,
				    AgsExpander *expander_with_grid);

void ags_expander_insert_child(AgsExpander *expander,
			       AgsExpanderChild *child);
void ags_expander_remove_child(AgsExpander *expander,
			       AgsExpanderChild *child);

/**
 * SECTION:ags_expander
 * @short_description: A expander widget
 * @title: AgsExpander
 * @section_id:
 * @include: ags/widget/ags_expander.h
 *
 * #AgsExpander is a composite widget containing a #GtkGrid. It
 * is mainly used to layout childs in rows and columns that can
 * be expanded.
 */

enum{
  PROP_0,
  PROP_CHILD,
};

static gpointer ags_expander_parent_class = NULL;

GType
ags_expander_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_expander = 0;

    static const GTypeInfo ags_expander_info = {
      sizeof(AgsExpanderClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_expander_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsExpander),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_expander_init,
    };

    ags_type_expander = g_type_register_static(GTK_TYPE_BOX,
					       "AgsExpander", &ags_expander_info,
					       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_expander);
  }

  return g_define_type_id__volatile;
}

void
ags_expander_class_init(AgsExpanderClass *expander)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  GParamSpec *param_spec;

  ags_expander_parent_class = g_type_class_peek_parent(expander);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(expander);

#if 0
  gobject->set_property = ags_expander_set_property;
  gobject->get_property = ags_expander_get_property;
#endif
  
  gobject->finalize = ags_expander_finalize;

  /* properties */
#if 0
  param_spec = g_param_spec_object("child",
				   "child of expander",
				   "The child which this expander is packed into",
				   GTK_TYPE_WIDGET,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHILD,
				  param_spec);
#endif
  
  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) expander;
}

void
ags_expander_init(AgsExpander *expander)
{
  expander->flags = 0;

  expander->expander = gtk_expander_new("pad");
  gtk_box_append(expander,
		 expander->expander);

  gtk_widget_set_vexpand(expander->expander,
			 TRUE);  
  gtk_widget_set_hexpand(expander->expander,
			 TRUE);  

  gtk_widget_set_valign(expander->expander,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign(expander->expander,
			GTK_ALIGN_FILL);
  
  gtk_widget_show(expander->expander);
  
  expander->grid = (GtkGrid *) gtk_grid_new();
  gtk_expander_set_child(expander->expander,
			 expander->grid);  
  
  gtk_widget_set_vexpand(expander->grid,
			 TRUE);  
  gtk_widget_set_hexpand(expander->grid,
			 TRUE);  

  gtk_widget_set_valign(expander->grid,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign(expander->grid,
			GTK_ALIGN_FILL);

  gtk_widget_show(expander->grid);

  expander->children = NULL;

  g_signal_connect_after(expander->expander, "activate",
			 G_CALLBACK(ags_expander_activate_callback), expander);
}

void
ags_expander_set_property(GObject *gobject,
			  guint prop_id,
			  const GValue *value,
			  GParamSpec *param_spec)
{
  AgsExpander *expander;

  expander = AGS_EXPANDER(gobject);

  switch(prop_id){
  case PROP_CHILD:
    {
      AgsExpanderChild *child;

      child = (AgsExpanderChild *) g_value_get_pointer(value);

      ags_expander_insert_child(expander,
				child);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_expander_get_property(GObject *gobject,
			  guint prop_id,
			  GValue *value,
			  GParamSpec *param_spec)
{
  AgsExpander *expander;

  expander = AGS_EXPANDER(gobject);

  switch(prop_id){
  case PROP_CHILD:
  {
    g_value_set_pointer(value, expander->children);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_expander_finalize(GObject *gobject)
{
  AgsExpander *expander;

  expander = AGS_EXPANDER(gobject);

  if(expander->children != NULL){
    g_list_free_full(expander->children,
		     g_free);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_expander_parent_class)->finalize(gobject);
}

void
ags_expander_activate_callback(GtkExpander *expander,
			       AgsExpander *expander_with_grid)
{  
  if(gtk_expander_get_expanded(expander_with_grid->expander)){
    gtk_widget_show(GTK_WIDGET(expander_with_grid->grid));
  }else{
    gtk_widget_hide(GTK_WIDGET(expander_with_grid->grid));
  }
}

/**
 * ags_expander_child_alloc:
 * @x: the x position
 * @y: the y position
 * @width: the width
 * @height: the height
 * 
 * Allocate #AgsExpanderChild-struct.
 * 
 * Returns: (type gpointer) (transfer full): the new #AgsExpanderChild-struct
 * 
 * Since: 3.0.0
 */
AgsExpanderChild*
ags_expander_child_alloc(guint x, guint y,
			 guint width, guint height)
{
  AgsExpanderChild *child;

  child = (AgsExpanderChild *) g_malloc(sizeof(AgsExpanderChild));

  child->x = x;
  child->y = y;
  child->width = width;
  child->height = height;

  return(child);
}

/**
 * ags_expander_child_free:
 * @expander_child: (type gpointer) (transfer full): the #AgsExpanderChild-struct
 * 
 * Free @expander_child.
 * 
 * Since: 3.2.2
 */
void
ags_expander_child_free(AgsExpanderChild *expander_child)
{
  if(expander_child == NULL){
    return;
  }

  g_free(expander_child);
}

/**
 * ags_expander_child_find:
 * @expander: the #AgsExpander
 * @child: the #GtkWidget
 * 
 * Find @child of @expander.
 * 
 * Returns: (type gpointer) (transfer none): the matching #AgsExpanderChild-struct
 * 
 * Since: 3.0.0
 */
AgsExpanderChild*
ags_expander_child_find(AgsExpander *expander,
			GtkWidget *child)
{
  GList *list;

  if(!AGS_IS_EXPANDER(expander) ||
     !GTK_IS_WIDGET(child)){
    return(NULL);
  }
  
  list = expander->children;

  while(list != NULL){
    if(AGS_EXPANDER_CHILD(list->data)->child == child){
      return((AgsExpanderChild *) list->data);
    }

    list = list->next;
  }

  return(NULL);
}

void
ags_expander_insert_child(AgsExpander *expander,
			  AgsExpanderChild *child)
{
  GList *list;
  guint i;

  if(!AGS_IS_EXPANDER(expander) ||
     child == NULL){
    return;
  }
  
  list = expander->children;

  i = 0;
  
  while(list != NULL){
    if(AGS_EXPANDER_CHILD(list->data)->x > child->x){
      break;
    }

    if(AGS_EXPANDER_CHILD(list->data)->x == child->x){
      while(list != NULL){
	if(AGS_EXPANDER_CHILD(list->data)->x > child->x){
	  break;
	}

	if(AGS_EXPANDER_CHILD(list->data)->x == child->x &&
	   AGS_EXPANDER_CHILD(list->data)->y >= child->y){
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

  gtk_grid_attach(expander->grid,
		  child->child,
		  child->x, child->y,
		  child->width, child->height);
  
  expander->children = g_list_insert(expander->children,
				     child,
				     i);
}

void
ags_expander_remove_child(AgsExpander *expander,
			  AgsExpanderChild *child)
{
  if(!AGS_IS_EXPANDER(expander) ||
     child == NULL){
    return;
  }

  gtk_grid_remove(expander->grid,
		  child->child);

  expander->children = g_list_remove(expander->children,
				     child);

  ags_expander_child_free(child);
}

/**
 * ags_expander_add:
 * @expander: the #AgsExpander
 * @widget: the child #GtkWidget
 * @x: x-position
 * @y: y-position
 * @width: allocation width
 * @height: allocation height
 *
 * Adds a #GtkWidget to #AgsExpander
 *
 * Since: 3.0.0
 */
void
ags_expander_add(AgsExpander *expander,
		 GtkWidget *widget,
		 guint x, guint y,
		 guint width, guint height)
{
  AgsExpanderChild *child;

  if(!AGS_IS_EXPANDER(expander) ||
     !GTK_IS_WIDGET(widget)){
    return;
  }

  if(ags_expander_child_find(expander,
			     widget) != NULL){
    g_warning("widget already packed by expander");

    return;
  }

  child = ags_expander_child_alloc(x, y,
				   width, height);
  child->child = widget;
    
  ags_expander_insert_child(expander,
			    child);

  if(gtk_widget_is_visible(expander)){
    gtk_widget_show(child->child);
  }
}

/**
 * ags_expander_remove:
 * @expander: the #AgsExpander
 * @widget: the child #GtkWidget
 *
 * Removes a #GtkWidget of #AgsExpander
 *
 * Since: 3.0.0
 */
void
ags_expander_remove(AgsExpander *expander,
		    GtkWidget *widget)
{
  if(!AGS_IS_EXPANDER(expander) ||
     !GTK_IS_WIDGET(widget)){
    return;
  }
  
  ags_expander_remove_child(expander,
			    ags_expander_child_find(expander,
						    widget));
}

/**
 * ags_expander_new:
 *
 * Creates an #AgsExpander
 *
 * Returns: a new #AgsExpander
 *
 * Since: 3.0.0
 */
AgsExpander*
ags_expander_new()
{
  AgsExpander *expander;

  expander = (AgsExpander *) g_object_new(AGS_TYPE_EXPANDER,
					  NULL);
  
  return(expander);
}
