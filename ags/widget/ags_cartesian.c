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

#include <ags/widget/ags_cartesian.h>

#include <stdlib.h>
#include <math.h>

void ags_cartesian_class_init(AgsCartesianClass *cartesian);
void ags_cartesian_init(AgsCartesian *cartesian);
void ags_cartesian_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec);
void ags_cartesian_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec);
void ags_cartesian_finalize(GObject *gobject);
void ags_cartesian_show(GtkWidget *widget);

void ags_cartesian_map(GtkWidget *widget);
void ags_cartesian_realize(GtkWidget *widget);
void ags_cartesian_size_allocate(GtkWidget *widget,
				 GtkAllocation *allocation);
void ags_cartesian_get_preferred_width(GtkWidget *widget,
				       gint *minimum_width,
				       gint *natural_width);
void ags_cartesian_get_preferred_height(GtkWidget *widget,
					gint *minimum_height,
					gint *natural_height);

void ags_cartesian_draw_putpixel(guchar *data,
				 gdouble width, gdouble height,
				 guint stride,
				 int x, int y, unsigned long int pixel);

void ags_cartesian_draw(AgsCartesian *cartesian, cairo_t *cr);

/**
 * SECTION:ags_cartesian
 * @short_description: A cartesian widget
 * @title: AgsCartesian
 * @section_id:
 * @include: ags/widget/ags_cartesian.h
 *
 * #AgsCartesian enables you to plot data and shows a grid pattern and
 * some scales.
 */

enum{
  PROP_0,
  PROP_X_MARGIN,
  PROP_Y_MARGIN,
  PROP_CENTER,
  PROP_LINE_WIDTH,
  PROP_POINT_RADIUS,
  PROP_FONT_SIZE,
  PROP_X_STEP_WIDTH,
  PROP_Y_STEP_HEIGHT,
  PROP_X_SCALE_STEP_WIDTH,
  PROP_Y_SCALE_STEP_HEIGHT,
  PROP_X_UNIT_X0,
  PROP_X_UNIT_Y0,
  PROP_X_UNIT_SIZE,
  PROP_Y_UNIT_X0,
  PROP_Y_UNIT_Y0,
  PROP_Y_UNIT_SIZE,
  PROP_X_LABEL_START,
  PROP_X_LABEL_STEP_WIDTH,
  PROP_Y_LABEL_START,
  PROP_Y_LABEL_STEP_HEIGHT,
  PROP_X_STEP,
  PROP_Y_STEP,
  PROP_X_START,
  PROP_X_END,
  PROP_Y_START,
  PROP_Y_END,
  PROP_X_UNIT,
  PROP_Y_UNIT,
  PROP_X_LABEL,
  PROP_Y_LABEL,
  PROP_X_STEP_DATA,
  PROP_X_STEP_FACTOR,
  PROP_Y_STEP_DATA,
  PROP_Y_STEP_FACTOR,
  PROP_TRANSLATE_DATA,
  PROP_X_TRANSLATE_POINT,
  PROP_Y_TRANSLATE_POINT,
  PROP_X_SCALE_DATA,
  PROP_X_SMALL_SCALE_FACTOR,
  PROP_X_BIG_SCALE_FACTOR,
  PROP_Y_SCALE_DATA,
  PROP_Y_SMALL_SCALE_FACTOR,
  PROP_Y_BIG_SCALE_FACTOR,
  PROP_X_LABEL_DATA,
  PROP_X_LABEL_FACTOR,
  PROP_X_LABEL_PRECISISON,
  PROP_Y_LABEL_DATA,
  PROP_Y_LABEL_FACTOR,
  PROP_Y_LABEL_PRECISISON,
  PROP_SURFACE,
  PROP_PLOT,
};

static gpointer ags_cartesian_parent_class = NULL;

GType
ags_cartesian_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_cartesian;

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
						"AgsCartesian", &ags_cartesian_info,
						0);

    g_once_init_leave (&g_define_type_id__volatile, ags_type_cartesian);
  }

  return g_define_type_id__volatile;
}

void
ags_cartesian_class_init(AgsCartesianClass *cartesian)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_cartesian_parent_class = g_type_class_peek_parent(cartesian);

  /* GObjectClass */
  gobject = (GObjectClass *) cartesian;

  gobject->set_property = ags_cartesian_set_property;
  gobject->get_property = ags_cartesian_get_property;

  gobject->finalize = ags_cartesian_finalize;

  /* properties */
  /**
   * AgsCartesian:x-margin:
   *
   * The horizontal x margin.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("x-margin",
				   "x margin",
				   "The horizontal x margin",
				   0.0,
				   G_MAXDOUBLE,
				   AGS_CARTESIAN_DEFAULT_X_MARGIN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_X_MARGIN,
				  param_spec);

  /**
   * AgsCartesian:y-margin:
   *
   * The horizontal y margin. 
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("y-margin",
				   "y margin",
				   "The vertical y margin",
				   0.0,
				   G_MAXDOUBLE,
				   AGS_CARTESIAN_DEFAULT_Y_MARGIN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_Y_MARGIN,
				  param_spec);

  /**
   * AgsCartesian:center:
   *
   * The center of lines
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("center",
				   "center of lines",
				   "The center of lines",
				   0.0,
				   G_MAXDOUBLE,
				   0.5,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CENTER,
				  param_spec);

  /**
   * AgsCartesian:line-width:
   *
   * The line width.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("line-width",
				   "line width",
				   "The line width",
				   0.0,
				   G_MAXDOUBLE,
				   1.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LINE_WIDTH,
				  param_spec);
  
  /**
   * AgsCartesian:point-radius:
   *
   * The points radius.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("point-radius",
				   "point radius",
				   "The points radius",
				   0.0,
				   G_MAXDOUBLE,
				   0.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_POINT_RADIUS,
				  param_spec);

  /**
   * AgsCartesian:font-size:
   *
   * The font's size to draw labels and units.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("font-size",
				   "font size",
				   "The font's size",
				   0.0,
				   G_MAXDOUBLE,
				   12.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FONT_SIZE,
				  param_spec);

  /**
   * AgsCartesian:x-step-width:
   *
   * The width of a x step.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("x-step-width",
				   "x step width",
				   "The x step width",
				   0.0,
				   G_MAXDOUBLE,
				   AGS_CARTESIAN_DEFAULT_X_STEP_WIDTH,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_X_STEP_WIDTH,
				  param_spec);

  /**
   * AgsCartesian:y-step-height:
   *
   * The height of a y step.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("y-step-height",
				   "y step height",
				   "The y step height",
				   0.0,
				   G_MAXDOUBLE,
				   AGS_CARTESIAN_DEFAULT_Y_STEP_HEIGHT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_Y_STEP_HEIGHT,
				  param_spec);

  /**
   * AgsCartesian:x-scale-step-width:
   *
   * The width of a x scale step.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("x-scale-step-width",
				   "x scale step width",
				   "The x step width",
				   0.0,
				   G_MAXDOUBLE,
				   AGS_CARTESIAN_DEFAULT_X_SCALE_STEP_WIDTH,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_X_SCALE_STEP_WIDTH,
				  param_spec);

  /**
   * AgsCartesian:y-scale-step-height:
   *
   * The height of a y scale step.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("y-scale-step-height",
				   "y scale step height",
				   "The y scale step height",
				   0.0,
				   G_MAXDOUBLE,
				   AGS_CARTESIAN_DEFAULT_Y_SCALE_STEP_HEIGHT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_Y_SCALE_STEP_HEIGHT,
				  param_spec);

  /**
   * AgsCartesian:x-step:
   *
   * The x step.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("x-step",
				   "x step",
				   "The x step",
				   0.0,
				   G_MAXDOUBLE,
				   AGS_CARTESIAN_DEFAULT_X_STEP,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_X_STEP,
				  param_spec);

  /**
   * AgsCartesian:y-step:
   *
   * The y step.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("y-step",
				   "y step",
				   "The y step",
				   0.0,
				   G_MAXDOUBLE,
				   AGS_CARTESIAN_DEFAULT_Y_STEP,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_Y_STEP,
				  param_spec);
  
  /**
   * AgsCartesian:x-start:
   *
   * The x start.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("x-start",
				   "x start",
				   "The x start",
				   -1.0 * G_MAXDOUBLE,
				   G_MAXDOUBLE,
				   AGS_CARTESIAN_DEFAULT_X_START,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_X_START,
				  param_spec);

  /**
   * AgsCartesian:x-end:
   *
   * The x end.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("x-end",
				   "x end",
				   "The x end",
				   -1.0 * G_MAXDOUBLE,
				   G_MAXDOUBLE,
				   AGS_CARTESIAN_DEFAULT_X_END,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_X_END,
				  param_spec);
  
  /**
   * AgsCartesian:y-start:
   *
   * The y start.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("y-start",
				   "y start",
				   "The y start",
				   -1.0 * G_MAXDOUBLE,
				   G_MAXDOUBLE,
				   AGS_CARTESIAN_DEFAULT_Y_START,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_Y_START,
				  param_spec);

  /**
   * AgsCartesian:y-end:
   *
   * The y end.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("y-end",
				   "y end",
				   "The y end",
				   -1.0 * G_MAXDOUBLE,
				   G_MAXDOUBLE,
				   AGS_CARTESIAN_DEFAULT_Y_END,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_Y_END,
				  param_spec);

  /**
   * AgsCartesian:x-unit-x0:
   *
   * The x unit's x0 position.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("x-unit-x0",
				   "x unit x0",
				   "The x unit's x0 position",
				   0.0,
				   G_MAXDOUBLE,
				   0.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_X_UNIT_X0,
				  param_spec);

  /**
   * AgsCartesian:x-unit-y0:
   *
   * The x unit's y0 position.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("x-unit-y0",
				   "x unit y0",
				   "The x unit's y0 position",
				   0.0,
				   G_MAXDOUBLE,
				   0.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_X_UNIT_Y0,
				  param_spec);

  /**
   * AgsCartesian:x-unit:
   *
   * The x unit label.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("x-unit",
				   "x unit",
				   "The x unit label",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_X_UNIT,
				  param_spec);
  
  /**
   * AgsCartesian:y-unit-x0:
   *
   * The y unit's x0 position.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("y-unit-x0",
				   "y unit x0",
				   "The y unit's x0 position",
				   0.0,
				   G_MAXDOUBLE,
				   0.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_Y_UNIT_X0,
				  param_spec);

  /**
   * AgsCartesian:y-unit-y0:
   *
   * The y unit's y0 position.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("y-unit-y0",
				   "y unit y0",
				   "The y unit's y0 position",
				   0.0,
				   G_MAXDOUBLE,
				   0.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_Y_UNIT_Y0,
				  param_spec);
  
  /**
   * AgsCartesian:y-unit:
   *
   * The y unit label.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("y-unit",
				   "y unit",
				   "The y unit label",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_Y_UNIT,
				  param_spec);

  /**
   * AgsCartesian:x-label-start:
   *
   * The x label start position.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("x-label-start",
				   "x label start",
				   "The x label start position",
				   0.0,
				   G_MAXDOUBLE,
				   AGS_CARTESIAN_DEFAULT_X_LABEL_START,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_X_LABEL_START,
				  param_spec);

  /**
   * AgsCartesian:x-label-step-width:
   *
   * The x label step width.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("x-label-step-width",
				   "x label step width",
				   "The x label step width",
				   0.0,
				   G_MAXDOUBLE,
				   AGS_CARTESIAN_DEFAULT_X_LABEL_STEP_WIDTH,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_X_LABEL_STEP_WIDTH,
				  param_spec);

  /**
   * AgsCartesian:x-label:
   *
   * The x labels as a string array.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("x-label",
				    "x label",
				    "The x labels as string array",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_X_LABEL,
				  param_spec);

  /**
   * AgsCartesian:y-label-start:
   *
   * The y label start position.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("y-label-start",
				   "y label start",
				   "The y label start position",
				   0.0,
				   G_MAXDOUBLE,
				   AGS_CARTESIAN_DEFAULT_Y_LABEL_START,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_Y_LABEL_START,
				  param_spec);

  /**
   * AgsCartesian:y-label-step-height:
   *
   * The y label step height.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("y-label-step-height",
				   "y label step height",
				   "The y label step height",
				   0.0,
				   G_MAXDOUBLE,
				   AGS_CARTESIAN_DEFAULT_Y_LABEL_STEP_HEIGHT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_Y_LABEL_STEP_HEIGHT,
				  param_spec);

  /**
   * AgsCartesian:y-label:
   *
   * The y labels as a string array.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("y-label",
				    "y label",
				    "The y labels as string array",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_Y_LABEL,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) cartesian;

  //  widget->map = ags_cartesian_map;
  widget->realize = ags_cartesian_realize;
  widget->size_allocate = ags_cartesian_size_allocate;
  widget->get_preferred_width = ags_cartesian_get_preferred_width;
  widget->get_preferred_height = ags_cartesian_get_preferred_height;
  widget->show = ags_cartesian_show;
  widget->draw = ags_cartesian_draw;
}

void
ags_cartesian_init(AgsCartesian *cartesian)
{
  guint i, i_stop;
  
  g_object_set(G_OBJECT(cartesian),
	       "app-paintable", TRUE,
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
  cartesian->center = 0.5;

  cartesian->line_width = 1.0;
  cartesian->point_radius = 1.2;

  cartesian->font_size = 8.0;
  
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
  cartesian->x_label_start = AGS_CARTESIAN_DEFAULT_X_LABEL_START;
  cartesian->x_label_step_width = AGS_CARTESIAN_DEFAULT_X_LABEL_STEP_WIDTH;
  
  cartesian->y_label_start = AGS_CARTESIAN_DEFAULT_Y_LABEL_START;
  cartesian->y_label_step_height = AGS_CARTESIAN_DEFAULT_Y_LABEL_STEP_HEIGHT;

  /* region alignment */
  cartesian->x_step = AGS_CARTESIAN_DEFAULT_X_STEP;
  cartesian->y_step = AGS_CARTESIAN_DEFAULT_Y_STEP;

  cartesian->x_start = AGS_CARTESIAN_DEFAULT_X_START;
  cartesian->x_end = AGS_CARTESIAN_DEFAULT_X_END;
  
  cartesian->y_start = AGS_CARTESIAN_DEFAULT_Y_START;
  cartesian->y_end = AGS_CARTESIAN_DEFAULT_Y_END;

  /* unit */
  cartesian->x_unit = "x";
  cartesian->y_unit = "y";

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
  cartesian->x_label_factor = 5.0;
  cartesian->x_label_precision = 3.0;
  
  cartesian->y_label_data = cartesian;
  cartesian->y_label_factor = 5.0;
  cartesian->y_label_precision = 3.0;

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
ags_cartesian_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec)
{
  //TODO:JK: implement me
}

