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

#ifndef __AGS_HQ_PITCH_UTIL_H__
#define __AGS_HQ_PITCH_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <math.h>
#include <complex.h>

#include <ags/audio/ags_linear_interpolate_util.h>

G_BEGIN_DECLS

#define AGS_TYPE_HQ_PITCH_UTIL         (ags_hq_pitch_util_get_type())
#define AGS_HQ_PITCH_UTIL(ptr) ((AgsHQPitchUtil *)(ptr))

typedef struct _AgsHQPitchUtil AgsHQPitchUtil;

struct _AgsHQPitchUtil
{
  gpointer source;
  guint source_stride;

  gpointer destination;
  guint destination_stride;

  gpointer low_mix_buffer;
  gpointer new_mix_buffer;

  guint buffer_length;
  guint audio_buffer_util_format;
  guint samplerate;

  gdouble base_key;
  gdouble tuning;

  AgsLinearInterpolateUtil *linear_interpolate_util;
};

GType ags_hq_pitch_util_get_type(void);

AgsHQPitchUtil* ags_hq_pitch_util_alloc();

gpointer ags_hq_pitch_util_copy(AgsHQPitchUtil *ptr);
void ags_hq_pitch_util_free(AgsHQPitchUtil *ptr);

gpointer ags_hq_pitch_util_get_source(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_set_source(AgsHQPitchUtil *hq_pitch_util,
				  gpointer source);

guint ags_hq_pitch_util_get_source_stride(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_set_source_stride(AgsHQPitchUtil *hq_pitch_util,
					 guint source_stride);

gpointer ags_hq_pitch_util_get_destination(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_set_destination(AgsHQPitchUtil *hq_pitch_util,
				       gpointer destination);

guint ags_hq_pitch_util_get_destination_stride(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_set_destination_stride(AgsHQPitchUtil *hq_pitch_util,
					      guint destination_stride);

gpointer ags_hq_pitch_util_get_low_mix_buffer(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_set_low_mix_buffer(AgsHQPitchUtil *hq_pitch_util,
					  gpointer low_mix_buffer);

gpointer ags_hq_pitch_util_get_new_mix_buffer(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_set_new_mix_buffer(AgsHQPitchUtil *hq_pitch_util,
					  gpointer new_mix_buffer);

guint ags_hq_pitch_util_get_buffer_length(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_set_buffer_length(AgsHQPitchUtil *hq_pitch_util,
					 guint buffer_length);

guint ags_hq_pitch_util_get_audio_buffer_util_format(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_set_audio_buffer_util_format(AgsHQPitchUtil *hq_pitch_util,
						    guint audio_buffer_util_format);

guint ags_hq_pitch_util_get_samplerate(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_set_samplerate(AgsHQPitchUtil *hq_pitch_util,
				      guint samplerate);

gdouble ags_hq_pitch_util_get_base_key(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_set_base_key(AgsHQPitchUtil *hq_pitch_util,
				    gdouble base_key);

gdouble ags_hq_pitch_util_get_tuning(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_set_tuning(AgsHQPitchUtil *hq_pitch_util,
				  gdouble tuning);

void ags_hq_pitch_util_pitch_s8(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_pitch_s16(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_pitch_s24(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_pitch_s32(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_pitch_s64(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_pitch_float(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_pitch_double(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_pitch_complex(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_pitch(AgsHQPitchUtil *hq_pitch_util);

void ags_hq_pitch_util_compute_s8(gint8 *buffer,
				  guint buffer_length,
				  guint samplerate,
				  gdouble base_key,
				  gdouble tuning);
void ags_hq_pitch_util_compute_s16(gint16 *buffer,
				   guint buffer_length,
				   guint samplerate,
				   gdouble base_key,
				   gdouble tuning);
void ags_hq_pitch_util_compute_s24(gint32 *buffer,
				   guint buffer_length,
				   guint samplerate,
				   gdouble base_key,
				   gdouble tuning);
void ags_hq_pitch_util_compute_s32(gint32 *buffer,
				   guint buffer_length,
				   guint samplerate,
				   gdouble base_key,
				   gdouble tuning);
void ags_hq_pitch_util_compute_s64(gint64 *buffer,
				   guint buffer_length,
				   guint samplerate,
				   gdouble base_key,
				   gdouble tuning);
void ags_hq_pitch_util_compute_float(gfloat *buffer,
				     guint buffer_length,
				     guint samplerate,
				     gdouble base_key,
				     gdouble tuning);
void ags_hq_pitch_util_compute_double(gdouble *buffer,
				      guint buffer_length,
				      guint samplerate,
				      gdouble base_key,
				      gdouble tuning);
void ags_hq_pitch_util_compute_complex(AgsComplex *buffer,
				       guint buffer_length,
				       guint samplerate,
				       gdouble base_key,
				       gdouble tuning);

G_END_DECLS

#endif /*__AGS_HQ_PITCH_UTIL_H__*/
