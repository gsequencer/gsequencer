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

#include <ags/widget/ags_ruler.h>

#include <pango/pango.h>

#ifndef AGS_WITH_QUARTZ
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

void ags_ruler_map(GtkWidget *widget);
void ags_ruler_realize(GtkWidget *widget);
void ags_ruler_size_request(GtkWidget *widget,
			    GtkRequisition   *requisition);
void ags_ruler_size_allocate(GtkWidget *widget,
			     GtkAllocation *allocation);
gboolean ags_ruler_expose(GtkWidget *widget,
			  GdkEventExpose *event);

void ags_ruler_draw(AgsRuler *ruler);

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
  PROP_ADJUSTMENT,
};

static gpointer ags_ruler_parent_class = NULL;

GType
ags_ruler_get_type()
{
  static GType ags_type_ruler = 0;

  if(!ags_type_ruler){
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
					    "AgsRuler\0", &ags_ruler_info,
					    0);
  }

  return(ags_type_ruler);
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

  //  widget->map = ags_ruler_map;
  widget->realize = ags_ruler_realize;
  widget->expose_event = ags_ruler_expose;
  widget->size_request = ags_ruler_size_request;
  widget->size_allocate = ags_ruler_size_allocate;
  widget->show = ags_ruler_show;

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
ags_ruler_init(AgsRuler *ruler)
{
  g_object_set(G_OBJECT(ruler),
  	       "app-paintable\0", TRUE,
  	       NULL);

  ruler->flags = 0;

  ruler->font_size = 14;

  ruler->adjustment = NULL;

  ruler->step = AGS_RULER_DEFAULT_STEP;

  ruler->factor = 16.0;
  ruler->precision = 1.0;
  ruler->scale_precision = 1.0;

  gtk_widget_set_size_request((GtkWidget *) ruler,
			      20,
			      24);
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
  case PROP_ADJUSTMENT:
    g_value_set_object(value, ruler->adjustment);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_ruler_map(GtkWidget *widget)
{
  if (gtk_widget_get_realized (widget) && !gtk_widget_get_mapped (widget)) {
    GTK_WIDGET_CLASS (ags_ruler_parent_class)->map(widget);
    
    gdk_window_show(widget->window);
    ags_ruler_draw((AgsRuler *) widget);
  }
}

void
ags_ruler_realize(GtkWidget *widget)
{
  AgsRuler *ruler;
  GdkWindowAttr attributes;
  gint attributes_mask;
  gint border_left, border_top;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (AGS_IS_RULER (widget));

  ruler = AGS_RULER(widget);

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
  gdk_window_set_user_data (widget->window, ruler);

  widget->style = gtk_style_attach (widget->style, widget->window);
  gtk_style_set_background (widget->style, widget->window, GTK_STATE_NORMAL);

  gtk_widget_queue_resize (widget);
}

void
ags_ruler_show(GtkWidget *widget)
{
  GTK_WIDGET_CLASS(ags_ruler_parent_class)->show(widget);
}

void
ags_ruler_size_request(GtkWidget *widget,
		       GtkRequisition *requisition)
{
  GTK_WIDGET_CLASS(ags_ruler_parent_class)->size_request(widget, requisition);

  /* implement me */
  //TODO:JK:
}

void
ags_ruler_size_allocate(GtkWidget *widget,
			GtkAllocation *allocation)
{
  GTK_WIDGET_CLASS(ags_ruler_parent_class)->size_allocate(widget, allocation);

  /* implement me */
  //TODO:JK:
}

gboolean
ags_ruler_expose(GtkWidget *widget,
		 GdkEventExpose *event)
{
  ags_ruler_draw(AGS_RULER(widget));

  return(FALSE);
}

/**
 * ags_ruler_draw:
 * @ruler an #AgsRuler
 *
 * draws the widget
 */
void
ags_ruler_draw(AgsRuler *ruler)
{
  GtkWidget *widget;

  cairo_t *cr;

  gchar *str;

  gdouble tact_factor, zoom_factor;
  gdouble tact;

  guint offset;
  guint step;
  guint x0;
  guint z;
  guint i, i_stop;

  auto void ags_ruler_draw_string(cairo_t *cr, gchar *str);

  void ags_ruler_draw_string(cairo_t *cr, gchar *str){
    PangoLayout *layout;
    PangoFontDescription *desc;

    layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout, str, -1);
    desc = pango_font_description_copy_static(NULL); //pango_font_description_from_string("Georgia Bold 11");
    pango_layout_set_font_description(layout, desc);
    pango_font_description_free(desc);

    pango_cairo_update_layout(cr, layout);
    pango_cairo_show_layout(cr, layout);

#ifndef AGS_WITH_QUARTZ
    pango_fc_font_map_cache_clear(pango_cairo_font_map_get_default());
#endif
    
    g_object_unref(layout);
  }
  
  widget = GTK_WIDGET(ruler);

  cr = gdk_cairo_create(widget->window);
  
  cairo_surface_flush(cairo_get_target(cr));
  cairo_push_group(cr);

  /* calculate base step */
  tact = ruler->precision;

  step = ruler->step * 0.25 * ruler->factor * ruler->precision;

  /* draw bg */
  cairo_set_source_rgba(cr, 0.0, 0.0, 0.125, 1.0);
  cairo_rectangle(cr,
		  0, 0,
		  widget->allocation.width, widget->allocation.height);
  cairo_fill(cr);

  cairo_set_source_rgba(cr,
			1.0,
			1.0,
			1.0,
			1.0);
  cairo_set_line_width(cr,
		       1.25);

  /* draw scale */
  offset = (ruler->adjustment->value * step);
  x0 = offset % step;
  
  z = (guint) floor((offset - x0) / step);
  
  i_stop = (guint) ceil((double) (widget->allocation.width + (step + x0)) / (double) step);
  
  for(i = 0; i < i_stop; i++, z++){
    cairo_move_to(cr,
		  (double) (i * step - x0),
		  (double) (widget->allocation.height));
    
    if(tact < 1.0){
      /* draw large step */
      cairo_set_line_width(cr,
			   1.75);

      cairo_line_to(cr,
		    (double) (i * step - x0),
		    (double) (widget->allocation.height - AGS_RULER_LARGE_STEP));

      /* draw scale step */
      cairo_move_to(cr,
		    (double) (i * step - x0),
		    (double) (widget->allocation.height - AGS_RULER_LARGE_STEP - 14));
      
      str = g_strdup_printf("%u\0",
			    (guint) ((gdouble) z / tact));
      ags_ruler_draw_string(cr, str);
      
      g_free(str);
    }else{
      if(z % (guint) floor(tact) == 0){
	/* draw large step */
	cairo_set_line_width(cr,
			     1.75);

	cairo_line_to(cr,
		      (double) (i * step - x0),
		      (double) (widget->allocation.height - AGS_RULER_LARGE_STEP));

	/* draw scale step */
	cairo_move_to(cr,
		      (double) (i * step - x0),
		      (double) (widget->allocation.height - AGS_RULER_LARGE_STEP - 14));

	str = g_strdup_printf("%u\0",
			      (guint) ((gdouble) z / tact));
	ags_ruler_draw_string(cr, str);
      
	g_free(str);
      }else{
	/* draw small step */
	cairo_set_line_width(cr,
			     1.25);

	cairo_line_to(cr,
		      (double) (i * step - x0),
		      (double) (widget->allocation.height - AGS_RULER_SMALL_STEP));
      }
    }

    cairo_stroke(cr);
  }

  cairo_pop_group_to_source(cr);
  cairo_paint(cr);

  cairo_surface_mark_dirty(cairo_get_target(cr));
  cairo_destroy(cr);
}

/**
 * ags_ruler_new:
 *
 * Creates an #AgsRuler
 *
 * Returns: a new #AgsRuler
 *
 * Since: 0.4
 */
AgsRuler*
ags_ruler_new()
{
  AgsRuler *ruler;
  GtkAdjustment *adjustment;

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0);

  ruler = (AgsRuler *) g_object_new(AGS_TYPE_RULER,
				    "adjustment\0", adjustment,
				    NULL);

  return(ruler);
}
