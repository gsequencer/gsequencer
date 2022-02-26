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

#include <ags/widget/ags_ruler.h>

#include <pango/pango.h>

#ifndef __APPLE__
#include <pango/pangofc-fontmap.h>
#endif

#include <math.h>

void ags_ruler_class_init(AgsRulerClass *ruler);
void ags_ruler_orientable_interface_init(GtkOrientableIface *orientable);
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

void ags_ruler_frame_clock_update_callback(GdkFrameClock *frame_clock,
					   AgsRuler *ruler);

void ags_ruler_snapshot(GtkWidget *widget,
			GtkSnapshot *snapshot);

void ags_ruler_draw(AgsRuler *ruler,
		    cairo_t *cr,
		    gboolean is_animation);

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
  PROP_ORIENTATION,
  PROP_STEP,
  PROP_LARGE_STEP,
  PROP_SMALL_STEP,
  PROP_FACTOR,
  PROP_PRECISION,
  PROP_SCALE_PRECISION,
  PROP_ADJUSTMENT,
  PROP_FONT_SIZE,
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

    static const GInterfaceInfo ags_orientable_interface_info = {
      (GInterfaceInitFunc) ags_ruler_orientable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_ruler = g_type_register_static(GTK_TYPE_WIDGET,
					    "AgsRuler", &ags_ruler_info,
					    0);

    g_type_add_interface_static(ags_type_ruler,
				GTK_TYPE_ORIENTABLE,
				&ags_orientable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_ruler);
  }

  return g_define_type_id__volatile;
}

void
ags_ruler_orientable_interface_init(GtkOrientableIface *orientable)
{
  //empty
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

  g_object_class_override_property(gobject, PROP_ORIENTATION, "orientation");

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) ruler;

  widget->realize = ags_ruler_realize;
    
  widget->snapshot = ags_ruler_snapshot;

  /* properties */
  /**
   * AgsRuler:font-size:
   *
   * The font size.
   * 
   * Since: 3.6.15
   */
  param_spec = g_param_spec_uint("font-size",
				 "font size",
				 "The font size",
				 0,
				 G_MAXUINT,
				 AGS_RULER_FONT_SIZE,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FONT_SIZE,
				  param_spec);

  /**
   * AgsRuler:adjustment:
   *
   * The adjustment.
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

  /**
   * AgsRuler:step:
   *
   * The step's width.
   * 
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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

  /**
   * AgsRuler:factor:
   *
   * The factor.
   * 
   * Since: 4.0.0
   */
  param_spec = g_param_spec_double("factor",
				   "factor",
				   "The factor",
				   0.0,
				   G_MAXDOUBLE,
				   16.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FACTOR,
				  param_spec);

  /**
   * AgsRuler:precision:
   *
   * The precision.
   * 
   * Since: 4.0.0
   */
  param_spec = g_param_spec_double("precision",
				   "precision",
				   "The precision",
				   0.0,
				   G_MAXDOUBLE,
				   1.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PRECISION,
				  param_spec);

  /**
   * AgsRuler:scale-precision:
   *
   * The scale precision.
   * 
   * Since: 4.0.0
   */
  param_spec = g_param_spec_double("scale-precision",
				   "scale precision",
				   "The scale precision",
				   0.0,
				   G_MAXDOUBLE,
				   AGS_RULER_DEFAULT_SCALE_PRECISION,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SCALE_PRECISION,
				  param_spec);
}

