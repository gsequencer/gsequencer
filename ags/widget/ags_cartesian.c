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

#include <ags/widget/ags_cartesian.h>

#include <stdlib.h>
#include <math.h>

void ags_cartesian_class_init(AgsCartesianClass *cartesian);
void ags_cartesian_init(AgsCartesian *cartesian);
void ags_cartesian_show();

void ags_cartesian_map(GtkWidget *widget);
void ags_cartesian_realize(GtkWidget *widget);
void ags_cartesian_size_request(GtkWidget *widget,
				GtkRequisition   *requisition);
void ags_cartesian_size_allocate(GtkWidget *widget,
				 GtkAllocation *allocation);
gboolean ags_cartesian_expose(GtkWidget *widget,
			      GdkEventExpose *event);

void ags_cartesian_draw(AgsCartesian *cartesian);

static gpointer ags_cartesian_parent_class = NULL;

GType
ags_cartesian_get_type(void)
{
  static GType ags_type_cartesian = 0;

  if(!ags_type_cartesian){
    static const GTypeInfo ags_cartesian_info = {
      sizeof(AgsCartesianClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_cartesian_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsCartesian),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_cartesian_init,
    };

    ags_type_cartesian = g_type_register_static(GTK_TYPE_WIDGET,
						"AgsCartesian\0", &ags_cartesian_info,
						0);
  }

  return(ags_type_cartesian);
}

void
ags_cartesian_class_init(AgsCartesianClass *cartesian)
{
  GtkWidgetClass *widget;

  ags_cartesian_parent_class = g_type_class_peek_parent(cartesian);

  /* GObjectClass */

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) cartesian;

  //  widget->map = ags_cartesian_map;
  widget->realize = ags_cartesian_realize;
  widget->expose_event = ags_cartesian_expose;
  widget->size_request = ags_cartesian_size_request;
  widget->size_allocate = ags_cartesian_size_allocate;
  widget->show = ags_cartesian_show;
}

