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

#ifndef __AGS_PITCH_4X_ALIAS_UTIL_H__
#define __AGS_PITCH_4X_ALIAS_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <math.h>
#include <complex.h>

G_BEGIN_DECLS

#define AGS_TYPE_PITCH_4X_ALIAS_UTIL         (ags_pitch_4x_alias_util_get_type())
#define AGS_PITCH_4X_ALIAS_UTIL(ptr) ((AgsPitch4xAliasUtil *)(ptr))

#define AGS_PITCH_4X_ALIAS_UTIL_DEFAULT_MAX_BUFFER_SIZE (65536)

typedef struct _AgsPitch4xAliasUtil AgsPitch4xAliasUtil;

struct _AgsPitch4xAliasUtil
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
  
  guint vibrato_lfo_frame_count;
  guint vibrato_lfo_offset;

  guint frame_count;
  guint offset;
  
  gboolean note_256th_mode;

  guint offset_256th;
};

GType ags_pitch_4x_alias_util_get_type(void);

AgsPitch4xAliasUtil* ags_pitch_4x_alias_util_alloc();

gpointer ags_pitch_4x_alias_util_copy(AgsPitch4xAliasUtil *ptr);
void ags_pitch_4x_alias_util_free(AgsPitch4xAliasUtil *ptr);

gpointer ags_pitch_4x_alias_util_get_source(AgsPitch4xAliasUtil *pitch_4x_alias_util);
void ags_pitch_4x_alias_util_set_source(AgsPitch4xAliasUtil *pitch_4x_alias_util,
					gpointer source);

guint ags_pitch_4x_alias_util_get_source_stride(AgsPitch4xAliasUtil *pitch_4x_alias_util);
void ags_pitch_4x_alias_util_set_source_stride(AgsPitch4xAliasUtil *pitch_4x_alias_util,
					       guint source_stride);

gpointer ags_pitch_4x_alias_util_get_destination(AgsPitch4xAliasUtil *pitch_4x_alias_util);
void ags_pitch_4x_alias_util_set_destination(AgsPitch4xAliasUtil *pitch_4x_alias_util,
					     gpointer destination);

guint ags_pitch_4x_alias_util_get_destination_stride(AgsPitch4xAliasUtil *pitch_4x_alias_util);
void ags_pitch_4x_alias_util_set_destination_stride(AgsPitch4xAliasUtil *pitch_4x_alias_util,
						    guint destination_stride);

guint ags_pitch_4x_alias_util_get_buffer_length(AgsPitch4xAliasUtil *pitch_4x_alias_util);
void ags_pitch_4x_alias_util_set_buffer_length(AgsPitch4xAliasUtil *pitch_4x_alias_util,
					       guint buffer_length);

guint ags_pitch_4x_alias_util_get_format(AgsPitch4xAliasUtil *pitch_4x_alias_util);
void ags_pitch_4x_alias_util_set_format(AgsPitch4xAliasUtil *pitch_4x_alias_util,
					guint format);

guint ags_pitch_4x_alias_util_get_samplerate(AgsPitch4xAliasUtil *pitch_4x_alias_util);
void ags_pitch_4x_alias_util_set_samplerate(AgsPitch4xAliasUtil *pitch_4x_alias_util,
					    guint samplerate);

gdouble ags_pitch_4x_alias_util_get_base_key(AgsPitch4xAliasUtil *pitch_4x_alias_util);
void ags_pitch_4x_alias_util_set_base_key(AgsPitch4xAliasUtil *pitch_4x_alias_util,
					  gdouble base_key);

gdouble ags_pitch_4x_alias_util_get_tuning(AgsPitch4xAliasUtil *pitch_4x_alias_util);
void ags_pitch_4x_alias_util_set_tuning(AgsPitch4xAliasUtil *pitch_4x_alias_util,
					gdouble tuning);

gboolean ags_pitch_4x_alias_util_get_vibrato_enabled(AgsPitch4xAliasUtil *pitch_4x_alias_util);
void ags_pitch_4x_alias_util_set_vibrato_enabled(AgsPitch4xAliasUtil *pitch_4x_alias_util,
						 gboolean vibrato_enabled);

