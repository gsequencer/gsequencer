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

#ifndef __AGS_FAST_PITCH_UTIL_H__
#define __AGS_FAST_PITCH_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <math.h>
#include <complex.h>

G_BEGIN_DECLS

#define AGS_TYPE_FAST_PITCH_UTIL         (ags_fast_pitch_util_get_type())
#define AGS_FAST_PITCH_UTIL(ptr) ((AgsFastPitchUtil *)(ptr))

typedef struct _AgsFastPitchUtil AgsFastPitchUtil;

struct _AgsFastPitchUtil
{
  gpointer source;
  guint source_stride;

  gpointer destination;
  guint destination_stride;

  gpointer mix_buffer;
  gpointer im_mix_buffer;
  gpointer low_mix_buffer;
  gpointer new_mix_buffer;

  guint buffer_length;
  guint format;
  guint samplerate;

  gdouble base_key;
  gdouble tuning;
};

GType ags_fast_pitch_util_get_type(void);

AgsFastPitchUtil* ags_fast_pitch_util_alloc();

gpointer ags_fast_pitch_util_copy(AgsFastPitchUtil *ptr);
void ags_fast_pitch_util_free(AgsFastPitchUtil *ptr);

gpointer ags_fast_pitch_util_get_source(AgsFastPitchUtil *fast_pitch_util);
void ags_fast_pitch_util_set_source(AgsFastPitchUtil *fast_pitch_util,
				    gpointer source);

guint ags_fast_pitch_util_get_source_stride(AgsFastPitchUtil *fast_pitch_util);
void ags_fast_pitch_util_set_source_stride(AgsFastPitchUtil *fast_pitch_util,
					   guint source_stride);

gpointer ags_fast_pitch_util_get_destination(AgsFastPitchUtil *fast_pitch_util);
void ags_fast_pitch_util_set_destination(AgsFastPitchUtil *fast_pitch_util,
					 gpointer destination);

guint ags_fast_pitch_util_get_destination_stride(AgsFastPitchUtil *fast_pitch_util);
void ags_fast_pitch_util_set_destination_stride(AgsFastPitchUtil *fast_pitch_util,
						guint destination_stride);

guint ags_fast_pitch_util_get_buffer_length(AgsFastPitchUtil *fast_pitch_util);
void ags_fast_pitch_util_set_buffer_length(AgsFastPitchUtil *fast_pitch_util,
					   guint buffer_length);

guint ags_fast_pitch_util_get_format(AgsFastPitchUtil *fast_pitch_util);
void ags_fast_pitch_util_set_format(AgsFastPitchUtil *fast_pitch_util,
				    guint format);

guint ags_fast_pitch_util_get_samplerate(AgsFastPitchUtil *fast_pitch_util);
void ags_fast_pitch_util_set_samplerate(AgsFastPitchUtil *fast_pitch_util,
					guint samplerate);

gdouble ags_fast_pitch_util_get_base_key(AgsFastPitchUtil *fast_pitch_util);
void ags_fast_pitch_util_set_base_key(AgsFastPitchUtil *fast_pitch_util,
				      gdouble base_key);

gdouble ags_fast_pitch_util_get_tuning(AgsFastPitchUtil *fast_pitch_util);
void ags_fast_pitch_util_set_tuning(AgsFastPitchUtil *fast_pitch_util,
				    gdouble tuning);

void ags_fast_pitch_util_pitch_s8(AgsFastPitchUtil *fast_pitch_util);
void ags_fast_pitch_util_pitch_s16(AgsFastPitchUtil *fast_pitch_util);
void ags_fast_pitch_util_pitch_s24(AgsFastPitchUtil *fast_pitch_util);
void ags_fast_pitch_util_pitch_s32(AgsFastPitchUtil *fast_pitch_util);
void ags_fast_pitch_util_pitch_s64(AgsFastPitchUtil *fast_pitch_util);
void ags_fast_pitch_util_pitch_float(AgsFastPitchUtil *fast_pitch_util);
void ags_fast_pitch_util_pitch_double(AgsFastPitchUtil *fast_pitch_util);
void ags_fast_pitch_util_pitch_complex(AgsFastPitchUtil *fast_pitch_util);
void ags_fast_pitch_util_pitch(AgsFastPitchUtil *fast_pitch_util);

G_DEPRECATED_FOR(ags_fast_pitch_util_pitch_s8)
void ags_fast_pitch_util_compute_s8(gint8 *buffer,
				    guint buffer_length,
				    guint samplerate,
				    gdouble base_key,
				    gdouble tuning);
G_DEPRECATED_FOR(ags_fast_pitch_util_pitch_s16)
void ags_fast_pitch_util_compute_s16(gint16 *buffer,
				     guint buffer_length,
				     guint samplerate,
				     gdouble base_key,
				     gdouble tuning);
G_DEPRECATED_FOR(ags_fast_pitch_util_pitch_s24)
void ags_fast_pitch_util_compute_s24(gint32 *buffer,
				     guint buffer_length,
				     guint samplerate,
				     gdouble base_key,
				     gdouble tuning);
G_DEPRECATED_FOR(ags_fast_pitch_util_pitch_s32)
void ags_fast_pitch_util_compute_s32(gint32 *buffer,
				     guint buffer_length,
				     guint samplerate,
				     gdouble base_key,
				     gdouble tuning);
G_DEPRECATED_FOR(ags_fast_pitch_util_pitch_s64)
void ags_fast_pitch_util_compute_s64(gint64 *buffer,
				     guint buffer_length,
				     guint samplerate,
				     gdouble base_key,
				     gdouble tuning);
G_DEPRECATED_FOR(ags_fast_pitch_util_pitch_float)
void ags_fast_pitch_util_compute_float(gfloat *buffer,
				       guint buffer_length,
				       guint samplerate,
				       gdouble base_key,
				       gdouble tuning);
G_DEPRECATED_FOR(ags_fast_pitch_util_pitch_double)
void ags_fast_pitch_util_compute_double(gdouble *buffer,
					guint buffer_length,
					guint samplerate,
					gdouble base_key,
					gdouble tuning);
G_DEPRECATED_FOR(ags_fast_pitch_util_pitch_complex)
void ags_fast_pitch_util_compute_complex(AgsComplex *buffer,
					 guint buffer_length,
					 guint samplerate,
					 gdouble base_key,
					 gdouble tuning);

G_END_DECLS

#endif /*__AGS_FAST_PITCH_UTIL_H__*/