void
ags_cartesian_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec)
{
  //TODO:JK: implement me
}

void
ags_cartesian_finalize(GObject *gobject)
{
  //TODO:JK: implement me

  G_OBJECT_CLASS(ags_cartesian_parent_class)->finalize(gobject);
}

void
ags_cartesian_map(GtkWidget *widget)
{
  if(gtk_widget_get_realized (widget) && !gtk_widget_get_mapped(widget)){
    GTK_WIDGET_CLASS(ags_cartesian_parent_class)->map(widget);
    
    gdk_window_show(gtk_widget_get_window(widget));
//    ags_cartesian_draw((AgsCartesian *) widget);
  }
}

void
ags_cartesian_realize(GtkWidget *widget)
{
  AgsCartesian *cartesian;

  GdkWindow *window;
  
  GtkAllocation allocation;
  
  GdkWindowAttr attributes;

  gint attributes_mask;

  g_return_if_fail(widget != NULL);
  g_return_if_fail(AGS_IS_CARTESIAN(widget));

  cartesian = AGS_CARTESIAN(widget);

  gtk_widget_set_realized(widget, TRUE);

  /*  */
  attributes.window_type = GDK_WINDOW_CHILD;

  gtk_widget_get_allocation(widget,
			    &allocation);
  
  attributes.x = allocation.x;
  attributes.y = allocation.y;
  attributes.width = allocation.width;
  attributes.height = allocation.height;

  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL;

  attributes.window_type = GDK_WINDOW_CHILD;

  attributes.wclass = GDK_INPUT_OUTPUT;
  attributes.visual = gtk_widget_get_visual (widget);
  attributes.event_mask = gtk_widget_get_events (widget);
  attributes.event_mask |= (GDK_EXPOSURE_MASK);

  window = gdk_window_new(gtk_widget_get_parent_window (widget),
			  &attributes, attributes_mask);
  gtk_widget_set_window(widget, window);
  gdk_window_set_user_data(window, cartesian);

  gtk_widget_queue_resize(widget);
}

void
ags_cartesian_show(GtkWidget *widget)
{
  GTK_WIDGET_CLASS(ags_cartesian_parent_class)->show(widget);
}

void
ags_cartesian_size_allocate(GtkWidget *widget,
			    GtkAllocation *allocation)
{
  GTK_WIDGET_CLASS(ags_cartesian_parent_class)->size_allocate(widget,
							      allocation);

  //TODO:JK: implement me
}

void
ags_cartesian_get_preferred_width(GtkWidget *widget,
				  gint *minimum_width,
				  gint *natural_width)
{
  GTK_WIDGET_CLASS(ags_cartesian_parent_class)->get_preferred_width(widget,
								    minimum_width,
								    natural_width);

  //TODO:JK: implement me
}

void
ags_cartesian_get_preferred_height(GtkWidget *widget,
				   gint *minimum_height,
				   gint *natural_height)
{
  GTK_WIDGET_CLASS(ags_cartesian_parent_class)->get_preferred_height(widget,
								     minimum_height,
								     natural_height);

  //TODO:JK: implement me
}

void
ags_cartesian_draw_putpixel(guchar *data,
			    gdouble width, gdouble height,
			    guint stride,
			    int x, int y, unsigned long int pixel)
{
  int bpp = stride / width;
  /* Here p is the address to the pixel we want to set */
  guchar *p = data + y * stride + x * bpp;

  switch(bpp) {
  case 1:
    *p = pixel;
    break;

  case 2:
    *(gint16 *)p = pixel;
    break;

  case 3:
    p[0] = pixel & 0xff;
    p[1] = (pixel >> 8) & 0xff;
    p[2] = (pixel >> 16) & 0xff;
    break;

  case 4:
    *(gint32 *)p = pixel;
    break;
  }
}

void
ags_cartesian_draw(AgsCartesian *cartesian, cairo_t *cr)
{
  GtkWidget *widget;

  GtkStyleContext *cartesian_style_context;

  GtkAllocation allocation;
  
  GdkRGBA *fg_color;
  GdkRGBA *fg_color_active;
  GdkRGBA *bg_color;
  GdkRGBA *border_color;

  GList *list;
  
  guchar *data;
  guint32 *data_ptr;

  gchar *font_name;
  
  gdouble x, y;
  gdouble x_offset, y_offset;
  gdouble width, height;
  gdouble factor;
  gdouble scale_point;
  guint32 clear_color;
  guint stride;
  guint i_stop;
  guint i;

  GValue value = {0,};  
  
  /* entry point */
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }
  
  widget = GTK_WIDGET(cartesian);

  gtk_widget_get_allocation(GTK_WIDGET(cartesian),
			    &allocation);

  /* style context */
  cartesian_style_context = gtk_widget_get_style_context(GTK_WIDGET(cartesian));

  gtk_style_context_get_property(cartesian_style_context,
				 "color",
				 GTK_STATE_FLAG_NORMAL,
				 &value);

  fg_color = g_value_dup_boxed(&value);
  g_value_unset(&value);

  gtk_style_context_get_property(cartesian_style_context,
				 "color",
				 GTK_STATE_FLAG_ACTIVE,
				 &value);

  fg_color_active = g_value_dup_boxed(&value);
  g_value_unset(&value);

  gtk_style_context_get_property(cartesian_style_context,
				 "background-color",
				 GTK_STATE_FLAG_NORMAL,
				 &value);

  bg_color = g_value_dup_boxed(&value);
  g_value_unset(&value);

  gtk_style_context_get_property(cartesian_style_context,
				 "border-color",
				 GTK_STATE_FLAG_NORMAL,
				 &value);

  border_color = g_value_dup_boxed(&value);
  g_value_unset(&value);

  g_object_get(gtk_settings_get_default(),
	       "gtk-font-name", &font_name,
	       NULL);
  
  /* clear bg */
