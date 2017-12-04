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

#include <ags/widget/ags_scrolled_piano.h>

void ags_scrolled_piano_class_init(AgsScrolledPianoClass *scrolled_piano);
void ags_scrolled_piano_init(AgsScrolledPiano *scrolled_piano);
void ags_scrolled_piano_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_scrolled_piano_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_scrolled_piano_finalize(GObject *gobject);

void ags_scrolled_piano_size_allocate(GtkWidget *widget,
				      GtkAllocation *allocation);
void ags_scrolled_piano_size_request(GtkWidget *widget,
				     GtkRequisition *requisition);

/**
 * SECTION:ags_scrolled_piano
 * @short_description: scrolled piano widget
 * @title: AgsScrolledPiano
 * @section_id:
 * @include: ags/widget/ags_scrolled_piano.h
 *
 * The #AgsScrolledPiano lets you to have a scrolled piano widget.
 */

enum{
  PROP_0,
  PROP_MARGIN_TOP,
  PROP_MARGIN_BOTTOM,
  PROP_MARGIN_LEFT,
  PROP_MARGIN_RIGHT,
};

static gpointer ags_scrolled_piano_parent_class = NULL;
GType
ags_scrolled_piano_get_type(void)
{
  static GType ags_type_scrolled_piano = 0;

  if(!ags_type_scrolled_piano){
    static const GTypeInfo ags_scrolled_piano_info = {
      sizeof (AgsScrolledPianoClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_scrolled_piano_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsScrolledPiano),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_scrolled_piano_init,
    };

    ags_type_scrolled_piano = g_type_register_static(GTK_TYPE_BIN,
						     "AgsScrolledPiano", &ags_scrolled_piano_info,
						     0);
  }
  
  return(ags_type_scrolled_piano);
}

void
ags_scrolled_piano_class_init(AgsScrolledPianoClass *scrolled_piano)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  GParamSpec *param_spec;

  ags_scrolled_piano_parent_class = g_type_class_peek_parent(scrolled_piano);

  /* GObjectClass */
  gobject = (GObjectClass *) scrolled_piano;

  gobject->set_property = ags_scrolled_piano_set_property;
  gobject->get_property = ags_scrolled_piano_get_property;

  gobject->finalize = ags_scrolled_piano_finalize;

  /* properties */  
  /**
   * AgsScrolledPiano:margin-top:
   *
   * The margin top.
   * 
   * Since: 1.2.0
   */
  param_spec = g_param_spec_uint("margin-top",
				 "margin top",
				 "The margin top",
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MARGIN_TOP,
				  param_spec);

  /**
   * AgsScrolledPiano:margin-bottom:
   *
   * The margin bottom.
   * 
   * Since: 1.2.0
   */
  param_spec = g_param_spec_uint("margin-bottom",
				 "margin bottom",
				 "The margin bottom",
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MARGIN_BOTTOM,
				  param_spec);

  /**
   * AgsScrolledPiano:margin-left:
   *
   * The margin left.
   * 
   * Since: 1.2.0
   */
  param_spec = g_param_spec_uint("margin-left",
				 "margin left",
				 "The margin left",
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MARGIN_LEFT,
				  param_spec);

  /**
   * AgsScrolledPiano:margin-right:
   *
   * The margin right.
   * 
   * Since: 1.2.0
   */
  param_spec = g_param_spec_uint("margin-right",
				 "margin right",
				 "The margin right",
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MARGIN_RIGHT,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) scrolled_piano;

  widget->size_request = ags_scrolled_piano_size_request;
  widget->size_allocate = ags_scrolled_piano_size_allocate;
}

void
ags_scrolled_piano_init(AgsScrolledPiano *scrolled_piano)
{
  scrolled_piano->margin_top = 0;
  scrolled_piano->margin_bottom = 0;
  scrolled_piano->margin_left = 0;
  scrolled_piano->margin_right = 0;

  /* viewport */
  scrolled_piano->viewport = gtk_viewport_new(NULL,
					      NULL);
  gtk_container_add(scrolled_piano,
		    scrolled_piano->viewport);

  /* piano */
  scrolled_piano->piano = ags_piano_new();
  gtk_container_add(scrolled_piano->viewport,
		    scrolled_piano->piano);
}

