/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <ags/lib/ags_parameter.h>

#include <stdlib.h>
#include <string.h>
#include <gobject/gvaluecollector.h>

/**
 * SECTION:ags_parameter
 * @short_description: Complete GParameter
 * @title: AgsParameter
 * @section_id:
 * @include: ags/lib/ags_parameter.h
 *
 * Functions completing #GParameter API.
 */

static GParamSpecPool *pspec_pool = NULL;

/**
 * ags_parameter_grow:
 * @object_type: the #GType of the object's properties
 * @params: the #GParameter array
 * @n_params: location to store new size
 * @...: a %NULL-terminated list of properties name and value
 * 
 * Grow parameter array of @object_type object. This function is mainly used to
 * prepare a #GParameter array to instantiate a new #GObject by using g_object_newv().
 *
 * Returns: the resized GParameter array
 *
 * Since: 1.0.0
 */
GParameter*
ags_parameter_grow(GType object_type,
		   GParameter *params, guint *n_params, ...)
{
  //Thank you gimp

  GObjectClass *object_class;
  gchar *param_name;
  va_list ap;

  object_class = g_type_class_ref (object_type);

  va_start(ap, n_params);
  param_name = va_arg (ap, gchar *);

  while (param_name)
    {
      gchar *error;
      GParamSpec *pspec;

      error = NULL;
      pspec = g_object_class_find_property(object_class,
					   param_name);

      if (! pspec)
        {
          g_warning ("%s: object class `%s' has no property named `%s'",
                     G_STRFUNC, g_type_name (object_type), param_name);
          break;
        }

      if(params != NULL){
	params = g_renew (GParameter, params, *n_params + 1);
      }else{
	params = g_new(GParameter, 1);
      }

      params[*n_params].name         = param_name;
      params[*n_params].value.g_type = 0;

      g_value_init (&params[*n_params].value, G_PARAM_SPEC_VALUE_TYPE (pspec));

      G_VALUE_COLLECT (&params[*n_params].value, ap, 0, &error);

      if (error)
        {
          g_warning ("%s: %s", G_STRFUNC, error);
          g_free (error);
          g_value_unset (&params[*n_params].value);
          break;
        }

      *n_params = *n_params + 1;

      param_name = va_arg (ap, gchar *);
    }

  return(params);
}

/**
 * ags_parameter_find:
 * @params: the #GParameter-struct
 * @n_params: the count of @params
 * @name: the string to match against name of @params
 * 
 * Find #GValue-struct matching @name.
 * 
 * Returns: the matching #GValue-struct or %NULL if not found
 * 
 * Since: 1.2.0
 */
GValue*
ags_parameter_find(GParameter *params, guint n_params,
		   gchar *name)
{
  guint i;

  for(i = 0; i < n_params; i++){
    if(!g_strcmp0(params[i].name,
		  name)){
      return(&(params[i].value));
    }
  }

  return(NULL);
}

