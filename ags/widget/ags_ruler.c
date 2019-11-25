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

#include <ags/widget/ags_ruler.h>

#include <pango/pango.h>

#ifndef __APPLE__
#include <pango/pangofc-fontmap.h>
#endif

#include <math.h>

void ags_ruler_class_init(AgsRulerClass *ruler);
void ags_ruler_init(AgsRuler *ruler);
void ags_ruler_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec);
void ags_ruler_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec);
void ags_ruler_show(GtkWidget *widget);

void ags_ruler_realize(GtkWidget *widget);
void ags_ruler_size_allocate(GtkWidget *widget, GtkAllocation *allocation);
void ags_ruler_get_preferred_height(GtkWidget *widget,
				    gint *minimal_height,
				    gint *natural_height);

void ags_ruler_send_configure(AgsRuler *ruler);

void ags_ruler_draw(AgsRuler *ruler, cairo_t *cr);

/**
 * SECTION:ags_ruler
 * @short_description: A ruler widget
 * @title: AgsRuler
 * @section_id:
 * @include: ags/widget/ags_ruler.h
 *
 * #AgsRuler is a widget representing a #GtkAdjustment.
 */

enum{
  PROP_0,
  PROP_STEP,
  PROP_LARGE_STEP,
  PROP_SMALL_STEP,
  PROP_ADJUSTMENT,
};

static gpointer ags_ruler_parent_class = NULL;

GType
ags_ruler_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_ruler = 0;

    static const GTypeInfo ags_ruler_info = {
      sizeof(AgsRulerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_ruler_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsRuler),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_ruler_init,
    };

    ags_type_ruler = g_type_register_static(GTK_TYPE_WIDGET,
					    "AgsRuler", &ags_ruler_info,
					    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_ruler);
  }

  return g_define_type_id__volatile;
}

void
ags_ruler_class_init(AgsRulerClass *ruler)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_ruler_parent_class = g_type_class_peek_parent(ruler);

  /* GObjectClass */
  gobject = (GObjectClass *) ruler;

  gobject->set_property = ags_ruler_set_property;
  gobject->get_property = ags_ruler_get_property;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) ruler;

  widget->realize = ags_ruler_realize;
  widget->size_allocate = ags_ruler_size_allocate;
  widget->get_preferred_height = ags_ruler_get_preferred_height;
  widget->draw = ags_ruler_draw;
  widget->show = ags_ruler_show;

  /* properties */
  param_spec = g_param_spec_object("adjustment",
				   "assigned adjustment",
				   "The adjustment it is assigned with",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ADJUSTMENT,
				  param_spec);

  /**
   * AgsRuler:step:
   *
   * The step's width.
   * 
   * Since: 2.2.22
   */
  param_spec = g_param_spec_uint("step",
				 "step",
				 "The width of a step",
				 0,
				 G_MAXUINT,
				 AGS_RULER_DEFAULT_STEP,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_STEP,
				  param_spec);

  /**
   * AgsRuler:large-step:
   *
   * The large step's width.
   * 
   * Since: 2.2.22
   */
  param_spec = g_param_spec_uint("large-step",
				 "large step",
				 "The width of a large step",
				 0,
				 G_MAXUINT,
				 AGS_RULER_DEFAULT_LARGE_STEP,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LARGE_STEP,
				  param_spec);

  /**
   * AgsRuler:small-step:
   *
   * The small step's width.
   * 
   * Since: 2.2.22
   */
  param_spec = g_param_spec_uint("small-step",
				 "small step",
				 "The width of a small step",
				 0,
				 G_MAXUINT,
				 AGS_RULER_DEFAULT_SMALL_STEP,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SMALL_STEP,
				  param_spec);
}

void
ags_ruler_init(AgsRuler *ruler)
{
  g_object_set(G_OBJECT(ruler),
  	       "app-paintable", TRUE,
  	       NULL);

  ruler->flags = 0;

  ruler->font_size = AGS_RULER_FONT_SIZE;

  ruler->adjustment = NULL;

  ruler->step = AGS_RULER_DEFAULT_STEP;
  ruler->large_step = AGS_RULER_DEFAULT_LARGE_STEP;
  ruler->small_step = AGS_RULER_DEFAULT_SMALL_STEP;

  ruler->factor = 16.0;
  ruler->precision = 1.0;
  ruler->scale_precision = 1.0;
}