void
ags_ruler_init(AgsRuler *ruler)
{  
  ruler->orientation = GTK_ORIENTATION_HORIZONTAL;

  gtk_widget_set_hexpand(ruler,
			 TRUE);

  g_object_set(ruler,
	       "height-request", 24,
	       NULL);

  ruler->font_size = AGS_RULER_FONT_SIZE;

  ruler->adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 64.0, 0.1, 0.1, 0.0);

  ruler->step = AGS_RULER_DEFAULT_STEP;
  ruler->large_step = AGS_RULER_DEFAULT_LARGE_STEP;
  ruler->small_step = AGS_RULER_DEFAULT_SMALL_STEP;

  ruler->factor = AGS_RULER_DEFAULT_FACTOR;
  ruler->precision = AGS_RULER_DEFAULT_PRECISION;
  ruler->scale_precision = AGS_RULER_DEFAULT_SCALE_PRECISION;
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
  case PROP_ORIENTATION:
  {
    GtkOrientation orientation;

    orientation = g_value_get_enum(value);

    if(orientation != ruler->orientation){
      ruler->orientation = orientation;

      if(orientation == GTK_ORIENTATION_VERTICAL){
	gtk_widget_set_hexpand(ruler,
			       FALSE);
  
	gtk_widget_set_vexpand(ruler,
			       TRUE);
      }else{
	gtk_widget_set_hexpand(ruler,
			       TRUE);
  
	gtk_widget_set_vexpand(ruler,
			       FALSE);
      }
      
      gtk_widget_queue_resize(GTK_WIDGET(ruler));

      g_object_notify_by_pspec(gobject,
			       param_spec);
    }
  }
  break;
  case PROP_FONT_SIZE:
  {
    ruler->font_size = g_value_get_uint(value);
  }
  break;
  case PROP_STEP:
  {
    ruler->step = g_value_get_uint(value);
  }
  break;
  case PROP_FACTOR:
  {
    ruler->factor = g_value_get_double(value);
  }
  break;
  case PROP_PRECISION:
  {
    ruler->precision = g_value_get_double(value);
  }
  break;
  case PROP_SCALE_PRECISION:
  {
    ruler->scale_precision = g_value_get_double(value);
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
  case PROP_ORIENTATION:
  {
    g_value_set_enum(value, ruler->orientation);
  }
  break;
  case PROP_FONT_SIZE:
  {
    g_value_set_uint(value, ruler->font_size);
  }
  break;
  case PROP_STEP:
  {
    g_value_set_uint(value, ruler->step);
  }
  break;
  case PROP_FACTOR:
  {
    g_value_set_double(value, ruler->factor);
  }
  break;
  case PROP_PRECISION:
  {
    g_value_set_double(value, ruler->precision);
  }
  break;
  case PROP_SCALE_PRECISION:
  {
    g_value_set_double(value, ruler->scale_precision);
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
  GdkFrameClock *frame_clock;
  
  /* call parent */
  GTK_WIDGET_CLASS(ags_ruler_parent_class)->realize(widget);

  frame_clock = gtk_widget_get_frame_clock(widget);
  
  g_signal_connect(frame_clock, "update", 
		   G_CALLBACK(ags_ruler_frame_clock_update_callback), widget);

  gdk_frame_clock_begin_updating(frame_clock);
}

void
ags_ruler_frame_clock_update_callback(GdkFrameClock *frame_clock,
				      AgsRuler *ruler)
{
  gtk_widget_queue_draw((GtkWidget *) ruler);
}

void
ags_ruler_snapshot(GtkWidget *widget,
		   GtkSnapshot *snapshot)
{
  GtkStyleContext *style_context;

  cairo_t *cr;

  graphene_rect_t rect;
  
  int width, height;
  
  style_context = gtk_widget_get_style_context((GtkWidget *) widget);  

  width = gtk_widget_get_width(widget);
  height = gtk_widget_get_height(widget);
  
  graphene_rect_init(&rect,
		     0.0, 0.0,
		     (float) width, (float) height);
  
  cr = gtk_snapshot_append_cairo(snapshot,
				 &rect);
  
  /* clear bg */
  gtk_render_background(style_context,
			cr,
			0.0, 0.0,
			(gdouble) width, (gdouble) height);

  ags_ruler_draw((AgsRuler *) widget,
		 cr,
		 TRUE);
  
  cairo_destroy(cr);
}

/**
 * ags_ruler_draw:
 * @ruler: an #AgsRuler
 *
 * draws the widget
 */
void
ags_ruler_draw(AgsRuler *ruler,
	       cairo_t *cr,
	       gboolean is_animation)
{
  GtkStyleContext *style_context;
  GtkSettings *settings;

  GdkRGBA fg_color;
  GdkRGBA bg_color;
  GdkRGBA shadow_color;

  gchar *font_name;

  GtkOrientation orientation;

  gint width, height;
  gdouble tact_factor, zoom_factor;
  gdouble tact;

  guint offset;
  guint step;
  guint x0;
  guint z;
  guint i, i_stop;
  
  gboolean dark_theme;
  gboolean fg_success;
  gboolean bg_success;
  gboolean shadow_success;
  
  style_context = gtk_widget_get_style_context((GtkWidget *) ruler);
  
  settings = gtk_settings_get_default();

  font_name = NULL;  

  dark_theme = TRUE;
  
  g_object_get(settings,
	       "gtk-font-name", &font_name,
	       "gtk-application-prefer-dark-theme", &dark_theme,
	       NULL);

  orientation = gtk_orientable_get_orientation(GTK_ORIENTABLE(ruler));

  width = gtk_widget_get_width((GtkWidget *) ruler);
  height = gtk_widget_get_height((GtkWidget *) ruler);

  /* colors */
  fg_success = gtk_style_context_lookup_color(style_context,
					      "theme_fg_color",
					      &fg_color);
    
  bg_success = gtk_style_context_lookup_color(style_context,
					      "theme_bg_color",
					      &bg_color);
    
  shadow_success = gtk_style_context_lookup_color(style_context,
						  "theme_shadow_color",
						  &shadow_color);

  if(!fg_success ||
     !bg_success ||
     !shadow_success){
    gdk_rgba_parse(&fg_color,
		   "#101010");

    gdk_rgba_parse(&bg_color,
		   "#cbd5d9");

    gdk_rgba_parse(&shadow_color,
		   "#ffffff40");
  }

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
		  (double) width, (double) height);
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
  
  if(orientation == GTK_ORIENTATION_HORIZONTAL){
    i_stop = (guint) ceil((double) (width + (step + x0)) / (double) step);

    for(i = 0; i < i_stop; i++, z++){
      cairo_move_to(cr,
		    (double) (i * step - x0),
		    (double) (height));
    
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
		      (double) (height - ruler->large_step));

	/* draw scale step */
	cairo_move_to(cr,
		      (double) (i * step - x0),
		      (double) (height - ruler->large_step - (ruler->font_size + AGS_RULER_FREE_SPACE)));
      
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
			(double) (height - ruler->large_step));

	  /* draw scale step */
	  cairo_move_to(cr,
			(double) (i * step - x0),
			(double) (height - ruler->large_step - (ruler->font_size + AGS_RULER_FREE_SPACE)));
      
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
			(double) (height - ruler->small_step));
	}
      }

      cairo_stroke(cr);
    }
  }else{
    i_stop = (guint) ceil((double) (height + (step + x0)) / (double) step);

    for(i = 0; i < i_stop; i++, z++){
      cairo_move_to(cr,
		    (double) (width),
		    (double) (i * step - x0));
    
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
		      (double) (width - ruler->large_step),
		      (double) (i * step - x0));

	/* draw scale step */
	cairo_move_to(cr,
		      (double) (width - ruler->large_step - (ruler->font_size + AGS_RULER_FREE_SPACE)),
		      (double) (i * step - x0));
      
	cairo_rotate(cr,
		     2.0 * M_PI * 0.75);

	pango_cairo_show_layout(cr,
				layout);
      
	g_object_unref(layout);
      
	g_free(text);

	cairo_rotate(cr,
		     2.0 * M_PI * 1.25);
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
			(double) (width - ruler->large_step),
			(double) (i * step - x0));

	  /* draw scale step */
	  cairo_move_to(cr,
			(double) (width - ruler->large_step - (ruler->font_size + AGS_RULER_FREE_SPACE)),
			(double) (i * step - x0) + (logical_rect.width / PANGO_SCALE));
      
	  cairo_rotate(cr,
		       2.0 * M_PI * 0.75);

	  pango_cairo_show_layout(cr,
				  layout);
      
	  g_object_unref(layout);
      
	  g_free(text);

	  cairo_rotate(cr,
		       2.0 * M_PI * 1.25);
	}else{
	  /* draw small step */
	  cairo_set_line_width(cr,
			       1.25);

	  cairo_line_to(cr,
			(double) (width - ruler->small_step),
			(double) (i * step - x0));
	}
      }

      cairo_stroke(cr);
    }
  }

  g_free(font_name);

  cairo_pop_group_to_source(cr);
  cairo_paint(cr);
}

