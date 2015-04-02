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

#include "ags_vindicator.h"

void ags_vindicator_class_init(AgsVIndicatorClass *indicator);
void ags_vindicator_init(AgsVIndicator *indicator);
void ags_vindicator_show(GtkWidget *widget);

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
  static GType ags_type_vindicator = 0;

  if(!ags_type_vindicator){
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
						 "AgsVIndicator\0", &ags_vindicator_info,
						 0);
  }

  return(ags_type_vindicator);
}

void
ags_vindicator_class_init(AgsVIndicatorClass *indicator)
{
  GtkWidgetClass *widget;

  ags_vindicator_parent_class = g_type_class_peek_parent(indicator);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) indicator;

  widget->expose_event = ags_vindicator_expose;
  widget->configure_event = ags_vindicator_configure;
}

void
ags_vindicator_init(AgsVIndicator *indicator)
{
  gtk_widget_set_size_request(indicator,
			      16,
			      100);
}

gboolean
ags_vindicator_configure(GtkWidget *widget,
			 GdkEventConfigure *event)
{
  ags_vindicator_draw(widget);

  return(FALSE);
}

gboolean
ags_vindicator_expose(GtkWidget *widget,
		     GdkEventExpose *event)
{
  ags_vindicator_draw(widget);

  return(FALSE);
}

void
ags_vindicator_draw(AgsVIndicator *indicator)
{
  GtkWidget *widget;
  GtkAdjustment *adjustment;
  cairo_t *cr;
  gdouble value;
  guint width, height;
  guint segment_width, segment_height;
  guint padding;
  guint i;

  widget = GTK_WIDGET(indicator);
  adjustment = AGS_INDICATOR(indicator)->adjustment;

  //  g_message("draw %f\0", adjustment->value);

  cr = gdk_cairo_create(widget->window);

  if(cr == NULL){
    return;
  }

  width = 16;
  height = 100;

  segment_width = 16;
  segment_height = 7;

  padding = 3;

  cairo_push_group(cr);

  for(i = 0; i < height / (segment_height + padding); i++){
    if(adjustment->value > 0.0 &&
       (1 / adjustment->value * i < (height / (segment_height + padding)))){
      /* active */
      cairo_set_source_rgba(cr, 0.9, 0.7, 0.2, 1.0);
    }else{
      /* normal */
      cairo_set_source_rgba(cr, 0.0, 0.0, 0.4, 1.0);
    }

    cairo_rectangle(cr,
		    0, height - i * (segment_height + padding) - segment_height,
		    segment_width, segment_height);
    cairo_fill(cr);

    cairo_set_source_rgba(cr, 0.0, 0.0, 1.0, 0.3);
    cairo_rectangle(cr,
		    0, height - i * (segment_height + padding) - segment_height,
		    segment_width, segment_height);
    cairo_stroke(cr);
  }

  cairo_pop_group_to_source(cr);
  cairo_paint(cr);

  cairo_destroy(cr);
}

/**
 * ags_vindicator_new:
 *
 * Creates an #AgsVIndicator.
 *
 * Since: 0.4
 */
AgsVIndicator*
ags_vindicator_new()
{
  AgsVIndicator *indicator;
  GtkAdjustment *adjustment;

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0);

  indicator = (AgsVIndicator *) g_object_new(AGS_TYPE_VINDICATOR,
					     "adjustment\0", adjustment,
					     NULL);
  
  return(indicator);
}
