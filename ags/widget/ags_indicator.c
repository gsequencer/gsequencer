/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include "ags_indicator.h"

void ags_indicator_class_init(AgsIndicatorClass *indicator);
void ags_indicator_orientable_interface_init(AgsOrientableInterface *orientable);
void ags_indicator_init(AgsIndicator *indicator);
void ags_indicator_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec);
void ags_indicator_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec);
void ags_indicator_snapshot(AgsVIndicator *indicator,
			    GtkSnapshot *snapshot);

void ags__indicator_draw(AgsVIndicator *indicator,
			 cairo_t *cr,
			 gboolean is_animation);

void ags_indicator_show(GtkWidget *widget);

void ags_indicator_map(GtkWidget *widget);
void ags_indicator_realize(GtkWidget *widget);
void ags_indicator_size_allocate(GtkWidget *widget,
				 GtkAllocation *allocation);
void ags_indicator_get_preferred_width(GtkWidget *widget,
				       gint *minimal_width,
				       gint *natural_width);
void ags_indicator_get_preferred_height(GtkWidget *widget,
					gint *minimal_height,
					gint *natural_height);

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
  PROP_ORIENTATION,
  PROP_SEGMENT_WIDTH,
  PROP_SEGMENT_HEIGHT,
  PROP_SEGMENT_PADDING,
  PROP_SEGMENT_COUNT,
  PROP_ADJUSTMENT,
};

static gpointer ags_indicator_parent_class = NULL;

GType
ags_indicator_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_indicator = 0;

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

    static const GInterfaceInfo ags_orientable_interface_info = {
      (GInterfaceInitFunc) ags_indicator_orientable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_indicator = g_type_register_static(GTK_TYPE_WIDGET,
						"AgsIndicator", &ags_indicator_info,
						0);
    
    g_type_add_interface_static(ags_type_indicator,
				AGS_TYPE_ORIENTABLE,
				&ags_orientable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_indicator);
  }

  return g_define_type_id__volatile;
}