/**
 * ags_ruler_get_font_size:
 * @ruler: the #AgsRuler
 * 
 * Get font size of @ruler.
 * 
 * Returns: the font size
 * 
 * Since: 3.6.15
 */
guint
ags_ruler_get_font_size(AgsRuler *ruler)
{
  guint font_size;

  if(!AGS_IS_RULER(ruler)){
    return(0);
  }

  g_object_get(ruler,
	       "font-size", &font_size,
	       NULL);

  return(font_size);
}

/**
 * ags_ruler_set_font_size:
 * @ruler: the #AgsRuler
 * @font_size: the font size
 * 
 * Set font size of @ruler.
 * 
 * Since: 3.6.15
 */
void
ags_ruler_set_font_size(AgsRuler *ruler,
		   guint font_size)
{
  if(!AGS_IS_RULER(ruler)){
    return;
  }

  g_object_set(ruler,
	       "font-size", font_size,
	       NULL);
}

/**
 * ags_ruler_get_step:
 * @ruler: the #AgsRuler
 * 
 * Get step of @ruler.
 * 
 * Returns: the step
 * 
 * Since: 3.2.0
 */
guint
ags_ruler_get_step(AgsRuler *ruler)
{
  guint step;

  if(!AGS_IS_RULER(ruler)){
    return(0);
  }

  g_object_get(ruler,
	       "step", &step,
	       NULL);

  return(step);
}

