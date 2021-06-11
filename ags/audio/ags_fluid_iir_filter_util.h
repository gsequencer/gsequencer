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

#ifndef __AGS_FLUID_IIR_FILTER_UTIL_H__
#define __AGS_FLUID_IIR_FILTER_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <math.h>
#include <complex.h>

G_BEGIN_DECLS

#define AGS_FLUID_IIR_FILTER(ptr)                ((AgsFluidIIRFilter *)(ptr))

#define AGS_FLUID_IIR_DEFAULT_FILTER_TYPE (AGS_FLUID_IIR_DISABLED)
#define AGS_FLUID_IIR_DEFAULT_FILTER_GAIN (1.0)

typedef struct _AgsFluidIIRFilter AgsFluidIIRFilter;

typedef enum{
  AGS_FLUID_IIR_DISABLED,
  AGS_FLUID_IIR_LOWPASS,
  AGS_FLUID_IIR_HIGHPASS,
  AGS_FLUID_IIR_LAST,
}AgsFluidIIRFilterType;

struct _AgsFluidIIRFilter
{
  guint filter_type;
  gboolean filter_startup;
  gdouble filter_gain;
  gdouble q_lin;
  gdouble fres;
  gdouble last_fres;
  gdouble fres_mod;
  gdouble b02;
  gdouble b1;
  gdouble a1;
  gdouble a2;
  gdouble b02_incr;
  gdouble b1_incr;
  gdouble a1_incr;
  gdouble a2_incr;
  gint filter_coeff_incr_count;
  gint compensate_incr;
};

void ags_fluid_iir_filter_util_calc();

void ags_fluid_iir_filter_util_apply_s8(gint8 *destination,
					gint8 *source,
					guint buffer_length,
					gdouble factor);
void ags_fluid_iir_filter_util_apply_s16(gint16 *destination,
					 gint16 *source,
					 guint buffer_length,
					 gdouble factor);
void ags_fluid_iir_filter_util_apply_s24(gint32 *destination,
					 gint32 *source,
					 guint buffer_length,
					 gdouble factor);
void ags_fluid_iir_filter_util_apply_s32(gint32 *destination,
					 gint32 *source,
					 guint buffer_length,
					 gdouble factor);
void ags_fluid_iir_filter_util_apply_s64(gint64 *destination,
					 gint64 *source,
					 guint buffer_length,
					 gdouble factor);
void ags_fluid_iir_filter_util_apply_float(gfloat *destination,
					   gfloat *source,
					   guint buffer_length,
					   gdouble factor);
void ags_fluid_iir_filter_util_apply_double(gdouble *destination,
					    gdouble *source,
					    guint buffer_length,
					    gdouble factor);
void ags_fluid_iir_filter_util_apply_complex(AgsComplex *destination,
					     AgsComplex *source,
					     guint buffer_length,
					     gdouble factor);

G_END_DECLS

#endif /*__AGS_FLUID_IIR_FILTER_UTIL_H__*/
