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

#ifndef __AGS_LV2_CONVERSION_H__
#define __AGS_LV2_CONVERSION_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/lib/ags_conversion.h>

#define AGS_TYPE_CONVERSION                (ags_lv2_conversion_get_type())
#define AGS_LV2_CONVERSION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CONVERSION, AgsLv2Conversion))
#define AGS_LV2_CONVERSION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CONVERSION, AgsLv2ConversionClass))
#define AGS_IS_CONVERSION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CONVERSION))
#define AGS_IS_CONVERSION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CONVERSION))
#define AGS_LV2_CONVERSION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_CONVERSION, AgsLv2ConversionClass))

typedef struct _AgsLv2Conversion AgsLv2Conversion;
typedef struct _AgsLv2ConversionClass AgsLv2ConversionClass;

typedef enum{
  AGS_LV2_CONVERSION_LOGARITHMIC        = 1,
}AgsLv2ConversionFlaags;

struct _AgsLv2Conversion
{
  AgsConversion gobject;

  guint flags;
};

struct _AgsLv2ConversionClass
{
  AgsConversionClass gobject;
};

GType ags_lv2_conversion_get_type(void);

AgsLv2Conversion* ags_lv2_conversion_new();

#endif /*__AGS_LV2_CONVERSION_H__*/
