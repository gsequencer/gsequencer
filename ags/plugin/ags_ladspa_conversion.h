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

#ifndef __AGS_LADSPA_CONVERSION_H__
#define __AGS_LADSPA_CONVERSION_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_LADSPA_CONVERSION                (ags_ladspa_conversion_get_type())
#define AGS_LADSPA_CONVERSION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CONVERSION, AgsLadspaConversion))
#define AGS_LADSPA_CONVERSION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CONVERSION, AgsLadspaConversionClass))
#define AGS_IS_LADSPA_CONVERSION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CONVERSION))
#define AGS_IS_LADSPA_CONVERSION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CONVERSION))
#define AGS_LADSPA_CONVERSION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_CONVERSION, AgsLadspaConversionClass))

#define AGS_LADSPA_CONVERSION_DEFAULT_LOWER (0.00001)
#define AGS_LADSPA_CONVERSION_DEFAULT_UPPER (1.0)
#define AGS_LADSPA_CONVERSION_DEFAULT_STEP_COUNT (1025.0)

typedef struct _AgsLadspaConversion AgsLadspaConversion;
typedef struct _AgsLadspaConversionClass AgsLadspaConversionClass;

/**
 * AgsLadspaConversionFlags:
 * @AGS_LADSPA_CONVERSION_SAMPLERATE: samplerate
 * @AGS_LADSPA_CONVERSION_BOUNDED_BELOW: bounded below
 * @AGS_LADSPA_CONVERSION_BOUNDED_ABOVE: bounded above
 * @AGS_LADSPA_CONVERSION_LOGARITHMIC: logarithmic
 * 
 * Enum values to control the behavior or indicate internal state of #AgsLadspaConversion by
 * enable/disable as flags.
 */
typedef enum{
  AGS_LADSPA_CONVERSION_SAMPLERATE         = 1,
  AGS_LADSPA_CONVERSION_BOUNDED_BELOW      = 1 <<  1,
  AGS_LADSPA_CONVERSION_BOUNDED_ABOVE      = 1 <<  2,
  AGS_LADSPA_CONVERSION_LOGARITHMIC        = 1 <<  3,
}AgsLadspaConversionFlags;

struct _AgsLadspaConversion
{
  AgsConversion conversion;

  guint flags;

  guint samplerate;

  gdouble lower;
  gdouble upper;
  gdouble step_count;
};

struct _AgsLadspaConversionClass
{
  AgsConversionClass conversion;
};

GType ags_ladspa_conversion_get_type(void);
GType ags_ladspa_conversion_flags_get_type();

gboolean ags_ladspa_conversion_test_flags(AgsLadspaConversion *ladspa_conversion, AgsLadspaConversionFlags flags);
void ags_ladspa_conversion_set_flags(AgsLadspaConversion *ladspa_conversion, AgsLadspaConversionFlags flags);
void ags_ladspa_conversion_unset_flags(AgsLadspaConversion *ladspa_conversion, AgsLadspaConversionFlags flags);

AgsLadspaConversion* ags_ladspa_conversion_new();

G_END_DECLS

#endif /*__AGS_LADSPA_CONVERSION_H__*/
