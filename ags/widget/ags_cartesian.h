/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#ifndef __AGS_CARTESIAN_H__
#define __AGS_CARTESIAN_H__

#include <glib.h>
#include <glib-object.h>

#include <cairo.h>

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define AGS_TYPE_CARTESIAN                (ags_cartesian_get_type())
#define AGS_CARTESIAN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CARTESIAN, AgsCartesian))
#define AGS_CARTESIAN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CARTESIAN, AgsCartesianClass))
#define AGS_IS_CARTESIAN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_CARTESIAN))
#define AGS_IS_CARTESIAN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_CARTESIAN))
#define AGS_CARTESIAN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_CARTESIAN, AgsCartesianClass))

#define AGS_PLOT(ptr) ((AgsPlot *)(ptr))

#define AGS_CARTESIAN_STEP_CONVERSION_FUNC(current, is_abscissae, data) ((AgsCartesianStepConversionFunc)(current, is_abscissae, data))
#define AGS_CARTESIAN_TRANSLATE_FUNCT(x, y, ret_x, ret_y, data) ((AgsCartesianTranslateFunc)(x, y, ret_x, ret_y, data))
#define AGS_CARTESIAN_SCALE_FUNC(value, data) ((AgsCartesianScaleFunc)(value, data))
#define AGS_CARTESIAN_LABEL_FUNC(value, data) ((AgsCartesianLabelFunc)(value, data))

#define AGS_CARTESIAN_DEFAULT_X_MARGIN (24.0)
#define AGS_CARTESIAN_DEFAULT_Y_MARGIN (24.0)

#define AGS_CARTESIAN_DEFAULT_X_STEP_WIDTH (10.0)
#define AGS_CARTESIAN_DEFAULT_Y_STEP_HEIGHT (10.0)

#define AGS_CARTESIAN_DEFAULT_X_SCALE_STEP_WIDTH (10.0)
#define AGS_CARTESIAN_DEFAULT_Y_SCALE_STEP_HEIGHT (10.0)

#define AGS_CARTESIAN_DEFAULT_X_STEP (1.0)
#define AGS_CARTESIAN_DEFAULT_Y_STEP (1.0)

#define AGS_CARTESIAN_DEFAULT_X_START (-60.0)
#define AGS_CARTESIAN_DEFAULT_X_END (199.0)

#define AGS_CARTESIAN_DEFAULT_Y_START (-70.0)
#define AGS_CARTESIAN_DEFAULT_Y_END (99.0)

#define AGS_CARTESIAN_DEFAULT_X_LABEL_START (10.0)
#define AGS_CARTESIAN_DEFAULT_X_LABEL_STEP_WIDTH (50.0)

#define AGS_CARTESIAN_DEFAULT_Y_LABEL_START (20.0)
#define AGS_CARTESIAN_DEFAULT_Y_LABEL_STEP_HEIGHT (50.0)


typedef struct _AgsCartesian AgsCartesian;
typedef struct _AgsCartesianClass AgsCartesianClass;

typedef struct _AgsPlot AgsPlot;

typedef gdouble (*AgsCartesianStepConversionFunc)(gdouble current,
						  gboolean is_abscissae,
						  gpointer data);

typedef void (*AgsCartesianTranslateFunc)(gdouble x,
					  gdouble y,
					  gdouble *ret_x,
					  gdouble *ret_y,
					  gpointer data);

typedef gdouble (*AgsCartesianScaleFunc)(gdouble value,
					 gpointer data);

typedef gchar* (*AgsCartesianLabelFunc)(gdouble value,
					gpointer data);

typedef enum{
  AGS_CARTESIAN_ABSCISSAE  =  1,
  AGS_CARTESIAN_ORDINATE   =  1 << 1,
  AGS_CARTESIAN_X_SCALE    =  1 << 2,
  AGS_CARTESIAN_Y_SCALE    =  1 << 3,
  AGS_CARTESIAN_X_UNIT     =  1 << 4,
  AGS_CARTESIAN_Y_UNIT     =  1 << 5,
  AGS_CARTESIAN_X_LABEL    =  1 << 6,
  AGS_CARTESIAN_Y_LABEL    =  1 << 7,
}AgsCartesianFlags;

typedef enum{
  AGS_PLOT_FILL_REPLACE    = 1,
  //TODO:JK: implement me  AGS_PLOT_FILL_ADDITIVE   = 1 <<  1,
}AgsPlotFillFlags;
  
struct _AgsCartesian
{
  GtkWidget widget;

  AgsCartesianFlags flags;
  
  gdouble x_margin;
  gdouble y_margin;

  gdouble center;

  gdouble line_width;
  gdouble point_radius;

  gdouble font_size;

