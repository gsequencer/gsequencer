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

#include "ags_hindicator.h"

void ags_hindicator_class_init(AgsHIndicatorClass *indicator);
void ags_hindicator_init(AgsHIndicator *indicator);
void ags_hindicator_show(GtkWidget *widget);

void ags_hindicator_size_request(GtkWidget *widget,
				 GtkRequisition *requisition);
void ags_hindicator_size_allocate(GtkWidget *widget,
				  GtkAllocation *allocation);

gboolean ags_hindicator_expose(GtkWidget *widget,
			       GdkEventExpose *event);
gboolean ags_hindicator_configure(GtkWidget *widget,
				  GdkEventConfigure *event);

void ags_hindicator_draw(AgsHIndicator *indicator);

/**
 * SECTION:ags_hindicator
 * @short_description: A horizontal indicator widget
 * @title: AgsHIndicator
 * @section_id:
 * @include: ags/widget/ags_hindicator.h
 *
 * #AgsHIndicator is a widget visualizing a #GtkAdjustment.
 */

static gpointer ags_hindicator_parent_class = NULL;

GType
ags_hindicator_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_hindicator = 0;

    static const GTypeInfo ags_hindicator_info = {
      sizeof(AgsHIndicatorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_hindicator_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsHIndicator),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_hindicator_init,
    };

    ags_type_hindicator = g_type_register_static(AGS_TYPE_INDICATOR,
						 "AgsHIndicator", &ags_hindicator_info,
						 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_hindicator);
  }

  return g_define_type_id__volatile;
}

void
ags_hindicator_class_init(AgsHIndicatorClass *indicator)
{
  GtkWidgetClass *widget;

  ags_hindicator_parent_class = g_type_class_peek_parent(indicator);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) indicator;

  widget->size_request = ags_hindicator_size_request;
  widget->size_allocate = ags_hindicator_size_allocate;

  widget->expose_event = ags_hindicator_expose;
  widget->configure_event = ags_hindicator_configure;
}

void
ags_hindicator_init(AgsHIndicator *indicator)
{
  g_object_set(indicator,
	       "segment-width", AGS_HINDICATOR_DEFAULT_SEGMENT_WIDTH,
	       "segment-height", AGS_HINDICATOR_DEFAULT_SEGMENT_HEIGHT,
	       NULL);
}

void
ags_hindicator_size_request(GtkWidget *widget,
			    GtkRequisition *requisition)
{
  AgsIndicator *indicator;

  indicator = AGS_INDICATOR(widget);
  
  requisition->height = indicator->segment_height;
  requisition->width = (indicator->segment_count * indicator->segment_width) + (indicator->segment_count * indicator->segment_padding);
}

void
ags_hindicator_size_allocate(GtkWidget *widget,
			     GtkAllocation *allocation)
{
  AgsIndicator *indicator;

  indicator = AGS_INDICATOR(widget);

  if(allocation->height < indicator->segment_height){
    allocation->height = indicator->segment_height;
  }
  
  allocation->width = (indicator->segment_count * indicator->segment_width) + ((indicator->segment_count - 1) * indicator->segment_padding);
  
  widget->allocation = *allocation;

  GTK_WIDGET_CLASS(ags_hindicator_parent_class)->size_allocate(widget,
							       allocation);
}

gboolean
ags_hindicator_configure(GtkWidget *widget,
			 GdkEventConfigure *event)
{
  ags_hindicator_draw((AgsHIndicator *) widget);

  return(FALSE);
}

gboolean
ags_hindicator_expose(GtkWidget *widget,
		     GdkEventExpose *event)
{
  ags_hindicator_draw((AgsHIndicator *) widget);

  return(FALSE);
}

void
ags_hindicator_draw(AgsHIndicator *indicator)
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
  
  width = (AGS_INDICATOR(indicator)->segment_count * AGS_INDICATOR(indicator)->segment_width) + ((AGS_INDICATOR(indicator)->segment_count - 1) * AGS_INDICATOR(indicator)->segment_padding);
  height = AGS_INDICATOR(indicator)->segment_height;

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
		    width - i * (segment_width + padding) - segment_width, 0,
		    segment_width, segment_height);
    cairo_fill(cr);

    /* outline */
    cairo_set_source_rgb(cr,
			 indicator_style->fg[0].red / white_gc,
			 indicator_style->fg[0].green / white_gc,
			 indicator_style->fg[0].blue / white_gc);
    cairo_rectangle(cr,
		    width - i * (segment_width + padding) - segment_width, 0,
		    segment_width, segment_height);
    cairo_stroke(cr);
  }

  cairo_pop_group_to_source(cr);
  cairo_paint(cr);

  cairo_surface_mark_dirty(cairo_get_target(cr));
  cairo_destroy(cr);
}

/**
 * ags_hindicator_new:
 *
 * Creates an #AgsHIndicator.
 *
 * Returns: a new #AgsHIndicator
 *
 * Since: 2.0.0
 */
AgsHIndicator*
ags_hindicator_new()
{
  AgsHIndicator *indicator;
  GtkAdjustment *adjustment;

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0);

  indicator = (AgsHIndicator *) g_object_new(AGS_TYPE_HINDICATOR,
					     "adjustment", adjustment,
					     NULL);
  
  return(indicator);
}