#if 0
  cairo_set_source_rgba(cr,
			bg_color->red,
			bg_color->green,
			bg_color->blue,
			bg_color->alpha);

  cairo_rectangle(cr,
		  0.0, 0.0,
		  allocation.width, allocation.height);
  cairo_fill(cr);
#else
  gtk_render_background(cartesian_style_context,
			cr,
			0.0, 0.0,
			(gdouble) allocation.width, (gdouble) allocation.height);
#endif
  
  /* cartesian offset, width and height */
  width = (cartesian->x_end - cartesian->x_start);
  height = (cartesian->y_end - cartesian->y_start);

  x_offset = cartesian->x_margin - cartesian->x_start - cartesian->center;
  y_offset = cartesian->y_margin + cartesian->y_start + height + cartesian->center;
  
  /* clear surface */
  data = cairo_image_surface_get_data(cartesian->surface);
  stride = cairo_image_surface_get_stride(cartesian->surface);

  data_ptr = data;
  
  clear_color = (((guint) (255.0 * bg_color->red) << 16) |
		 ((guint) (255.0 * bg_color->green) << 8) |
		 ((guint) (255.0 * bg_color->blue)));
  
  //  memset(data, clear_color, (4 * width * height * sizeof(guchar)));
  for(i = 0; i < width * height; i++){
    data_ptr[i] = clear_color;
  }
  
