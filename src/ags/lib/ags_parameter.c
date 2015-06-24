/* This file is part of GSequencer.
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
/* This file is part of GSequencer.
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
/* AGS - Advanced Gtk+ Sequencer
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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
 * Since: 0.3
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
          g_warning ("%s: object class `%s' has no property named `%s'\0",
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
          g_warning ("%s: %s\0", G_STRFUNC, error);
          g_free (error);
          g_value_unset (&params[*n_params].value);
          break;
        }

      *n_params = *n_params + 1;

      param_name = va_arg (ap, gchar *);
    }

  return(params);
}