void
ags_cartesian_init(AgsCartesian *cartesian)
{
  guint i, i_stop;
  
  g_object_set(G_OBJECT(cartesian),
	       "app-paintable\0", TRUE,
	       NULL);

  /* flags */
  cartesian->flags = (AGS_CARTESIAN_ABSCISSAE |
		      AGS_CARTESIAN_ORDINATE |
		      AGS_CARTESIAN_X_SCALE |
		      AGS_CARTESIAN_Y_SCALE |
		      AGS_CARTESIAN_X_UNIT |
		      AGS_CARTESIAN_Y_UNIT |
		      AGS_CARTESIAN_X_LABEL |
		      AGS_CARTESIAN_Y_LABEL);

  /* margin */
  cartesian->x_margin = AGS_CARTESIAN_DEFAULT_X_MARGIN;
  cartesian->y_margin = AGS_CARTESIAN_DEFAULT_Y_MARGIN;

  /* line width */
  cartesian->line_width = 1.0;
  cartesian->center = 0.5;  
  
  /* step */
  cartesian->x_step_width = AGS_CARTESIAN_DEFAULT_X_STEP_WIDTH;
  cartesian->y_step_height = AGS_CARTESIAN_DEFAULT_Y_STEP_HEIGHT;

  /* scale step */
  cartesian->x_scale_step_width = AGS_CARTESIAN_DEFAULT_X_SCALE_STEP_WIDTH;
  cartesian->y_scale_step_height = AGS_CARTESIAN_DEFAULT_Y_SCALE_STEP_HEIGHT;

  /* x unit position and font size */
  cartesian->x_unit_x0 = (AGS_CARTESIAN_DEFAULT_X_END + AGS_CARTESIAN_DEFAULT_X_MARGIN) - 12.0;
  cartesian->x_unit_y0 = 0.0;

  cartesian->x_unit_size = 12.0;

  /* y unit position and font size */
  cartesian->y_unit_x0 = 0.0;
  cartesian->y_unit_y0 = (AGS_CARTESIAN_DEFAULT_Y_END + AGS_CARTESIAN_DEFAULT_Y_MARGIN) - 12.0;

  cartesian->y_unit_size = 12.0;

  /* label step width */
  cartesian->x_label_step_width = AGS_CARTESIAN_DEFAULT_X_SCALE_STEP_WIDTH;
  cartesian->y_label_step_height = AGS_CARTESIAN_DEFAULT_Y_SCALE_STEP_HEIGHT;

  /* region alignment */
  cartesian->x_step = AGS_CARTESIAN_DEFAULT_X_STEP;
  cartesian->y_step = AGS_CARTESIAN_DEFAULT_Y_STEP;

  cartesian->x_start = AGS_CARTESIAN_DEFAULT_X_START;
  cartesian->x_end = AGS_CARTESIAN_DEFAULT_X_END;
  
  cartesian->y_start = AGS_CARTESIAN_DEFAULT_Y_START;
  cartesian->y_end = AGS_CARTESIAN_DEFAULT_Y_END;

  /* unit */
  cartesian->x_unit = "x\0";
  cartesian->y_unit = "y\0";

  /* label */
  cartesian->x_label = NULL;
  cartesian->y_label = NULL;

  /*
   * Here are some common conversion and translate functions.
   * As well functions to calculate scale and fill their labels.
   */
  /* conversion function */
  cartesian->step_conversion_func = ags_cartesian_linear_step_conversion_func;

  /* translate function */
  cartesian->translate_func = ags_cartesian_linear_translate_func;

  /* scale functions */
  cartesian->x_small_scale_func = ags_cartesian_linear_x_small_scale_func;
  cartesian->x_big_scale_func = ags_cartesian_linear_x_big_scale_func;
  
  cartesian->y_small_scale_func = ags_cartesian_linear_y_small_scale_func;
  cartesian->y_big_scale_func = ags_cartesian_linear_y_big_scale_func;

  /* label functions */
  cartesian->x_label_func = ags_cartesian_linear_x_label_func;
  cartesian->y_label_func = ags_cartesian_linear_y_label_func;

  /* factors */
  cartesian->x_step_data = cartesian;
  cartesian->x_step_factor = 1.0;
  
  cartesian->y_step_data = cartesian;
  cartesian->y_step_factor = 1.0;
  
  cartesian->x_scale_data = cartesian;
  cartesian->x_small_scale_factor = 1.0;
  cartesian->x_big_scale_factor = 5.0;
  
  cartesian->y_scale_data = cartesian;
  cartesian->y_small_scale_factor = 1.0;
  cartesian->y_big_scale_factor = 5.0;

  cartesian->x_label_data = cartesian;
  cartesian->x_label_factor = 1.0;
  cartesian->x_label_precision = 0.0;
  
  cartesian->y_label_data = cartesian;
  cartesian->y_label_factor = 1.0;
  cartesian->y_label_precision = 0.0;

  /* plot */
  cartesian->surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24,
						  cartesian->x_end - cartesian->x_start, cartesian->y_end - cartesian->y_start);

  cartesian->plot = NULL;
  
  /*
   * reallocate and fill label
   */
  ags_cartesian_reallocate_label(cartesian,
				 TRUE);
  ags_cartesian_reallocate_label(cartesian,
				 FALSE);

  ags_cartesian_fill_label(cartesian,
			   TRUE);
  ags_cartesian_fill_label(cartesian,
			   FALSE);  
}

void
ags_cartesian_map(GtkWidget *widget)
{
  if(gtk_widget_get_realized (widget) && !gtk_widget_get_mapped(widget)){
    GTK_WIDGET_CLASS(ags_cartesian_parent_class)->map(widget);
    
    gdk_window_show(widget->window);
    ags_cartesian_draw((AgsCartesian *) widget);
  }
}

void
ags_cartesian_realize(GtkWidget *widget)
{
  AgsCartesian *cartesian;
  GdkWindowAttr attributes;
  gint attributes_mask;
  gint buttons_width;
  gint border_left, border_top;

  g_return_if_fail(widget != NULL);
  g_return_if_fail(AGS_IS_CARTESIAN(widget));

  cartesian = AGS_CARTESIAN(widget);

  gtk_widget_set_realized(widget, TRUE);

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
  attributes.event_mask |= (GDK_EXPOSURE_MASK);

  widget->window = gdk_window_new(gtk_widget_get_parent_window (widget),
				  &attributes, attributes_mask);
  gdk_window_set_user_data(widget->window, cartesian);

  widget->style = gtk_style_attach(widget->style,
				   widget->window);
  gtk_style_set_background(widget->style,
			   widget->window,
			   GTK_STATE_NORMAL);

  gtk_widget_queue_resize(widget);
}

