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

#include "ags_expander.h"

#include <ags/lib/ags_list.h>

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

void ags_expander_realize(GtkWidget *widget);
void ags_expander_size_request(GtkWidget *widget,
			       GtkRequisition   *requisition);
void ags_expander_size_allocate(GtkWidget *widget,
				GtkAllocation *allocation);
gboolean ags_expander_expose(GtkWidget *widget,
			     GdkEventExpose *event);

void ags_expander_activate(GtkExpander *expander);

void ags_expander_width_changed(GObject *gobject,
				GParamSpec *pspec,
				AgsExpander *expander);
void ags_expander_height_changed(GObject *gobject,
				 GParamSpec *pspec,
				 AgsExpander *expander);

void ags_expander_draw(AgsExpander *expander);
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
 * #AgsExpander is a composite widget containing a #GtkTable. It
 * is mainly used to layout childs in rows and columns that can
 * be expanded.
 */

enum{
  PROP_0,
  PROP_WIDTH,
  PROP_HEIGHT,
  PROP_CHILD,
};

static gpointer ags_expander_parent_class = NULL;

GType
ags_expander_get_type(void)
{
  static GType ags_type_expander = 0;

  if(!ags_type_expander){
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

    ags_type_expander = g_type_register_static(GTK_TYPE_EXPANDER,
					       "AgsExpander\0", &ags_expander_info,
					       0);
  }

  return(ags_type_expander);
}

void
ags_expander_class_init(AgsExpanderClass *expander)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GtkExpanderClass *parent;
  GParamSpec *param_spec;

  ags_expander_parent_class = g_type_class_peek_parent(expander);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(expander);

  gobject->set_property = ags_expander_set_property;
  gobject->get_property = ags_expander_get_property;

  gobject->finalize = ags_expander_finalize;

  /* properties */
  param_spec = g_param_spec_uint("width\0",
				 "width of expander grid\0",
				 "The width of the expander's grid\0",
				 0, G_MAXUINT,
				 1,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WIDTH,
				  param_spec);

  param_spec = g_param_spec_uint("height\0",
				 "height of expander\0",
				 "The height which this expander is packed into\0",
				 0, G_MAXUINT,
				 1,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_HEIGHT,
				  param_spec);

  param_spec = g_param_spec_object("child\0",
				   "child of expander\0",
				   "The child which this expander is packed into\0",
				   GTK_TYPE_WIDGET,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHILD,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) expander;

  //  widget->realize = ags_expander_realize;
  //  widget->expose_event = ags_expander_expose;
  //  widget->size_request = ags_expander_size_request;
  //  widget->size_allocate = ags_expander_size_allocate;

  /* GtkExpanderClass */
  parent = (GtkExpanderClass *) expander;

  parent->activate = ags_expander_activate;
}

void
ags_expander_init(AgsExpander *expander)
{
  g_object_set(G_OBJECT(expander),
	       "app-paintable\0", TRUE,
	       NULL);

  g_signal_connect(expander, "notify::width\0",
  		   G_CALLBACK(ags_expander_width_changed),
		   expander);
  g_signal_connect(expander, "notify::height\0",
		   G_CALLBACK(ags_expander_height_changed),
		   expander);

  expander->flags = 0;

  expander->table = (GtkTable *) gtk_table_new(2, 2, FALSE);
  gtk_container_add(GTK_CONTAINER(expander),
		    GTK_WIDGET(expander->table));

  expander->children = NULL;
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
  case PROP_WIDTH:
    {
      guint width, height;

      gtk_table_get_size(expander->table,
			 &width, &height);

      width = g_value_get_uint(value);
      
      gtk_table_resize(expander->table,
		       width, height);
    }
    break;
  case PROP_HEIGHT:
    {
      guint width, height;

      gtk_table_get_size(expander->table,
			 &width, &height);

      height = g_value_get_uint(value);

      gtk_table_resize(expander->table,
		       width, height);
    }
    break;
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
  case PROP_WIDTH:
    {
      guint width, height;

      gtk_table_get_size(expander->table,
			 &width, &height);

      g_value_set_uint(value, width);
    }
    break;
  case PROP_HEIGHT:
    {
      guint width, height;

      gtk_table_get_size(expander->table,
			 &width, &height);

      g_value_set_uint(value, height);
    }
    break;
  case PROP_CHILD:
    g_value_set_pointer(value, expander->children);
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
    ags_list_free_and_free_link(expander->children);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_expander_parent_class)->finalize(gobject);
}

