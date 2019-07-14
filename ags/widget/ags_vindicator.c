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

#include "ags_vindicator.h"

void ags_vindicator_class_init(AgsVIndicatorClass *indicator);
void ags_vindicator_init(AgsVIndicator *indicator);
void ags_vindicator_show(GtkWidget *widget);

void ags_vindicator_size_request(GtkWidget *widget,
				 GtkRequisition *requisition);
void ags_vindicator_size_allocate(GtkWidget *widget,
				  GtkAllocation *allocation);

gboolean ags_vindicator_expose(GtkWidget *widget,
			       GdkEventExpose *event);
gboolean ags_vindicator_configure(GtkWidget *widget,
				  GdkEventConfigure *event);

void ags_vindicator_draw(AgsVIndicator *indicator);

/**
 * SECTION:ags_vindicator
 * @short_description: A vertical indicator widget
 * @title: AgsVIndicator
 * @section_id:
 * @include: ags/widget/ags_vindicator.h
 *
 * #AgsVIndicator is a widget visualizing a #GtkAdjustment.
 */

static gpointer ags_vindicator_parent_class = NULL;

GType
ags_vindicator_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_vindicator = 0;

    static const GTypeInfo ags_vindicator_info = {
      sizeof(AgsVIndicatorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_vindicator_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsVIndicator),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_vindicator_init,
    };

    ags_type_vindicator = g_type_register_static(AGS_TYPE_INDICATOR,
						 "AgsVIndicator", &ags_vindicator_info,
						 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_vindicator);
  }

  return g_define_type_id__volatile;
}

void
ags_vindicator_class_init(AgsVIndicatorClass *indicator)
{
  GtkWidgetClass *widget;

  ags_vindicator_parent_class = g_type_class_peek_parent(indicator);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) indicator;

  widget->size_request = ags_vindicator_size_request;
  widget->size_allocate = ags_vindicator_size_allocate;

  widget->expose_event = ags_vindicator_expose;
  widget->configure_event = ags_vindicator_configure;
}

void
ags_vindicator_init(AgsVIndicator *indicator)
{
  g_object_set(indicator,
	       "segment-width", AGS_VINDICATOR_DEFAULT_SEGMENT_WIDTH,
	       "segment-height", AGS_VINDICATOR_DEFAULT_SEGMENT_HEIGHT,
	       NULL);
  gtk_widget_set_size_request((GtkWidget *) indicator,
			      16,
			      100);
}

void
ags_vindicator_size_request(GtkWidget *widget,
			    GtkRequisition *requisition)
{
  AgsIndicator *indicator;

  indicator = AGS_INDICATOR(widget);
  
  requisition->height = (indicator->segment_count * indicator->segment_height) + ((indicator->segment_count - 1) * indicator->segment_padding);
  requisition->width = indicator->segment_width;
}

void
ags_vindicator_size_allocate(GtkWidget *widget,
			     GtkAllocation *allocation)
{
  AgsIndicator *indicator;

  indicator = AGS_INDICATOR(widget);

  allocation->height = (indicator->segment_count * indicator->segment_height) + ((indicator->segment_count - 1) * indicator->segment_padding);

  if(allocation->width < indicator->segment_width){
    allocation->width = indicator->segment_width;
  }
  
  widget->allocation = *allocation;

  GTK_WIDGET_CLASS(ags_vindicator_parent_class)->size_allocate(widget,
							       allocation);
}

gboolean
ags_vindicator_configure(GtkWidget *widget,
			 GdkEventConfigure *event)
{
  ags_vindicator_draw((AgsVIndicator *) widget);

  return(FALSE);
}

gboolean
ags_vindicator_expose(GtkWidget *widget,
		     GdkEventExpose *event)
{
  ags_vindicator_draw((AgsVIndicator *) widget);

  return(FALSE);
}

void
ags_vindicator_draw(AgsVIndicator *indicator)
{
  GtkWidget *widget;
  
  GtkAdjustment *adjustment;
  GtkStyle *indicator_style;
  cairo_t *cr;
  
  gdouble value;
  guint width, height;
  guint padding_top, padding_left;
  guint segment_width, segment_height;
  guint padding;
  guint i;

  static const gdouble white_gc = 65535.0;

  widget = GTK_WIDGET(indicator);
  indicator_style = gtk_widget_get_style(widget);
  
  adjustment = AGS_INDICATOR(indicator)->adjustment;

  if(adjustment == NULL){
    return;
  }
  
  //  g_message("draw %f", adjustment->value);

  cr = gdk_cairo_create(widget->window);

  if(cr == NULL){
    return;
  }
  
  width = AGS_INDICATOR(indicator)->segment_width;
  height = (AGS_INDICATOR(indicator)->segment_count * AGS_INDICATOR(indicator)->segment_height) + ((AGS_INDICATOR(indicator)->segment_count - 1) * AGS_INDICATOR(indicator)->segment_padding);

  padding_top = (GTK_WIDGET(indicator)->allocation.height - height) / 2;
  padding_left = (GTK_WIDGET(indicator)->allocation.width - width) / 2;
  
  segment_width = AGS_INDICATOR(indicator)->segment_width;
  segment_height = AGS_INDICATOR(indicator)->segment_height;

  padding = AGS_INDICATOR(indicator)->segment_padding;

  cairo_surface_flush(cairo_get_target(cr));
  cairo_push_group(cr);

  for(i = 0; i < AGS_INDICATOR(indicator)->segment_count; i++){
    if(adjustment->value > 0.0 &&
       (1.0 / adjustment->value * i < AGS_INDICATOR(indicator)->segment_count)){
      /* active */
      cairo_set_source_rgb(cr,
			   indicator_style->light[0].red / white_gc,
			   indicator_style->light[0].green / white_gc,
			   indicator_style->light[0].blue / white_gc);
    }else{
      /* normal */
      cairo_set_source_rgb(cr,
			   indicator_style->dark[0].red / white_gc,
			   indicator_style->dark[0].green / white_gc,
			   indicator_style->dark[0].blue / white_gc);
    }

    cairo_rectangle(cr,
		    padding_left, padding_top + (height - i * (segment_height + padding) - segment_height),
		    segment_width, segment_height);
    cairo_fill(cr);

    /* outline */
    cairo_set_source_rgb(cr,
			 indicator_style->fg[0].red / white_gc,
			 indicator_style->fg[0].green / white_gc,
			 indicator_style->fg[0].blue / white_gc);
    cairo_rectangle(cr,
		    padding_left, padding_top + (height - i * (segment_height + padding) - segment_height),
		    segment_width, segment_height);
    cairo_stroke(cr);
  }

  cairo_pop_group_to_source(cr);
  cairo_paint(cr);

  cairo_surface_mark_dirty(cairo_get_target(cr));
  cairo_destroy(cr);
}

/**
 * ags_vindicator_new:
 *
 * Create a new instance of #AgsVIndicator.
 *
 * Returns: the new #AgsVIndicator
 *
 * Since: 2.0.0
 */
AgsVIndicator*
ags_vindicator_new()
{
  AgsVIndicator *indicator;
  GtkAdjustment *adjustment;

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0);

  indicator = (AgsVIndicator *) g_object_new(AGS_TYPE_VINDICATOR,
					     "adjustment", adjustment,
					     NULL);
  
  return(indicator);
}
