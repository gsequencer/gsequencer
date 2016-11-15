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

  /* step */
  cartesian->x_step_width = AGS_CARTESIAN_DEFAULT_X_STEP_WIDTH;
  cartesian->y_step_height = AGS_CARTESIAN_DEFAULT_Y_STEP_HEIGHT;

  /* scale step */
  cartesian->x_step_width = AGS_CARTESIAN_DEFAULT_X_SCALE_STEP_WIDTH;
  cartesian->y_step_height = AGS_CARTESIAN_DEFAULT_Y_SCALE_STEP_HEIGHT;

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
  cartesian->x_big_scale_factor = 1.0;
  
  cartesian->y_scale_data = cartesian;
  cartesian->y_small_scale_factor = 1.0;
  cartesian->y_big_scale_factor = 1.0;

  cartesian->x_label_data = cartesian;
  cartesian->x_label_factor = 1.0;
  cartesian->x_label_precision = 0.0;
  
  cartesian->y_label_data = cartesian;
  cartesian->y_label_factor = 1.0;
  cartesian->y_label_precision = 0.0;

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
ags_cartesian_draw(AgsCartesian *cartesian)
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
	  cartesian->x_label = NULL;
	}
      }
    }

    if(cartesian->x_label != NULL){
      for(; i < i_stop; i++){
	cartesian->x_label[i] = NULL;
      }

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
	  cartesian->y_label = NULL;
	}
      }
    }

    if(cartesian->y_label != NULL){
      for(; i < i_stop; i++){
	cartesian->y_label[i] = NULL;
      }

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
      cartesian->x_label = cartesian->x_label_func((gdouble) i,
						   cartesian->x_label_data);
    }
  }else{
    i_stop = g_strv_length(cartesian->y_label);

    for(i = 0; i < i_stop; i++){
      cartesian->y_label = cartesian->y_label_func((gdouble) i,
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