//  cairo_surface_flush(cartesian->surface);

  /* surface */
  cairo_set_source_surface(cr,
  			   cartesian->surface,
  			   cartesian->x_margin, cartesian->y_margin);
  cairo_surface_mark_dirty(cartesian->surface);

  cairo_paint(cr);

  /* draw plot */
  list = cartesian->plot;

  while(list != NULL){
    AgsPlot *plot;
      
    guint i;

    plot = list->data;
      
    cairo_set_source_rgba(cr,
			  fg_color_active->red,
			  fg_color_active->blue,
			  fg_color_active->green,
			  fg_color_active->alpha);

    /* points */
    for(i = 0; i < plot->n_points; i++){
      if(plot->point[i] != NULL){
	if(plot->point_color != NULL &&
	   plot->point_color[i] != NULL){
	  cairo_set_source_rgb(cr,
			       plot->point_color[i][0],
			       plot->point_color[i][1],
			       plot->point_color[i][2]);
	}
	
#ifdef AGS_DEBUG
	g_message("%f %f", plot->point[i][0], plot->point[i][1]);
#endif
	
	cairo_arc(cr,
		  x_offset + plot->point[i][0] - cartesian->point_radius,
		  y_offset - (plot->point[i][1] - cartesian->point_radius),
		  cartesian->point_radius,
		  -1.0 * M_PI,
		  1.0 * M_PI);

	if(plot->join_points){
	  if(i == 0){
	    cairo_move_to(cr,
			  x_offset, y_offset);
	    
	    cairo_line_to(cr,
			  x_offset + plot->point[i][0], y_offset - plot->point[i][1]);
	  }else{
	    cairo_line_to(cr,
			  x_offset + plot->point[i][0], y_offset - plot->point[i][1]);
	  }
	}
      }
    }
    
    cairo_stroke(cr);
    cairo_fill(cr);

    /* bitmaps */
    for(i = 0; i < plot->n_bitmaps; i++){
      if(plot->bitmap[i] != NULL){
	unsigned long int pixel;
	guint nth;
	guint x, y;

	if(plot->bitmap_color != NULL &&
	   plot->bitmap_color[i] != NULL){
	  pixel = ((255 << 16) * plot->bitmap_color[i][0] +
		   (255 << 8) * plot->bitmap_color[i][1] +
		   255 * plot->bitmap_color[i][0]);
	}else{
	  pixel = 0xffffff;
	}
	
	for(y = 0, nth = 0; y < height; y++){
	  for(x = 0; x < width; x++, nth++){
	    if(((1 << (nth % 8)) & (plot->bitmap[i][(guint) floor(nth / 8.0)])) != 0){
	      ags_cartesian_draw_putpixel(data,
					  width, height,
					  stride,
					  x, y, pixel);
	    }
	  }
	}
      }
    }

    /* pixmaps */
    for(i = 0; i < plot->n_pixmaps; i++){
      if(plot->pixmap[i] != NULL){
	guint nth;
	guint x, y;
	
	for(y = 0, nth = 0; y < height; y++){
	  for(x = 0; x < width; x++, nth++){
	    if(plot->pixmap[i][nth] != NULL){
	      unsigned long int pixel;

	      pixel = ((255 << 16) * plot->pixmap[i][nth][0] +
		       (255 << 8) * plot->pixmap[i][nth][1] +
		       255 * plot->pixmap[i][nth][0]);
	      
	      ags_cartesian_draw_putpixel(data,
					  width, height,
					  stride,
					  x, y, pixel);
	    }
	  }
	}
      }
    }

    list = list->next;
  }

  /* push group */
  cairo_push_group(cr);

  {
    static const double dashes[] = {
      2.0,
      3.0
    };

    /*  line width */
    cairo_set_line_width(cr, cartesian->line_width);

    /* color */
    cairo_set_source_rgba(cr,
			  fg_color->red,
			  fg_color->blue,
			  fg_color->green,
			  fg_color->alpha);
    
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

  /* abscissae */
  if((AGS_CARTESIAN_ABSCISSAE & (cartesian->flags)) != 0){
    if((cartesian->x_end == 0.0) ||
       (cartesian->x_start == 0.0) ||
       (cartesian->x_start < 0.0 &&
	cartesian->x_end > 0.0)){
      /* small scale */
      {
	/*  line width */
	cairo_set_line_width(cr, cartesian->line_width);

	/* color */
	cairo_set_source_rgba(cr,
			      fg_color->red,
			      fg_color->blue,
			      fg_color->green,
			      fg_color->alpha);
    
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

      /* big scale */
      {
	/* line width */
	cairo_set_line_width(cr, cartesian->line_width);

	/* color */
	cairo_set_source_rgba(cr,
			      fg_color->red,
			      fg_color->blue,
			      fg_color->green,
			      fg_color->alpha);

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
    }
  }

  /* ordinate */
  if((AGS_CARTESIAN_ORDINATE & (cartesian->flags)) != 0){
    if((cartesian->y_end == 0.0) ||
       (cartesian->y_start == 0.0) ||
       (cartesian->y_start < 0.0 &&
	cartesian->y_end > 0.0)){
      /* small scale */
      {
	/* color and line width */
	cairo_set_source_rgba(cr,
			      fg_color->red,
			      fg_color->blue,
			      fg_color->green,
			      fg_color->alpha);
    
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

      /* big scale */
      {
	/* line width */
	cairo_set_line_width(cr, cartesian->line_width);

	/* color */
	cairo_set_source_rgba(cr,
			      fg_color->red,
			      fg_color->blue,
			      fg_color->green,
			      fg_color->alpha);

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
    }
  }

  /* x and y label */
  if((AGS_CARTESIAN_X_LABEL & (cartesian->flags)) != 0){
    guint i;

    for(i = 0; i < width / cartesian->x_label_step_width; i++){
      PangoLayout *layout;
      PangoFontDescription *desc;

      PangoRectangle ink_rect, logical_rect;

      if(cartesian->x_label[i] == NULL){
	break;
      }
      
      layout = pango_cairo_create_layout(cr);
      pango_layout_set_text(layout,
			    cartesian->x_label[i],
			    -1);
      desc = pango_font_description_from_string(font_name);
      pango_font_description_set_size(desc,
				      cartesian->font_size * PANGO_SCALE);
      pango_layout_set_font_description(layout,
					desc);
      pango_font_description_free(desc);    

      pango_layout_get_extents(layout,
			       &ink_rect,
			       &logical_rect);  

      cairo_move_to(cr,
		    cartesian->x_margin + cartesian->x_label_start + (i * cartesian->x_label_step_width) + cartesian->font_size / 3.0,
		    cartesian->y_margin + height + cartesian->y_start + cartesian->font_size + (logical_rect.height / PANGO_SCALE));
      
      pango_cairo_show_layout(cr,
			      layout);
      
      g_object_unref(layout);
    }
  }
  
  if((AGS_CARTESIAN_Y_LABEL & (cartesian->flags)) != 0){
    guint i;

    for(i = 0; i < height / cartesian->y_label_step_height; i++){
      PangoLayout *layout;
      PangoFontDescription *desc;

      PangoRectangle ink_rect, logical_rect;

      if(cartesian->y_label[i] == NULL){
	break;
      }

      layout = pango_cairo_create_layout(cr);
      pango_layout_set_text(layout,
			    cartesian->y_label[i],
			    -1);
      desc = pango_font_description_from_string(font_name);
      pango_font_description_set_size(desc,
				      cartesian->font_size * PANGO_SCALE);
      pango_layout_set_font_description(layout,
					desc);
      pango_font_description_free(desc);    

      pango_layout_get_extents(layout,
			       &ink_rect,
			       &logical_rect);  

      cairo_move_to(cr,
		    cartesian->x_margin - cartesian->x_start + cartesian->font_size / 2.0,
		    cartesian->y_margin + height - cartesian->y_label_start - (i * cartesian->y_label_step_height) - cartesian->font_size / 3.0);
            
      pango_cairo_show_layout(cr,
			      layout);
      
      g_object_unref(layout);
    }

    cairo_stroke(cr);
  }

  /* x unit */
  if((AGS_CARTESIAN_X_UNIT & (cartesian->flags)) != 0){
    PangoLayout *layout;
    PangoFontDescription *desc;

    PangoRectangle ink_rect, logical_rect;

    layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout,
			  cartesian->x_unit,
			  -1);
    desc = pango_font_description_from_string(font_name);
    pango_font_description_set_size(desc,
				    cartesian->font_size * PANGO_SCALE);
    pango_layout_set_font_description(layout,
				      desc);
    pango_font_description_free(desc);    

    pango_layout_get_extents(layout,
			     &ink_rect,
			     &logical_rect);  

    cairo_move_to(cr,
		  cartesian->x_margin - (logical_rect.width / PANGO_SCALE) - 3.0,
		  cartesian->y_margin + height + cartesian->y_start);
      
    pango_cairo_show_layout(cr,
			    layout);
      
    g_object_unref(layout);
  }
  
  /* y unit */
  if((AGS_CARTESIAN_Y_UNIT & (cartesian->flags)) != 0){
    PangoLayout *layout;
    PangoFontDescription *desc;

    PangoRectangle ink_rect, logical_rect;

    layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout,
			  cartesian->y_unit,
			  -1);
    desc = pango_font_description_from_string(font_name);
    pango_font_description_set_size(desc,
				    cartesian->font_size * PANGO_SCALE);
    pango_layout_set_font_description(layout,
				      desc);
    pango_font_description_free(desc);    

    pango_layout_get_extents(layout,
			     &ink_rect,
			     &logical_rect);  

    cairo_move_to(cr,
		  cartesian->x_margin - cartesian->x_start,
		  cartesian->y_margin + height + (logical_rect.height / PANGO_SCALE) + 3.0);
      
    pango_cairo_show_layout(cr,
			    layout);
      
    g_object_unref(layout);
  }
  
  g_free(font_name);

  /* pop group */
  cairo_pop_group_to_source(cr);
  
  /* paint */
  cairo_paint(cr);

  g_boxed_free(GDK_TYPE_RGBA, fg_color);
  g_boxed_free(GDK_TYPE_RGBA, fg_color_active);
  g_boxed_free(GDK_TYPE_RGBA, bg_color);
  g_boxed_free(GDK_TYPE_RGBA, border_color);

//  cairo_surface_mark_dirty(cairo_get_target(cr));
}

/**
 * ags_plot_alloc:
 * @n_points: number of points
 * @n_bitmaps: number of bitmaps
 * @n_pixmaps: number of pixmaps
 * 
 * Allocate #AgsPlot-struct.
 * 
 * Returns: (type gpointer) (transfer full): the newly allocated #AgsPlot-struct
 * 
 * Since: 3.0.0
 */
AgsPlot*
ags_plot_alloc(guint n_points, guint n_bitmaps, guint n_pixmaps)
{
  AgsPlot *plot;

  guint i;

  plot = (AgsPlot *) g_malloc(sizeof(AgsPlot));

  plot->fill_flags = AGS_PLOT_FILL_REPLACE;

  /* points */
  plot->n_points = n_points;
  plot->join_points = FALSE;

  if(n_points == 0){
    plot->point = NULL;
    plot->point_color = NULL;
  }else{
    plot->point = (gdouble **) g_malloc(n_points * sizeof(gdouble *));
    plot->point_color = (gdouble **) g_malloc(n_points * sizeof(gdouble *));
    plot->point_label = (gchar **) g_malloc((n_points + 1) * sizeof(gchar *));

    for(i = 0; i < n_points; i++){
      plot->point[i] = (gdouble *) g_malloc(2 * sizeof(gdouble));
      plot->point[i][0] = 0.0;
      plot->point[i][1] = 0.0;
      
      plot->point_color[i] = (gdouble *) g_malloc(3 * sizeof(gdouble));
      plot->point_color[i][0] = 0.0;
      plot->point_color[i][1] = 0.0;
      plot->point_color[i][2] = 0.0;
      
      plot->point_label[i] = NULL;
    }

    plot->point_label[i] = NULL;
  }

  /* bitmaps */
  plot->n_bitmaps = n_bitmaps;
  
  if(n_bitmaps == 0){
    plot->bitmap = NULL;
    plot->bitmap_color = NULL;
  }else{
    plot->bitmap = (guchar **) g_malloc(n_bitmaps * sizeof(guchar *));
    plot->bitmap_color = (gdouble **) g_malloc(n_bitmaps * sizeof(gdouble *));

    for(i = 0; i < n_bitmaps; i++){
      plot->bitmap[i] = NULL;
      
      plot->bitmap_color[i] = (gdouble *) g_malloc(3 * sizeof(gdouble));
      plot->bitmap_color[i][0] = 1.0;
      plot->bitmap_color[i][1] = 0.0;
      plot->bitmap_color[i][2] = 0.0;
    }
  }
  
  /* pixmaps */
  plot->n_pixmaps = n_pixmaps;
  
  if(n_pixmaps == 0){
    plot->pixmap = NULL;
  }else{
    plot->pixmap = (gdouble ***) g_malloc(n_pixmaps * sizeof(gdouble **));

    for(i = 0; i < n_pixmaps; i++){
      plot->pixmap[i] = NULL;
    }
  }
  
  return(plot);
}

/**
 * ags_plot_free:
 * @plot: (type gpointer) (transfer full): the #AgsPlot-struct
 * 
 * Free @plot.
 * 
 * Since: 3.0.0
 */
void
ags_plot_free(AgsPlot *plot)
{
  guint i;
  
  if(plot == NULL){
    return;
  }

  /* points */
  for(i = 0; i < plot->n_points; i++){
    g_free(plot->point[i]);

    g_free(plot->point_color[i]);
    
    g_free(plot->point_label[i]);
  }

  g_free(plot->point);

  g_free(plot->point_color);

  g_free(plot->point_label);

  /* bitmap */
  for(i = 0; i < plot->n_bitmaps; i++){
    g_free(plot->bitmap[i]);

    g_free(plot->bitmap_color[i]);
  }

  g_free(plot->bitmap);

  g_free(plot->bitmap_color);

  /* pixmap */
  for(i = 0; i < plot->n_pixmaps; i++){
    g_free(plot->pixmap[i]);
  }

  g_free(plot->pixmap);

  g_free(plot);
}

/**
 * ags_plot_get_n_points:
 * @plot: (type gpointer): the #AgsPlot-struct
 * 
 * Get number of points.
 * 
 * Returns: n_points field
 * 
 * Since: 3.2.0
 */
guint
ags_plot_get_n_points(AgsPlot *plot)
{
  if(plot == NULL){
    return(0);
  }

  return(plot->n_points);
}

/**
 * ags_plot_set_n_points:
 * @plot: (type gpointer): the #AgsPlot-struct
 * @n_points: the number of points
 * 
 * Set n_points field of @plot.
 * 
 * Since: 3.2.0
 */
void
ags_plot_set_n_points(AgsPlot *plot,
		      guint n_points)
{
  if(plot == NULL){
    return;
  }

  plot->n_points = n_points;
}

/**
 * ags_plot_get_join_points:
 * @plot: (type gpointer): the #AgsPlot-struct
 * 
 * Get join points.
 * 
 * Returns: join_points field
 * 
 * Since: 3.2.0
 */
gboolean
ags_plot_get_join_points(AgsPlot *plot)
{
  if(plot == NULL){
    return(FALSE);
  }

  return(plot->join_points);
}

/**
 * ags_plot_set_join_points:
 * @plot: (type gpointer): the #AgsPlot-struct
 * @join_points: if %TRUE join points, otherwise not
 * 
 * Set join_points field of @plot.
 * 
 * Since: 3.2.0
 */
void
ags_plot_set_join_points(AgsPlot *plot,
			 gboolean join_points)
{
  if(plot == NULL){
    return;
  }

  plot->join_points = join_points;
}

/**
 * ags_plot_get_point:
 * @plot: (type gpointer): the #AgsPlot-struct
 * 
 * Get points.
 * 
 * Returns: (transfer none): point field
 * 
 * Since: 3.2.0
 */
gdouble**
ags_plot_get_point(AgsPlot *plot)
{
  if(plot == NULL){
    return(NULL);
  }

  return(plot->point);
}

/**
 * ags_plot_set_point:
 * @plot: (type gpointer): the #AgsPlot-struct
 * @point: (transfer full): the points as array of coordinates
 * 
 * Set point field of @plot.
 * 
 * Since: 3.2.0
 */
void
ags_plot_set_point(AgsPlot *plot,
		   gdouble **point)
{
  if(plot == NULL){
    return;
  }
  
  plot->point = point;
}

/**
 * ags_plot_get_point_color:
 * @plot: (type gpointer): the #AgsPlot-struct
 * 
 * Get point colors.
 * 
 * Returns: (transfer none): point color field
 * 
 * Since: 3.2.0
 */
gdouble**
ags_plot_get_point_color(AgsPlot *plot)
{
  if(plot == NULL){
    return(NULL);
  }

  return(plot->point_color);
}

/**
 * ags_plot_set_point_color:
 * @plot: (type gpointer): the #AgsPlot-struct
 * @point_color: (transfer full): the points colors as array of RGB value
 * 
 * Set point color field of @plot.
 * 
 * Since: 3.2.0
 */
void
ags_plot_set_point_color(AgsPlot *plot,
			 gdouble **point_color)
{
  if(plot == NULL){
    return;
  }
  
  plot->point_color = point_color;
}

/**
 * ags_plot_get_point_label:
 * @plot: (type gpointer): the #AgsPlot-struct
 * 
 * Get point labels.
 * 
 * Returns: (element-type utf8) (transfer none): point label field
 * 
 * Since: 3.2.0
 */
gchar**
ags_plot_get_point_label(AgsPlot *plot)
{
  if(plot == NULL){
    return(NULL);
  }

  return(plot->point_label);
}

/**
 * ags_plot_set_point_label:
 * @plot: (type gpointer): the #AgsPlot-struct
 * @point_label: (transfer full): the points labels as string vector
 * 
 * Set point label field of @plot.
 * 
 * Since: 3.2.0
 */
void
ags_plot_set_point_label(AgsPlot *plot,
			 gchar **point_label)
{
  if(plot == NULL){
    return;
  }
  
  plot->point_label = point_label;
}

/**
 * ags_plot_get_n_bitmaps:
 * @plot: (type gpointer): the #AgsPlot-struct
 * 
 * Get number of bitmaps.
 * 
 * Returns: n_bitmaps field
 * 
 * Since: 3.2.0
 */
guint
ags_plot_get_n_bitmaps(AgsPlot *plot)
{
  if(plot == NULL){
    return(0);
  }

  return(plot->n_bitmaps);
}

/**
 * ags_plot_set_n_bitmaps:
 * @plot: (type gpointer): the #AgsPlot-struct
 * @n_bitmaps: the number of bitmaps
 * 
 * Set n_bitmaps field of @plot.
 * 
 * Since: 3.2.0
 */
void
ags_plot_set_n_bitmaps(AgsPlot *plot,
		       guint n_bitmaps)
{
  if(plot == NULL){
    return;
  }
  
  plot->n_bitmaps = n_bitmaps;
}

/**
 * ags_plot_get_bitmap:
 * @plot: (type gpointer): the #AgsPlot-struct
 * 
 * Get bitmaps.
 * 
 * Returns: (transfer none): bitmap field
 * 
 * Since: 3.2.0
 */
guchar**
ags_plot_get_bitmap(AgsPlot *plot)
{
  if(plot == NULL){
    return(NULL);
  }

  return(plot->bitmap);
}

/**
 * ags_plot_set_bitmap:
 * @plot: (type gpointer): the #AgsPlot-struct
 * @bitmap: (transfer full): the bitmaps
 * 
 * Set bitmap field of @plot.
 * 
 * Since: 3.2.0
 */
void
ags_plot_set_bitmap(AgsPlot *plot,
		    guchar **bitmap)
{
  if(plot == NULL){
    return;
  }

  plot->bitmap = bitmap;
}

/**
 * ags_plot_get_bitmap_color:
 * @plot: (type gpointer): the #AgsPlot-struct
 * 
 * Get bitmap colors.
 * 
 * Returns: (transfer none): bitmap color field
 * 
 * Since: 3.2.0
 */
gdouble**
ags_plot_get_bitmap_color(AgsPlot *plot)
{
  if(plot == NULL){
    return(NULL);
  }

  return(plot->bitmap_color);
}

/**
 * ags_plot_set_bitmap_color:
 * @plot: (type gpointer): the #AgsPlot-struct
 * @bitmap_color: (transfer full): the bitmaps colors as array of RGB value
 * 
 * Set bitmap color field of @plot.
 * 
 * Since: 3.2.0
 */
void
ags_plot_set_bitmap_color(AgsPlot *plot,
			  gdouble **bitmap_color)
{
  if(plot == NULL){
    return;
  }
  
  plot->bitmap_color = bitmap_color;
}

/**
 * ags_plot_get_n_pixmaps:
 * @plot: (type gpointer): the #AgsPlot-struct
 * 
 * Get number of pixmaps.
 * 
 * Returns: n_pixmaps field
 * 
 * Since: 3.2.0
 */
guint
ags_plot_get_n_pixmaps(AgsPlot *plot)
{
  if(plot == NULL){
    return(0);
  }

  return(plot->n_pixmaps);
}

/**
 * ags_plot_set_n_pixmaps:
 * @plot: (type gpointer): the #AgsPlot-struct
 * @n_pixmaps: the number of pixmaps
 * 
 * Set n_pixmaps field of @plot.
 * 
 * Since: 3.2.0
 */
void
ags_plot_set_n_pixmaps(AgsPlot *plot,
		       guint n_pixmaps)
{
  if(plot == NULL){
    return;
  }
  
  plot->n_pixmaps = n_pixmaps;
}

/**
 * ags_plot_get_pixmap:
 * @plot: (type gpointer): the #AgsPlot-struct
 * 
 * Get pixmaps.
 * 
 * Returns: (transfer none): pixmap field
 * 
 * Since: 3.2.0
 */
guchar**
ags_plot_get_pixmap(AgsPlot *plot)
{
  if(plot == NULL){
    return(NULL);
  }

  return(plot->pixmap);
}

/**
 * ags_plot_set_pixmap:
 * @plot: (type gpointer): the #AgsPlot-struct
 * @pixmap: (transfer full): the pixmaps
 * 
 * Set pixmap field of @plot.
 * 
 * Since: 3.2.0
 */
void
ags_plot_set_pixmap(AgsPlot *plot,
		    guchar **pixmap)
{
  if(plot == NULL){
    return;
  }

  plot->pixmap = pixmap;
}

/**
 * ags_cartesian_add_plot:
 * @cartesian: the #AgsCartesian
 * @plot: (type gpointer): the #AgsPlot-struct
 * 
 * Add @plot to @cartesian.
 * 
 * Since: 3.0.0
 */
void
ags_cartesian_add_plot(AgsCartesian *cartesian,
		       AgsPlot *plot)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  cartesian->plot = g_list_prepend(cartesian->plot,
				   plot);
}

/**
 * ags_cartesian_remove_plot:
 * @cartesian: the #AgsCartesian
 * @plot: (type gpointer): the #AgsPlot-struct
 * 
 * Remove @plot from @cartesian.
 * 
 * Since: 3.0.0
 */
void
ags_cartesian_remove_plot(AgsCartesian *cartesian,
			  AgsPlot *plot)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  cartesian->plot = g_list_remove(cartesian->plot,
				  plot);
}

