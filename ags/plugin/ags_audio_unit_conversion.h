/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#ifndef __AGS_AUDIO_UNIT_CONVERSION_H__
#define __AGS_AUDIO_UNIT_CONVERSION_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_AUDIO_UNIT_CONVERSION                (ags_audio_unit_conversion_get_type())
#define AGS_AUDIO_UNIT_CONVERSION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CONVERSION, AgsAudioUnitConversion))
#define AGS_AUDIO_UNIT_CONVERSION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CONVERSION, AgsAudioUnitConversionClass))
#define AGS_IS_AUDIO_UNIT_CONVERSION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CONVERSION))
#define AGS_IS_AUDIO_UNIT_CONVERSION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CONVERSION))
#define AGS_AUDIO_UNIT_CONVERSION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_CONVERSION, AgsAudioUnitConversionClass))

typedef struct _AgsAudioUnitConversion AgsAudioUnitConversion;
typedef struct _AgsAudioUnitConversionClass AgsAudioUnitConversionClass;

struct _AgsAudioUnitConversion
{
  AgsConversion conversion;

  guint flags;
};

struct _AgsAudioUnitConversionClass
{
  AgsConversionClass conversion;
};

GType ags_audio_unit_conversion_get_type(void);

AgsAudioUnitConversion* ags_audio_unit_conversion_new();

G_END_DECLS

#endif /*__AGS_AUDIO_UNIT_CONVERSION_H__*/