  gdouble x_step_width;
  gdouble y_step_height;

  gdouble x_scale_step_width;
  gdouble y_scale_step_height;

  gdouble x_step;
  gdouble y_step;
  
  gdouble x_start;
  gdouble x_end;

  gdouble y_start;
  gdouble y_end;

  gdouble x_unit_x0;
  gdouble x_unit_y0;
  gdouble x_unit_size;
    
  gchar *x_unit;

  gdouble y_unit_x0;
  gdouble y_unit_y0;
  gdouble y_unit_size;

  gchar *y_unit;
  
  gdouble x_label_start;
  gdouble x_label_step_width;
  
  gchar **x_label;
  
  gdouble y_label_start;
  gdouble y_label_step_height;

  gchar **y_label;
  
  gdouble (*step_conversion_func)(gdouble current,
				  gboolean is_abscissae,
				  gpointer data);

  void (*translate_func)(gdouble x,
			 gdouble y,
			 gdouble *ret_x,
			 gdouble *ret_y,
			 gpointer data); 

  gdouble (*x_small_scale_func)(gdouble value,
				gpointer data);
  gdouble (*x_big_scale_func)(gdouble value,
			      gpointer data);

  gdouble (*y_small_scale_func)(gdouble value,
				gpointer data);
  gdouble (*y_big_scale_func)(gdouble value,
			      gpointer data);

  gchar* (*x_label_func)(gdouble value,
			 gpointer data);
  gchar* (*y_label_func)(gdouble value,
			 gpointer data);

  gpointer x_step_data;
  gdouble x_step_factor;
  
  gpointer y_step_data;
  gdouble y_step_factor;

  gpointer translate_data;
  gdouble x_translate_point;
  gdouble y_translate_point;

  gpointer x_scale_data;
  gdouble x_small_scale_factor;
  gdouble x_big_scale_factor;

  gpointer y_scale_data;
  gdouble y_small_scale_factor;
  gdouble y_big_scale_factor;

  gpointer x_label_data;
  gdouble x_label_factor;
  gdouble x_label_precision;

  gpointer y_label_data;
  gdouble y_label_factor;
  gdouble y_label_precision;

  cairo_surface_t *surface;
  
  GList *plot;
};

struct _AgsCartesianClass
{
  GtkWidgetClass widget;
};

struct _AgsPlot
{			
  AgsPlotFillFlags fill_flags;

  guint n_points;
  gboolean join_points;
  gdouble **point;
  gdouble **point_color;
  gchar **point_label;

  guint n_bitmaps;
  guchar **bitmap;
  gdouble **bitmap_color;

  guint n_pixmaps;
  gdouble ***pixmap;
};

GType ags_cartesian_get_type(void);

/* plot data */
AgsPlot* ags_plot_alloc(guint n_points, guint n_bitmaps, guint n_pixmaps);
void ags_plot_free(AgsPlot *plot);

guint ags_plot_get_n_points(AgsPlot *plot);
void ags_plot_set_n_points(AgsPlot *plot,
			   guint n_points);

gboolean ags_plot_get_join_points(AgsPlot *plot);
void ags_plot_set_join_points(AgsPlot *plot,
			      gboolean join_points);

gdouble** ags_plot_get_point(AgsPlot *plot);
void ags_plot_set_point(AgsPlot *plot,
			gdouble **point);

gdouble** ags_plot_get_point_color(AgsPlot *plot);
void ags_plot_set_point_color(AgsPlot *plot,
			      gdouble **point_color);

gchar** ags_plot_get_point_label(AgsPlot *plot);
void ags_plot_set_point_label(AgsPlot *plot,
			      gchar **point_label);

guint ags_plot_get_n_bitmaps(AgsPlot *plot);
void ags_plot_set_n_bitmaps(AgsPlot *plot,
			    guint n_bitmaps);

guchar** ags_plot_get_bitmap(AgsPlot *plot);
void ags_plot_set_bitmap(AgsPlot *plot,
			 guchar **bitmap);

gdouble** ags_plot_get_bitmap_color(AgsPlot *plot);
void ags_plot_set_bitmap_color(AgsPlot *plot,
			       gdouble **bitmap_color);

guint ags_plot_get_n_pixmaps(AgsPlot *plot);
void ags_plot_set_n_pixmaps(AgsPlot *plot,
			    guint n_pixmaps);

guchar** ags_plot_get_pixmap(AgsPlot *plot);
void ags_plot_set_pixmap(AgsPlot *plot,
			 guchar **pixmap);

void ags_cartesian_add_plot(AgsCartesian *cartesian,
			    AgsPlot *plot);
void ags_cartesian_remove_plot(AgsCartesian *cartesian,
			       AgsPlot *plot);