void
ags_ruler_set_property(GObject *gobject,
		       guint prop_id,
		       const GValue *value,
		       GParamSpec *param_spec)
{
  AgsRuler *ruler;

  ruler = AGS_RULER(gobject);

  switch(prop_id){
  case PROP_STEP:
  {
    ruler->step = g_value_get_uint(value);
  }
  break;
  case PROP_LARGE_STEP:
  {
    ruler->large_step = g_value_get_uint(value);
  }
  break;
  case PROP_SMALL_STEP:
  {
    ruler->small_step = g_value_get_uint(value);
  }
  break;
  case PROP_ADJUSTMENT:
    {
      GtkAdjustment *adjustment;

      adjustment = (GtkAdjustment *) g_value_get_object(value);

      if(ruler->adjustment == adjustment)
	return;

      if(ruler->adjustment != NULL){
	g_object_unref(G_OBJECT(ruler->adjustment));
      }

      if(adjustment != NULL){
	g_object_ref(G_OBJECT(adjustment));
      }

      ruler->adjustment = adjustment;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_ruler_get_property(GObject *gobject,
		       guint prop_id,
		       GValue *value,
		       GParamSpec *param_spec)
{
  AgsRuler *ruler;

  ruler = AGS_RULER(gobject);

  switch(prop_id){
  case PROP_STEP:
  {
    g_value_set_uint(value, ruler->step);
  }
  break;
  case PROP_LARGE_STEP:
  {
    g_value_set_uint(value, ruler->large_step);
  }
  break;
  case PROP_SMALL_STEP:
  {
    g_value_set_uint(value, ruler->small_step);
  }
  break;
  case PROP_ADJUSTMENT:
  {
    g_value_set_object(value, ruler->adjustment);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_ruler_realize(GtkWidget *widget)
{
  AgsRuler *ruler;

  GdkWindow *window;

  GtkAllocation allocation;
  GdkWindowAttr attributes;

  gint attributes_mask;
  gint border_left, border_top;

  g_return_if_fail(widget != NULL);
  g_return_if_fail(AGS_IS_RULER(widget));

  ruler = AGS_RULER(widget);

//  GTK_WIDGET_CLASS(ags_ruler_parent_class)->realize(widget);
  
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

  attributes.wclass = GDK_INPUT_OUTPUT;
  attributes.visual = gtk_widget_get_visual(widget);
  attributes.event_mask = gtk_widget_get_events(widget);
  attributes.event_mask |= (GDK_EXPOSURE_MASK |
                            GDK_BUTTON_PRESS_MASK |
                            GDK_BUTTON_RELEASE_MASK |
                            GDK_BUTTON1_MOTION_MASK |
                            GDK_BUTTON3_MOTION_MASK |
                            GDK_POINTER_MOTION_HINT_MASK |
                            GDK_POINTER_MOTION_MASK |
                            GDK_ENTER_NOTIFY_MASK |
                            GDK_LEAVE_NOTIFY_MASK);

  window = gdk_window_new(gtk_widget_get_parent_window(widget),
			  &attributes, attributes_mask);

  gtk_widget_register_window(widget, window);
  gtk_widget_set_window(widget, window);

  ags_ruler_send_configure(ruler);
}

void
ags_ruler_size_allocate(GtkWidget *widget, GtkAllocation *allocation)
{
  AgsRuler *ruler;
  
  g_return_if_fail(AGS_IS_RULER(widget));
  g_return_if_fail(allocation != NULL);

  ruler = AGS_RULER(widget);
  
  gtk_widget_set_allocation(widget, allocation);

  if(gtk_widget_get_realized(widget)){
    gdk_window_move_resize(gtk_widget_get_window(widget),
			   allocation->x, allocation->y,
			   allocation->width, allocation->height);

    ags_ruler_send_configure(ruler);
  }
}

void
ags_ruler_send_configure(AgsRuler *ruler)
{
  GtkAllocation allocation;
  GtkWidget *widget;
  GdkEvent *event = gdk_event_new (GDK_CONFIGURE);

  widget = GTK_WIDGET(ruler);
  gtk_widget_get_allocation(widget, &allocation);

  event->configure.window = g_object_ref(gtk_widget_get_window (widget));
  event->configure.send_event = TRUE;
  event->configure.x = allocation.x;
  event->configure.y = allocation.y;
  event->configure.width = allocation.width;
  event->configure.height = allocation.height;

  gtk_widget_event(widget, event);
  gdk_event_free(event);
}

void
ags_ruler_show(GtkWidget *widget)
{
  GTK_WIDGET_CLASS(ags_ruler_parent_class)->show(widget);
}

void
ags_ruler_get_preferred_height(GtkWidget *widget,
			       gint *minimal_height,
			       gint *natural_height)
{
  minimal_height[0] =
    natural_height[0] = (gint) AGS_RULER_DEFAULT_HEIGHT;
}

/**
 * ags_ruler_draw:
 * @ruler an #AgsRuler
 *
 * draws the widget
 */
void
ags_ruler_draw(AgsRuler *ruler, cairo_t *cr)
{
  GtkWidget *widget;
    
  GtkAllocation allocation;

  gchar *font_name;

  gdouble tact_factor, zoom_factor;
  gdouble tact;

  guint offset;
  guint step;
  guint x0;
  guint z;
  guint i, i_stop;
  
  widget = GTK_WIDGET(ruler);

//  GTK_WIDGET_CLASS(ags_ruler_parent_class)->draw(widget, cr);

  g_object_get(gtk_settings_get_default(),
	       "gtk-font-name", &font_name,
	       NULL);

  gtk_widget_get_allocation(widget,
			    &allocation);
    
  cairo_surface_flush(cairo_get_target(cr));
  cairo_push_group(cr);

  /* calculate base step */
  tact = ruler->precision;

  step = ruler->step * 0.25 * ruler->factor * ruler->precision;

  /* draw bg */
  cairo_set_source_rgba(cr,
			0.0,
			0.0,
			0.125,
			1.0);
  cairo_rectangle(cr,
		  0.0, 0.0,
		  (double) allocation.width, (double) allocation.height);
  cairo_fill(cr);

  cairo_set_source_rgba(cr,
			1.0,
			1.0,
			1.0,
			1.0);
  cairo_set_line_width(cr,
		       1.25);

  /* draw scale */
  offset = (gtk_adjustment_get_value(ruler->adjustment) * step);
  x0 = offset % step;
  
  z = (guint) floor((offset - x0) / step);
  
  i_stop = (guint) ceil((double) (allocation.width + (step + x0)) / (double) step);
  
  for(i = 0; i < i_stop; i++, z++){
    cairo_move_to(cr,
		  (double) (i * step - x0),
		  (double) (allocation.height));
    
    if(tact < 1.0){
      PangoLayout *layout;
      PangoFontDescription *desc;

      PangoRectangle ink_rect, logical_rect;

      gchar *text;

      text = g_strdup_printf("%u",
			     (guint) ((gdouble) z / tact));

      layout = pango_cairo_create_layout(cr);
      pango_layout_set_text(layout,
			    text,
			    -1);
      desc = pango_font_description_from_string(font_name);
      pango_font_description_set_size(desc,
				      ruler->font_size * PANGO_SCALE);
      pango_layout_set_font_description(layout,
					desc);
      pango_font_description_free(desc);    

      pango_layout_get_extents(layout,
			       &ink_rect,
			       &logical_rect);  
      
      /* draw large step */
      cairo_set_line_width(cr,
			   1.75);

      cairo_line_to(cr,
		    (double) (i * step - x0),
		    (double) (allocation.height - ruler->large_step));

      /* draw scale step */
      cairo_move_to(cr,
		    (double) (i * step - x0),
		    (double) (allocation.height - ruler->large_step - (ruler->font_size + AGS_RULER_FREE_SPACE)));
      
      pango_cairo_show_layout(cr,
			      layout);
      
      g_object_unref(layout);
      
      g_free(text);
    }else{
      if(z % (guint) floor(tact) == 0){
	PangoLayout *layout;
	PangoFontDescription *desc;

	PangoRectangle ink_rect, logical_rect;

	gchar *text;

	text = g_strdup_printf("%u",
			       (guint) ((gdouble) z / tact));
	
	layout = pango_cairo_create_layout(cr);
	pango_layout_set_text(layout,
			      text,
			      -1);
	desc = pango_font_description_from_string(font_name);
	pango_font_description_set_size(desc,
					ruler->font_size * PANGO_SCALE);
	pango_layout_set_font_description(layout,
					  desc);
	pango_font_description_free(desc);    

	pango_layout_get_extents(layout,
				 &ink_rect,
				 &logical_rect);

	/* draw large step */
	cairo_set_line_width(cr,
			     1.75);

	cairo_line_to(cr,
		      (double) (i * step - x0),
		      (double) (allocation.height - ruler->large_step));

	/* draw scale step */
	cairo_move_to(cr,
		      (double) (i * step - x0),
		      (double) (allocation.height - ruler->large_step - (ruler->font_size + AGS_RULER_FREE_SPACE)));
      
	pango_cairo_show_layout(cr,
				layout);
      
	g_object_unref(layout);
      
	g_free(text);
      }else{
	/* draw small step */
	cairo_set_line_width(cr,
			     1.25);

	cairo_line_to(cr,
		      (double) (i * step - x0),
		      (double) (allocation.height - ruler->small_step));
      }
    }

    cairo_stroke(cr);
  }

  g_free(font_name);

  cairo_pop_group_to_source(cr);
  cairo_paint(cr);

  cairo_surface_mark_dirty(cairo_get_target(cr));
}

/**
 * ags_ruler_new:
 *
 * Create a new instance of #AgsRuler
 *
 * Returns: the new #AgsRuler
 *
 * Since: 2.0.0
 */
AgsRuler*
ags_ruler_new()
{
  AgsRuler *ruler;
  GtkAdjustment *adjustment;

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0);

  ruler = (AgsRuler *) g_object_new(AGS_TYPE_RULER,
				    "adjustment", adjustment,
				    NULL);

  return(ruler);
}
