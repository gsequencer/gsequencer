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

#include "ags_indicator.h"

void ags_indicator_class_init(AgsIndicatorClass *indicator);
void ags_indicator_init(AgsIndicator *indicator);
void ags_indicator_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec);
void ags_indicator_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec);
void ags_indicator_show(GtkWidget *widget);

void ags_indicator_map(GtkWidget *widget);
void ags_indicator_realize(GtkWidget *widget);
void ags_indicator_size_request(GtkWidget *widget,
				GtkRequisition   *requisition);
void ags_indicator_size_allocate(GtkWidget *widget,
				 GtkAllocation *allocation);

/**
 * SECTION:ags_indicator
 * @short_description: A indicator widget
 * @title: AgsIndicator
 * @section_id:
 * @include: ags/widget/ags_indicator.h
 *
 * #AgsIndicator is a widget representing a #GtkAdjustment.
 */

enum{
  PROP_0,
  PROP_ADJUSTMENT,
};

static gpointer ags_indicator_parent_class = NULL;

GType
ags_indicator_get_type(void)
{
  static GType ags_type_indicator = 0;

  if(!ags_type_indicator){
    static const GTypeInfo ags_indicator_info = {
      sizeof(AgsIndicatorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_indicator_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsIndicator),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_indicator_init,
    };

    ags_type_indicator = g_type_register_static(GTK_TYPE_WIDGET,
						"AgsIndicator\0", &ags_indicator_info,
						0);
  }

  return(ags_type_indicator);
}

void
ags_indicator_class_init(AgsIndicatorClass *indicator)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_indicator_parent_class = g_type_class_peek_parent(indicator);

  /* GObjectClass */
  gobject = (GObjectClass *) indicator;

  gobject->set_property = ags_indicator_set_property;
  gobject->get_property = ags_indicator_get_property;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) indicator;

  widget->realize = ags_indicator_realize;
  widget->size_request = ags_indicator_size_request;
  widget->size_allocate = ags_indicator_size_allocate;
  widget->show = ags_indicator_show;

  /* properties */
  param_spec = g_param_spec_object("adjustment\0",
				   "assigned adjustment\0",
				   "The adjustment it is assigned with\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ADJUSTMENT,
				  param_spec);
}

void
ags_indicator_init(AgsIndicator *indicator)
{
  g_object_set(G_OBJECT(indicator),
	       "app-paintable\0", TRUE,
	       NULL);

  indicator->adjustment = NULL;
}

void
ags_indicator_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec)
{
  AgsIndicator *indicator;

  indicator = AGS_INDICATOR(gobject);

  switch(prop_id){
  case PROP_ADJUSTMENT:
    {
      GtkAdjustment *adjustment;

      adjustment = (GtkAdjustment *) g_value_get_object(value);

      if(indicator->adjustment == adjustment)
	return;

      if(indicator->adjustment != NULL){
	g_object_unref(G_OBJECT(indicator->adjustment));
      }

      if(adjustment != NULL){
	g_object_ref(G_OBJECT(adjustment));
      }

      indicator->adjustment = adjustment;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_indicator_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec)
{
  AgsIndicator *indicator;

  indicator = AGS_INDICATOR(gobject);

  switch(prop_id){
  case PROP_ADJUSTMENT:
    g_value_set_object(value, indicator->adjustment);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_indicator_map(GtkWidget *widget)
{
  if (gtk_widget_get_realized (widget) && !gtk_widget_get_mapped (widget)) {
    GTK_WIDGET_CLASS (ags_indicator_parent_class)->map(widget);
    
    gdk_window_show(widget->window);
    //    ags_indicator_draw(widget);
  }
}

void
ags_indicator_realize(GtkWidget *widget)
{
  AgsIndicator *indicator;
  GdkWindowAttr attributes;
  gint attributes_mask;
  gint buttons_width;
  gint border_left, border_top;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (AGS_IS_INDICATOR (widget));

  indicator = AGS_INDICATOR(widget);

  gtk_widget_set_realized (widget, TRUE);

  /*  */
  attributes.window_type = GDK_WINDOW_CHILD;
  
  attributes.x = widget->allocation.x;
  attributes.y = widget->allocation.y;
  attributes.width = widget->allocation.width;
  attributes.height = widget->allocation.height;

  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

  attributes.window_type = GDK_WINDOW_CHILD;

  attributes.wclass = GDK_INPUT_OUTPUT;
  attributes.visual = gtk_widget_get_visual (widget);
  attributes.colormap = gtk_widget_get_colormap (widget);
  attributes.event_mask = gtk_widget_get_events (widget);
  attributes.event_mask |= (GDK_EXPOSURE_MASK |
                            GDK_BUTTON_PRESS_MASK |
                            GDK_BUTTON_RELEASE_MASK |
                            GDK_BUTTON1_MOTION_MASK |
                            GDK_BUTTON3_MOTION_MASK |
                            GDK_POINTER_MOTION_HINT_MASK |
                            GDK_POINTER_MOTION_MASK |
                            GDK_ENTER_NOTIFY_MASK |
                            GDK_LEAVE_NOTIFY_MASK);

  widget->window = gdk_window_new (gtk_widget_get_parent_window (widget),
				   &attributes, attributes_mask);
  gdk_window_set_user_data (widget->window, indicator);

  widget->style = gtk_style_attach (widget->style, widget->window);
  gtk_style_set_background (widget->style, widget->window, GTK_STATE_NORMAL);

  gtk_widget_queue_resize (widget);
}

void
ags_indicator_show(GtkWidget *widget)
{
  GTK_WIDGET_CLASS(ags_indicator_parent_class)->show(widget);
}

void
ags_indicator_size_request(GtkWidget *widget,
			   GtkRequisition *requisition)
{
  GTK_WIDGET_CLASS(ags_indicator_parent_class)->size_request(widget, requisition);

  /* implement me */
  //TODO:JK:
}

void
ags_indicator_size_allocate(GtkWidget *widget,
			    GtkAllocation *allocation)
{
  GTK_WIDGET_CLASS(ags_indicator_parent_class)->size_allocate(widget, allocation);

  /* implement me */
  //TODO:JK:
}

/**
 * ags_indicator_new:
 *
 * Creates an #AgsIndicator. Note, use rather its implementation #AgsVIndicator or
 * #AgsHIndicator.
 *
 * Returns: a new #AgsIndicator
 *
 * Since: 0.4
 */
AgsIndicator*
ags_indicator_new()
{
  AgsIndicator *indicator;

  indicator = (AgsIndicator *) g_object_new(AGS_TYPE_INDICATOR,
					    NULL);
  
  return(indicator);
}
