/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

void ags_scrolled_piano_get_preferred_width(GtkWidget *widget,
					    gint *minimal_width,
					    gint *natural_width);
void ags_scrolled_piano_get_preferred_height(GtkWidget *widget,
					     gint *minimal_height,
					     gint *natural_height);
void ags_scrolled_piano_size_allocate(GtkWidget *widget,
				      GtkAllocation *allocation);

void ags_scrolled_piano_allocate_viewport(GtkWidget *widget);

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
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_scrolled_piano = 0;

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

    g_once_init_leave(&g_define_type_id__volatile, ags_type_scrolled_piano);
  }

  return g_define_type_id__volatile;
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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

  widget->get_preferred_width = ags_scrolled_piano_get_preferred_width;
  widget->get_preferred_height = ags_scrolled_piano_get_preferred_height;
  widget->size_allocate = ags_scrolled_piano_size_allocate;
}

void
ags_scrolled_piano_init(AgsScrolledPiano *scrolled_piano)
{
  gtk_widget_set_events(GTK_WIDGET(scrolled_piano), GDK_EXPOSURE_MASK);
  
  scrolled_piano->margin_top = 0;
  scrolled_piano->margin_bottom = 0;
  scrolled_piano->margin_left = 0;
  scrolled_piano->margin_right = 0;

  /* viewport */
  scrolled_piano->viewport = gtk_viewport_new(NULL,
					      NULL);
  g_object_set(scrolled_piano->viewport,
	       "shadow-type", GTK_SHADOW_NONE,
	       NULL);
  gtk_container_add((GtkContainer *) scrolled_piano,
		    (GtkWidget *) scrolled_piano->viewport);

  /* piano */
  scrolled_piano->piano = ags_piano_new();
  gtk_container_add((GtkContainer *) scrolled_piano->viewport,
		    (GtkWidget *) scrolled_piano->piano);
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
ags_scrolled_piano_get_preferred_width(GtkWidget *widget,
				       gint *minimal_width,
				       gint *natural_width)
{
  AgsScrolledPiano *scrolled_piano;
  
  scrolled_piano = AGS_SCROLLED_PIANO(widget);

  if(scrolled_piano->piano == NULL){
    minimal_width =
      natural_width = NULL;

    return;
  }
  
  if(scrolled_piano->piano->layout == AGS_PIANO_LAYOUT_VERTICAL){
    minimal_width[0] =
      natural_width[0] = scrolled_piano->piano->key_width + (scrolled_piano->margin_left + scrolled_piano->margin_right);
  }else if(scrolled_piano->piano->layout == AGS_PIANO_LAYOUT_HORIZONTAL){
    minimal_width =
      natural_width = NULL;
  }
}

void
ags_scrolled_piano_get_preferred_height(GtkWidget *widget,
					gint *minimal_height,
					gint *natural_height)
{
  AgsScrolledPiano *scrolled_piano;
  
  scrolled_piano = AGS_SCROLLED_PIANO(widget);

  if(scrolled_piano->piano == NULL){
    minimal_height =
      natural_height = NULL;

    return;
  }
  
  if(scrolled_piano->piano->layout == AGS_PIANO_LAYOUT_VERTICAL){
    minimal_height =
      natural_height = NULL;
  }else if(scrolled_piano->piano->layout == AGS_PIANO_LAYOUT_HORIZONTAL){
    minimal_height[0] =
      natural_height[0] = scrolled_piano->piano->key_width + (scrolled_piano->margin_top + scrolled_piano->margin_bottom);
  }
}

void
ags_scrolled_piano_size_allocate(GtkWidget *widget,
				 GtkAllocation *allocation)
{
  AgsScrolledPiano *scrolled_piano;

  GtkAdjustment *piano_adjustment;

  GtkAllocation child_allocation;

  gdouble upper;

  scrolled_piano = AGS_SCROLLED_PIANO(widget);  

  if(scrolled_piano->piano->layout == AGS_PIANO_LAYOUT_VERTICAL){
    allocation->width = scrolled_piano->piano->key_width + (scrolled_piano->margin_left + scrolled_piano->margin_right);
  }else if(scrolled_piano->piano->layout == AGS_PIANO_LAYOUT_HORIZONTAL){
    allocation->height = scrolled_piano->piano->key_width + (scrolled_piano->margin_top + scrolled_piano->margin_bottom);
  }

  gtk_widget_set_allocation(widget,
			    allocation);
  
  /* viewport allocation */
  ags_scrolled_piano_allocate_viewport(widget);
}

void
ags_scrolled_piano_allocate_viewport(GtkWidget *widget)
{
  AgsScrolledPiano *scrolled_piano;

  GtkAdjustment *piano_adjustment;

  GdkWindow *viewport_window;
  
  GtkAllocation allocation;
  GtkAllocation child_allocation;

  gdouble upper;

  scrolled_piano = AGS_SCROLLED_PIANO(widget);

  gtk_widget_get_allocation(widget,
			    &allocation);

  /* viewport allocation */
  child_allocation.x = allocation.x + scrolled_piano->margin_left;
  child_allocation.y = allocation.y + scrolled_piano->margin_top;

  if(scrolled_piano->piano->layout == AGS_PIANO_LAYOUT_VERTICAL){
    child_allocation.width = scrolled_piano->piano->key_width;

    if(allocation.height < scrolled_piano->piano->key_count * scrolled_piano->piano->key_height){
      child_allocation.height = scrolled_piano->piano->key_count * scrolled_piano->piano->key_height;
    }else{
      child_allocation.height = allocation.height;
    }
  }else if(scrolled_piano->piano->layout == AGS_PIANO_LAYOUT_HORIZONTAL){
    if(allocation.width < scrolled_piano->piano->key_count * scrolled_piano->piano->key_height){
      child_allocation.width = scrolled_piano->piano->key_count * scrolled_piano->piano->key_height;
    }else{
      child_allocation.width = allocation.width;
    }
    
    child_allocation.height = scrolled_piano->piano->key_width;
  }

  gtk_widget_size_allocate((GtkWidget *) scrolled_piano->viewport,
			   &child_allocation);

  /* viewport */
  if(scrolled_piano->piano->layout == AGS_PIANO_LAYOUT_VERTICAL){
    g_object_get(scrolled_piano->viewport,
		 "vadjustment", &piano_adjustment,
		 NULL);

    upper = child_allocation.height - (allocation.height - (scrolled_piano->margin_top + scrolled_piano->margin_bottom));

    gtk_adjustment_set_upper(piano_adjustment,
			     upper);
  }else{
    g_object_get(scrolled_piano->viewport,
		 "hadjustment", &piano_adjustment,
		 NULL);

    upper = child_allocation.width - (allocation.width - (scrolled_piano->margin_left + scrolled_piano->margin_right));

    gtk_adjustment_set_upper(piano_adjustment,
			     upper);
  }
}

/**
 * ags_scrolled_piano_new:
 *
 * Create a new #AgsScrolledPiano.
 *
 * Returns: a new #AgsScrolledPiano
 *
 * Since: 2.0.0
 */
AgsScrolledPiano*
ags_scrolled_piano_new()
{
  AgsScrolledPiano *scrolled_piano;

  scrolled_piano = (AgsScrolledPiano *) g_object_new(AGS_TYPE_SCROLLED_PIANO,
						     NULL);

  return(scrolled_piano);
}
