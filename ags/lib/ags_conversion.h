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

#ifndef __AGS_CONVERSION_H__
#define __AGS_CONVERSION_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define AGS_TYPE_CONVERSION                (ags_conversion_get_type())
#define AGS_CONVERSION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CONVERSION, AgsConversion))
#define AGS_CONVERSION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CONVERSION, AgsConversionClass))
#define AGS_IS_CONVERSION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CONVERSION))
#define AGS_IS_CONVERSION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CONVERSION))
#define AGS_CONVERSION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_CONVERSION, AgsConversionClass))

#define AGS_CONVERSION_GET_OBJ_MUTEX(obj) (&(((AgsConversion *) obj)->obj_mutex))

typedef struct _AgsConversion AgsConversion;
typedef struct _AgsConversionClass AgsConversionClass;

struct _AgsConversion
{
  GObject gobject;

  GRecMutex obj_mutex;

  gchar *name;
  gchar *description;
};

struct _AgsConversionClass
{
  GObjectClass gobject;

  gdouble (*convert)(AgsConversion *conversion,
		     gdouble x,
		     gboolean reverse);
};

GType ags_conversion_get_type(void);

gdouble ags_conversion_convert(AgsConversion *conversion,
			       gdouble x,
			       gboolean reverse);

AgsConversion* ags_conversion_new();

G_END_DECLS

#endif /*__AGS_CONVERSION_H__*/
