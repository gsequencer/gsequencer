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

#include <ags/widget/ags_scrolled_level_box.h>

#include <ags/widget/ags_level.h>
#include <ags/widget/ags_vlevel_box.h>

void ags_scrolled_level_box_class_init(AgsScrolledLevelBoxClass *scrolled_level_box);
void ags_scrolled_level_box_init(AgsScrolledLevelBox *scrolled_level_box);
void ags_scrolled_level_box_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_scrolled_level_box_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_scrolled_level_box_finalize(GObject *gobject);

void ags_scrolled_level_get_preferred_width(GtkWidget *widget,
					    gint *minimal_width,
					    gint *natural_width);
void ags_scrolled_level_get_preferred_height(GtkWidget *widget,
					     gint *minimal_height,
					     gint *natural_height);
void ags_scrolled_level_box_size_allocate(GtkWidget *widget,
					  GtkAllocation *allocation);

/**
 * SECTION:ags_scrolled_level_box
 * @short_description: scrolled level box widget
 * @title: AgsScrolledLevelBox
 * @section_id:
 * @include: ags/widget/ags_scrolled_level_box.h
 *
 * The #AgsScrolledLevelBox lets you to have a scrolled level box widget.
 */

enum{
  PROP_0,
  PROP_MARGIN_TOP,
  PROP_MARGIN_BOTTOM,
  PROP_MARGIN_LEFT,
  PROP_MARGIN_RIGHT,
};

static gpointer ags_scrolled_level_box_parent_class = NULL;

GType
ags_scrolled_level_box_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_scrolled_level_box = 0;

    static const GTypeInfo ags_scrolled_level_box_info = {
      sizeof (AgsScrolledLevelBoxClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_scrolled_level_box_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsScrolledLevelBox),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_scrolled_level_box_init,
    };

    ags_type_scrolled_level_box = g_type_register_static(GTK_TYPE_BIN,
							 "AgsScrolledLevelBox", &ags_scrolled_level_box_info,
							 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_scrolled_level_box);
  }

  return g_define_type_id__volatile;
}

void
ags_scrolled_level_box_class_init(AgsScrolledLevelBoxClass *scrolled_level_box)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  GParamSpec *param_spec;

  ags_scrolled_level_box_parent_class = g_type_class_peek_parent(scrolled_level_box);

  /* GObjectClass */
  gobject = (GObjectClass *) scrolled_level_box;

  gobject->set_property = ags_scrolled_level_box_set_property;
  gobject->get_property = ags_scrolled_level_box_get_property;

  gobject->finalize = ags_scrolled_level_box_finalize;

  /* properties */  
  /**
   * AgsScrolledLevelBox:margin-top:
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
   * AgsScrolledLevelBox:margin-bottom:
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
   * AgsScrolledLevelBox:margin-left:
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
   * AgsScrolledLevelBox:margin-right:
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
  widget = (GtkWidgetClass *) scrolled_level_box;

  widget->get_preferred_width = ags_scrolled_level_get_preferred_width;
  widget->get_preferred_height = ags_scrolled_level_get_preferred_height;
  widget->size_allocate = ags_scrolled_level_box_size_allocate;
}

void
ags_scrolled_level_box_init(AgsScrolledLevelBox *scrolled_level_box)
{
  scrolled_level_box->margin_top = 0;
  scrolled_level_box->margin_bottom = 0;
  scrolled_level_box->margin_left = 0;
  scrolled_level_box->margin_right = 0;

  /* viewport */
  scrolled_level_box->viewport = (GtkViewport *) gtk_viewport_new(NULL,
								  NULL);
  g_object_set(scrolled_level_box->viewport,
	       "shadow-type", GTK_SHADOW_NONE,
	       NULL);
  gtk_container_add((GtkContainer *) scrolled_level_box,
		    (GtkWidget *) scrolled_level_box->viewport);

  /* level box */
  scrolled_level_box->level_box = NULL;
  
#if 0
  scrolled_level_box->level_box = ags_vlevel_box_new();
  gtk_container_add((GtkContainer *) scrolled_level_box->viewport,
		    (GtkWidget *) scrolled_level_box->level_box);
