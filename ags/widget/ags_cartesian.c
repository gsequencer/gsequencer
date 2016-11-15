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

#include "ags_cartesian.h"

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
  g_object_set(G_OBJECT(cartesian),
	       "app-paintable\0", TRUE,
	       NULL);
}

gdouble
ags_cartesian_linear_step_conversion(gdouble current,
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
			   AGS_CARTESIAN(data)->x_label_precision);

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
			   AGS_CARTESIAN(data)->y_label_precision);

  str = g_strdup_printf(format,
			value);
  g_free(format);

  return(str);
}

void
ags_cartesian_draw(AgsCartesian *cartesian)
{
  //TODO:JK: implement me
}

AgsCartesian*
ags_cartesian_new()
{
  AgsCartesian *cartesian;

  cartesian = (AgsCartesian *) g_object_new(AGS_TYPE_CARTESIAN,
					    NULL);
  
  return(cartesian);
}