void
ags_cartesian_show(GtkWidget *widget)
{
  GTK_WIDGET_CLASS(ags_cartesian_parent_class)->show(widget);
}

void
ags_cartesian_size_request(GtkWidget *widget,
			   GtkRequisition *requisition)
{
  GTK_WIDGET_CLASS(ags_cartesian_parent_class)->size_request(widget,
							     requisition);

  //TODO:JK: implement me
}

void
ags_cartesian_size_allocate(GtkWidget *widget,
			    GtkAllocation *allocation)
{
  GTK_WIDGET_CLASS(ags_cartesian_parent_class)->size_allocate(widget,
							      allocation);

  //TODO:JK: implement me
}

gboolean
ags_cartesian_expose(GtkWidget *widget,
		     GdkEventExpose *event)
{
  ags_cartesian_draw(AGS_CARTESIAN(widget));

  return(FALSE);
}

void
ags_cartesian_draw(AgsCartesian *cartesian)
{
  GtkWidget *widget;

  GtkStyle *cartesian_style;
  cairo_t *cr;

  cairo_text_extents_t te_x_unit, te_y_unit;

  unsigned char *data;
  
  gdouble x, y;
  gdouble x_offset, y_offset;
  gdouble width, height;
  gdouble factor;
  gdouble scale_point;
  guint stride;
  guint i_stop;
  guint i;
  
  static const gdouble white_gc = 65535.0;

  auto void ags_cartesian_draw_area();

  auto void ags_cartesian_draw_x_small_scale();
  auto void ags_cartesian_draw_x_big_scale();

  auto void ags_cartesian_draw_y_small_scale();
  auto void ags_cartesian_draw_y_big_scale();

  void ags_cartesian_draw_area(){
    static const double dashes[] = {
      2.0,
      3.0
    };

    /*  line width */
    cairo_set_line_width(cr, cartesian->line_width);

    /* color */
    cairo_set_source_rgb(cr,
			 cartesian_style->mid[0].red / white_gc,
			 cartesian_style->mid[0].green / white_gc,
			 cartesian_style->mid[0].blue / white_gc);
    
    /* dash */
    cairo_set_dash(cr,
		   dashes,
		   2,
		   0.0);

    /* draw small area steps */
    factor = (cartesian->x_scale_step_width / cartesian->x_step_width);
      
    for(x = cartesian->x_start; ; ){
      scale_point = cartesian->x_small_scale_func(x,
						  cartesian->x_scale_data);

      if(scale_point < factor * cartesian->x_start){
	x += cartesian->x_step_width;

	continue;
      }
      
      if(scale_point < factor * cartesian->x_end + cartesian->x_scale_step_width){
	/* draw scale step */
	cairo_move_to(cr,
		      x_offset + scale_point,
		      y_offset - cartesian->y_start - height);
	cairo_line_to(cr,
		      x_offset + scale_point,
		      y_offset - cartesian->y_start);
	cairo_stroke(cr);
      }else{
	break;
      }
	
      x += cartesian->x_step_width;
    }

    /* draw small scale steps */
    factor = (cartesian->y_scale_step_height / cartesian->y_step_height);
      
    for(y = cartesian->y_start; ; ){
      scale_point = cartesian->y_small_scale_func(y,
						  cartesian->y_scale_data);

      if(scale_point < factor * cartesian->y_start){
	y += cartesian->y_step_height;

	continue;
      }
      
      if(scale_point < factor * cartesian->y_end + cartesian->y_scale_step_height){
	/* draw scale step */
	cairo_move_to(cr,
		      x_offset + cartesian->x_start,
		      y_offset - scale_point);
	cairo_line_to(cr,
		      x_offset + cartesian->x_start + width,
		      y_offset - scale_point);
	cairo_stroke(cr);
      }else{
	break;
      }
	
      y += cartesian->y_step_height;
    }
    

    /* undash */    
    cairo_set_dash(cr,
		   NULL,
		   0,
		   0.0);

    /* draw big area steps */
    factor = (cartesian->x_scale_step_width / cartesian->x_step_width);
      
    for(x = cartesian->x_start; ; ){
      scale_point = cartesian->x_big_scale_func(x,
						cartesian->x_scale_data);
      
      if(scale_point <= factor * cartesian->x_start){
	x += cartesian->x_step_width;

	continue;
      }
      
      if(scale_point < factor * cartesian->x_end + cartesian->x_scale_step_width){
	/* draw scale step */
	cairo_move_to(cr,
		      x_offset + scale_point,
		      y_offset - cartesian->y_start - height);
	cairo_line_to(cr,
		      x_offset + scale_point,
		      y_offset - cartesian->y_start);
	cairo_stroke(cr);
      }else{
	break;
      }
	
      x += cartesian->x_step_width;
    }

    /* draw big scale steps */
    factor = (cartesian->y_scale_step_height / cartesian->y_step_height);
      
    for(y = cartesian->y_start; ; ){
      scale_point = cartesian->y_big_scale_func(y,
						cartesian->y_scale_data);
      
      if(scale_point < factor * cartesian->y_start){
	y += cartesian->y_step_height;

	continue;
      }
      
      if(scale_point < factor * cartesian->y_end + cartesian->y_scale_step_height){
	/* draw scale step */
	cairo_move_to(cr,
		      x_offset + cartesian->x_start,
		      y_offset - scale_point);
	cairo_line_to(cr,
		      x_offset + cartesian->x_start + width,
		      y_offset - scale_point);
	cairo_stroke(cr);
      }else{
	break;
      }
	
      y += cartesian->y_step_height;
    }
  }
  
  void ags_cartesian_draw_x_small_scale(){
    /*  line width */
    cairo_set_line_width(cr, cartesian->line_width);

    /* color */
    cairo_set_source_rgb(cr,
			 cartesian_style->fg[0].red / white_gc,
			 cartesian_style->fg[0].green / white_gc,
			 cartesian_style->fg[0].blue / white_gc);
    
    /* draw line */
    cairo_move_to(cr,
		  x_offset + cartesian->x_start,
		  y_offset);
    cairo_line_to(cr,
		  x_offset + cartesian->x_start + width,
		  y_offset);
    cairo_stroke(cr);
    
    /* draw small scale steps */
    factor = (cartesian->x_scale_step_width / cartesian->x_step_width);
      
    for(x = cartesian->x_start; ; ){
      scale_point = cartesian->x_small_scale_func(x,
						  cartesian->x_scale_data);

      if(scale_point < factor * cartesian->x_start){
	x += cartesian->x_step_width;

	continue;
      }
      
      if(scale_point < factor * cartesian->x_end + cartesian->x_scale_step_width){
	/* draw scale step */
	cairo_move_to(cr,
		      x_offset + scale_point,
		      y_offset - 4.0);
	cairo_line_to(cr,
		      x_offset + scale_point,
		      y_offset + 4.0);
	cairo_stroke(cr);
      }else{
	break;
      }
	
      x += cartesian->x_step_width;
    }
  }

  void ags_cartesian_draw_x_big_scale(){
    /* line width */
    cairo_set_line_width(cr, cartesian->line_width);

    /* color */
    cairo_set_source_rgb(cr,
			 cartesian_style->fg[0].red / white_gc,
			 cartesian_style->fg[0].green / white_gc,
			 cartesian_style->fg[0].blue / white_gc);    

    /* draw big scale steps */
    factor = (cartesian->x_scale_step_width / cartesian->x_step_width);
      
    for(x = cartesian->x_start; ; ){
      scale_point = cartesian->x_big_scale_func(x,
						cartesian->x_scale_data);

      if(scale_point < factor * cartesian->x_start){
	x += cartesian->x_step_width;

	continue;
      }
      
      if(scale_point < factor * cartesian->x_end + cartesian->x_scale_step_width){
	/* draw scale step */
	cairo_move_to(cr,
		      x_offset + scale_point,
		      y_offset - 6.0);
	cairo_line_to(cr,
		      x_offset + scale_point,
		      y_offset + 6.0);
	cairo_stroke(cr);
      }else{
	break;
      }
	
      x += cartesian->x_step_width;
    }
  }

  void ags_cartesian_draw_y_small_scale(){
    /* color and line width */
    cairo_set_source_rgb(cr,
			 cartesian_style->fg[0].red / white_gc,
			 cartesian_style->fg[0].green / white_gc,
			 cartesian_style->fg[0].blue / white_gc);
    
    cairo_set_line_width(cr, cartesian->line_width);

    /* draw line */
    cairo_move_to(cr,
		  x_offset,
		  y_offset - cartesian->y_start);
    cairo_line_to(cr,
		  x_offset,
		  y_offset - cartesian->y_start - height);
    cairo_stroke(cr);

    /* draw small scale steps */
    factor = (cartesian->y_scale_step_height / cartesian->y_step_height);
      
    for(y = cartesian->y_start; ; ){
      scale_point = cartesian->y_small_scale_func(y,
						  cartesian->y_scale_data);

      if(scale_point < factor * cartesian->y_start){
	y += cartesian->y_step_height;

	continue;
      }
      
      if(scale_point < factor * cartesian->y_end + cartesian->y_scale_step_height){
	/* draw scale step */
	cairo_move_to(cr,
		      x_offset - 4.0,
		      y_offset - scale_point);
	cairo_line_to(cr,
		      x_offset + 4.0,
		      y_offset - scale_point);
	cairo_stroke(cr);
      }else{
	break;
      }
	
      y += cartesian->y_step_height;
    }
  }

  void ags_cartesian_draw_y_big_scale(){
    /* line width */
    cairo_set_line_width(cr, cartesian->line_width);

    /* color */
    cairo_set_source_rgb(cr,
			 cartesian_style->fg[0].red / white_gc,
			 cartesian_style->fg[0].green / white_gc,
			 cartesian_style->fg[0].blue / white_gc);    

    /* draw big scale steps */
    factor = (cartesian->y_scale_step_height / cartesian->y_step_height);
      
    for(y = cartesian->y_start; ; ){
      scale_point = cartesian->y_big_scale_func(y,
						cartesian->y_scale_data);

      if(scale_point < factor * cartesian->y_start){
	y += cartesian->y_step_height;

	continue;
      }
      
      if(scale_point < factor * cartesian->y_end + cartesian->y_scale_step_height){
	/* draw scale step */
	cairo_move_to(cr,
		      x_offset - 6.0,
		      y_offset - scale_point);
	cairo_line_to(cr,
		      x_offset + 6.0,
		      y_offset - scale_point);
	cairo_stroke(cr);
      }else{
	break;
      }
	
      y += cartesian->y_step_height;
    }
  }

  /* entry point */
  widget = GTK_WIDGET(cartesian);
  cartesian_style = gtk_widget_get_style(widget);
  
  cr = gdk_cairo_create(widget->window);
  
  /* clear bg */
  cairo_set_source_rgb(cr,
		       cartesian_style->bg[0].red / white_gc,
		       cartesian_style->bg[0].green / white_gc,
		       cartesian_style->bg[0].blue / white_gc);

  cairo_rectangle(cr,
		  0.0, 0.0,
		  widget->allocation.width, widget->allocation.height);
  cairo_fill(cr);
  
  /* cartesian offset, width and height */
  width = (cartesian->x_end - cartesian->x_start);
  height = (cartesian->y_end - cartesian->y_start);

  x_offset = cartesian->x_margin - cartesian->x_start - cartesian->center;
  y_offset = cartesian->y_margin + cartesian->y_start + height + cartesian->center;
  
  /* clear surface to white */
  data = cairo_image_surface_get_data(cartesian->surface);
  stride = cairo_image_surface_get_stride(cartesian->surface);
  
  memset(data, 0xaf, (4 * width * height * sizeof(unsigned char)));
  
  /* draw plot */
  //TODO:JK: implement me

  cairo_surface_flush(cartesian->surface);
  
  /* surface */
  cairo_set_source_surface(cr,
			   cartesian->surface,
  			   cartesian->x_margin, cartesian->y_margin);
  cairo_surface_mark_dirty(cartesian->surface);

  cairo_paint(cr);
  
  /* push group */
  cairo_push_group(cr);

  ags_cartesian_draw_area();

  /* abscissae */
  if((AGS_CARTESIAN_ABSCISSAE & (cartesian->flags)) != 0){
    if((cartesian->x_end == 0.0) ||
       (cartesian->x_start == 0.0) ||
       (cartesian->x_start < 0.0 &&
	cartesian->x_end > 0.0)){
      ags_cartesian_draw_y_small_scale();
      ags_cartesian_draw_y_big_scale();
    }
  }

  /* ordinate */
  if((AGS_CARTESIAN_ORDINATE & (cartesian->flags)) != 0){
    if((cartesian->y_end == 0.0) ||
       (cartesian->y_start == 0.0) ||
       (cartesian->y_start < 0.0 &&
	cartesian->y_end > 0.0)){
      ags_cartesian_draw_x_small_scale();
      ags_cartesian_draw_x_big_scale();
    }
  }
  
  /* pop group */
  cairo_pop_group_to_source(cr);
  
  /* paint */
  cairo_paint(cr);
}