/**
 * ags_cartesian_get_x_margin:
 * @cartesian: the #AgsCartesian
 * 
 * Get x margin of @cartesian.
 * 
 * Returns: the x margin
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_x_margin(AgsCartesian *cartesian)
{
  gdouble x_margin;

  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "x-margin", &x_margin,
	       NULL);

  return(x_margin);
}

/**
 * ags_cartesian_set_x_margin:
 * @cartesian: the #AgsCartesian
 * @x_margin: the x margin
 * 
 * Set x margin of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_x_margin(AgsCartesian *cartesian,
			   gdouble x_margin)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "x-margin", x_margin,
	       NULL);
}

/**
 * ags_cartesian_get_y_margin:
 * @cartesian: the #AgsCartesian
 * 
 * Get y margin of @cartesian.
 * 
 * Returns: the y margin
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_y_margin(AgsCartesian *cartesian)
{
  gdouble y_margin;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "y-margin", &y_margin,
	       NULL);

  return(y_margin);
}

/**
 * ags_cartesian_set_y_margin:
 * @cartesian: the #AgsCartesian
 * @y_margin: the y margin
 * 
 * Set y margin of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_y_margin(AgsCartesian *cartesian,
			   gdouble y_margin)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "y-margin", y_margin,
	       NULL);
}

/**
 * ags_cartesian_get_center:
 * @cartesian: the #AgsCartesian
 * 
 * Get center of @cartesian.
 * 
 * Returns: the center
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_center(AgsCartesian *cartesian)
{
  gdouble center;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "center", &center,
	       NULL);

  return(center);
}

/**
 * ags_cartesian_set_center:
 * @cartesian: the #AgsCartesian
 * @center: the center
 * 
 * Set center of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_center(AgsCartesian *cartesian,
			 gdouble center)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "center", center,
	       NULL);
}

/**
 * ags_cartesian_get_line_width:
 * @cartesian: the #AgsCartesian
 * 
 * Get line width of @cartesian.
 * 
 * Returns: the line width
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_line_width(AgsCartesian *cartesian)
{
  gdouble line_width;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "line-width", &line_width,
	       NULL);

  return(line_width);
}

/**
 * ags_cartesian_set_line_width:
 * @cartesian: the #AgsCartesian
 * @line_width: the line width
 * 
 * Set line width of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_line_width(AgsCartesian *cartesian,
			     gdouble line_width)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "line-width", line_width,
	       NULL);
}

/**
 * ags_cartesian_get_point_radius:
 * @cartesian: the #AgsCartesian
 * 
 * Get point radius.
 * 
 * Returns: the point radius
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_point_radius(AgsCartesian *cartesian)
{
  gdouble point_radius;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "point-radius", &point_radius,
	       NULL);

  return(point_radius);
}

/**
 * ags_cartesian_set_point_radius:
 * @cartesian: the #AgsCartesian
 * @point_radius: the point radius
 * 
 * Set point radius.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_point_radius(AgsCartesian *cartesian,
			       gdouble point_radius)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "point-radius", point_radius,
	       NULL);
}

/**
 * ags_cartesian_get_font_size:
 * @cartesian: the #AgsCartesian
 * 
 * Get font size of @cartesian.
 * 
 * Returns: the font size
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_font_size(AgsCartesian *cartesian)
{
  gdouble font_size;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "font-size", &font_size,
	       NULL);

  return(font_size);
}

/**
 * ags_cartesian_set_font_size:
 * @cartesian: the #AgsCartesian
 * @font_size: the font size
 * 
 * Set font size of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_font_size(AgsCartesian *cartesian,
			    gdouble font_size)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "font-size", font_size,
	       NULL);
}

/**
 * ags_cartesian_get_x_step_width:
 * @cartesian: the #AgsCartesian
 * 
 * Get x step width of @cartesian.
 * 
 * Returns: the x step width
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_x_step_width(AgsCartesian *cartesian)
{
  gdouble x_step_width;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "x-step-width", &x_step_width,
	       NULL);

  return(x_step_width);
}

/**
 * ags_cartesian_set_x_step_width:
 * @cartesian: the #AgsCartesian
 * @x_step_width: the x step width
 * 
 * Set x step width of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_x_step_width(AgsCartesian *cartesian,
			       gdouble x_step_width)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "x-step-width", x_step_width,
	       NULL);
}

/**
 * ags_cartesian_get_y_step_height:
 * @cartesian: the #AgsCartesian
 * 
 * Get y step height.
 * 
 * Returns: the y step height
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_y_step_height(AgsCartesian *cartesian)
{
  gdouble y_step_height;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "y-step-height", &y_step_height,
	       NULL);

  return(y_step_height);
}

/**
 * ags_cartesian_set_y_step_height:
 * @cartesian: the #AgsCartesian
 * @y_step_height: the y step height
 * 
 * Set y step height of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_y_step_height(AgsCartesian *cartesian,
				gdouble y_step_height)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "y-step-height", y_step_height,
	       NULL);
}

/**
 * ags_cartesian_get_x_scale_step_width:
 * @cartesian: the #AgsCartesian
 * 
 * Get x scale step width of @cartesian.
 * 
 * Returns: the x scale step width
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_x_scale_step_width(AgsCartesian *cartesian)
{
  gdouble x_scale_step_width;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "x-scale-step-width", &x_scale_step_width,
	       NULL);

  return(x_scale_step_width);
}

/**
 * ags_cartesian_set_x_scale_step_width:
 * @cartesian: the #AgsCartesian
 * @x_scale_step_width: the x scale step width
 * 
 * Set x scale step width of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_x_scale_step_width(AgsCartesian *cartesian,
				     gdouble x_scale_step_width)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "x-scale-step-width", x_scale_step_width,
	       NULL);
}

/**
 * ags_cartesian_get_y_scale_step_height:
 * @cartesian: the #AgsCartesian
 * 
 * Get y scale step height of @cartesian.
 * 
 * Returns: the y scale step height
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_y_scale_step_height(AgsCartesian *cartesian)
{
  gdouble y_scale_step_height;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "y-scale-step-height", &y_scale_step_height,
	       NULL);

  return(y_scale_step_height);
}

/**
 * ags_cartesian_set_y_scale_step_height:
 * @cartesian: the #AgsCartesian
 * @y_scale_step_height: the y scale step height
 * 
 * Set y scale step height of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_y_scale_step_height(AgsCartesian *cartesian,
				      gdouble y_scale_step_height)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "y-scale-step-height", y_scale_step_height,
	       NULL);
}

/**
 * ags_cartesian_get_x_unit_x0:
 * @cartesian: the #AgsCartesian
 * 
 * Get x unit x0 of @cartesian.
 * 
 * Returns: the x unit x0
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_x_unit_x0(AgsCartesian *cartesian)
{
  gdouble x_unit_x0;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "x-unit-x0", &x_unit_x0,
	       NULL);

  return(x_unit_x0);
}

/**
 * ags_cartesian_set_x_unit_x0:
 * @cartesian: the #AgsCartesian
 * @x_unit_x0: the x unit x0
 * 
 * Set x unit x0 of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_x_unit_x0(AgsCartesian *cartesian,
			    gdouble x_unit_x0)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "x-unit-x0", x_unit_x0,
	       NULL);
}

/**
 * ags_cartesian_get_x_unit_y0:
 * @cartesian: the #AgsCartesian
 * 
 * Get x unit y0 of @cartesian.
 * 
 * Returns: the x unit y0
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_x_unit_y0(AgsCartesian *cartesian)
{
  gdouble x_unit_y0;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "x-unit-y0", &x_unit_y0,
	       NULL);

  return(x_unit_y0);
}

/**
 * ags_cartesian_set_x_unit_y0:
 * @cartesian: the #AgsCartesian
 * @x_unit_y0: the x unit y0
 * 
 * Set x unit y0 of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_x_unit_y0(AgsCartesian *cartesian,
			    gdouble x_unit_y0)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "x-unit-y0", x_unit_y0,
	       NULL);
}

/**
 * ags_cartesian_get_x_unit_size:
 * @cartesian: the #AgsCartesian
 * 
 * Get x unit size of @cartesian.
 * 
 * Returns: the x unit size
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_x_unit_size(AgsCartesian *cartesian)
{
  gdouble x_unit_size;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "x-unit-size", &x_unit_size,
	       NULL);

  return(x_unit_size);
}

/**
 * ags_cartesian_set_x_unit_size:
 * @cartesian: the #AgsCartesian
 * @x_unit_size: the x unit size
 * 
 * Set x unit size of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_x_unit_size(AgsCartesian *cartesian,
			      gdouble x_unit_size)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "x-unit-size", x_unit_size,
	       NULL);
}

/**
 * ags_cartesian_get_y_unit_x0:
 * @cartesian: the #AgsCartesian
 * 
 * Get y unit x0 of @cartesian.
 * 
 * Returns: the y unit x0
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_y_unit_x0(AgsCartesian *cartesian)
{
  gdouble y_unit_x0;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "y-unit-x0", &y_unit_x0,
	       NULL);

  return(y_unit_x0);
}

/**
 * ags_cartesian_set_y_unit_x0:
 * @cartesian: the #AgsCartesian
 * @y_unit_x0: the y unit x0
 * 
 * Set y unit x0 of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_y_unit_x0(AgsCartesian *cartesian,
			    gdouble y_unit_x0)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "y-unit-x0", y_unit_x0,
	       NULL);
}

/**
 * ags_cartesian_get_y_unit_y0:
 * @cartesian: the #AgsCartesian
 * 
 * Get y unit y0 of @cartesian.
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_y_unit_y0(AgsCartesian *cartesian)
{
  gdouble y_unit_y0;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "y-unit-y0", &y_unit_y0,
	       NULL);

  return(y_unit_y0);
}

/**
 * ags_cartesian_set_y_unit_y0:
 * @cartesian: the #AgsCartesian
 * @y_unit_y0: the y unit y0
 * 
 * Set y unit y0 of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_y_unit_y0(AgsCartesian *cartesian,
			    gdouble y_unit_y0)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "y-unit-y0", y_unit_y0,
	       NULL);
}

/**
 * ags_cartesian_get_y_unit_size:
 * @cartesian: the #AgsCartesian
 * 
 * Get y unit size of @cartesian.
 * 
 * Returns: the y unit size
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_y_unit_size(AgsCartesian *cartesian)
{
  gdouble y_unit_size;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "y-unit-size", &y_unit_size,
	       NULL);

  return(y_unit_size);
}

/**
 * ags_cartesian_set_y_unit_size:
 * @cartesian: the #AgsCartesian
 * @y_unit_size: the y unit size
 * 
 * Set y unit size of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_y_unit_size(AgsCartesian *cartesian,
			      gdouble y_unit_size)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "y-unit-size", y_unit_size,
	       NULL);
}

/**
 * ags_cartesian_get_x_label_start:
 * @cartesian: the #AgsCartesian
 * 
 * Get x label start of @cartesian.
 * 
 * Returns: the x label start
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_x_label_start(AgsCartesian *cartesian)
{
  gdouble x_label_start;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "x-label-start", &x_label_start,
	       NULL);

  return(x_label_start);
}

/**
 * ags_cartesian_set_x_label_start:
 * @cartesian: the #AgsCartesian
 * @x_label_start: the x label start
 * 
 * Set x label start of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_x_label_start(AgsCartesian *cartesian,
				gdouble x_label_start)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "x-label-start", x_label_start,
	       NULL);
}

/**
 * ags_cartesian_get_x_label_step_width:
 * @cartesian: the #AgsCartesian
 * 
 * Get x label step width.
 * 
 * Returns: the x label step width
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_x_label_step_width(AgsCartesian *cartesian)
{
  gdouble x_label_step_width;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "x-label-step-width", &x_label_step_width,
	       NULL);

  return(x_label_step_width);
}

/**
 * ags_cartesian_set_x_label_step_width:
 * @cartesian: the #AgsCartesian
 * @x_label_step_width: the x label step width
 * 
 * Set x label step width of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_x_label_step_width(AgsCartesian *cartesian,
				     gdouble x_label_step_width)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "x-label-step-width", x_label_step_width,
	       NULL);
}

/**
 * ags_cartesian_get_y_label_start:
 * @cartesian: the #AgsCartesian
 * 
 * Get y label start of @cartesian.
 * 
 * Returns: the y label start
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_y_label_start(AgsCartesian *cartesian)
{
  gdouble y_label_start;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "y-label-start", &y_label_start,
	       NULL);

  return(y_label_start);
}

/**
 * ags_cartesian_set_y_label_start:
 * @cartesian: the #AgsCartesian
 * @y_label_start: the y label start
 * 
 * Set y label start of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_y_label_start(AgsCartesian *cartesian,
				gdouble y_label_start)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "y-label-start", y_label_start,
	       NULL);
}

/**
 * ags_cartesian_get_y_label_step_height:
 * @cartesian: the #AgsCartesian
 * 
 * Get y label step height of @cartesian.
 * 
 * Returns: the y label step height
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_y_label_step_height(AgsCartesian *cartesian)
{
  gdouble y_label_step_height;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "y-label-step-height", &y_label_step_height,
	       NULL);

  return(y_label_step_height);
}

/**
 * ags_cartesian_set_y_label_step_height:
 * @cartesian: the #AgsCartesian
 * @y_label_step_height: the y label step height
 * 
 * Set y label step height of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_y_label_step_height(AgsCartesian *cartesian,
				      gdouble y_label_step_height)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "y-label-step-height", y_label_step_height,
	       NULL);
}

/**
 * ags_cartesian_get_x_step:
 * @cartesian: the #AgsCartesian
 * 
 * Get x step of @cartesian.
 * 
 * Returns: the x step
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_x_step(AgsCartesian *cartesian)
{
  gdouble x_step;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "x-step", &x_step,
	       NULL);

  return(x_step);
}

/**
 * ags_cartesian_set_x_step:
 * @cartesian: the #AgsCartesian
 * @x_step: the x step
 * 
 * Set x step of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_x_step(AgsCartesian *cartesian,
			 gdouble x_step)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "x-step", x_step,
	       NULL);
}

/**
 * ags_cartesian_get_y_step:
 * @cartesian: the #AgsCartesian
 * 
 * Get y step of @cartesian.
 * 
 * Returns: the y step
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_y_step(AgsCartesian *cartesian)
{
  gdouble y_step;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "y-step", &y_step,
	       NULL);

  return(y_step);
}

/**
 * ags_cartesian_set_y_step:
 * @cartesian: the #AgsCartesian
 * @y_step: the y step
 * 
 * Set y step of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_y_step(AgsCartesian *cartesian,
			 gdouble y_step)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "y-step", y_step,
	       NULL);
}

/**
 * ags_cartesian_get_x_start:
 * @cartesian: the #AgsCartesian
 * 
 * Get x start of @cartesian.
 * 
 * Returns: the x start
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_x_start(AgsCartesian *cartesian)
{
  gdouble x_start;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "x-start", &x_start,
	       NULL);

  return(x_start);
}

/**
 * ags_cartesian_set_x_start:
 * @cartesian: the #AgsCartesian
 * @x_start: the x start
 * 
 * Set x start of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_x_start(AgsCartesian *cartesian,
			  gdouble x_start)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "x-start", x_start,
	       NULL);
}

/**
 * ags_cartesian_get_x_end:
 * @cartesian: the #AgsCartesian
 * 
 * Get x end of @cartesian.
 * 
 * Returns: the x end
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_x_end(AgsCartesian *cartesian)
{
  gdouble x_end;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "x-end", &x_end,
	       NULL);

  return(x_end);
}

/**
 * ags_cartesian_set_x_end:
 * @cartesian: the #AgsCartesian
 * @x_end: the x end
 * 
 * Set x end of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_x_end(AgsCartesian *cartesian,
			gdouble x_end)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "x-end", x_end,
	       NULL);
}

/**
 * ags_cartesian_get_y_start:
 * @cartesian: the #AgsCartesian
 * 
 * Get y start of @cartesian.
 * 
 * Returns: the y start
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_y_start(AgsCartesian *cartesian)
{
  gdouble y_start;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "y-start", &y_start,
	       NULL);

  return(y_start);
}

/**
 * ags_cartesian_set_y_start:
 * @cartesian: the #AgsCartesian
 * @y_start: the y start
 * 
 * Set y start of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_y_start(AgsCartesian *cartesian,
			  gdouble y_start)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "y-start", y_start,
	       NULL);
}

/**
 * ags_cartesian_get_y_end:
 * @cartesian: the #AgsCartesian
 * 
 * Get y end of @cartesian.
 * 
 * Returns: the y end
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_y_end(AgsCartesian *cartesian)
{
  gdouble y_end;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "y-end", &y_end,
	       NULL);

  return(y_end);
}

/**
 * ags_cartesian_set_y_end:
 * @cartesian: the #AgsCartesian
 * @y_end: the y end
 * 
 * Set y end of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_y_end(AgsCartesian *cartesian,
			gdouble y_end)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "y-end", y_end,
	       NULL);
}

/**
 * ags_cartesian_get_x_unit:
 * @cartesian: the #AgsCartesian
 * 
 * Get x unit of @cartesian.
 * 
 * Returns: the x unit
 * 
 * Since: 3.2.0
 */
