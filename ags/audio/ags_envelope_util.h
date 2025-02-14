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

#ifndef __AGS_ENVELOPE_UTIL_H__
#define __AGS_ENVELOPE_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_ENVELOPE_UTIL         (ags_envelope_util_get_type())
#define AGS_ENVELOPE_UTIL(ptr) ((AgsEnvelopeUtil *)(ptr))

#define AGS_ENVELOPE_UTIL_INITIALIZER ((AgsEnvelopeUtil) {	\
      .source = NULL,						\
      .source_stride = 1,					\
      .destination = NULL,					\
      .destination_stride = 1,					\
      .buffer_length = 0,					\
      .format = AGS_SOUNDCARD_DEFAULT_FORMAT,			\
      .samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE,		\
      .volume = 1.0,						\
      .amount = 0.0,						\
      .lfo_enabled = FALSE,					\
      .lfo_freq = 6.0,						\
      .frame_count = (AGS_SOUNDCARD_DEFAULT_SAMPLERATE / 6.0),	\
      .offset = 0 })

typedef struct _AgsEnvelopeUtil AgsEnvelopeUtil;

struct _AgsEnvelopeUtil
{
  gpointer destination;
  guint destination_stride;

  gpointer source;
  guint source_stride;
  
  guint buffer_length;
  AgsSoundcardFormat format;
  guint samplerate;
  
  gdouble volume;
  gdouble amount;

  gboolean lfo_enabled;
  gdouble lfo_freq;

  guint frame_count;
  guint offset;
};

GType ags_envelope_util_get_type(void);

AgsEnvelopeUtil* ags_envelope_util_alloc();

gpointer ags_envelope_util_copy(AgsEnvelopeUtil *ptr);
void ags_envelope_util_free(AgsEnvelopeUtil *ptr);

gpointer ags_envelope_util_get_destination(AgsEnvelopeUtil *envelope_util);
void ags_envelope_util_set_destination(AgsEnvelopeUtil *envelope_util,
				       gpointer destination);

guint ags_envelope_util_get_destination_stride(AgsEnvelopeUtil *envelope_util);
void ags_envelope_util_set_destination_stride(AgsEnvelopeUtil *envelope_util,
					      guint destination_stride);

gpointer ags_envelope_util_get_source(AgsEnvelopeUtil *envelope_util);
void ags_envelope_util_set_source(AgsEnvelopeUtil *envelope_util,
				  gpointer source);

guint ags_envelope_util_get_source_stride(AgsEnvelopeUtil *envelope_util);
void ags_envelope_util_set_source_stride(AgsEnvelopeUtil *envelope_util,
					 guint source_stride);

guint ags_envelope_util_get_buffer_length(AgsEnvelopeUtil *envelope_util);
void ags_envelope_util_set_buffer_length(AgsEnvelopeUtil *envelope_util,
					 guint buffer_length);

guint ags_envelope_util_get_format(AgsEnvelopeUtil *envelope_util);
void ags_envelope_util_set_format(AgsEnvelopeUtil *envelope_util,
				  guint format);

gdouble ags_envelope_util_get_volume(AgsEnvelopeUtil *envelope_util);
void ags_envelope_util_set_volume(AgsEnvelopeUtil *envelope_util,
				  gdouble volume);

gdouble ags_envelope_util_get_amount(AgsEnvelopeUtil *envelope_util);
void ags_envelope_util_set_amount(AgsEnvelopeUtil *envelope_util,
				  gdouble amount);

gboolean ags_envelope_util_get_lfo_enabled(AgsEnvelopeUtil *envelope_util);
void ags_envelope_util_set_lfo_enabled(AgsEnvelopeUtil *envelope_util,
				       gboolean lfo_enabled);

gdouble ags_envelope_util_get_lfo_freq(AgsEnvelopeUtil *envelope_util);
void ags_envelope_util_set_lfo_freq(AgsEnvelopeUtil *envelope_util,
				    gdouble lfo_freq);

guint ags_envelope_util_get_frame_count(AgsEnvelopeUtil *envelope_util);
void ags_envelope_util_set_frame_count(AgsEnvelopeUtil *envelope_util,
				       guint frame_count);

guint ags_envelope_util_get_offset(AgsEnvelopeUtil *envelope_util);
void ags_envelope_util_set_offset(AgsEnvelopeUtil *envelope_util,
				  guint offset);

void ags_envelope_util_compute_s8(AgsEnvelopeUtil *envelope_util);
void ags_envelope_util_compute_s16(AgsEnvelopeUtil *envelope_util);
void ags_envelope_util_compute_s24(AgsEnvelopeUtil *envelope_util);
void ags_envelope_util_compute_s32(AgsEnvelopeUtil *envelope_util);
void ags_envelope_util_compute_s64(AgsEnvelopeUtil *envelope_util);
void ags_envelope_util_compute_float(AgsEnvelopeUtil *envelope_util);
void ags_envelope_util_compute_double(AgsEnvelopeUtil *envelope_util);
void ags_envelope_util_compute_complex(AgsEnvelopeUtil *envelope_util);

void ags_envelope_util_compute(AgsEnvelopeUtil *envelope_util);

G_END_DECLS

#endif /*__AGS_ENVELOPE_UTIL_H__*/