void
ags_scrolled_piano_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsScrolledPiano *scrolled_piano;

  scrolled_piano = AGS_SCROLLED_PIANO(gobject);

  switch(prop_id){
  case PROP_MARGIN_TOP:
    {
      scrolled_piano->margin_top = g_value_get_uint(value);
    }
    break;
  case PROP_MARGIN_BOTTOM:
    {
      scrolled_piano->margin_bottom = g_value_get_uint(value);
    }
    break;
  case PROP_MARGIN_LEFT:
    {
      scrolled_piano->margin_left = g_value_get_uint(value);
    }
    break;
  case PROP_MARGIN_RIGHT:
    {
      scrolled_piano->margin_right = g_value_get_uint(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_scrolled_piano_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec)
{
  AgsScrolledPiano *scrolled_piano;

  scrolled_piano = AGS_SCROLLED_PIANO(gobject);

  switch(prop_id){
  case PROP_MARGIN_TOP:
    {
      g_value_set_uint(value,
		       scrolled_piano->margin_top);
    }
    break;
  case PROP_MARGIN_BOTTOM:
    {
      g_value_set_uint(value,
		       scrolled_piano->margin_bottom);
    }
    break;
  case PROP_MARGIN_LEFT:
    {
      g_value_set_uint(value,
		       scrolled_piano->margin_left);
    }
    break;
  case PROP_MARGIN_RIGHT:
    {
      g_value_set_uint(value,
		       scrolled_piano->margin_right);
    }
    break;  
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_scrolled_piano_finalize(GObject *gobject)
{
  AgsScrolledPiano *scrolled_piano;
  
  scrolled_piano = AGS_SCROLLED_PIANO(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_scrolled_piano_parent_class)->finalize(gobject);
}

void
ags_scrolled_piano_size_allocate(GtkWidget *widget,
				 GtkAllocation *allocation)
{
  AgsScrolledPiano *scrolled_piano;

  GtkAllocation child_allocation;

  scrolled_piano = AGS_SCROLLED_PIANO(widget);
  
  widget->allocation = *allocation;

  if(scrolled_piano->piano->layout == AGS_PIANO_LAYOUT_VERTICAL){
    widget->allocation.width = scrolled_piano->piano->key_width + (scrolled_piano->margin_left + scrolled_piano->margin_right);
  }else if(scrolled_piano->piano->layout == AGS_PIANO_LAYOUT_HORIZONTAL){
    widget->allocation.height = scrolled_piano->piano->key_width + (scrolled_piano->margin_top + scrolled_piano->margin_bottom);
  }

  /* viewport allocation */
  child_allocation.x = allocation->x + scrolled_piano->margin_left;
  child_allocation.y = allocation->y + scrolled_piano->margin_top;

  if(scrolled_piano->piano->layout == AGS_PIANO_LAYOUT_VERTICAL){
    child_allocation.width = scrolled_piano->piano->key_width;

    if(widget->allocation.height > (scrolled_piano->margin_top + scrolled_piano->margin_bottom)){
      child_allocation.height = widget->allocation.height - (scrolled_piano->margin_top + scrolled_piano->margin_bottom);
    }else{
      child_allocation.height = 0;
    }
  }else if(scrolled_piano->piano->layout == AGS_PIANO_LAYOUT_HORIZONTAL){
    if(widget->allocation.width > (scrolled_piano->margin_left + scrolled_piano->margin_right)){
      child_allocation.width = widget->allocation.width - (scrolled_piano->margin_left + scrolled_piano->margin_right);
    }else{
      child_allocation.width = 0;
    }
    
    child_allocation.height = scrolled_piano->piano->key_width;
  }

  gtk_widget_size_allocate((GtkWidget *) scrolled_piano->viewport,
			   &child_allocation);
}

void
ags_scrolled_piano_size_request(GtkWidget *widget,
				GtkRequisition *requisition)
{
  AgsScrolledPiano *scrolled_piano;
  
  GtkRequisition child_requisition;

  scrolled_piano = AGS_SCROLLED_PIANO(widget);

  if(scrolled_piano->piano->layout == AGS_PIANO_LAYOUT_VERTICAL){
    requisition->width = scrolled_piano->piano->key_width + (scrolled_piano->margin_left + scrolled_piano->margin_right);
    requisition->height = -1;
  }else if(scrolled_piano->piano->layout == AGS_PIANO_LAYOUT_HORIZONTAL){
    requisition->width = -1;
    requisition->height = scrolled_piano->piano->key_width + (scrolled_piano->margin_top + scrolled_piano->margin_bottom);
  }

  gtk_widget_size_request((GtkWidget *) gtk_bin_get_child(scrolled_piano),
			  &child_requisition);
}

/**
 * ags_scrolled_piano_new:
 *
 * Create a new #AgsScrolledPiano.
 *
 * Returns: a new #AgsScrolledPiano
 *
 * Since: 1.2.0
 */
AgsScrolledPiano*
ags_scrolled_piano_new()
{
  AgsScrolledPiano *scrolled_piano;

  scrolled_piano = (AgsScrolledPiano *) g_object_new(AGS_TYPE_SCROLLED_PIANO,
						     NULL);

  return(scrolled_piano);
}
