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

#ifndef __AGS_PITCH_16X_ALIAS_UTIL_H__
#define __AGS_PITCH_16X_ALIAS_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <math.h>
#include <complex.h>

G_BEGIN_DECLS

#define AGS_TYPE_PITCH_16X_ALIAS_UTIL         (ags_pitch_16x_alias_util_get_type())
#define AGS_PITCH_16X_ALIAS_UTIL(ptr) ((AgsPitch16xAliasUtil *)(ptr))

#define AGS_PITCH_16X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE (4 * 64 * 65536)

#define AGS_PITCH_16X_ALIAS_UTIL_INITIALIZER ((AgsPitch16xAliasUtil) {	\
      .source = NULL,							\
      .source_stride = 1,						\
      .destination = NULL,						\
      .destination_stride = 1,						\
      .alias_source_buffer = ags_stream_alloc(AGS_PITCH_16X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE, AGS_SOUNDCARD_DEFAULT_FORMAT), \
      .alias_source_max_buffer_length = AGS_PITCH_16X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE, \
      .alias_new_source_buffer = ags_stream_alloc(AGS_PITCH_16X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE, AGS_SOUNDCARD_DEFAULT_FORMAT), \
      .alias_new_source_max_buffer_length = AGS_PITCH_16X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE, \
      .buffer_length = 0,						\
      .format = AGS_SOUNDCARD_DEFAULT_FORMAT,				\
      .samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE,			\
      .base_key = 0.0,							\
      .tuning = 0.0,							\
      .vibrato_enabled = FALSE,						\
      .vibrato_gain = 1.0,						\
      .vibrato_lfo_depth = 1.0,						\
      .vibrato_lfo_freq = 8.172,					\
      .vibrato_tuning = 0.0,						\
      .frame_count = (AGS_SOUNDCARD_DEFAULT_SAMPLERATE / 8.172),	\
      .offset = 0,							\
      .note_256th_mode = FALSE,						\
      .offset_256th = 0 })

typedef struct _AgsPitch16xAliasUtil AgsPitch16xAliasUtil;

struct _AgsPitch16xAliasUtil
{
  gpointer source;
  guint source_stride;

  gpointer destination;
  guint destination_stride;

  gpointer alias_source_buffer;
  guint alias_source_max_buffer_length;
  
  gpointer alias_new_source_buffer;
  guint alias_new_source_max_buffer_length;

  guint buffer_length;
  guint format;
  guint samplerate;

  gdouble base_key;
  gdouble tuning;

  gdouble vibrato_enabled;

  gdouble vibrato_gain;
  gdouble vibrato_lfo_depth;
  gdouble vibrato_lfo_freq;
  gdouble vibrato_tuning;

  guint frame_count;
  guint offset;
  
  gboolean note_256th_mode;

  guint offset_256th;
};

GType ags_pitch_16x_alias_util_get_type(void);

AgsPitch16xAliasUtil* ags_pitch_16x_alias_util_alloc();

gpointer ags_pitch_16x_alias_util_copy(AgsPitch16xAliasUtil *ptr);
void ags_pitch_16x_alias_util_free(AgsPitch16xAliasUtil *ptr);

gpointer ags_pitch_16x_alias_util_get_source(AgsPitch16xAliasUtil *pitch_16x_alias_util);
void ags_pitch_16x_alias_util_set_source(AgsPitch16xAliasUtil *pitch_16x_alias_util,
					 gpointer source);

guint ags_pitch_16x_alias_util_get_source_stride(AgsPitch16xAliasUtil *pitch_16x_alias_util);
void ags_pitch_16x_alias_util_set_source_stride(AgsPitch16xAliasUtil *pitch_16x_alias_util,
						guint source_stride);

gpointer ags_pitch_16x_alias_util_get_destination(AgsPitch16xAliasUtil *pitch_16x_alias_util);
void ags_pitch_16x_alias_util_set_destination(AgsPitch16xAliasUtil *pitch_16x_alias_util,
					      gpointer destination);

guint ags_pitch_16x_alias_util_get_destination_stride(AgsPitch16xAliasUtil *pitch_16x_alias_util);
void ags_pitch_16x_alias_util_set_destination_stride(AgsPitch16xAliasUtil *pitch_16x_alias_util,
						     guint destination_stride);

guint ags_pitch_16x_alias_util_get_buffer_length(AgsPitch16xAliasUtil *pitch_16x_alias_util);
void ags_pitch_16x_alias_util_set_buffer_length(AgsPitch16xAliasUtil *pitch_16x_alias_util,
						guint buffer_length);

guint ags_pitch_16x_alias_util_get_format(AgsPitch16xAliasUtil *pitch_16x_alias_util);
void ags_pitch_16x_alias_util_set_format(AgsPitch16xAliasUtil *pitch_16x_alias_util,
					 guint format);