void
ags_indicator_orientable_interface_init(AgsOrientableInterface *orientable)
{
  //empty
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

  g_object_class_override_property(gobject, PROP_ORIENTATION, "orientation");
  
  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) indicator;

  widget->snapshot = ags_indicator_snapshot;
  
  widget->realize = ags_indicator_realize;
  widget->size_allocate = ags_indicator_size_allocate;
  widget->get_preferred_width = ags_indicator_get_preferred_width;
  widget->get_preferred_height = ags_indicator_get_preferred_height;
  widget->show = ags_indicator_show;

  /* properties */
  /**
   * AgsIndicator:segment-width:
   *
   * The indicator's segment width.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("segment-width",
				 "segment width",
				 "The indicator's segment width",
				 1,
				 G_MAXUINT,
				 AGS_INDICATOR_DEFAULT_SEGMENT_WIDTH,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SEGMENT_WIDTH,
				  param_spec);

  /**
   * AgsIndicator:segment-height:
   *
   * The indicator's segment height.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("segment-height",
				 "segment height",
				 "The indicator's segment height",
				 1,
				 G_MAXUINT,
				 AGS_INDICATOR_DEFAULT_SEGMENT_HEIGHT,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SEGMENT_HEIGHT,
				  param_spec);

  /**
   * AgsIndicator:segment-padding:
   *
   * The indicator's segment padding.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("segment-padding",
				 "segment padding",
				 "The indicator's segment padding",
				 0,
				 G_MAXUINT,
				 AGS_INDICATOR_DEFAULT_SEGMENT_PADDING,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SEGMENT_PADDING,
				  param_spec);

  /**
   * AgsIndicator:segment-count:
   *
   * The indicator's segment count.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("segment-count",
				 "segment count",
				 "The indicator's segment count",
				 1,
				 G_MAXUINT,
				 AGS_INDICATOR_DEFAULT_SEGMENT_COUNT,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SEGMENT_COUNT,
				  param_spec);

  /**
   * AgsIndicator:adjustment:
   *
   * The adjustment giving indicator value.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("adjustment",
				   "assigned adjustment",
				   "The adjustment it is assigned with",
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
	       "app-paintable", TRUE,
	       NULL);

  indicator->orientation = GTK_ORIENTATION_VERTICAL;
  
  /* segment alignment */
  indicator->segment_width = AGS_INDICATOR_DEFAULT_SEGMENT_WIDTH;
  indicator->segment_height = AGS_INDICATOR_DEFAULT_SEGMENT_HEIGHT;
  
  indicator->segment_padding = AGS_INDICATOR_DEFAULT_SEGMENT_PADDING;

  indicator->segment_count = AGS_INDICATOR_DEFAULT_SEGMENT_COUNT;

  /* adjustment */
  indicator->adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 10.0, 0.01, 0.01, 0.01);
  g_object_ref(indicator->adjustment);
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
  case PROP_ORIENTATION:
  {
    GtkOrientation orientation;

    orientation = g_value_get_enum(value);

    if(orientation != indicator->orientation){
      GtkLayoutManager *layout;

      layout = gtk_widget_get_layout_manager(GTK_WIDGET(indicator));
      
      gtk_orientable_set_orientation(GTK_ORIENTABLE(layout),
				     orientation);

      indicator->orientation = orientation;
      
      gtk_widget_update_orientation(GTK_WIDGET(indicator),
				    indicator->orientation);
      
      gtk_widget_queue_resize(GTK_WIDGET(indicator));

      g_object_notify_by_pspec(gobject,
			       pspec);
      
      gtk_accessible_update_property(GTK_ACCESSIBLE(indicator),
				     GTK_ACCESSIBLE_PROPERTY_ORIENTATION, orientation,
				     -1);
    }
  }
  break;
  case PROP_SEGMENT_WIDTH:
  {
    indicator->segment_width = g_value_get_uint(value);
  }
  break;
  case PROP_SEGMENT_HEIGHT:
  {
    indicator->segment_height = g_value_get_uint(value);
  }
  break;
  case PROP_SEGMENT_PADDING:
  {
    indicator->segment_padding = g_value_get_uint(value);
  }
  break;
  case PROP_SEGMENT_COUNT:
  {
    indicator->segment_count = g_value_get_uint(value);
  }
  break;
  case PROP_ADJUSTMENT:
    {
      GtkAdjustment *adjustment;

      adjustment = (GtkAdjustment *) g_value_get_object(value);

      if(indicator->adjustment == adjustment){
	return;
      }

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
  case PROP_ORIENTATION:
  {
    g_value_set_enum(value, indicator->orientation);
  }
  break;
  case PROP_SEGMENT_WIDTH:
  {
    g_value_set_uint(value, indicator->segment_width);
  }
  break;
  case PROP_SEGMENT_HEIGHT:
  {
    g_value_set_uint(value, indicator->segment_height);
  }
  break;
  case PROP_SEGMENT_PADDING:
  {
    g_value_set_uint(value, indicator->segment_padding);
  }
  break;
  case PROP_SEGMENT_COUNT:
  {
    g_value_set_uint(value, indicator->segment_count);
  }
  break;
  case PROP_ADJUSTMENT:
  {
    g_value_set_object(value, indicator->adjustment);
  }
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
    
    gdk_window_show(gtk_widget_get_window(widget));
    //    ags_indicator_draw(widget);
  }
}

void
ags_indicator_realize(GtkWidget *widget)
{
  AgsIndicator *indicator;

  GdkWindow *window;

  GtkAllocation allocation;
  GdkWindowAttr attributes;
  
  gint attributes_mask;
  gint buttons_width;
  gint border_left, border_top;

  g_return_if_fail(widget != NULL);
  g_return_if_fail(AGS_IS_INDICATOR(widget));

  indicator = AGS_INDICATOR(widget);

  gtk_widget_set_realized(widget, TRUE);

  gtk_widget_get_allocation(widget,
			    &allocation);
  
  /*  */
  attributes.window_type = GDK_WINDOW_CHILD;
  
  attributes.x = allocation.x;
  attributes.y = allocation.y;
  attributes.width = allocation.width;
  attributes.height = allocation.height;

  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL;

  attributes.window_type = GDK_WINDOW_CHILD;

  attributes.wclass = GDK_INPUT_OUTPUT;
  attributes.visual = gtk_widget_get_visual (widget);
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

  window = gdk_window_new(gtk_widget_get_parent_window (widget),
			  &attributes, attributes_mask);

  gtk_widget_register_window(widget, window);
  gtk_widget_set_window(widget, window);

  gtk_widget_queue_resize(widget);
}

