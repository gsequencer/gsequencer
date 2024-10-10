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

#ifndef __AGS_TREMOLO_UTIL_H__
#define __AGS_TREMOLO_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio_buffer_util.h>

G_BEGIN_DECLS

#define AGS_TYPE_TREMOLO_UTIL         (ags_tremolo_util_get_type())
#define AGS_TREMOLO_UTIL(ptr) ((AgsTremoloUtil *)(ptr))

#define AGS_TREMOLO_UTIL_INITIALIZER ((AgsTremoloUtil) {	\
      .source = NULL,						\
      .source_stride = 1,					\
      .destination = NULL,					\
      .destination_stride = 1,					\
      .buffer_length = 0,					\
      .format = AGS_SOUNDCARD_DEFAULT_FORMAT,			\
      .samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE,		\
      .tremolo_gain = 1.0,					\
      .tremolo_lfo_depth = 0.0,					\
      .tremolo_lfo_freq = 6.0,					\
      .tremolo_tuning = 0.0,					\
      .frame_count = (AGS_SOUNDCARD_DEFAULT_SAMPLERATE / 6.0),	\
      .offset = 0,						\
      .note_256th_mode = TRUE,					\
      .offset_256th = 0 })

typedef struct _AgsTremoloUtil AgsTremoloUtil;

struct _AgsTremoloUtil
{
  gpointer destination;
  guint destination_stride;

  gpointer source;
  guint source_stride;
  
  guint buffer_length;
  AgsSoundcardFormat format;
  guint samplerate;
  
  gdouble tremolo_gain;
  gdouble tremolo_lfo_depth;
  gdouble tremolo_lfo_freq;
  gdouble tremolo_tuning;

  guint frame_count;
  guint offset;

  gboolean note_256th_mode;

  guint offset_256th;
};

GType ags_tremolo_util_get_type(void);

AgsTremoloUtil* ags_tremolo_util_alloc();

gpointer ags_tremolo_util_copy(AgsTremoloUtil *ptr);
void ags_tremolo_util_free(AgsTremoloUtil *ptr);

/* getter/setter */
gpointer ags_tremolo_util_get_destination(AgsTremoloUtil *tremolo_util);
void ags_tremolo_util_set_destination(AgsTremoloUtil *tremolo_util,
				     gpointer destination);

guint ags_tremolo_util_get_destination_stride(AgsTremoloUtil *tremolo_util);
void ags_tremolo_util_set_destination_stride(AgsTremoloUtil *tremolo_util,
					    guint destination_stride);

gpointer ags_tremolo_util_get_source(AgsTremoloUtil *tremolo_util);
void ags_tremolo_util_set_source(AgsTremoloUtil *tremolo_util,
				gpointer source);

guint ags_tremolo_util_get_source_stride(AgsTremoloUtil *tremolo_util);
void ags_tremolo_util_set_source_stride(AgsTremoloUtil *tremolo_util,
				       guint source_stride);

guint ags_tremolo_util_get_buffer_length(AgsTremoloUtil *tremolo_util);
void ags_tremolo_util_set_buffer_length(AgsTremoloUtil *tremolo_util,
				       guint buffer_length);

AgsSoundcardFormat ags_tremolo_util_get_format(AgsTremoloUtil *tremolo_util);
void ags_tremolo_util_set_format(AgsTremoloUtil *tremolo_util,
				AgsSoundcardFormat format);

guint ags_tremolo_util_get_samplerate(AgsTremoloUtil *tremolo_util);
void ags_tremolo_util_set_samplerate(AgsTremoloUtil *tremolo_util,
				     guint samplerate);

gdouble ags_tremolo_util_get_tremolo_gain(AgsTremoloUtil *tremolo_util);
void ags_tremolo_util_set_tremolo_gain(AgsTremoloUtil *tremolo_util,
				       gdouble tremolo_gain);

gdouble ags_tremolo_util_get_tremolo_lfo_depth(AgsTremoloUtil *tremolo_util);
void ags_tremolo_util_set_tremolo_lfo_depth(AgsTremoloUtil *tremolo_util,
					    gdouble tremolo_lfo_depth);

gdouble ags_tremolo_util_get_tremolo_lfo_freq(AgsTremoloUtil *tremolo_util);
void ags_tremolo_util_set_tremolo_lfo_freq(AgsTremoloUtil *tremolo_util,
					   gdouble tremolo_lfo_freq);

gdouble ags_tremolo_util_get_tremolo_tuning(AgsTremoloUtil *tremolo_util);
void ags_tremolo_util_set_tremolo_tuning(AgsTremoloUtil *tremolo_util,
					 gdouble tremolo_tuning);

guint ags_tremolo_util_get_frame_count(AgsTremoloUtil *tremolo_util);
void ags_tremolo_util_set_frame_count(AgsTremoloUtil *tremolo_util,
				      guint frame_count);

guint ags_tremolo_util_get_offset(AgsTremoloUtil *tremolo_util);
void ags_tremolo_util_set_offset(AgsTremoloUtil *tremolo_util,
				 guint offset);

gboolean ags_tremolo_util_get_note_256th_mode(AgsTremoloUtil *tremolo_util);
void ags_tremolo_util_set_note_256th_mode(AgsTremoloUtil *tremolo_util,
					  gboolean note_256th_mode);

guint ags_tremolo_util_get_offset_256th(AgsTremoloUtil *tremolo_util);
void ags_tremolo_util_set_offset_256th(AgsTremoloUtil *tremolo_util,
				       guint offset_256th);

/* compute */
void ags_tremolo_util_compute_s8(AgsTremoloUtil *tremolo_util);
void ags_tremolo_util_compute_s16(AgsTremoloUtil *tremolo_util);
void ags_tremolo_util_compute_s24(AgsTremoloUtil *tremolo_util);
void ags_tremolo_util_compute_s32(AgsTremoloUtil *tremolo_util);
void ags_tremolo_util_compute_s64(AgsTremoloUtil *tremolo_util);
void ags_tremolo_util_compute_float(AgsTremoloUtil *tremolo_util);
void ags_tremolo_util_compute_double(AgsTremoloUtil *tremolo_util);
void ags_tremolo_util_compute_complex(AgsTremoloUtil *tremolo_util);

void ags_tremolo_util_compute(AgsTremoloUtil *tremolo_util);

G_END_DECLS

#endif /*__AGS_TREMOLO_UTIL_H__*/
