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

#include <ags/X/editor/ags_automation_edit_box.h>

void ags_automation_edit_box_class_init(AgsAutomationEditBoxClass *automation_edit_box);
void ags_automation_edit_box_init(AgsAutomationEditBox *automation_edit_box);
void ags_automation_edit_box_set_property(GObject *gobject,
					  guint prop_id,
					  const GValue *value,
					  GParamSpec *param_spec);
void ags_automation_edit_box_get_property(GObject *gobject,
					  guint prop_id,
					  GValue *value,
					  GParamSpec *param_spec);
void ags_automation_edit_box_finalize(GObject *gobject);

void ags_automation_edit_box_size_allocate(AgsAutomationEditBox *automation_edit_box,
					   GtkAllocation *allocation);
void ags_automation_edit_box_size_request(AgsAutomationEditBox *automation_edit_box,
					  GtkRequisition *requisition);

GType ags_automation_edit_box_child_type(GtkContainer *container);

/**
 * SECTION:ags_automation_edit_box
 * @short_description: abstract box widget
 * @title: AgsAutomationEditBox
 * @section_id:
 * @include: ags/widget/ags_automation_edit_box.h
 *
 * The #AgsAutomationEditBox is an abstract box widget containing #AgsAutomationEdit.
 */

enum{
  PROP_0,
  PROP_FIXED_EDIT_WIDTH,
  PROP_FIXED_EDIT_HEIGHT,
};

static gpointer ags_automation_edit_box_parent_class = NULL;

GType
ags_automation_edit_box_get_type(void)
{
  static GType ags_type_automation_edit_box = 0;

  if(!ags_type_automation_edit_box){
    static const GTypeInfo ags_automation_edit_box_info = {
      sizeof (AgsAutomationEditBoxClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_automation_edit_box_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAutomationEditBox),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_automation_edit_box_init,
    };

    ags_type_automation_edit_box = g_type_register_static(GTK_TYPE_BOX,
							  "AgsAutomationEditBox", &ags_automation_edit_box_info,
							  0);
  }
  
  return(ags_type_automation_edit_box);
}

void
ags_automation_edit_box_class_init(AgsAutomationEditBoxClass *automation_edit_box)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GtkContainerClass *container;

  GParamSpec *param_spec;

  ags_automation_edit_box_parent_class = g_type_class_peek_parent(automation_edit_box);

  /* GObjectClass */
  gobject = (GObjectClass *) automation_edit_box;

  gobject->set_property = ags_automation_edit_box_set_property;
  gobject->get_property = ags_automation_edit_box_get_property;

  gobject->finalize = ags_automation_edit_box_finalize;

  /* properties */
  /**
   * AgsAutomationEditBox:fixed-edit-height:
   *
   * The fixed height of a edit.
   * 
   * Since: 1.3.0
   */
  param_spec = g_param_spec_string("fixed-edit-height",
				   "fixed edit height",
				   "The fixed height of a edit",
				   AGS_AUTOMATION_EDIT_BOX_DEFAULT_FIXED_EDIT_HEIGHT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FIXED_EDIT_HEIGHT,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) automation_edit_box;

  widget->size_allocate = ags_automation_edit_box_size_allocate;
  widget->size_request = ags_automation_edit_box_size_request;

  /* GtkContainerClass */
  container = (GtkWidgetClass *) automation_edit_box;

  container->child_type = gtk_automation_edit_box_child_type;
}

void
ags_automation_edit_box_init(AgsAutomationEditBox *automation_edit_box)
{
  g_object_set(automation_edit_box,
	       "homogeneous", FALSE,
	       "spacing", 0,
	       NULL);
  
  automation_edit_box->flags = 0;

  automation_edit_box->fixed_edit_height = AGS_AUTOMATION_EDIT_BOX_DEFAULT_FIXED_EDIT_HEIGHT;
}