gchar*
ags_cartesian_get_x_unit(AgsCartesian *cartesian)
{
  gchar *x_unit;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(NULL);
  }

  g_object_get(cartesian,
	       "x-unit", &x_unit,
	       NULL);

  return(x_unit);
}

/**
 * ags_cartesian_set_x_unit:
 * @cartesian: the #AgsCartesian
 * @x_unit: the x unit
 * 
 * Set x unit of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_x_unit(AgsCartesian *cartesian,
			 gchar *x_unit)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "x-unit", x_unit,
	       NULL);
}

/**
 * ags_cartesian_get_y_unit:
 * @cartesian: the #AgsCartesian
 * 
 * Get y unit of @cartesian.
 * 
 * Returns: the y unit
 * 
 * Since: 3.2.0
 */
gchar*
ags_cartesian_get_y_unit(AgsCartesian *cartesian)
{
  gchar *y_unit;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(NULL);
  }

  g_object_get(cartesian,
	       "y-unit", &y_unit,
	       NULL);

  return(y_unit);
}

/**
 * ags_cartesian_set_y_unit:
 * @cartesian: the #AgsCartesian
 * @y_unit: the y unit
 * 
 * Set y unit of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_y_unit(AgsCartesian *cartesian,
			 gchar *y_unit)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "y-unit", y_unit,
	       NULL);
}

/**
 * ags_cartesian_get_x_label:
 * @cartesian: the #AgsCartesian
 * 
 * Get x label of @cartesian.
 * 
 * Returns: (transfer none): the x label
 * 
 * Since: 3.2.0
 */
