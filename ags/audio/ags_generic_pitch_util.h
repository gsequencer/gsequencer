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

#ifndef __AGS_GENERIC_PITCH_UTIL_H__
#define __AGS_GENERIC_PITCH_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <math.h>
#include <complex.h>

G_BEGIN_DECLS

#define AGS_GENERIC_PITCH(ptr)                ((AgsGenericPitch *)(ptr))

typedef struct _AgsGenericPitch AgsGenericPitch;

/**
 * AgsPitchType:
 * @AGS_FAST_PITCH: the AGS fast pitch
 * @AGS_HQ_PITCH: the AGS HQ pitch
 * @AGS_FLUID_NO_INTERPOLATE: the Fluidsynth no interpolate
 * @AGS_FLUID_LINEAR_INTERPOLATE: the Fluidsynth linear interpolate
 * @AGS_FLUID_4TH_ORDER_INTERPOLATE: the Fluidsynth qubic interpolate
 * @AGS_FLUID_7TH_ORDER_INTERPOLATE: the Fluidsynth 7th order interpolate
 *
 * Enum values to select the desired pitch strategy.
 */
typedef enum{
  AGS_FAST_PITCH,
  AGS_HQ_PITCH,
  AGS_FLUID_NO_INTERPOLATE,
  AGS_FLUID_LINEAR_INTERPOLATE,
  AGS_FLUID_4TH_ORDER_INTERPOLATE,
  AGS_FLUID_7TH_ORDER_INTERPOLATE,
  AGS_GENERIC_PITCH_LAST,
}AgsPitchType;

struct _AgsGenericPitch
{
  guint pitch_type;

  gpointer data;
};

void ags_generic_pitch_util_compute_s8(AgsGenericPitch *generic_pitch,
				       gint8 *buffer,
				       guint buffer_length,
				       guint samplerate,
				       gdouble base_key,
				       gdouble tuning);
void ags_generic_pitch_util_compute_s16(AgsGenericPitch *generic_pitch,
					gint16 *buffer,
					guint buffer_length,
					guint samplerate,
					gdouble base_key,
					gdouble tuning);
void ags_generic_pitch_util_compute_s24(AgsGenericPitch *generic_pitch,
					gint32 *buffer,
					guint buffer_length,
					guint samplerate,
					gdouble base_key,
					gdouble tuning);
void ags_generic_pitch_util_compute_s32(AgsGenericPitch *generic_pitch,
					gint32 *buffer,
					guint buffer_length,
					guint samplerate,
					gdouble base_key,
					gdouble tuning);
void ags_generic_pitch_util_compute_s64(AgsGenericPitch *generic_pitch,
					gint64 *buffer,
					guint buffer_length,
					guint samplerate,
					gdouble base_key,
					gdouble tuning);
void ags_generic_pitch_util_compute_float(AgsGenericPitch *generic_pitch,
					  gfloat *buffer,
					  guint buffer_length,
					  guint samplerate,
					  gdouble base_key,
					  gdouble tuning);
void ags_generic_pitch_util_compute_double(AgsGenericPitch *generic_pitch,
					   gdouble *buffer,
					   guint buffer_length,
					   guint samplerate,
					   gdouble base_key,
					   gdouble tuning);
void ags_generic_pitch_util_compute_complex(AgsGenericPitch *generic_pitch,
					    AgsComplex *buffer,
					    guint buffer_length,
					    guint samplerate,
					    gdouble base_key,
					    gdouble tuning);

G_END_DECLS

#endif /*__AGS_GENERIC_PITCH_UTIL_H__*/