AgsPlot*
ags_plot_alloc(guint n_points, guint n_bitmap, guint n_pixmap)
{
  AgsPlot *plot;

  plot = (AgsPlot *) malloc(sizeof(AgsPlot));
  
  //TODO:JK: implement me


  return(plot);
}

void
ags_cartesian_add_plot(AgsCartesian *cartesian,
		       AgsPlot *plot)
{
  //TODO:JK: implement me
}

void
ags_cartesian_remove_plot(AgsCartesian *cartesian,
			  AgsPlot *plot)
{
  //TODO:JK: implement me
}

gdouble
ags_cartesian_linear_step_conversion_func(gdouble current,
					  gboolean is_abscissae,
					  gpointer data)
{
  if(data == NULL ||
     !AGS_IS_CARTESIAN(data)){
    return(current);
  }
  
  if(is_abscissae){
    return(AGS_CARTESIAN(data)->x_step_factor * current);
  }else{
    return(AGS_CARTESIAN(data)->y_step_factor * current);
  }
}

void
ags_cartesian_linear_translate_func(gdouble x,
				    gdouble y,
				    gdouble *ret_x,
				    gdouble *ret_y,
				    gpointer data)
{
  if(data == NULL ||
     !AGS_IS_CARTESIAN(data)){
    if(ret_x != NULL){
      *ret_x = x;
    }

    if(ret_y != NULL){
      *ret_y = y;
    }

    return;
  }

  if(ret_x != NULL){
    *ret_x = (AGS_CARTESIAN(data)->x_translate_point) - (x - AGS_CARTESIAN(data)->x_translate_point);
  }

  if(ret_y != NULL){
    *ret_y = (AGS_CARTESIAN(data)->y_translate_point) - (y - AGS_CARTESIAN(data)->y_translate_point);
  }
}