/**
 * ags_ruler_set_step:
 * @ruler: the #AgsRuler
 * @step: the step
 * 
 * Set step of @ruler.
 * 
 * Since: 3.2.0
 */
void
ags_ruler_set_step(AgsRuler *ruler,
		   guint step)
{
  if(!AGS_IS_RULER(ruler)){
    return;
  }

  g_object_set(ruler,
	       "step", step,
	       NULL);
}

/**
 * ags_ruler_get_large_step:
 * @ruler: the #AgsRuler
 * 
 * Get large step of @ruler.
 * 
 * Returns: the large step
 * 
 * Since: 3.2.0
 */
guint
ags_ruler_get_large_step(AgsRuler *ruler)
{
  guint large_step;
  
  if(!AGS_IS_RULER(ruler)){
    return(0);
  }

  g_object_get(ruler,
	       "large-step", &large_step,
	       NULL);

  return(large_step);
}

/**
 * ags_ruler_set_large_step:
 * @ruler: the #AgsRuler
 * @large_step: the large step
 * 
 * Set large step of @ruler.
 * 
 * Since: 3.2.0
 */
void
ags_ruler_set_large_step(AgsRuler *ruler,
			 guint large_step)
{
  if(!AGS_IS_RULER(ruler)){
    return;
  }

  g_object_set(ruler,
	       "large-step", large_step,
	       NULL);
}

/**
 * ags_ruler_get_small_step:
 * @ruler: the #AgsRuler
 * 
 * Get small step of @ruler.
 * 
 * Returns: the small step
 * 
 * Since: 3.2.0
 */
guint
ags_ruler_get_small_step(AgsRuler *ruler)
{
  guint small_step;
  
  if(!AGS_IS_RULER(ruler)){
    return(0);
  }

  g_object_get(ruler,
	       "small-step", &small_step,
	       NULL);

  return(small_step);
}

/**
 * ags_ruler_set_small_step:
 * @ruler: the #AgsRuler
 * @small_step: the small step
 * 
 * Set small step of @ruler.
 * 
 * Since: 3.2.0
 */
void
ags_ruler_set_small_step(AgsRuler *ruler,
			 guint small_step)
{
  if(!AGS_IS_RULER(ruler)){
    return;
  }

  g_object_set(ruler,
	       "small-step", small_step,
	       NULL);
}

/**
 * ags_ruler_get_factor:
 * @ruler: the #AgsRuler
 * 
 * Get factor of @ruler.
 * 
 * Returns: the factor
 * 
 * Since: 4.0.0
 */
gdouble
ags_ruler_get_factor(AgsRuler *ruler)
{
  gdouble factor;

  if(!AGS_IS_RULER(ruler)){
    return(0);
  }

  g_object_get(ruler,
	       "factor", &factor,
	       NULL);

  return(factor);
}