#endif
}

void
ags_scrolled_level_box_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec)
{
  AgsScrolledLevelBox *scrolled_level_box;

  scrolled_level_box = AGS_SCROLLED_LEVEL_BOX(gobject);

  switch(prop_id){
  case PROP_MARGIN_TOP:
    {
      scrolled_level_box->margin_top = g_value_get_uint(value);
    }
    break;
  case PROP_MARGIN_BOTTOM:
    {
      scrolled_level_box->margin_bottom = g_value_get_uint(value);
    }
    break;
  case PROP_MARGIN_LEFT:
    {
      scrolled_level_box->margin_left = g_value_get_uint(value);
    }
    break;
  case PROP_MARGIN_RIGHT:
    {
      scrolled_level_box->margin_right = g_value_get_uint(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_scrolled_level_box_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec)
{
  AgsScrolledLevelBox *scrolled_level_box;

  scrolled_level_box = AGS_SCROLLED_LEVEL_BOX(gobject);

  switch(prop_id){
  case PROP_MARGIN_TOP:
    {
      g_value_set_uint(value,
		       scrolled_level_box->margin_top);
    }
    break;
  case PROP_MARGIN_BOTTOM:
    {
      g_value_set_uint(value,
		       scrolled_level_box->margin_bottom);
    }
    break;
  case PROP_MARGIN_LEFT:
    {
      g_value_set_uint(value,
		       scrolled_level_box->margin_left);
    }
    break;
  case PROP_MARGIN_RIGHT:
    {
      g_value_set_uint(value,
		       scrolled_level_box->margin_right);
    }
    break;  
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_scrolled_level_box_finalize(GObject *gobject)
{
  AgsScrolledLevelBox *scrolled_level_box;
  
  scrolled_level_box = AGS_SCROLLED_LEVEL_BOX(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_scrolled_level_box_parent_class)->finalize(gobject);
}


void
ags_scrolled_level_get_preferred_width(GtkWidget *widget,
				       gint *minimal_width,
				       gint *natural_width)
{
  minimal_width[0] =
    natural_width[0] = AGS_LEVEL_DEFAULT_LEVEL_WIDTH;
}

void
ags_scrolled_level_get_preferred_height(GtkWidget *widget,
					gint *minimal_height,
					gint *natural_height)
{
  minimal_height[0] =
    natural_height[0] = -1;
}

void
ags_scrolled_level_box_size_allocate(GtkWidget *widget,
				     GtkAllocation *allocation)
{
  AgsScrolledLevelBox *scrolled_level_box;

  GtkAllocation child_allocation;
  GtkRequisition child_requisition;
  
  scrolled_level_box = AGS_SCROLLED_LEVEL_BOX(widget);

  /* viewport allocation */
  gtk_widget_get_child_requisition((GtkWidget *) scrolled_level_box->viewport,
				   &child_requisition);

  child_allocation.x = allocation->x;
  child_allocation.y = allocation->y;

  child_allocation.width = allocation->width;
  child_allocation.height = allocation->height;

  gtk_widget_size_allocate((GtkWidget *) scrolled_level_box->viewport,
			   &child_allocation);

  /* box */
  gtk_widget_get_child_requisition((GtkWidget *) scrolled_level_box->level_box,
				   &child_requisition);

  child_allocation.x = 0;
  child_allocation.y = 0;

  child_allocation.width = allocation->width;
  child_allocation.height = child_requisition.height;
  
  gtk_widget_size_allocate((GtkWidget *) scrolled_level_box->level_box,
			   &child_allocation);
}

/**
 * ags_scrolled_level_box_new:
 *
 * Create a new #AgsScrolledLevelBox.
 *
 * Returns: a new #AgsScrolledLevelBox
 *
 * Since: 2.0.0
 */
AgsScrolledLevelBox*
ags_scrolled_level_box_new()
{
  AgsScrolledLevelBox *scrolled_level_box;

  scrolled_level_box = (AgsScrolledLevelBox *) g_object_new(AGS_TYPE_SCROLLED_LEVEL_BOX,
							    NULL);

  return(scrolled_level_box);
}
