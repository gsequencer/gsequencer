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

#ifndef __AGS_RESAMPLE_UTIL_H__
#define __AGS_RESAMPLE_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio_buffer_util.h>

G_BEGIN_DECLS

#define AGS_TYPE_RESAMPLE_UTIL         (ags_resample_util_get_type())
#define AGS_RESAMPLE_UTIL(ptr) ((AgsResampleUtil *)(ptr))

#define AGS_RESAMPLE_UTIL_DEFAULT_TARGET_SAMPLERATE (AGS_SOUNDCARD_DEFAULT_SAMPLERATE)

#define AGS_RESAMPLE_UTIL_INITIALIZER ((AgsResampleUtil) {		\
      .source = NULL,							\
      .source_stride = 1,						\
      .destination = NULL,						\
      .destination_stride = 1,						\
      .buffer_length = 0,						\
      .format = AGS_SOUNDCARD_DEFAULT_FORMAT,				\
      .samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE,			\
      .target_samplerate = AGS_RESAMPLE_UTIL_DEFAULT_TARGET_SAMPLERATE, \
      .src_ratio = 1.0, 						\
      .saved_frames = 0,						\
      .input_frames_used = 0,						\
      .end_of_input = 0,						\
      .input_frames = 0,						\
      .data_in = NULL,							\
      .output_frames = 0,						\
      .data_out = NULL,							\
      .increment = 2381,						\
      .bypass_cache = FALSE,						\
      .in_count = 0,							\
      .in_used = 0,							\
      .out_count = 0,							\
      .out_gen = 0,							\
      .coeff_half_len = ARRAY_LEN (ags_samplerate_coeffs) - 2,		\
      .index_inc = 2381,						\
      .input_index = 0,							\
      .b_current = 0,							\
      .b_end = 0,							\
      .b_real_end = -1,							\
      .b_len = 0,							\
      .coeffs = ags_samplerate_coeffs,					\
      .left_calc = 0.0,							\
      .right_calc = 0.0,						\
      .last_ratio = -1.0,						\
      .last_position = 0.0,						\
      .buffer = NULL,							\
      .audio_buffer_util = ags_audio_buffer_util_alloc() })

typedef struct _AgsResampleUtil AgsResampleUtil;

struct _AgsResampleUtil
{
  gpointer destination;
  guint destination_stride;

  gpointer source;
  guint source_stride;
  
  guint buffer_length;
  AgsSoundcardFormat format;
  guint samplerate;

  guint target_samplerate;

  gdouble src_ratio;

  glong saved_frames;
  glong	input_frames_used;
  glong output_frames_gen;

  gint end_of_input;

  gint input_frames;
  gpointer data_in;
  
  gint output_frames;
  gpointer data_out;
  
  gint increment;

  gboolean bypass_cache;
  
  glong	in_count;
  glong in_used;
  glong	out_count;
  glong out_gen ;

  gint coeff_half_len;
  gint index_inc;

  gdouble input_index;

  gint b_current;
  gint b_end;
  gint b_real_end;
  gint b_len;

  gdouble *coeffs;
  
  gdouble left_calc;
  gdouble right_calc;

  gdouble last_ratio;
  gdouble last_position;

  gpointer buffer;  

  AgsAudioBufferUtil *audio_buffer_util;
};

GType ags_resample_util_get_type(void);

AgsResampleUtil* ags_resample_util_alloc();

void ags_resample_util_init(AgsResampleUtil *resample_util);

gpointer ags_resample_util_copy(AgsResampleUtil *ptr);
void ags_resample_util_free(AgsResampleUtil *ptr);

/* getter/setter */
gpointer ags_resample_util_get_destination(AgsResampleUtil *resample_util);
void ags_resample_util_set_destination(AgsResampleUtil *resample_util,
				       gpointer destination);

guint ags_resample_util_get_destination_stride(AgsResampleUtil *resample_util);
void ags_resample_util_set_destination_stride(AgsResampleUtil *resample_util,
					      guint destination_stride);

gpointer ags_resample_util_get_source(AgsResampleUtil *resample_util);
void ags_resample_util_set_source(AgsResampleUtil *resample_util,
				  gpointer source);

guint ags_resample_util_get_source_stride(AgsResampleUtil *resample_util);
void ags_resample_util_set_source_stride(AgsResampleUtil *resample_util,
					 guint source_stride);

guint ags_resample_util_get_buffer_length(AgsResampleUtil *resample_util);
void ags_resample_util_set_buffer_length(AgsResampleUtil *resample_util,
					 guint buffer_length);

AgsSoundcardFormat ags_resample_util_get_format(AgsResampleUtil *resample_util);
void ags_resample_util_set_format(AgsResampleUtil *resample_util,
				  AgsSoundcardFormat format);

guint ags_resample_util_get_samplerate(AgsResampleUtil *resample_util);
void ags_resample_util_set_samplerate(AgsResampleUtil *resample_util,
				      guint samplerate);

guint ags_resample_util_get_target_samplerate(AgsResampleUtil *resample_util);
void ags_resample_util_set_target_samplerate(AgsResampleUtil *resample_util,
					     guint target_samplerate);

/* compute */
void ags_resample_util_compute_s8(AgsResampleUtil *resample_util);
void ags_resample_util_compute_s16(AgsResampleUtil *resample_util);
void ags_resample_util_compute_s24(AgsResampleUtil *resample_util);
void ags_resample_util_compute_s32(AgsResampleUtil *resample_util);
void ags_resample_util_compute_s64(AgsResampleUtil *resample_util);
void ags_resample_util_compute_float(AgsResampleUtil *resample_util);
void ags_resample_util_compute_double(AgsResampleUtil *resample_util);
void ags_resample_util_compute_complex(AgsResampleUtil *resample_util);

void ags_resample_util_compute(AgsResampleUtil *resample_util);

G_END_DECLS

#endif /*__AGS_RESAMPLE_UTIL_H__*/