void
ags_indicator_size_allocate(GtkWidget *widget,
			    GtkAllocation *allocation)
{
  GTK_WIDGET_CLASS(ags_indicator_parent_class)->size_allocate(widget, allocation);

  /* implement me */
  //TODO:JK:
}

void
ags_indicator_get_preferred_width(GtkWidget *widget,
				  gint *minimal_width,
				  gint *natural_width)
{
  GTK_WIDGET_CLASS(ags_indicator_parent_class)->get_preferred_width(widget,
								    minimal_width,
								    natural_width);
}

void
ags_indicator_get_preferred_height(GtkWidget *widget,
				   gint *minimal_height,
				   gint *natural_height)
{
  GTK_WIDGET_CLASS(ags_indicator_parent_class)->get_preferred_height(widget,
								     minimal_height,
								     natural_height);
}

void
ags_indicator_show(GtkWidget *widget)
{
  GTK_WIDGET_CLASS(ags_indicator_parent_class)->show(widget);
}

void
ags_indicator_snapshot(AgsVIndicator *indicator,
		       GtkSnapshot *snapshot)
{
  cairo_t *cr;

  graphene_rect_t rect;
  
  int width, height;

  width = gtk_widget_get_width((GtkWidget *) indicator);
  height = gtk_widget_get_height((GtkWidget *) indicator);
  
  graphene_rect_init(&rect,
		     0.0, 0.0,
		     (float) width, (float) height);
  
  cr = gtk_snapshot_append_cairo(snapshot,
				 &rect);

  ags_indicator_draw(indicator,
		     cr,
		     FALSE);
  
  cairo_destroy(cr);
}