/* properties */
gdouble ags_cartesian_get_x_margin(AgsCartesian *cartesian);
void ags_cartesian_set_x_margin(AgsCartesian *cartesian,
				gdouble x_margin);

gdouble ags_cartesian_get_y_margin(AgsCartesian *cartesian);
void ags_cartesian_set_y_margin(AgsCartesian *cartesian,
				gdouble y_margin);

gdouble ags_cartesian_get_center(AgsCartesian *cartesian);
void ags_cartesian_set_center(AgsCartesian *cartesian,
			      gdouble center);

gdouble ags_cartesian_get_line_width(AgsCartesian *cartesian);
void ags_cartesian_set_line_width(AgsCartesian *cartesian,
				  gdouble line_width);

gdouble ags_cartesian_get_point_radius(AgsCartesian *cartesian);
void ags_cartesian_set_point_radius(AgsCartesian *cartesian,
				    gdouble point_radius);

gdouble ags_cartesian_get_font_size(AgsCartesian *cartesian);
void ags_cartesian_set_font_size(AgsCartesian *cartesian,
				 gdouble font_size);

gdouble ags_cartesian_get_x_step_width(AgsCartesian *cartesian);
void ags_cartesian_set_x_step_width(AgsCartesian *cartesian,
				    gdouble x_step_width);

gdouble ags_cartesian_get_y_step_height(AgsCartesian *cartesian);
void ags_cartesian_set_y_step_height(AgsCartesian *cartesian,
				     gdouble y_step_height);

gdouble ags_cartesian_get_x_scale_step_width(AgsCartesian *cartesian);
void ags_cartesian_set_x_scale_step_width(AgsCartesian *cartesian,
					  gdouble x_scale_step_width);

gdouble ags_cartesian_get_y_scale_step_height(AgsCartesian *cartesian);
void ags_cartesian_set_y_scale_step_height(AgsCartesian *cartesian,
					   gdouble y_scale_step_height);

gdouble ags_cartesian_get_x_unit_x0(AgsCartesian *cartesian);
void ags_cartesian_set_x_unit_x0(AgsCartesian *cartesian,
				 gdouble x_unit_x0);

gdouble ags_cartesian_get_x_unit_y0(AgsCartesian *cartesian);
void ags_cartesian_set_x_unit_y0(AgsCartesian *cartesian,
				 gdouble x_unit_y0);

gdouble ags_cartesian_get_x_unit_size(AgsCartesian *cartesian);
void ags_cartesian_set_x_unit_size(AgsCartesian *cartesian,
				   gdouble x_unit_size);

gdouble ags_cartesian_get_y_unit_x0(AgsCartesian *cartesian);
void ags_cartesian_set_y_unit_x0(AgsCartesian *cartesian,
				 gdouble y_unit_x0);

gdouble ags_cartesian_get_y_unit_y0(AgsCartesian *cartesian);
void ags_cartesian_set_y_unit_y0(AgsCartesian *cartesian,
				 gdouble y_unit_y0);

gdouble ags_cartesian_get_y_unit_size(AgsCartesian *cartesian);
void ags_cartesian_set_y_unit_size(AgsCartesian *cartesian,
				   gdouble y_unit_size);

gdouble ags_cartesian_get_x_label_start(AgsCartesian *cartesian);
void ags_cartesian_set_x_label_start(AgsCartesian *cartesian,
				     gdouble x_label_start);

gdouble ags_cartesian_get_x_label_step_width(AgsCartesian *cartesian);
void ags_cartesian_set_x_label_step_width(AgsCartesian *cartesian,
					  gdouble x_label_step_width);

gdouble ags_cartesian_get_y_label_start(AgsCartesian *cartesian);
void ags_cartesian_set_y_label_start(AgsCartesian *cartesian,
				     gdouble y_label_start);

gdouble ags_cartesian_get_y_label_step_height(AgsCartesian *cartesian);
void ags_cartesian_set_y_label_step_height(AgsCartesian *cartesian,
					   gdouble y_label_step_height);

gdouble ags_cartesian_get_x_step(AgsCartesian *cartesian);
void ags_cartesian_set_x_step(AgsCartesian *cartesian,
			      gdouble x_step);

gdouble ags_cartesian_get_y_step(AgsCartesian *cartesian);
void ags_cartesian_set_y_step(AgsCartesian *cartesian,
			      gdouble y_step);

gdouble ags_cartesian_get_x_start(AgsCartesian *cartesian);
void ags_cartesian_set_x_start(AgsCartesian *cartesian,
			       gdouble x_start);

gdouble ags_cartesian_get_x_end(AgsCartesian *cartesian);
void ags_cartesian_set_x_end(AgsCartesian *cartesian,
			     gdouble x_end);