guint ags_pitch_16x_alias_util_get_samplerate(AgsPitch16xAliasUtil *pitch_16x_alias_util);
void ags_pitch_16x_alias_util_set_samplerate(AgsPitch16xAliasUtil *pitch_16x_alias_util,
					     guint samplerate);

gdouble ags_pitch_16x_alias_util_get_base_key(AgsPitch16xAliasUtil *pitch_16x_alias_util);
void ags_pitch_16x_alias_util_set_base_key(AgsPitch16xAliasUtil *pitch_16x_alias_util,
					   gdouble base_key);

gdouble ags_pitch_16x_alias_util_get_tuning(AgsPitch16xAliasUtil *pitch_16x_alias_util);
void ags_pitch_16x_alias_util_set_tuning(AgsPitch16xAliasUtil *pitch_16x_alias_util,
					 gdouble tuning);

gboolean ags_pitch_16x_alias_util_get_vibrato_enabled(AgsPitch16xAliasUtil *pitch_16x_alias_util);
void ags_pitch_16x_alias_util_set_vibrato_enabled(AgsPitch16xAliasUtil *pitch_16x_alias_util,
						  gboolean vibrato_enabled);

gdouble ags_pitch_16x_alias_util_get_vibrato_gain(AgsPitch16xAliasUtil *pitch_16x_alias_util);
void ags_pitch_16x_alias_util_set_vibrato_gain(AgsPitch16xAliasUtil *pitch_16x_alias_util,
					       gdouble vibrato_gain);

gdouble ags_pitch_16x_alias_util_get_vibrato_lfo_depth(AgsPitch16xAliasUtil *pitch_16x_alias_util);
void ags_pitch_16x_alias_util_set_vibrato_lfo_depth(AgsPitch16xAliasUtil *pitch_16x_alias_util,
						    gdouble vibrato_lfo_depth);

gdouble ags_pitch_16x_alias_util_get_vibrato_lfo_freq(AgsPitch16xAliasUtil *pitch_16x_alias_util);
void ags_pitch_16x_alias_util_set_vibrato_lfo_freq(AgsPitch16xAliasUtil *pitch_16x_alias_util,
						   gdouble vibrato_lfo_freq);

gdouble ags_pitch_16x_alias_util_get_vibrato_tuning(AgsPitch16xAliasUtil *pitch_16x_alias_util);
void ags_pitch_16x_alias_util_set_vibrato_tuning(AgsPitch16xAliasUtil *pitch_16x_alias_util,
						 gdouble vibrato_tuning);

guint ags_pitch_16x_alias_util_get_frame_count(AgsPitch16xAliasUtil *pitch_16x_alias_util);
void ags_pitch_16x_alias_util_set_frame_count(AgsPitch16xAliasUtil *pitch_16x_alias_util,
					      guint frame_count);

guint ags_pitch_16x_alias_util_get_offset(AgsPitch16xAliasUtil *pitch_16x_alias_util);
void ags_pitch_16x_alias_util_set_offset(AgsPitch16xAliasUtil *pitch_16x_alias_util,
					 guint offset);

gboolean ags_pitch_16x_alias_util_get_note_256th_mode(AgsPitch16xAliasUtil *pitch_16x_alias_util);
void ags_pitch_16x_alias_util_set_note_256th_mode(AgsPitch16xAliasUtil *pitch_16x_alias_util,
						  gboolean note_256th_mode);

guint ags_pitch_16x_alias_util_get_offset_256th(AgsPitch16xAliasUtil *pitch_16x_alias_util);
void ags_pitch_16x_alias_util_set_offset_256th(AgsPitch16xAliasUtil *pitch_16x_alias_util,
					       guint offset_256th);

void ags_pitch_16x_alias_util_pitch_s8(AgsPitch16xAliasUtil *pitch_16x_alias_util);
void ags_pitch_16x_alias_util_pitch_s16(AgsPitch16xAliasUtil *pitch_16x_alias_util);
void ags_pitch_16x_alias_util_pitch_s24(AgsPitch16xAliasUtil *pitch_16x_alias_util);
void ags_pitch_16x_alias_util_pitch_s32(AgsPitch16xAliasUtil *pitch_16x_alias_util);
void ags_pitch_16x_alias_util_pitch_s64(AgsPitch16xAliasUtil *pitch_16x_alias_util);
void ags_pitch_16x_alias_util_pitch_float(AgsPitch16xAliasUtil *pitch_16x_alias_util);
void ags_pitch_16x_alias_util_pitch_double(AgsPitch16xAliasUtil *pitch_16x_alias_util);
void ags_pitch_16x_alias_util_pitch_complex(AgsPitch16xAliasUtil *pitch_16x_alias_util);

void ags_pitch_16x_alias_util_pitch(AgsPitch16xAliasUtil *pitch_16x_alias_util);

G_END_DECLS

#endif /*__AGS_PITCH_16X_ALIAS_UTIL_H__*/
