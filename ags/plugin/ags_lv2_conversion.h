/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_LV2_CONVERSION                (ags_lv2_conversion_get_type())
#define AGS_LV2_CONVERSION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CONVERSION, AgsLv2Conversion))
#define AGS_LV2_CONVERSION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CONVERSION, AgsLv2ConversionClass))
#define AGS_IS_LV2_CONVERSION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CONVERSION))
#define AGS_IS_LV2_CONVERSION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CONVERSION))
#define AGS_LV2_CONVERSION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_CONVERSION, AgsLv2ConversionClass))

#define AGS_LV2_CONVERSION_DEFAULT_LOWER (0.00001)
#define AGS_LV2_CONVERSION_DEFAULT_UPPER (1.0)
#define AGS_LV2_CONVERSION_DEFAULT_STEP_COUNT (1025.0)

typedef struct _AgsLv2Conversion AgsLv2Conversion;
typedef struct _AgsLv2ConversionClass AgsLv2ConversionClass;

/**
 * AgsLv2ConversionFlags:
 * @AGS_LV2_CONVERSION_LOGARITHMIC: logarithmic
 * 
 * Enum values to control the behavior or indicate internal state of #AgsLv2Conversion by
 * enable/disable as flags.
 */
typedef enum{
  AGS_LV2_CONVERSION_LOGARITHMIC        = 1,
}AgsLv2ConversionFlags;

struct _AgsLv2Conversion
{
  AgsConversion conversion;

  guint flags;

  gdouble lower;
  gdouble upper;
  gdouble step_count;
};

struct _AgsLv2ConversionClass
{
  AgsConversionClass conversion;
};

GType ags_lv2_conversion_get_type(void);
GType ags_lv2_conversion_flags_get_type();

gboolean ags_lv2_conversion_test_flags(AgsLv2Conversion *lv2_conversion, AgsLv2ConversionFlags flags);
void ags_lv2_conversion_set_flags(AgsLv2Conversion *lv2_conversion, AgsLv2ConversionFlags flags);
void ags_lv2_conversion_unset_flags(AgsLv2Conversion *lv2_conversion, AgsLv2ConversionFlags flags);

AgsLv2Conversion* ags_lv2_conversion_new();

G_END_DECLS

#endif /*__AGS_LV2_CONVERSION_H__*/