void
ags_expander_realize(GtkWidget *widget)
{
  //TODO:JK: implement me
}

void
ags_expander_size_request(GtkWidget *widget,
			  GtkRequisition   *requisition)
{
  //TODO:JK: implement me
}

void
ags_expander_size_allocate(GtkWidget *widget,
			   GtkAllocation *allocation)
{
  //TODO:JK: implement me
}

gboolean
ags_expander_expose(GtkWidget *widget,
		    GdkEventExpose *event)
{
  //TODO:JK: implement me
}

void
ags_expander_activate(GtkExpander *expander)
{
  GTK_EXPANDER_CLASS(ags_expander_parent_class)->activate(expander);

  if(gtk_expander_get_expanded(expander)){
    gtk_widget_show_all(GTK_WIDGET(AGS_EXPANDER(expander)->table));
  }else{
    gtk_widget_hide(GTK_WIDGET(AGS_EXPANDER(expander)->table));
  }
}

void
ags_expander_width_changed(GObject *gobject,
			   GParamSpec *pspec,
			   AgsExpander *expander)
{
  //TODO:JK: implement me
}

void
ags_expander_height_changed(GObject *gobject,
			    GParamSpec *pspec,
			    AgsExpander *expander)
{
  //TODO:JK: implement me
}

void
ags_expander_flags(AgsExpander *expander,
		   guint flags)
{
  //TODO:JK: implement me
}

AgsExpanderChild*
ags_expander_child_alloc(guint x, guint y,
			 guint width, guint height)
{
  AgsExpanderChild *child;

  child = (AgsExpanderChild *) malloc(sizeof(AgsExpanderChild));

  child->x = x;
  child->y = y;
  child->width = width;
  child->height = height;

  return(child);
}

void
ags_expander_draw(AgsExpander *expander)
{
  //TODO:JK: implement me
}

AgsExpanderChild*
ags_expander_child_find(AgsExpander *expander,
			GtkWidget *child)
{
  GList *list;
  
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

  gtk_table_attach(expander->table,
		   child->child,
		   child->x, child->x + child->width,
		   child->y, child->y + child->height,
		   0,
		   0,
		   0, 0);

  expander->children = g_list_insert(expander->children,
				     child,
				     i);
}

void
ags_expander_remove_child(AgsExpander *expander,
			  AgsExpanderChild *child)
{
  gtk_container_remove(GTK_CONTAINER(expander->table),
		       child->child);

  expander->children = g_list_remove(expander->children,
				     child);

  free(child);
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
 * Since: 0.4
 */
void
ags_expander_add(AgsExpander *expander,
		 GtkWidget *widget,
		 guint x, guint y,
		 guint width, guint height)
{
  AgsExpanderChild *child;

  child = ags_expander_child_alloc(x, y,
				   width, height);
  child->child = widget;
    
  ags_expander_insert_child(expander,
			    child);

  if(GTK_WIDGET_VISIBLE(expander)){
    gtk_widget_show_all(child->child);
  }
}

/**
 * ags_expander_remove:
 * @expander: the #AgsExpander
 * @widget: the child #GtkWidget
 *
 * Removes a #GtkWidget of #AgsExpander
 *
 * Since: 0.4
 */
void
ags_expander_remove(AgsExpander *expander,
		    GtkWidget *widget)
{
  ags_expander_remove_child(expander,
			    ags_expander_child_find(expander,
						    widget));
}

/**
 * ags_expander_new:
 * @height: the table's height
 * @width: the table's width
 *
 * Creates an #AgsExpander
 *
 * Returns: a new #AgsExpander
 *
 * Since: 0.4
 */
AgsExpander*
ags_expander_new(guint width, guint height)
{
  AgsExpander *expander;

  expander = (AgsExpander *) g_object_new(AGS_TYPE_EXPANDER,
					  "width\0", width,
					  "height\0", height,
					  NULL);
  
  return(expander);
}