gdouble ags_pitch_4x_alias_util_get_vibrato_gain(AgsPitch4xAliasUtil *pitch_4x_alias_util);
void ags_pitch_4x_alias_util_set_vibrato_gain(AgsPitch4xAliasUtil *pitch_4x_alias_util,
					      gdouble vibrato_gain);

gdouble ags_pitch_4x_alias_util_get_vibrato_lfo_depth(AgsPitch4xAliasUtil *pitch_4x_alias_util);
void ags_pitch_4x_alias_util_set_vibrato_lfo_depth(AgsPitch4xAliasUtil *pitch_4x_alias_util,
						   gdouble vibrato_lfo_depth);

gdouble ags_pitch_4x_alias_util_get_vibrato_lfo_freq(AgsPitch4xAliasUtil *pitch_4x_alias_util);
void ags_pitch_4x_alias_util_set_vibrato_lfo_freq(AgsPitch4xAliasUtil *pitch_4x_alias_util,
						  gdouble vibrato_lfo_freq);

gdouble ags_pitch_4x_alias_util_get_vibrato_tuning(AgsPitch4xAliasUtil *pitch_4x_alias_util);
void ags_pitch_4x_alias_util_set_vibrato_tuning(AgsPitch4xAliasUtil *pitch_4x_alias_util,
						gdouble vibrato_tuning);

G_DEPRECATED_FOR(ags_pitch_4x_alias_util_get_offset)
  guint ags_pitch_4x_alias_util_get_vibrato_lfo_offset(AgsPitch4xAliasUtil *pitch_4x_alias_util);
G_DEPRECATED_FOR(ags_pitch_4x_alias_util_set_offset)
  void ags_pitch_4x_alias_util_set_vibrato_lfo_offset(AgsPitch4xAliasUtil *pitch_4x_alias_util,
						      guint vibrato_lfo_offset);

guint ags_pitch_4x_alias_util_get_frame_count(AgsPitch4xAliasUtil *pitch_4x_alias_util);
void ags_pitch_4x_alias_util_set_frame_count(AgsPitch4xAliasUtil *pitch_4x_alias_util,
					     guint frame_count);

guint ags_pitch_4x_alias_util_get_offset(AgsPitch4xAliasUtil *pitch_4x_alias_util);
void ags_pitch_4x_alias_util_set_offset(AgsPitch4xAliasUtil *pitch_4x_alias_util,
					guint offset);

gboolean ags_pitch_4x_alias_util_get_note_256th_mode(AgsPitch4xAliasUtil *pitch_4x_alias_util);
void ags_pitch_4x_alias_util_set_note_256th_mode(AgsPitch4xAliasUtil *pitch_4x_alias_util,
						 gboolean note_256th_mode);

guint ags_pitch_4x_alias_util_get_offset_256th(AgsPitch4xAliasUtil *pitch_4x_alias_util);
void ags_pitch_4x_alias_util_set_offset_256th(AgsPitch4xAliasUtil *pitch_4x_alias_util,
					      guint offset_256th);

void ags_pitch_4x_alias_util_pitch_s8(AgsPitch4xAliasUtil *pitch_4x_alias_util);
void ags_pitch_4x_alias_util_pitch_s16(AgsPitch4xAliasUtil *pitch_4x_alias_util);
void ags_pitch_4x_alias_util_pitch_s24(AgsPitch4xAliasUtil *pitch_4x_alias_util);
void ags_pitch_4x_alias_util_pitch_s32(AgsPitch4xAliasUtil *pitch_4x_alias_util);
void ags_pitch_4x_alias_util_pitch_s64(AgsPitch4xAliasUtil *pitch_4x_alias_util);
void ags_pitch_4x_alias_util_pitch_float(AgsPitch4xAliasUtil *pitch_4x_alias_util);
void ags_pitch_4x_alias_util_pitch_double(AgsPitch4xAliasUtil *pitch_4x_alias_util);
void ags_pitch_4x_alias_util_pitch_complex(AgsPitch4xAliasUtil *pitch_4x_alias_util);
void ags_pitch_4x_alias_util_pitch(AgsPitch4xAliasUtil *pitch_4x_alias_util);

G_END_DECLS

#endif /*__AGS_PITCH_4X_ALIAS_UTIL_H__*/