/**
 * ags_ruler_set_factor:
 * @ruler: the #AgsRuler
 * @factor: the factor
 * 
 * Set factor of @ruler.
 * 
 * Since: 4.0.0
 */
void
ags_ruler_set_factor(AgsRuler *ruler,
		     gdouble factor)
{
  if(!AGS_IS_RULER(ruler)){
    return;
  }

  g_object_set(ruler,
	       "factor", factor,
	       NULL);
}

/**
 * ags_ruler_get_precision:
 * @ruler: the #AgsRuler
 * 
 * Get precision of @ruler.
 * 
 * Returns: the precision
 * 
 * Since: 4.0.0
 */
gdouble
ags_ruler_get_precision(AgsRuler *ruler)
{
  gdouble precision;

  if(!AGS_IS_RULER(ruler)){
    return(0);
  }

  g_object_get(ruler,
	       "precision", &precision,
	       NULL);

  return(precision);
}

/**
 * ags_ruler_set_precision:
 * @ruler: the #AgsRuler
 * @precision: the precision
 * 
 * Set precision of @ruler.
 * 
 * Since: 4.0.0
 */
void
ags_ruler_set_precision(AgsRuler *ruler,
			gdouble precision)
{
  if(!AGS_IS_RULER(ruler)){
    return;
  }

  g_object_set(ruler,
	       "precision", precision,
	       NULL);
}

/**
 * ags_ruler_get_scale_precision:
 * @ruler: the #AgsRuler
 * 
 * Get scale precision of @ruler.
 * 
 * Returns: the scale precision
 * 
 * Since: 4.0.0
 */
gdouble
ags_ruler_get_scale_precision(AgsRuler *ruler)
{
  gdouble scale_precision;

  if(!AGS_IS_RULER(ruler)){
    return(0);
  }

  g_object_get(ruler,
	       "scale-precision", &scale_precision,
	       NULL);

  return(scale_precision);
}

/**
 * ags_ruler_set_scale_precision:
 * @ruler: the #AgsRuler
 * @scale_precision: the scale precision
 * 
 * Set scale precision of @ruler.
 * 
 * Since: 4.0.0
 */
void
ags_ruler_set_scale_precision(AgsRuler *ruler,
			      gdouble scale_precision)
{
  if(!AGS_IS_RULER(ruler)){
    return;
  }

  g_object_set(ruler,
	       "scale-precision", scale_precision,
	       NULL);
}

/**
 * ags_ruler_get_adjustment:
 * @ruler: the #AgsRuler
 * 
 * Get adjustment of @ruler.
 * 
 * Returns: (transfer full): the #GtkAdjustment
 * 
 * Since: 3.2.0
 */
GtkAdjustment*
ags_ruler_get_adjustment(AgsRuler *ruler)
{
  GtkAdjustment *adjustment;
  
  if(!AGS_IS_RULER(ruler)){
    return(NULL);
  }

  g_object_get(ruler,
	       "adjustment", &adjustment,
	       NULL);

  return(adjustment);
}

/**
 * ags_ruler_set_adjustment:
 * @ruler: the #AgsRuler
 * @adjustment: (transfer none): the #GtkAdjustment
 * 
 * Set adjustment of @ruler.
 * 
 * Since: 3.2.0
 */
void
ags_ruler_set_adjustment(AgsRuler *ruler,
			 GtkAdjustment *adjustment)
{  
  if(!AGS_IS_RULER(ruler)){
    return;
  }

  g_object_set(ruler,
	       "adjustment", adjustment,
	       NULL);
}

/**
 * ags_ruler_new:
 * @orientation: the #GtkOrientation
 * @step: the step size
 *
 * Create a new instance of #AgsRuler
 *
 * Returns: the new #AgsRuler
 *
 * Since: 3.0.0
 */
AgsRuler*
ags_ruler_new(GtkOrientation orientation,
	      guint step,
	      gdouble factor,
	      gdouble precision,
	      gdouble scale_precision)
{
  AgsRuler *ruler;

  ruler = (AgsRuler *) g_object_new(AGS_TYPE_RULER,
				    "orientation", orientation,
				    "step", step,
				    "factor", factor,
				    "precision", precision,
				    "scale-precision", scale_precision,
				    NULL);

  return(ruler);
}