void
ags_indicator_draw(AgsVIndicator *indicator,
		   cairo_t *cr,
		   gboolean is_animation)
{
  GtkStyleContext *style_context;
  GtkSettings *settings;

  GtkAdjustment *adjustment;

  GdkRGBA fg_color;
  GdkRGBA bg_color;

  GtkOrientation orientation;

  int width, height;
  gint padding_top, padding_left;
  guint segment_width, segment_height;
  guint segment_padding;
  guint segment_count;
  guint i;
  gboolean dark_theme;
  gboolean fg_success;
  gboolean bg_success;
  
  style_context = gtk_widget_get_style_context((GtkWidget *) indicator);
  
  settings = gtk_settings_get_default();

  dark_theme = TRUE;
  
  g_object_set(settings,
	       "gtk-application-prefer-dark-theme", &dark_theme,
	       NULL);

  adjustment = ags_indicator_get_adjustment(indicator);  

  width = gtk_widget_get_width((GtkWidget *) indicator);
  height = gtk_widget_get_height((GtkWidget *) indicator);

  orientation = gtk_orientable_get_orientation(GTK_ORIENTABLE(indicator));

  segment_width = ags_indicator_get_segment_width(indicator);
  segment_height = ags_indicator_get_segment_height(indicator);

  segment_padding = ags_indicator_get_segment_padding(indicator);

  segment_count = ags_indicator_get_segment_count(indicator);
  
  if(orientation = GTK_ORIENTATION_VERTICAL){
    padding_top = (height - (segment_count * segment_height + (segment_count - 1) * segment_padding)) / 2;
    padding_left = (width - segment_width) / 2;
  }else{
    padding_top = (height - segment_height) / 2;
    padding_left = (width - (segment_count * segment_width + (segment_count - 1) * segment_padding)) / 2;
  }
  
  fg_success = gtk_style_context_lookup_color(style_context,
					      "theme_fg_color",
					      &fg_color);
    
  bg_success = gtk_style_context_lookup_color(style_context,
					      "theme_bg_color",
					      &bg_color);

  if(!fg_success ||
     !bg_success){
    gdk_rgba_parse(&fg_color,
		   "#101010");

    gdk_rgba_parse(&bg_color,
		   "#cbd5d9");
  }

  cairo_push_group(cr);

  /* clear bg */
  if(!is_animation){
    gtk_render_background(style_context,
			  cr,
			  0.0, 0.0,
			  (gdouble) width, (gdouble) height);
  }

  cairo_set_source_rgba(cr,
			bg_color->red,
			bg_color->green,
			bg_color->blue,
			bg_color->alpha);
    
  if(orientation = GTK_ORIENTATION_VERTICAL){
    for(i = 0; i < segment_count; i++){
      cairo_rectangle(cr,
		      (double) padding_left, (double) padding_top + (height - i * (segment_height + padding) - segment_height),
		      (double) segment_width, (double) segment_height);	

      cairo_fill(cr);
    }
  }else{
    for(i = 0; i < segment_count; i++){
      cairo_rectangle(cr,
		      (double) padding_top + (width - i * (segment_width + padding) - segment_width), (double) padding_top,
		      (double) segment_width, (double) segment_height);

      cairo_fill(cr);
    }
  }

  if(is_animation){
    gdouble value;

    value = gtk_adjustment_get_value(adjustment);
        
    if(!dark_theme){
      cairo_set_source_rgba(cr,
			    0.0,
			    0.0,
			    0.0,
			    1.0 / 3.0);
    }else{
      cairo_set_source_rgba(cr,
			    1.0,
			    1.0,
			    1.0,
			    1.0 / 3.0);
    }
    
    if(orientation = GTK_ORIENTATION_VERTICAL){
      for(i = 0; i < segment_count; i++){
	if(value > 0.0 &&
	   1.0 / value * i < segment_count){
	  cairo_rectangle(cr,
			  (double) padding_left, (double) padding_top + (height - i * (segment_height + padding) - segment_height),
			  (double) segment_width, (double) segment_height);	
	
	  cairo_fill(cr);
	}
      }
    }else{
      for(i = 0; i < segment_count; i++){
	if(value > 0.0 &&
	   1.0 / value * i < segment_count){
	  cairo_rectangle(cr,
			  (double) padding_top + (width - i * (segment_width + padding) - segment_width), (double) padding_top,
			  (double) segment_width, (double) segment_height);

	  cairo_fill(cr);
	}
      }
    }
  }

  cairo_pop_group_to_source(cr);
  cairo_paint(cr);
}

/**
 * ags_indicator_get_segment_width:
 * @indicator: the #AgsIndicator
 * 
 * Get segment width.
 * 
 * Returns: the segment width
 * 
 * Since: 3.2.0
 */
guint
ags_indicator_get_segment_width(AgsIndicator *indicator)
{
  guint segment_width;

  if(!AGS_IS_INDICATOR(indicator)){
    return(0);
  }

  g_object_get(indicator,
	       "segment-width", &segment_width,
	       NULL);

  return(segment_width);
}

/**
 * ags_indicator_set_segment_width:
 * @indicator: the #AgsIndicator
 * @segment_width: the segment width
 * 
 * Set segment width.
 * 
 * Since: 3.2.0
 */
void
ags_indicator_set_segment_width(AgsIndicator *indicator,
				guint segment_width)
{
  if(!AGS_IS_INDICATOR(indicator)){
    return;
  }

  g_object_set(indicator,
	       "segment-width", segment_width,
	       NULL);
}

/**
 * ags_indicator_get_segment_height:
 * @indicator: the #AgsIndicator
 * 
 * Get segment height.
 * 
 * Returns: the segment height
 * 
 * Since: 3.2.0
 */
guint
ags_indicator_get_segment_height(AgsIndicator *indicator)
{
  guint segment_height;
  
  if(!AGS_IS_INDICATOR(indicator)){
    return(0);
  }

  g_object_get(indicator,
	       "segment-height", &segment_height,
	       NULL);

  return(segment_height);
}

/**
 * ags_indicator_set_segment_height:
 * @indicator: the #AgsIndicator
 * @segment_height: the segment height
 * 
 * Set segment height.
 * 
 * Since: 3.2.0
 */
