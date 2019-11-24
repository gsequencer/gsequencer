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

void ags_vindicator_get_preferred_width(GtkWidget *widget,
					gint *minimal_width,
					gint *natural_width);
void ags_vindicator_get_preferred_height(GtkWidget *widget,
					 gint *minimal_height,
					 gint *natural_height);
void ags_vindicator_size_allocate(GtkWidget *widget,
				  GtkAllocation *allocation);

gboolean ags_vindicator_configure(GtkWidget *widget,
				  GdkEventConfigure *event);

void ags_vindicator_draw(AgsVIndicator *indicator, cairo_t *cr);

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

  widget->get_preferred_width = ags_vindicator_get_preferred_width;
  widget->get_preferred_height = ags_vindicator_get_preferred_height;
  widget->size_allocate = ags_vindicator_size_allocate;

  widget->draw = ags_vindicator_draw;
  widget->configure_event = ags_vindicator_configure;
}

void
ags_vindicator_init(AgsVIndicator *indicator)
{
  g_object_set(indicator,
	       "segment-width", AGS_VINDICATOR_DEFAULT_SEGMENT_WIDTH,
	       "segment-height", AGS_VINDICATOR_DEFAULT_SEGMENT_HEIGHT,
	       NULL);
}


void
ags_vindicator_get_preferred_width(GtkWidget *widget,
				   gint *minimal_width,
				   gint *natural_width)
{
  AgsIndicator *indicator;

  indicator = AGS_INDICATOR(widget);
  
  minimal_width[0] =
    natural_width[0] = indicator->segment_width;
}

void
ags_vindicator_get_preferred_height(GtkWidget *widget,
				    gint *minimal_height,
				    gint *natural_height)
{
  AgsIndicator *indicator;

  indicator = AGS_INDICATOR(widget);
  
  minimal_height[0] =
    natural_height[0] = (indicator->segment_count * indicator->segment_height) + ((indicator->segment_count - 1) * indicator->segment_padding);
}

void
ags_vindicator_size_allocate(GtkWidget *widget,
			     GtkAllocation *allocation)
{
  AgsIndicator *indicator;

  indicator = AGS_INDICATOR(widget);

  allocation->width = indicator->segment_width;
  allocation->height = (indicator->segment_count * indicator->segment_height) + ((indicator->segment_count - 1) * indicator->segment_padding);

  GTK_WIDGET_CLASS(ags_vindicator_parent_class)->size_allocate(widget, allocation);
}

gboolean
ags_vindicator_configure(GtkWidget *widget,
			 GdkEventConfigure *event)
{
  gtk_widget_queue_draw(widget);

  return(FALSE);
}

void
ags_vindicator_draw(AgsVIndicator *vindicator, cairo_t *cr)
{
  GtkWidget *widget;
  
  GtkAdjustment *adjustment;
  
  GtkStyleContext *vindicator_style_context;

  GtkAllocation allocation;
  
  GdkRGBA *fg_color;
  GdkRGBA *bg_color;
  GdkRGBA *border_color;

  guint width, height;
  guint padding_top, padding_left;
  guint segment_width, segment_height;
  guint padding;
  guint i;

  GValue value = {0,};
  
  widget = GTK_WIDGET(vindicator);
  
  adjustment = AGS_INDICATOR(vindicator)->adjustment;

  if(adjustment == NULL){
    return;
  }
  
  gtk_widget_get_allocation(GTK_WIDGET(vindicator),
			    &allocation);

  /* style context */
  vindicator_style_context = gtk_widget_get_style_context(GTK_WIDGET(vindicator));

  gtk_style_context_get_property(vindicator_style_context,
				 "color",
				 GTK_STATE_FLAG_NORMAL,
				 &value);

  fg_color = g_value_get_boxed(&value);
  g_value_unset(&value);

  gtk_style_context_get_property(vindicator_style_context,
				 "background-color",
				 GTK_STATE_FLAG_NORMAL,
				 &value);

  bg_color = g_value_get_boxed(&value);
  g_value_unset(&value);
  
  gtk_style_context_get_property(vindicator_style_context,
				 "border-color",
				 GTK_STATE_FLAG_NORMAL,
				 &value);

  border_color = g_value_get_boxed(&value);
  g_value_unset(&value);

  //  g_message("draw %f", adjustment->value);
  
  width = AGS_INDICATOR(vindicator)->segment_width;
  height = (AGS_INDICATOR(vindicator)->segment_count * AGS_INDICATOR(vindicator)->segment_height) + ((AGS_INDICATOR(vindicator)->segment_count - 1) * AGS_INDICATOR(vindicator)->segment_padding);

  padding_top = (allocation.height - height) / 2;
  padding_left = (allocation.width - width) / 2;
  
  segment_width = AGS_INDICATOR(vindicator)->segment_width;
  segment_height = AGS_INDICATOR(vindicator)->segment_height;

  padding = AGS_INDICATOR(vindicator)->segment_padding;

  cairo_surface_flush(cairo_get_target(cr));
  cairo_push_group(cr);

  for(i = 0; i < AGS_INDICATOR(vindicator)->segment_count; i++){
    if(gtk_adjustment_get_value(adjustment) > 0.0 &&
       (1.0 / gtk_adjustment_get_value(adjustment) * i < AGS_INDICATOR(vindicator)->segment_count)){
      /* active */
      cairo_set_source_rgba(cr,
			    fg_color->red,
			    fg_color->green,
			    fg_color->blue,
			    fg_color->alpha);
    }else{
      /* normal */
      cairo_set_source_rgba(cr,
			    bg_color->red,
			    bg_color->green,
			    bg_color->blue,
			    bg_color->alpha);
    }

    cairo_rectangle(cr,
		    padding_left, padding_top + (height - i * (segment_height + padding) - segment_height),
		    segment_width, segment_height);
    cairo_fill(cr);

    /* outline */
    cairo_set_source_rgba(cr,
			  border_color->red,
			  border_color->green,
			  border_color->blue,
			  border_color->alpha);

    cairo_rectangle(cr,
		    padding_left, padding_top + (height - i * (segment_height + padding) - segment_height),
		    segment_width, segment_height);
    cairo_stroke(cr);
  }

  cairo_pop_group_to_source(cr);
  cairo_paint(cr);

  cairo_surface_mark_dirty(cairo_get_target(cr));
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
