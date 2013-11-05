/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#include "ags_expander_set.h"

#include <ags/lib/ags_list.h>

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

void ags_expander_set_realize(GtkWidget *widget);
void ags_expander_set_size_request(GtkWidget *widget,
				   GtkRequisition   *requisition);
void ags_expander_set_size_allocate(GtkWidget *widget,
				    GtkAllocation *allocation);
gboolean ags_expander_set_expose(GtkWidget *widget,
				 GdkEventExpose *event);

void ags_expander_set_width_changed(GObject *gobject,
				    GParamSpec *pspec,
				    AgsExpanderSet *expander_set);
void ags_expander_set_height_changed(GObject *gobject,
				     GParamSpec *pspec,
				     AgsExpanderSet *expander_set);

void ags_expander_set_insert_child(AgsExpanderSet *expander_set,
				   AgsExpanderSetChild *child,
				   gboolean ghost);
void ags_expander_set_remove_child(AgsExpanderSet *expander_set,
				   AgsExpanderSetChild *child,
				   gboolean ghost);

enum{
  PROP_0,
  PROP_GHOST,
  PROP_LOCATION,
};

static gpointer ags_expander_set_parent_class = NULL;

GType
ags_expander_set_get_type(void)
{
  static GType ags_type_expander_set = 0;

  if(!ags_type_expander_set){
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

    ags_type_expander_set = g_type_register_static(GTK_TYPE_TABLE,
						   "AgsExpanderSet\0", &ags_expander_set_info,
						   0);
  }

  return(ags_type_expander_set);
}

void
ags_expander_set_class_init(AgsExpanderSetClass *expander_set)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_expander_set_parent_class = g_type_class_peek_parent(expander_set);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(expander_set);

  gobject->set_property = ags_expander_set_set_property;
  gobject->get_property = ags_expander_set_get_property;

  gobject->finalize = ags_expander_set_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) expander_set;

  widget->realize = ags_expander_set_realize;
  widget->expose_event = ags_expander_set_expose;
  widget->size_request = ags_expander_set_size_request;
  widget->size_allocate = ags_expander_set_size_allocate;
}

void
ags_expander_set_init(AgsExpanderSet *expander_set)
{
  g_object_set(G_OBJECT(expander_set),
	       "app-paintable\0", TRUE,
	       "homogeneous", TRUE,
	       NULL);

  g_signal_connect(expander_set, "notify::width\0",
		   G_CALLBACK(ags_expander_set_width_changed),
		   expander_set);
  g_signal_connect(expander_set, "notify::height\0",
		   G_CALLBACK(ags_expander_set_height_changed),
		   expander_set);

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
    ags_list_free_and_free_link(expander_set->ghost);
  }

  if(expander_set->location != NULL){
    ags_list_free_and_free_link(expander_set->location);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_expander_set_parent_class)->finalize(gobject);
}

void
ags_expander_set_realize(GtkWidget *widget)
{
  //TODO:JK: implement me
}

void
ags_expander_set_size_request(GtkWidget *widget,
			      GtkRequisition   *requisition)
{
  //TODO:JK: implement me
}

void
ags_expander_set_size_allocate(GtkWidget *widget,
			       GtkAllocation *allocation)
{
  //TODO:JK: implement me
}

gboolean
ags_expander_set_expose(GtkWidget *widget,
			GdkEventExpose *event)
{
  //TODO:JK: implement me
}

void
ags_expander_set_width_changed(GObject *gobject,
			       GParamSpec *pspec,
			       AgsExpanderSet *expander_set)
{
  //TODO:JK: implement me
}

void
ags_expander_set_height_changed(GObject *gobject,
				GParamSpec *pspec,
				AgsExpanderSet *expander_set)
{
  //TODO:JK: implement me
}

void
ags_expander_set_set_flags(AgsExpanderSet *expander_set,
			   guint flags)
{
  //TODO:JK: implement me
}

AgsExpanderSetChild*
ags_expander_set_child_alloc(guint x, guint y,
			     guint width, guint height)
{
  AgsExpanderSetChild *child;

  child = (AgsExpanderSetChild *) malloc(sizeof(AgsExpanderSetChild));

  child->x = x;
  child->y = y;
  child->width = width;
  child->height = height;

  return(child);
}

AgsExpanderSetChild*
ags_expander_set_child_find(AgsExpanderSet *expander_set,
			    GtkWidget *child)
{
  GList *list;
  
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
ags_expander_set_insert_child(AgsExpanderSet *expander_set,
			      AgsExpanderSetChild *child,
			      gboolean ghost)
{
  GList *list;
  guint i;
  
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

  gtk_table_attach(GTK_TABLE(expander_set),
		   child->child,
		   child->x, child->y,
		   child->x + child->width, child->y + child->height,
		   GTK_FILL |
		   GTK_EXPAND,
		   GTK_FILL |
		   GTK_EXPAND,
		   0, 0);

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
  gtk_container_remove(GTK_CONTAINER(expander_set),
		       child->child);

  if(ghost){
    expander_set->ghost = g_list_remove(expander_set->ghost,
					child);
  }else{
    expander_set->location = g_list_remove(expander_set->location,
					   child);
  }

  free(child);
}

void
ags_expander_set_add(AgsExpanderSet *expander_set,
		     GtkWidget *widget,
		     guint x, guint y,
		     guint width, guint height)
{
  AgsExpanderSetChild *child;

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

void
ags_expander_set_remove(AgsExpanderSet *expander_set,
			GtkWidget *widget)
{
  ags_expander_set_remove_child(expander_set,
				ags_expander_set_child_find(expander_set,
							    widget),
				FALSE);
}

AgsExpanderSet*
ags_expander_set_new(guint width, guint height)
{
  AgsExpanderSet *expander_set;

  expander_set = (AgsExpanderSet *) g_object_new(AGS_TYPE_EXPANDER_SET,
						 "width\0", width,
						 "height\0", height,
						 NULL);
  
  return(expander_set);
}
