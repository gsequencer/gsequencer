/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#ifndef __AGS_VST3_CONVERSION_H__
#define __AGS_VST3_CONVERSION_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_VST3_CONVERSION                (ags_vst3_conversion_get_type())
#define AGS_VST3_CONVERSION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CONVERSION, AgsVst3Conversion))
#define AGS_VST3_CONVERSION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CONVERSION, AgsVst3ConversionClass))
#define AGS_IS_VST3_CONVERSION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CONVERSION))
#define AGS_IS_VST3_CONVERSION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CONVERSION))
#define AGS_VST3_CONVERSION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_CONVERSION, AgsVst3ConversionClass))

#define AGS_VST3_CONVERSION_DEFAULT_LOWER (0.00001)
#define AGS_VST3_CONVERSION_DEFAULT_UPPER (1.0)
#define AGS_VST3_CONVERSION_DEFAULT_STEP_COUNT (1025.0)

typedef struct _AgsVst3Conversion AgsVst3Conversion;
typedef struct _AgsVst3ConversionClass AgsVst3ConversionClass;

struct _AgsVst3Conversion
{
  AgsConversion conversion;

  guint flags;

  gdouble lower;
  gdouble upper;
  gdouble step_count;
};

struct _AgsVst3ConversionClass
{
  AgsConversionClass conversion;
};

GType ags_vst3_conversion_get_type(void);

AgsVst3Conversion* ags_vst3_conversion_new();

G_END_DECLS

#endif /*__AGS_VST3_CONVERSION_H__*/