gdouble
ags_cartesian_linear_x_small_scale_func(gdouble value,
					gpointer data)
{
  if(data == NULL ||
     !AGS_IS_CARTESIAN(data)){
    return(value);
  }

  return(AGS_CARTESIAN(data)->x_small_scale_factor * value);
}

gdouble
ags_cartesian_linear_x_big_scale_func(gdouble value,
				      gpointer data)
{
  if(data == NULL ||
     !AGS_IS_CARTESIAN(data)){
    return(value);
  }

  return(AGS_CARTESIAN(data)->x_big_scale_factor * value);
}

gdouble
ags_cartesian_linear_y_small_scale_func(gdouble value,
					gpointer data)
{
  if(data == NULL ||
     !AGS_IS_CARTESIAN(data)){
    return(value);
  }

  return(AGS_CARTESIAN(data)->y_small_scale_factor * value);
}

gdouble
ags_cartesian_linear_y_big_scale_func(gdouble value,
				      gpointer data)
{
  if(data == NULL ||
     !AGS_IS_CARTESIAN(data)){
    return(value);
  }

  return(AGS_CARTESIAN(data)->y_big_scale_factor * value);
}

gchar*
ags_cartesian_linear_x_label_func(gdouble value,
				  gpointer data)
{
  gchar *format;
  gchar *str;
  
  if(data == NULL ||
     !AGS_IS_CARTESIAN(data)){
    str = g_strdup_printf("%f\0",
			  value);
    
    return(str);
  }

  format = g_strdup_printf("%%.%df\0",
			   (guint) ceil(AGS_CARTESIAN(data)->x_label_precision));

  str = g_strdup_printf(format,
			value);
  g_free(format);

  return(str);
}