void
ags_automation_edit_box_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec)
{
  AgsAutomationEditBox *automation_edit_box;

  automation_edit_box = AGS_AUTOMATION_EDIT_BOX(gobject);

  switch(prop_id){
  case PROP_FIXED_EDIT_HEIGHT:
    {
      automation_edit_box->fixed_edit_height = g_value_get_uint(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_automation_edit_box_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec)
{
  AgsAutomationEditBox *automation_edit_box;

  automation_edit_box = AGS_AUTOMATION_EDIT_BOX(gobject);

  switch(prop_id){
  case PROP_FIXED_EDIT_HEIGHT:
    {
      g_value_set_uint(automation_edit_box->fixed_edit_height);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_automation_edit_box_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_automation_edit_box_parent_class)->finalize(gobject);
}

void
ags_automation_edit_box_size_allocate(AgsAutomationEditBox *automation_edit_box,
				      GtkAllocation *allocation)
{
  GtkAllocation child_allocation;
  GtkRequisition child_requisition;

  GList *list, *list_start;

  GtkOrientation orientation;
  guint width, height;
  guint x, y;
  
  GTK_WIDGET(notebook->navigation)->allocation = *allocation;

  list_start = 
    list = gtk_container_get_children((GtkContainer *) notebook->hbox);

  orientation = gtk_orientable_get_orientation(GTK_ORIENTABLE(automation_edit_box));
  
  /* guess size */
  width = 0; 
  height = 0;
  
  while(list != NULL){
    AgsAutomationEdit *automation_edit;

    guint padding;
    
    automation_edit = AGS_AUTOMATION_EDIT(list->data);

    gtk_container_child_get(automation_edit_box,
			    automation_edit,
			    "padding", &padding,
			    NULL);
    
    switch(orientation){
    case GTK_ORIENTATION_HORIZONTAL:
      {
	if(GTK_WIDGET(automation_edit)->allocation.height > height){
	  height = GTK_WIDGET(automation_edit)->allocation.height;
	}
      
	if(GTK_WIDGET(automation_edit)->allocation.width >= 0){
	  width += GTK_WIDGET(automation_edit)->allocation.width;
	}

	if(list->prev != NULL &&
	   list->next != NULL){
	  width += (2 * padding);
	}else{
	  width += padding;
	}
      }
      break;
    case GTK_ORIENTATION_VERTICAL:
      {
	if(GTK_WIDGET(automation_edit)->allocation.height >= 0){
	  height += GTK_WIDGET(automation_edit)->allocation.height;
	}
	
	if(GTK_WIDGET(automation_edit)->allocation.width > width){
	  width = GTK_WIDGET(automation_edit)->allocation.width;
	}	

	if(list->prev != NULL &&
	   list->next != NULL){
	  height += (2 * padding);
	}else{
	  height += padding;
	}
      }
      break;
    }
    
    list = list->next;
  }

  /* apply size */
  allocation->height = height;
  allocation->width = width;

  /* child allocation */
  list = list_start;
  
  x = 0;
  y = 0;
  
  while(list != NULL){
    AgsAutomationEdit *automation_edit;

    guint padding;

    automation_edit = AGS_AUTOMATION_EDIT(list->data);
    
    gtk_container_child_get(automation_edit_box,
			    automation_edit,
			    "padding", &padding,
			    NULL);

    gtk_widget_get_child_requisition((GtkWidget *) list->data,
				     &child_requisition);

    child_allocation.x = x;
    child_allocation.y = y;

    switch(orientation){
    case GTK_ORIENTATION_HORIZONTAL:
      {    
	child_allocation.width = AGS_AUTOMATION_EDIT_DEFAULT_WIDTH;
	child_allocation.height = -1;
      }
      break;
    case GTK_ORIENTATION_VERTICAL:
      {	
	child_allocation.width = -1;
	child_allocation.height = AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT;
      }
      break;
    }
    
    gtk_widget_size_allocate(list->data,
			     &child_allocation);

    switch(orientation){
    case GTK_ORIENTATION_HORIZONTAL:
      {
	x += AGS_AUTOMATION_EDIT_DEFAULT_WIDTH;

	if(list->prev != NULL &&
	   list->next != NULL){
	  x += (2 * padding);
	}else{
	  x += padding;
	}
      }
      break;
    case GTK_ORIENTATION_VERTICAL:
      {    
	y += AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT;

	if(list->prev != NULL &&
	   list->next != NULL){
	  y += (2 * padding);
	}else{
	  y += padding;
	}
      }
      break;
    }
    
    list = list->next;
  }
  
  g_list_free(list_start);
}

void
ags_automation_edit_box_size_request(AgsAutomationEditBox *automation_edit_box,
				     GtkRequisition *requisition)
{
  GtkRequisition child_requisition;
  
  GList *list, *list_start;

  GtkOrientation orientation;
  
  list_start =
    list = gtk_container_get_children((GtkContainer *) automation_edit_box);

  orientation = gtk_orientable_get_orientation(GTK_ORIENTABLE(automation_edit_box));

  switch(orientation){
  case GTK_ORIENTATION_HORIZONTAL:
    {
      guint width;

      width = 0;

      while(list != NULL){
	if(AGS_AUTOMATION_EDIT(list->start)->layout == AGS_AUTOMATION_EDIT_LAYOUT_VERTICAL){
	  width += AGS_AUTOMATION_EDIT(list->start)->automation_edit_width;
	}else if(AGS_AUTOMATION_EDIT(list->start)->layout == AGS_AUTOMATION_EDIT_LAYOUT_HORIZONTAL){
	  width += AGS_AUTOMATION_EDIT(list->start)->automation_edit_height;
	}
	
	list = list->next;
      }
      
      requisition->width = width;
      
      requisition->height = -1;
    }
    break;
  case GTK_ORIENTATION_VERTICAL:
    {
      guint height;
      
      requisition->width = -1;

      height = 0;

      while(list != NULL){
	if(AGS_AUTOMATION_EDIT(list->start)->layout == AGS_AUTOMATION_EDIT_LAYOUT_VERTICAL){
	  height += AGS_AUTOMATION_EDIT(list->start)->automation_edit_height;
	}else if(AGS_AUTOMATION_EDIT(list->start)->layout == AGS_AUTOMATION_EDIT_LAYOUT_HORIZONTAL){
	  height += AGS_AUTOMATION_EDIT(list->start)->automation_edit_width;
	}
	
	list = list->next;
      }

      requisition->height = height;
    }
    break;
  }

  list = list_start;
  
  while(list != NULL){
    gtk_widget_size_request((GtkWidget *) list->data,
			    &child_requisition);

    list = list->next;
  }

  g_list_free(list_start);
}

GType ags_automation_edit_box_child_type(GtkContainer *container)
{
  return(AGS_TYPE_AUTOMATION_EDIT);
}

/**
 * ags_automation_edit_box_new:
 * 
 * Create a new instance of #AgsAutomationEditBox.
 * 
 * Returns: the new #AgsAutomationEditBox instance
 * 
 * Since: 1.3.0
 */
AgsAutomationEditBox*
ags_automation_edit_box_new()
{
  AgsAutomationEditBox *automation_edit_box;

  automation_edit_box = (AgsAutomationEditBox *) g_object_new(AGS_TYPE_AUTOMATION_EDIT_BOX,
							      NULL);
  
  return(automation_edit_box);
}
