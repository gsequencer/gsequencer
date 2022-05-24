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

#ifndef __AGS_CHORUS_UTIL_H__
#define __AGS_CHORUS_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <math.h>
#include <complex.h>

#include <ags/audio/ags_common_pitch_util.h>
#include <ags/audio/ags_synth_enums.h>

G_BEGIN_DECLS

#define AGS_TYPE_CHORUS_UTIL         (ags_chorus_util_get_type())
#define AGS_CHORUS_UTIL(ptr) ((AgsChorusUtil *)(ptr))

#define AGS_CHORUS_UTIL_DEFAULT_PITCH_MIX_BUFFER_SIZE (65536)

#define AGS_CHORUS_UTIL_DEFAULT_HISTORY_BUFFER_LENGTH (256)

#define AGS_CHORUS_UTIL_DEFAULT_LFO_OSCILLATOR (AGS_SYNTH_OSCILLATOR_SIN)
#define AGS_CHORUS_UTIL_DEFAULT_LFO_FREQUENCY (10.0)

#define AGS_CHORUS_UTIL_DEFAULT_DEPTH (0.0)
#define AGS_CHORUS_UTIL_DEFAULT_MIX (1.0)
#define AGS_CHORUS_UTIL_DEFAULT_DELAY (0.0)

typedef struct _AgsChorusUtil AgsChorusUtil;

struct _AgsChorusUtil
{
  gpointer source;
  guint source_stride;

  gpointer destination;
  guint destination_stride;

  gpointer pitch_mix_buffer;
  guint pitch_mix_max_buffer_length;
  
  gpointer pitch_mix_buffer_history;
  guint history_buffer_length;
  
  guint buffer_length;
  guint format;
  guint samplerate;

  guint64 offset;
  
  gdouble base_key;
  
  gdouble input_volume;
  gdouble output_volume;
  
  guint lfo_oscillator;
  gdouble lfo_frequency;
  
  gdouble depth;
  gdouble mix;
  gdouble delay;

  GType pitch_type;  
  gpointer pitch_util;
};

GType ags_chorus_util_get_type(void);

AgsChorusUtil* ags_chorus_util_alloc();

gpointer ags_chorus_util_copy(AgsChorusUtil *ptr);
void ags_chorus_util_free(AgsChorusUtil *ptr);

gpointer ags_chorus_util_get_source(AgsChorusUtil *chorus_util);
void ags_chorus_util_set_source(AgsChorusUtil *chorus_util,
				gpointer source);

guint ags_chorus_util_get_source_stride(AgsChorusUtil *chorus_util);
void ags_chorus_util_set_source_stride(AgsChorusUtil *chorus_util,
				       guint source_stride);

gpointer ags_chorus_util_get_destination(AgsChorusUtil *chorus_util);
void ags_chorus_util_set_destination(AgsChorusUtil *chorus_util,
				     gpointer destination);

guint ags_chorus_util_get_destination_stride(AgsChorusUtil *chorus_util);
void ags_chorus_util_set_destination_stride(AgsChorusUtil *chorus_util,
					    guint destination_stride);

guint ags_chorus_util_get_buffer_length(AgsChorusUtil *chorus_util);
void ags_chorus_util_set_buffer_length(AgsChorusUtil *chorus_util,
				       guint buffer_length);

guint ags_chorus_util_get_format(AgsChorusUtil *chorus_util);
void ags_chorus_util_set_format(AgsChorusUtil *chorus_util,
				guint format);

guint ags_chorus_util_get_samplerate(AgsChorusUtil *chorus_util);
void ags_chorus_util_set_samplerate(AgsChorusUtil *chorus_util,
				    guint samplerate);

guint64 ags_chorus_util_get_offset(AgsChorusUtil *chorus_util);
void ags_chorus_util_set_offset(AgsChorusUtil *chorus_util,
				guint64 offset);

gdouble ags_chorus_util_get_base_key(AgsChorusUtil *chorus_util);
void ags_chorus_util_set_base_key(AgsChorusUtil *chorus_util,
				  gdouble base_key);

gdouble ags_chorus_util_get_input_volume(AgsChorusUtil *chorus_util);
void ags_chorus_util_set_input_volume(AgsChorusUtil *chorus_util,
				      gdouble input_volume);

gdouble ags_chorus_util_get_output_volume(AgsChorusUtil *chorus_util);
void ags_chorus_util_set_output_volume(AgsChorusUtil *chorus_util,
				       gdouble output_volume);

guint ags_chorus_util_get_lfo_oscillator(AgsChorusUtil *chorus_util);
void ags_chorus_util_set_lfo_oscillator(AgsChorusUtil *chorus_util,
					guint lfo_oscillator);

gdouble ags_chorus_util_get_lfo_frequency(AgsChorusUtil *chorus_util);
void ags_chorus_util_set_lfo_frequency(AgsChorusUtil *chorus_util,
				       gdouble lfo_frequency);

gdouble ags_chorus_util_get_depth(AgsChorusUtil *chorus_util);
void ags_chorus_util_set_depth(AgsChorusUtil *chorus_util,
			       gdouble depth);

gdouble ags_chorus_util_get_mix(AgsChorusUtil *chorus_util);
void ags_chorus_util_set_mix(AgsChorusUtil *chorus_util,
			     gdouble mix);

gdouble ags_chorus_util_get_delay(AgsChorusUtil *chorus_util);
void ags_chorus_util_set_delay(AgsChorusUtil *chorus_util,
			       gdouble delay);

GType ags_chorus_util_get_pitch_type(AgsChorusUtil *chorus_util);
void ags_chorus_util_set_pitch_type(AgsChorusUtil *chorus_util,
				    GType pitch_type);

gpointer ags_chorus_util_get_pitch_util(AgsChorusUtil *chorus_util);
void ags_chorus_util_set_pitch_util(AgsChorusUtil *chorus_util,
				    gpointer pitch_util);

void ags_chorus_util_compute_s8(AgsChorusUtil *chorus_util);
void ags_chorus_util_compute_s16(AgsChorusUtil *chorus_util);
void ags_chorus_util_compute_s24(AgsChorusUtil *chorus_util);
void ags_chorus_util_compute_s32(AgsChorusUtil *chorus_util);
void ags_chorus_util_compute_s64(AgsChorusUtil *chorus_util);
void ags_chorus_util_compute_float(AgsChorusUtil *chorus_util);
void ags_chorus_util_compute_double(AgsChorusUtil *chorus_util);
void ags_chorus_util_compute_complex(AgsChorusUtil *chorus_util);
void ags_chorus_util_compute(AgsChorusUtil *chorus_util);

G_END_DECLS

#endif /*__AGS_CHORUS_UTIL_H__*/