gdouble ags_cartesian_get_y_start(AgsCartesian *cartesian);
void ags_cartesian_set_y_start(AgsCartesian *cartesian,
			       gdouble y_start);

gdouble ags_cartesian_get_y_end(AgsCartesian *cartesian);
void ags_cartesian_set_y_end(AgsCartesian *cartesian,
			     gdouble y_end);

gchar* ags_cartesian_get_x_unit(AgsCartesian *cartesian);
void ags_cartesian_set_x_unit(AgsCartesian *cartesian,
			      gchar *x_unit);

gchar* ags_cartesian_get_y_unit(AgsCartesian *cartesian);
void ags_cartesian_set_y_unit(AgsCartesian *cartesian,
			      gchar *y_unit);

gchar** ags_cartesian_get_x_label(AgsCartesian *cartesian);
void ags_cartesian_set_x_label(AgsCartesian *cartesian,
			       gchar **x_label);

gchar** ags_cartesian_get_y_label(AgsCartesian *cartesian);
void ags_cartesian_set_y_label(AgsCartesian *cartesian,
			       gchar **y_label);

gdouble ags_cartesian_get_x_step_factor(AgsCartesian *cartesian);
void ags_cartesian_set_x_step_factor(AgsCartesian *cartesian,
				     gdouble x_step_factor);

gdouble ags_cartesian_get_y_step_factor(AgsCartesian *cartesian);
void ags_cartesian_set_y_step_factor(AgsCartesian *cartesian,
				     gdouble y_step_factor);

gdouble ags_cartesian_get_x_small_scale_factor(AgsCartesian *cartesian);
void ags_cartesian_set_x_small_scale_factor(AgsCartesian *cartesian,
					    gdouble x_small_scale_factor);

gdouble ags_cartesian_get_x_big_scale_factor(AgsCartesian *cartesian);
void ags_cartesian_set_x_big_scale_factor(AgsCartesian *cartesian,
					  gdouble x_big_scale_factor);

gdouble ags_cartesian_get_y_small_scale_factor(AgsCartesian *cartesian);
void ags_cartesian_set_y_small_scale_factor(AgsCartesian *cartesian,
					    gdouble y_small_scale_factor);

gdouble ags_cartesian_get_y_big_scale_factor(AgsCartesian *cartesian);
void ags_cartesian_set_y_big_scale_factor(AgsCartesian *cartesian,
					  gdouble y_big_scale_factor);

gdouble ags_cartesian_get_x_label_factor(AgsCartesian *cartesian);
void ags_cartesian_set_x_label_factor(AgsCartesian *cartesian,
				      gdouble x_label_factor);

gdouble ags_cartesian_get_x_label_precision(AgsCartesian *cartesian);
void ags_cartesian_set_x_label_precision(AgsCartesian *cartesian,
					 gdouble x_label_precision);

gdouble ags_cartesian_get_y_label_factor(AgsCartesian *cartesian);
void ags_cartesian_set_y_label_factor(AgsCartesian *cartesian,
				      gdouble y_label_factor);

gdouble ags_cartesian_get_y_label_precision(AgsCartesian *cartesian);
void ags_cartesian_set_y_label_precision(AgsCartesian *cartesian,
					 gdouble y_label_precision);

cairo_surface_t* ags_cartesian_get_surface(AgsCartesian *cartesian);

/* predefined linear system */
gdouble ags_cartesian_linear_step_conversion_func(gdouble current,
						  gboolean is_abscissae,
						  gpointer data);

void ags_cartesian_linear_translate_func(gdouble x,
					 gdouble y,
					 gdouble *ret_x,
					 gdouble *ret_y,
					 gpointer data);

gdouble ags_cartesian_linear_x_small_scale_func(gdouble value,
						gpointer data);
gdouble ags_cartesian_linear_x_big_scale_func(gdouble value,
					      gpointer data);

gdouble ags_cartesian_linear_y_small_scale_func(gdouble value,
						gpointer data);
gdouble ags_cartesian_linear_y_big_scale_func(gdouble value,
					      gpointer data);

gchar* ags_cartesian_linear_x_label_func(gdouble value,
					 gpointer data);
gchar* ags_cartesian_linear_y_label_func(gdouble value,
					 gpointer data);

/* label functions */
void ags_cartesian_reallocate_label(AgsCartesian *cartesian,
				    gboolean do_x_label);
void ags_cartesian_fill_label(AgsCartesian *cartesian,
			      gboolean do_x_label);

/* instantiate */
AgsCartesian* ags_cartesian_new();

G_END_DECLS

#endif /*__AGS_CARTESIAN_H__*/