gchar*
ags_cartesian_linear_y_label_func(gdouble value,
				  gpointer data)
{
  gchar *format;
  gchar *str;
  
  if(data == NULL ||
     !AGS_IS_CARTESIAN(data)){
    str = g_strdup_printf("%f\0",
			  value);
    
    return(str);
  }

  format = g_strdup_printf("%%.%df\0",
			   (guint) ceil(AGS_CARTESIAN(data)->y_label_precision));

  str = g_strdup_printf(format,
			value);
  g_free(format);

  return(str);
}

void
ags_cartesian_reallocate_label(AgsCartesian *cartesian,
			       gboolean do_x_label)
{
  guint i_start, i_stop;
  guint i;

  if(cartesian == NULL ||
     !AGS_IS_CARTESIAN(cartesian)){
    return;
  }
  
  if(do_x_label){
    i_stop = (guint) ceil((1.0 / cartesian->x_step_width) *
			  (cartesian->x_end - cartesian->x_start));
      
    if(cartesian->x_label == NULL){
      /* allocate */
      cartesian->x_label = (gchar **) malloc((i_stop + 1)  * sizeof(gchar *));

      /* iteration control */
      i_start = 0;
      i = 0;
    }else{
      /* reallocate */
      cartesian->x_label = (gchar **) realloc(cartesian->x_label,
					      i_stop);

      /* iteration control */
      i_start = g_strv_length(cartesian->x_label);

      if(i_start < i_stop){	
	i = i_start;
      }else{
	i = i_stop;

	if(i_stop == 0){
	  /* set label to NULL */
	  cartesian->x_label = NULL;
	}
      }
    }

    if(cartesian->x_label != NULL){
      for(; i < i_stop; i++){
	/* initialize array to NULL */
	cartesian->x_label[i] = NULL;
      }

      /* NULL terminated end */
      cartesian->x_label[i] = NULL;
    }
  }else{
    i_stop = (guint) ceil((1.0 / cartesian->y_step_height) *
			  (cartesian->y_end - cartesian->y_start));
      
    if(cartesian->y_label == NULL){
      /* allocate */
      cartesian->y_label = (gchar **) malloc((i_stop + 1)  * sizeof(gchar *));

      /* iteration control */
      i_start = 0;
      i = 0;
    }else{
      /* reallocate */
      cartesian->y_label = (gchar **) realloc(cartesian->y_label,
					      i_stop);

      /* iteration control */
      i_start = g_strv_length(cartesian->y_label);

      if(i_start < i_stop){	
	i = i_start;
      }else{
	i = i_stop;

	if(i_stop == 0){
	  /* set label to NULL */
	  cartesian->y_label = NULL;
	}
      }
    }

    if(cartesian->y_label != NULL){
      for(; i < i_stop; i++){
	/* initialize array to NULL */
	cartesian->y_label[i] = NULL;
      }

      /* NULL terminated end */
      cartesian->y_label[i] = NULL;
    }
  }
}

void
ags_cartesian_fill_label(AgsCartesian *cartesian,
			 gboolean do_x_label)
{
  guint i_stop;
  guint i;

  if(cartesian == NULL ||
     !AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  if(do_x_label){
    i_stop = g_strv_length(cartesian->x_label);

    for(i = 0; i < i_stop; i++){
      /* fill x label */
      cartesian->x_label = cartesian->x_label_func(cartesian->x_start + ((gdouble) i * cartesian->x_step),
						   cartesian->x_label_data);
    }
  }else{
    i_stop = g_strv_length(cartesian->y_label);

    for(i = 0; i < i_stop; i++){
      /* fill y label */
      cartesian->y_label = cartesian->y_label_func(cartesian->y_start + ((gdouble) i * cartesian->y_step),
						   cartesian->y_label_data);
    }
  }
}

AgsCartesian*
ags_cartesian_new()
{
  AgsCartesian *cartesian;

  cartesian = (AgsCartesian *) g_object_new(AGS_TYPE_CARTESIAN,
					    NULL);
  
  return(cartesian);
}
