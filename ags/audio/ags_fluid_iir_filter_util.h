/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#define AGS_TYPE_FLUID_IIR_FILTER_UTIL         (ags_fluid_iir_filter_util_get_type())
#define AGS_FLUID_IIR_FILTER_UTIL(ptr)         ((AgsFluidIIRFilterUtil *)(ptr))

typedef struct _AgsFluidIIRFilterUtil AgsFluidIIRFilterUtil;

typedef enum{
  AGS_FLUID_IIR_DISABLED,
  AGS_FLUID_IIR_LOWPASS,
  AGS_FLUID_IIR_HIGHPASS,
  AGS_FLUID_IIR_LAST,
}AgsFluidIIRFilterType;

struct _AgsFluidIIRFilterUtil
{
  gpointer source;
  guint source_stride;

  gpointer destination;
  guint destination_stride;

  guint buffer_length;
  guint format;
  guint samplerate;
  
  guint filter_type;

  guint flags;

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

  gdouble hist1;
  gdouble hist2;

  gboolean filter_startup;

  gdouble fres;
  gdouble last_fres;

  gdouble q_lin;
  gdouble filter_gain;
};

GType ags_fluid_iir_filter_util_get_type(void);

AgsFluidIIRFilterUtil* ags_fluid_iir_filter_util_alloc();

gpointer ags_fluid_iir_filter_util_copy(AgsFluidIIRFilterUtil *ptr);
void ags_fluid_iir_filter_util_free(AgsFluidIIRFilterUtil *ptr);

gpointer ags_fluid_iir_filter_util_get_source(AgsFluidIIRFilterUtil *fluid_iir_filter_util);
void ags_fluid_iir_filter_util_set_source(AgsFluidIIRFilterUtil *fluid_iir_filter_util,
					  gpointer source);

guint ags_fluid_iir_filter_util_get_source_stride(AgsFluidIIRFilterUtil *fluid_iir_filter_util);
void ags_fluid_iir_filter_util_set_source_stride(AgsFluidIIRFilterUtil *fluid_iir_filter_util,
						 guint source_stride);

gpointer ags_fluid_iir_filter_util_get_destination(AgsFluidIIRFilterUtil *fluid_iir_filter_util);
void ags_fluid_iir_filter_util_set_destination(AgsFluidIIRFilterUtil *fluid_iir_filter_util,
					       gpointer destination);

guint ags_fluid_iir_filter_util_get_destination_stride(AgsFluidIIRFilterUtil *fluid_iir_filter_util);
void ags_fluid_iir_filter_util_set_destination_stride(AgsFluidIIRFilterUtil *fluid_iir_filter_util,
						      guint destination_stride);

guint ags_fluid_iir_filter_util_get_buffer_length(AgsFluidIIRFilterUtil *fluid_iir_filter_util);
void ags_fluid_iir_filter_util_set_buffer_length(AgsFluidIIRFilterUtil *fluid_iir_filter_util,
						 guint buffer_length);

guint ags_fluid_iir_filter_util_get_format(AgsFluidIIRFilterUtil *fluid_iir_filter_util);
void ags_fluid_iir_filter_util_set_format(AgsFluidIIRFilterUtil *fluid_iir_filter_util,
					  guint format);

guint ags_fluid_iir_filter_util_get_samplerate(AgsFluidIIRFilterUtil *fluid_iir_filter_util);
void ags_fluid_iir_filter_util_set_samplerate(AgsFluidIIRFilterUtil *fluid_iir_filter_util,
					      guint samplerate);

guint ags_fluid_iir_filter_util_get_filter_type(AgsFluidIIRFilterUtil *fluid_iir_filter_util);
void ags_fluid_iir_filter_util_set_filter_type(AgsFluidIIRFilterUtil *fluid_iir_filter_util,
					       guint filter_type);

guint ags_fluid_iir_filter_util_get_flags(AgsFluidIIRFilterUtil *fluid_iir_filter_util);
void ags_fluid_iir_filter_util_set_flags(AgsFluidIIRFilterUtil *fluid_iir_filter_util,
					 guint flags);

gboolean ags_fluid_iir_filter_util_get_filter_startup(AgsFluidIIRFilterUtil *fluid_iir_filter_util);
void ags_fluid_iir_filter_util_set_filter_startup(AgsFluidIIRFilterUtil *fluid_iir_filter_util,
						  gboolean filter_startup);

gdouble ags_fluid_iir_filter_util_get_q_lin(AgsFluidIIRFilterUtil *fluid_iir_filter_util);
void ags_fluid_iir_filter_util_set_q_lin(AgsFluidIIRFilterUtil *fluid_iir_filter_util,
					 gdouble q_lin);

gdouble ags_fluid_iir_filter_util_get_filter_gain(AgsFluidIIRFilterUtil *fluid_iir_filter_util);
void ags_fluid_iir_filter_util_set_filter_gain(AgsFluidIIRFilterUtil *fluid_iir_filter_util,
					       gdouble filter_gain);

void ags_fluid_iir_filter_util_process_s8(AgsFluidIIRFilterUtil *fluid_iir_filter_util);
void ags_fluid_iir_filter_util_process_s16(AgsFluidIIRFilterUtil *fluid_iir_filter_util);
void ags_fluid_iir_filter_util_process_s24(AgsFluidIIRFilterUtil *fluid_iir_filter_util);
void ags_fluid_iir_filter_util_process_s32(AgsFluidIIRFilterUtil *fluid_iir_filter_util);
void ags_fluid_iir_filter_util_process_s64(AgsFluidIIRFilterUtil *fluid_iir_filter_util);
void ags_fluid_iir_filter_util_process_float(AgsFluidIIRFilterUtil *fluid_iir_filter_util);
void ags_fluid_iir_filter_util_process_double(AgsFluidIIRFilterUtil *fluid_iir_filter_util);
void ags_fluid_iir_filter_util_process_complex(AgsFluidIIRFilterUtil *fluid_iir_filter_util);
void ags_fluid_iir_filter_util_process(AgsFluidIIRFilterUtil *fluid_iir_filter_util);

G_END_DECLS

#endif /*__AGS_FLUID_IIR_FILTER_UTIL_H__*/