gchar**
ags_cartesian_get_x_label(AgsCartesian *cartesian)
{
  gchar **x_label;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(NULL);
  }

  x_label = cartesian->x_label;

  return(x_label);
}

/**
 * ags_cartesian_set_x_label:
 * @cartesian: the #AgsCartesian
 * @x_label: (transfer none): the x label
 * 
 * Set x label of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_x_label(AgsCartesian *cartesian,
			  gchar **x_label)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  if(cartesian->x_label != NULL){
    g_strfreev(cartesian->x_label);
  }

  cartesian->x_label = g_strdupv(x_label);
}

/**
 * ags_cartesian_get_y_label:
 * @cartesian: the #AgsCartesian
 * 
 * Get y label of @cartesian.
 * 
 * Returns: (transfer none): the y label
 * 
 * Since: 3.2.0
 */
gchar**
ags_cartesian_get_y_label(AgsCartesian *cartesian)
{
  gchar **y_label;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(NULL);
  }

  y_label = cartesian->y_label;

  return(y_label);
}

/**
 * ags_cartesian_set_y_label:
 * @cartesian: the #AgsCartesian
 * @y_label: (transfer none): the y label
 * 
 * Set y label of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_y_label(AgsCartesian *cartesian,
			  gchar **y_label)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  if(cartesian->y_label != NULL){
    g_strfreev(cartesian->y_label);
  }

  cartesian->y_label = g_strdupv(y_label);
}

/**
 * ags_cartesian_get_x_step_factor:
 * @cartesian: the #AgsCartesian
 * 
 * Get x step factor of @cartesian.
 * 
 * Returns: the x step factor
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_x_step_factor(AgsCartesian *cartesian)
{
  gdouble x_step_factor;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "x-step-factor", &x_step_factor,
	       NULL);

  return(x_step_factor);
}

/**
 * ags_cartesian_set_x_step_factor:
 * @cartesian: the #AgsCartesian
 * @x_step_factor: the x step factor
 * 
 * Set x step factor of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_x_step_factor(AgsCartesian *cartesian,
				gdouble x_step_factor)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "x-step-factor", x_step_factor,
	       NULL);
}

/**
 * ags_cartesian_get_y_step_factor:
 * @cartesian: the #AgsCartesian
 * 
 * Get y step factor of @cartesian.
 * 
 * Returns: the y step factor
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_y_step_factor(AgsCartesian *cartesian)
{
  gdouble y_step_factor;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "y-step-factor", &y_step_factor,
	       NULL);

  return(y_step_factor);
}

/**
 * ags_cartesian_set_y_step_factor:
 * @cartesian: the #AgsCartesian
 * @y_step_factor: the y step factor
 * 
 * Set y step factor of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_y_step_factor(AgsCartesian *cartesian,
				gdouble y_step_factor)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "y-step-factor", y_step_factor,
	       NULL);
}

/**
 * ags_cartesian_get_x_small_scale_factor:
 * @cartesian: the #AgsCartesian
 * 
 * Get x small scale factor of @cartesian.
 *
 * Returns: the x small factor
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_x_small_scale_factor(AgsCartesian *cartesian)
{
  gdouble x_small_scale_factor;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "x-small-scale-factor", &x_small_scale_factor,
	       NULL);

  return(x_small_scale_factor);
}

/**
 * ags_cartesian_set_x_small_scale_factor:
 * @cartesian: the #AgsCartesian
 * @x_small_scale_factor: the x small scale factor
 * 
 * Set x small scale factor of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_x_small_scale_factor(AgsCartesian *cartesian,
				       gdouble x_small_scale_factor)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "x-small-scale-factor", x_small_scale_factor,
	       NULL);
}

/**
 * ags_cartesian_get_x_big_scale_factor:
 * @cartesian: the #AgsCartesian
 * 
 * Get x big scale factor of @cartesian.
 *
 * Returns: the x big factor
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_x_big_scale_factor(AgsCartesian *cartesian)
{
  gdouble x_big_scale_factor;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "x-big-scale-factor", &x_big_scale_factor,
	       NULL);

  return(x_big_scale_factor);
}

/**
 * ags_cartesian_set_x_big_scale_factor:
 * @cartesian: the #AgsCartesian
 * @x_big_scale_factor: the x big scale factor
 * 
 * Set x big scale factor of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_x_big_scale_factor(AgsCartesian *cartesian,
				     gdouble x_big_scale_factor)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "x-big-scale-factor", x_big_scale_factor,
	       NULL);
}

/**
 * ags_cartesian_get_y_small_scale_factor:
 * @cartesian: the #AgsCartesian
 * 
 * Get y small scale factor of @cartesian.
 *
 * Returns: the y small factor
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_y_small_scale_factor(AgsCartesian *cartesian)
{
  gdouble y_small_scale_factor;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "y-small-scale-factor", &y_small_scale_factor,
	       NULL);

  return(y_small_scale_factor);
}

/**
 * ags_cartesian_set_y_small_scale_factor:
 * @cartesian: the #AgsCartesian
 * @y_small_scale_factor: the y small scale factor
 * 
 * Set y small scale factor of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_y_small_scale_factor(AgsCartesian *cartesian,
				       gdouble y_small_scale_factor)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "y-small-scale-factor", y_small_scale_factor,
	       NULL);
}

/**
 * ags_cartesian_get_y_big_scale_factor:
 * @cartesian: the #AgsCartesian
 * 
 * Get y big scale factor of @cartesian.
 *
 * Returns: the y big factor
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_y_big_scale_factor(AgsCartesian *cartesian)
{
  gdouble y_big_scale_factor;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "y-big-scale-factor", &y_big_scale_factor,
	       NULL);

  return(y_big_scale_factor);
}

/**
 * ags_cartesian_set_y_big_scale_factor:
 * @cartesian: the #AgsCartesian
 * @y_big_scale_factor: the y big scale factor
 * 
 * Set y big scale factor of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_y_big_scale_factor(AgsCartesian *cartesian,
				     gdouble y_big_scale_factor)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "y-big-scale-factor", y_big_scale_factor,
	       NULL);
}

/**
 * ags_cartesian_get_x_label_factor:
 * @cartesian: the #AgsCartesian
 * 
 * Get x label factor of @cartesian.
 * 
 * Returns: the x label factor
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_x_label_factor(AgsCartesian *cartesian)
{
  gdouble x_label_factor;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "x-label-factor", &x_label_factor,
	       NULL);

  return(x_label_factor);
}

/**
 * ags_cartesian_set_x_label_factor:
 * @cartesian: the #AgsCartesian
 * @x_label_factor: the x label factor
 * 
 * Set x label factor of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_x_label_factor(AgsCartesian *cartesian,
				 gdouble x_label_factor)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "x-label-factor", x_label_factor,
	       NULL);
}

/**
 * ags_cartesian_get_x_label_precision:
 * @cartesian: the #AgsCartesian
 * 
 * Get x label precision of @cartesian.
 * 
 * Returns: the x label precision
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_x_label_precision(AgsCartesian *cartesian)
{
  gdouble x_label_precision;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "x-label-precision", &x_label_precision,
	       NULL);

  return(x_label_precision);
}

/**
 * ags_cartesian_set_x_label_precision:
 * @cartesian: the #AgsCartesian
 * @x_label_precision: the x label precision
 * 
 * Set x label precision of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_x_label_precision(AgsCartesian *cartesian,
				 gdouble x_label_precision)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "x-label-precision", x_label_precision,
	       NULL);
}

/**
 * ags_cartesian_get_y_label_factor:
 * @cartesian: the #AgsCartesian
 * 
 * Get y label factor of @cartesian.
 * 
 * Returns: the y label factor
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_y_label_factor(AgsCartesian *cartesian)
{
  gdouble y_label_factor;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "y-label-factor", &y_label_factor,
	       NULL);

  return(y_label_factor);
}

/**
 * ags_cartesian_set_y_label_factor:
 * @cartesian: the #AgsCartesian
 * @y_label_factor: the y label factor
 * 
 * Set y label factor of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_y_label_factor(AgsCartesian *cartesian,
				 gdouble y_label_factor)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "y-label-factor", y_label_factor,
	       NULL);
}

/**
 * ags_cartesian_get_y_label_precision:
 * @cartesian: the #AgsCartesian
 * 
 * Get y label precision of @cartesian.
 * 
 * Returns: the y label precision
 * 
 * Since: 3.2.0
 */