void
ags_indicator_set_segment_height(AgsIndicator *indicator,
				 guint segment_height)
{
  if(!AGS_IS_INDICATOR(indicator)){
    return;
  }

  g_object_set(indicator,
	       "segment-height", segment_height,
	       NULL);
}

/**
 * ags_indicator_get_segment_padding:
 * @indicator: the #AgsIndicator
 * 
 * Get segment padding.
 * 
 * Returns: the segment padding
 * 
 * Since: 3.2.0
 */
guint
ags_indicator_get_segment_padding(AgsIndicator *indicator)
{
  guint segment_padding;
  
  if(!AGS_IS_INDICATOR(indicator)){
    return(0);
  }

  g_object_get(indicator,
	       "segment-padding", &segment_padding,
	       NULL);

  return(segment_padding);
}

/**
 * ags_indicator_set_segment_padding:
 * @indicator: the #AgsIndicator
 * @segment_padding: the segment padding
 * 
 * Set segment padding.
 * 
 * Since: 3.2.0
 */
void
ags_indicator_set_segment_padding(AgsIndicator *indicator,
				  guint segment_padding)
{
  if(!AGS_IS_INDICATOR(indicator)){
    return;
  }

  g_object_set(indicator,
	       "segment-padding", segment_padding,
	       NULL);
}

/**
 * ags_indicator_get_segment_count:
 * @indicator: the #AgsIndicator
 * 
 * Get segment count.
 * 
 * Returns: the segment count
 * 
 * Since: 3.2.0
 */
guint
ags_indicator_get_segment_count(AgsIndicator *indicator)
{
  guint segment_count;
  
  if(!AGS_IS_INDICATOR(indicator)){
    return(0);
  }

  g_object_get(indicator,
	       "segment-count", &segment_count,
	       NULL);

  return(segment_count);
}

/**
 * ags_indicator_set_segment_count:
 * @indicator: the #AgsIndicator
 * @segment_count: the segment count
 * 
 * Set segment count.
 * 
 * Since: 3.2.0
 */
void
ags_indicator_set_segment_count(AgsIndicator *indicator,
				     guint segment_count)
{
  if(!AGS_IS_INDICATOR(indicator)){
    return;
  }

  g_object_set(indicator,
	       "segment-count", segment_count,
	       NULL);
}

/**
 * ags_indicator_get_adjustment:
 * @indicator: the #AgsIndicator
 * 
 * Get adjustment.
 * 
 * Returns: (transfer full): the #GtkAdjustment
 * 
 * Since: 3.2.0
 */
GtkAdjustment*
ags_indicator_get_adjustment(AgsIndicator *indicator)
{
  GtkAdjustment *adjustment;
  
  if(!AGS_IS_INDICATOR(indicator)){
    return(NULL);
  }

  g_object_get(indicator,
	       "adjustment", &adjustment,
	       NULL);

  return(adjustment);
}

/**
 * ags_indicator_set_adjustment:
 * @indicator: the #AgsIndicator
 * @adjustment: (transfer none): the #GtkAdjustment
 * 
 * Set adjustment.
 * 
 * Since: 3.2.0
 */
void
ags_indicator_set_adjustment(AgsIndicator *indicator,
			     GtkAdjustment *adjustment)
{
  if(!AGS_IS_INDICATOR(indicator)){
    return;
  }

  g_object_set(indicator,
	       "adjustment", adjustment,
	       NULL);
}

/**
 * ags_indicator_new:
 * @orientation: the #GtkOrientation
 * @segment_width: the width of one segment
 * @segment_height: the height of one segment
 *
 * Creates an #AgsIndicator. Note, use rather its implementation #AgsVIndicator or
 * #AgsHIndicator.
 *
 * Returns: a new #AgsIndicator
 *
 * Since: 3.0.0
 */
AgsIndicator*
ags_indicator_new(GtkOrientation orientation,
		  guint segment_width,
		  guint segment_height)
{
  AgsIndicator *indicator;

  indicator = (AgsIndicator *) g_object_new(AGS_TYPE_INDICATOR,
					    "orientation", orientation,
					    "segment-width", segment_width,
					    "segment-height", segment_height,					    
					    NULL);
  
  return(indicator);
}
