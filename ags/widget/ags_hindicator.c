/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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
  static GType ags_type_hindicator = 0;

  if(!ags_type_hindicator){
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
						 "AgsHIndicator\0", &ags_hindicator_info,
						 0);
  }

  return(ags_type_hindicator);
}

void
ags_hindicator_class_init(AgsHIndicatorClass *indicator)
{
  GtkWidgetClass *widget;

  ags_hindicator_parent_class = g_type_class_peek_parent(indicator);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) indicator;

  widget->expose_event = ags_hindicator_expose;
  widget->configure_event = ags_hindicator_configure;
}

void
ags_hindicator_init(AgsHIndicator *indicator)
{
  gtk_widget_set_size_request((GtkWidget *) indicator,
			      100,
			      16);
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
  //  g_message("draw %f\0", adjustment->value);

  cr = gdk_cairo_create(widget->window);

  if(cr == NULL){
    return;
  }
  
  width = 100;
  height = 16;

  segment_width = 7;
  segment_height = height;

  padding = 3;

  cairo_surface_flush(cairo_get_target(cr));
  cairo_push_group(cr);

  for(i = 0; i < width / (segment_width + padding); i++){
    if(adjustment->value > 0.0 &&
       (1.0 / adjustment->value * i < (width / (segment_width + padding)))){
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
 * Since: 1.0.0
 */
AgsHIndicator*
ags_hindicator_new()
{
  AgsHIndicator *indicator;
  GtkAdjustment *adjustment;

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0);

  indicator = (AgsHIndicator *) g_object_new(AGS_TYPE_HINDICATOR,
					     "adjustment\0", adjustment,
					     NULL);
  
  return(indicator);
}