gdouble
ags_cartesian_get_y_label_precision(AgsCartesian *cartesian)
{
  gdouble y_label_precision;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(0.0);
  }

  g_object_get(cartesian,
	       "y-label-precision", &y_label_precision,
	       NULL);

  return(y_label_precision);
}

/**
 * ags_cartesian_set_y_label_precision:
 * @cartesian: the #AgsCartesian
 * @y_label_precision: the y label precision
 * 
 * Set y label precision of @cartesian.
 * 
 * Since: 3.2.0
 */
void
ags_cartesian_set_y_label_precision(AgsCartesian *cartesian,
				 gdouble y_label_precision)
{
  if(!AGS_IS_CARTESIAN(cartesian)){
    return;
  }

  g_object_set(cartesian,
	       "y-label-precision", y_label_precision,
	       NULL);
}

/**
 * ags_cartesian_get_surface:
 * @cartesian: the #AgsCartesian
 * 
 * Get cairo surface of @cartesian.
 * 
 * Returns: (transfer full): the cairo surface
 * 
 * Since: 3.2.0
 */
cairo_surface_t*
ags_cartesian_get_surface(AgsCartesian *cartesian)
{
  cairo_surface_t *surface;
  
  if(!AGS_IS_CARTESIAN(cartesian)){
    return(NULL);
  }

  surface = cartesian->surface;
  cairo_surface_reference(surface);
  
  return(surface);
}

/**
 * ags_cartesian_linear_step_conversion_func:
 * @current: current step value
 * @is_abscissae: is abscissae
 * @data: the data
 * 
 * Perform linear step conversion.
 * 
 * Returns: the converted step value
 * 
 * Since: 3.0.0
 */
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

/**
 * ags_cartesian_linear_translate_func:
 * @x: the x position
 * @y: the y position
 * @ret_x: (out): the return location of translated x
 * @ret_y: (out): the return location of transloated y
 * @data: the #AgsCartesian
 * 
 * Perform linear translation.
 * 
 * Since: 3.0.0
 */
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

/**
 * ags_cartesian_linear_x_small_scale_func:
 * @value: the value
 * @data: the #AgsCartesian
 * 
 * Labeling function of x small scale
 * 
 * Returns: the matching scale value
 * 
 * Since: 3.0.0
 */
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

/**
 * ags_cartesian_linear_x_big_scale_func:
 * @value: the value
 * @data: the #AgsCartesian
 * 
 * Labeling function of x big scale
 * 
 * Returns: the matching scale value
 * 
 * Since: 3.0.0
 */
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

/**
 * ags_cartesian_linear_y_small_scale_func:
 * @value: the value
 * @data: the #AgsCartesian
 * 
 * Labeling function of y small scale
 * 
 * Returns: the matching scale value
 * 
 * Since: 3.0.0
 */
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

/**
 * ags_cartesian_linear_y_big_scale_func:
 * @value: the value
 * @data: the #AgsCartesian
 * 
 * Labeling function of y big scale
 * 
 * Returns: the matching scale value
 * 
 * Since: 3.0.0
 */
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

/**
 * ags_cartesian_linear_x_label_func:
 * @value: the value
 * @data: the #AgsCartesian
 * 
 * Format x label @value appropriately.
 * 
 * Returns: the formatted string
 * 
 * Since: 3.0.0
 */
gchar*
ags_cartesian_linear_x_label_func(gdouble value,
				  gpointer data)
{
  gchar *format;
  gchar *str;
  
  if(data == NULL ||
     !AGS_IS_CARTESIAN(data)){
    str = g_strdup_printf("%f",
			  value);
    
    return(str);
  }

  format = g_strdup_printf("%%.%df",
			   (guint) ceil(AGS_CARTESIAN(data)->x_label_precision));

  str = g_strdup_printf(format,
			value);
  g_free(format);

  return(str);
}

/**
 * ags_cartesian_linear_y_label_func:
 * @value: the value
 * @data: the #AgsCartesian
 * 
 * Format y label @value appropriately.
 * 
 * Returns: the formatted string
 * 
 * Since: 3.0.0
 */
gchar*
ags_cartesian_linear_y_label_func(gdouble value,
				  gpointer data)
{
  gchar *format;
  gchar *str;
  
  if(data == NULL ||
     !AGS_IS_CARTESIAN(data)){
    str = g_strdup_printf("%f",
			  value);
    
    return(str);
  }

  format = g_strdup_printf("%%.%df",
			   (guint) ceil(AGS_CARTESIAN(data)->y_label_precision));

  str = g_strdup_printf(format,
			value);
  g_free(format);

  return(str);
}

/**
 * ags_cartesian_reallocate_label:
 * @cartesian: the #AgsCartesian
 * @do_x_label: do x label
 * 
 * Reallocate x label if @do_x_label, otherwise y label.
 * 
 * Since: 3.0.0
 */
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
    i_stop = (guint) ceil((1.0 / cartesian->x_label_step_width) *
			  (cartesian->x_end - cartesian->x_start));
      
    if(cartesian->x_label == NULL){
      /* allocate */
      cartesian->x_label = (gchar **) g_malloc((i_stop + 1)  * sizeof(gchar *));

      /* iteration control */
      i_start = 0;
      i = 0;
    }else{
      /* reallocate */
      cartesian->x_label = (gchar **) g_realloc(cartesian->x_label,
						(i_stop + 1) * sizeof(gchar *));

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
    i_stop = (guint) ceil((1.0 / cartesian->y_label_step_height) *
			  (cartesian->y_end - cartesian->y_start));
      
    if(cartesian->y_label == NULL){
      /* allocate */
      cartesian->y_label = (gchar **) g_malloc((i_stop + 1)  * sizeof(gchar *));

      /* iteration control */
      i_start = 0;
      i = 0;
    }else{
      /* reallocate */
      cartesian->y_label = (gchar **) g_realloc(cartesian->y_label,
						(i_stop + 1) * sizeof(gchar *));

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

/**
 * ags_cartesian_fill_label:
 * @cartesian: the #AgsCartesian
 * @do_x_label: do x label
 * 
 * Fill x label if @do_x_label, otherwise y label.
 * 
 * Since: 3.0.0
 */
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
    i_stop = (guint) ceil((1.0 / cartesian->x_label_step_width) *
			  (cartesian->x_end - cartesian->x_start));

    if(cartesian->x_label != NULL &&
       cartesian->x_label_func != NULL){
      for(i = 0; i < i_stop; i++){
	/* fill x label */
	cartesian->x_label[i] = cartesian->x_label_func((cartesian->x_label_factor *
							 (gdouble) i + (cartesian->x_start / cartesian->x_step_width) +
							 (cartesian->x_label_start / cartesian->x_step_width)),
							cartesian->x_label_data);
      }
    }
  }else{
    i_stop = (guint) ceil((1.0 / cartesian->y_label_step_height) *
			  (cartesian->y_end - cartesian->y_start));

    if(cartesian->y_label != NULL &&
       cartesian->y_label_func != NULL){
      for(i = 0; i < i_stop; i++){
	/* fill y label */
	cartesian->y_label[i] = cartesian->y_label_func((cartesian->y_label_factor *
							 (gdouble) i + (cartesian->y_start / cartesian->y_step_height) +
							 (cartesian->y_label_start / cartesian->y_step_height)),
							cartesian->y_label_data);
      }
    }
  }
}

/**
 * ags_cartesian_new:
 * 
 * Create a new instance of #AgsCartesian
 *
 * Returns: the new #AgsCartesian
 * 
 * Since: 3.0.0
 */
AgsCartesian*
ags_cartesian_new()
{
  AgsCartesian *cartesian;

  cartesian = (AgsCartesian *) g_object_new(AGS_TYPE_CARTESIAN,
					    NULL);
  
  return(cartesian);
}
