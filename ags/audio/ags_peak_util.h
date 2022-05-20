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

#ifndef __AGS_PEAK_UTIL_H__
#define __AGS_PEAK_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio_buffer_util.h>

G_BEGIN_DECLS

#define AGS_TYPE_PEAK_UTIL         (ags_peak_util_get_type())
#define AGS_PEAK_UTIL(ptr) ((AgsPeakUtil *)(ptr))

#define AGS_PEAK_UTIL_DEFAULT_FORMAT (AGS_SOUNDCARD_SIGNED_16_BIT)
#define AGS_PEAK_UTIL_DEFAULT_AUDIO_BUFFER_UTIL_FORMAT (AGS_AUDIO_BUFFER_UTIL_S16)
#define AGS_PEAK_UTIL_DEFAULT_SAMPLERATE (AGS_SOUNDCARD_DEFAULT_SAMPLERATE)
#define AGS_PEAK_UTIL_DEFAULT_HARMONIC_RATE (440.0)
#define AGS_PEAK_UTIL_DEFAULT_PRESSURE_FACTOR (1.0)

typedef struct _AgsPeakUtil AgsPeakUtil;

struct _AgsPeakUtil
{
  gpointer source;
  guint source_stride;
  
  guint buffer_length;
  guint format;
  guint samplerate;
  
  gdouble harmonic_rate;
  gdouble pressure_factor;
  
  gdouble peak;
};

GType ags_peak_util_get_type(void);

AgsPeakUtil* ags_peak_util_alloc();

gpointer ags_peak_util_copy(AgsPeakUtil *ptr);
void ags_peak_util_free(AgsPeakUtil *ptr);

gpointer ags_peak_util_get_source(AgsPeakUtil *peak_util);
void ags_peak_util_set_source(AgsPeakUtil *peak_util,
			      gpointer source);

guint ags_peak_util_get_source_stride(AgsPeakUtil *peak_util);
void ags_peak_util_set_source_stride(AgsPeakUtil *peak_util,
				     guint source_stride);

guint ags_peak_util_get_buffer_length(AgsPeakUtil *peak_util);
void ags_peak_util_set_buffer_length(AgsPeakUtil *peak_util,
				     guint buffer_length);

guint ags_peak_util_get_format(AgsPeakUtil *peak_util);
void ags_peak_util_set_format(AgsPeakUtil *peak_util,
			      guint format);

guint ags_peak_util_get_samplerate(AgsPeakUtil *peak_util);
void ags_peak_util_set_samplerate(AgsPeakUtil *peak_util,
				  guint samplerate);

gdouble ags_peak_util_get_harmonic_rate(AgsPeakUtil *peak_util);
void ags_peak_util_set_harmonic_rate(AgsPeakUtil *peak_util,
				     gdouble harmonic_rate);

gdouble ags_peak_util_get_pressure_factor(AgsPeakUtil *peak_util);
void ags_peak_util_set_pressure_factor(AgsPeakUtil *peak_util,
				       gdouble pressure_factor);

gdouble ags_peak_util_get_peak(AgsPeakUtil *peak_util);
void ags_peak_util_set_peak(AgsPeakUtil *peak_util,
			    gdouble peak);

void ags_peak_util_compute_s8(AgsPeakUtil *peak_util);
void ags_peak_util_compute_s16(AgsPeakUtil *peak_util);
void ags_peak_util_compute_s24(AgsPeakUtil *peak_util);
void ags_peak_util_compute_s32(AgsPeakUtil *peak_util);
void ags_peak_util_compute_s64(AgsPeakUtil *peak_util);
void ags_peak_util_compute_float(AgsPeakUtil *peak_util);
void ags_peak_util_compute_double(AgsPeakUtil *peak_util);
void ags_peak_util_compute_complex(AgsPeakUtil *peak_util);

void ags_peak_util_compute(AgsPeakUtil *peak_util);

G_END_DECLS

#endif /*__AGS_PEAK_UTIL_H__*/
