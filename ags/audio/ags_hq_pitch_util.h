/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#define AGS_HQ_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE (65536)

#define AGS_HQ_PITCH_UTIL_INITIALIZER ((AgsHQPitchUtil) {		\
      .source = NULL,							\
	.source_stride = 1,						\
	.destination = NULL,						\
	.destination_stride = 1,					\
	.low_mix_buffer = ags_stream_alloc(AGS_HQ_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE, AGS_SOUNDCARD_DEFAULT_FORMAT), \
	.low_mix_buffer_max_buffer_length = AGS_HQ_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE, \
	.new_mix_buffer = ags_stream_alloc(AGS_HQ_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE, AGS_SOUNDCARD_DEFAULT_FORMAT), \
	.new_mix_buffer_max_buffer_length = AGS_HQ_PITCH_UTIL_DEFAULT_MAX_BUFFER_SIZE, \
	.buffer_length = 0,						\
	.format = AGS_SOUNDCARD_DEFAULT_FORMAT,				\
	.samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE,			\
	.base_key = 0.0,						\
	.tuning = 0.0,							\
	.linear_interpolate_util = ags_linear_interpolate_util_alloc(),	\
	.vibrato_enabled = FALSE,					\
	.vibrato_gain = 1.0,						\
	.vibrato_lfo_depth = 1.0,					\
	.vibrato_lfo_freq = 6.0,					\
	.vibrato_tuning = 0.0,						\
	.frame_count = (AGS_SOUNDCARD_DEFAULT_SAMPLERATE / 6.0),	\
	.offset = 0,							\
	.note_256th_mode = FALSE,					\
	.offset_256th = 0 })

typedef struct _AgsHQPitchUtil AgsHQPitchUtil;

struct _AgsHQPitchUtil
{
  gpointer source;
  guint source_stride;

  gpointer destination;
  guint destination_stride;

  gpointer low_mix_buffer;
  guint low_mix_buffer_max_buffer_length;

  gpointer new_mix_buffer;
  guint new_mix_buffer_max_buffer_length;

  guint buffer_length;
  AgsSoundcardFormat format;
  guint samplerate;

  gdouble base_key;
  gdouble tuning;

  AgsLinearInterpolateUtil *linear_interpolate_util;

  gdouble vibrato_enabled;

  gdouble vibrato_gain;
  gdouble vibrato_lfo_depth;
  gdouble vibrato_lfo_freq;
  gdouble vibrato_tuning;
  
  guint vibrato_lfo_frame_count;
  guint vibrato_lfo_offset;

  guint frame_count;
  guint offset;
  
  gboolean note_256th_mode;

  guint offset_256th;
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

guint ags_hq_pitch_util_get_buffer_length(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_set_buffer_length(AgsHQPitchUtil *hq_pitch_util,
					 guint buffer_length);

AgsSoundcardFormat ags_hq_pitch_util_get_format(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_set_format(AgsHQPitchUtil *hq_pitch_util,
				  AgsSoundcardFormat format);

guint ags_hq_pitch_util_get_samplerate(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_set_samplerate(AgsHQPitchUtil *hq_pitch_util,
				      guint samplerate);

gdouble ags_hq_pitch_util_get_base_key(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_set_base_key(AgsHQPitchUtil *hq_pitch_util,
				    gdouble base_key);

gdouble ags_hq_pitch_util_get_tuning(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_set_tuning(AgsHQPitchUtil *hq_pitch_util,
				  gdouble tuning);

gboolean ags_hq_pitch_util_get_vibrato_enabled(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_set_vibrato_enabled(AgsHQPitchUtil *hq_pitch_util,
					   gboolean vibrato_enabled);

gdouble ags_hq_pitch_util_get_vibrato_gain(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_set_vibrato_gain(AgsHQPitchUtil *hq_pitch_util,
					gdouble vibrato_gain);

gdouble ags_hq_pitch_util_get_vibrato_lfo_depth(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_set_vibrato_lfo_depth(AgsHQPitchUtil *hq_pitch_util,
					     gdouble vibrato_lfo_depth);

gdouble ags_hq_pitch_util_get_vibrato_lfo_freq(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_set_vibrato_lfo_freq(AgsHQPitchUtil *hq_pitch_util,
					    gdouble vibrato_lfo_freq);

gdouble ags_hq_pitch_util_get_vibrato_tuning(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_set_vibrato_tuning(AgsHQPitchUtil *hq_pitch_util,
					  gdouble vibrato_tuning);

guint ags_hq_pitch_util_get_frame_count(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_set_frame_count(AgsHQPitchUtil *hq_pitch_util,
				       guint frame_count);

guint ags_hq_pitch_util_get_offset(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_set_offset(AgsHQPitchUtil *hq_pitch_util,
				  guint offset);

gboolean ags_hq_pitch_util_get_note_256th_mode(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_set_note_256th_mode(AgsHQPitchUtil *hq_pitch_util,
					   gboolean note_256th_mode);

guint ags_hq_pitch_util_get_offset_256th(AgsHQPitchUtil *hq_pitch_util);
void ags_hq_pitch_util_set_offset_256th(AgsHQPitchUtil *hq_pitch_util,
					guint offset_256th);

void ags_hq_pitch_util_pitch(AgsHQPitchUtil *hq_pitch_util);

G_END_DECLS

#endif /*__AGS_HQ_PITCH_UTIL_H__*/
