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

#ifndef __AGS_SEQ_SYNTH_UTIL_H__
#define __AGS_SEQ_SYNTH_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_synth_enums.h>

G_BEGIN_DECLS

#define AGS_TYPE_SEQ_SYNTH_UTIL         (ags_seq_synth_util_get_type())

#define AGS_SEQ_SYNTH_UTIL_DEFAULT_FREQUENCY (440.0)
#define AGS_SEQ_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY (12.0)
#define AGS_SEQ_SYNTH_UTIL_DEFAULT_LFO_DEPTH (0.0)
#define AGS_SEQ_SYNTH_UTIL_DEFAULT_TUNING (0.0)

#define AGS_SEQ_SYNTH_UTIL_INITIALIZER ((AgsSeqSynthUtil) {		\
      .source = NULL,							\
      .source_stride = 1,						\
      .buffer_length = 0,						\
      .format = AGS_SOUNDCARD_DEFAULT_FORMAT,				\
      .samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE,			\
      .synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_SIN,		\
      .frequency = AGS_SEQ_SYNTH_UTIL_DEFAULT_FREQUENCY,		\
      .phase = 0.0,							\
      .volume = 1.0,							\
      .seq_tuning_0 = 0.0,						\
      .seq_tuning_1 = 0.0,						\
      .seq_tuning_2 = 0.0,						\
      .seq_tuning_3 = 0.0,						\
      .seq_tuning_4 = 0.0,						\
      .seq_tuning_5 = 0.0,						\
      .seq_tuning_6 = 0.0,						\
      .seq_tuning_7 = 0.0,						\
      .seq_tuning_pingpong = FALSE,					\
      .seq_tuning_lfo_frequency = 6.0,					\
      .seq_volume_0 = 0.0,						\
      .seq_volume_1 = 0.0,						\
      .seq_volume_2 = 0.0,						\
      .seq_volume_3 = 0.0,						\
      .seq_volume_4 = 0.0,						\
      .seq_volume_5 = 0.0,						\
      .seq_volume_6 = 0.0,						\
      .seq_volume_7 = 0.0,						\
      .seq_volume_pingpong = FALSE,					\
      .seq_volume_lfo_frequency = 6.0,					\
      .lfo_oscillator_mode = AGS_SYNTH_OSCILLATOR_SIN,			\
      .lfo_frequency = AGS_SEQ_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY,	\
      .lfo_depth = AGS_SEQ_SYNTH_UTIL_DEFAULT_LFO_DEPTH,		\
      .tuning = AGS_SEQ_SYNTH_UTIL_DEFAULT_TUNING,			\
      .vibrato_enabled = FALSE,						\
      .vibrato_gain = 1.0,						\
      .vibrato_lfo_depth = 1.0,						\
      .vibrato_lfo_freq = 8.172,					\
      .vibrato_tuning = 0.0,						\
      .frame_count = (AGS_SOUNDCARD_DEFAULT_SAMPLERATE / 6.0),		\
      .offset = 0,							\
      .note_256th_mode = TRUE,						\
      .offset_256th = 0 })

typedef struct _AgsSeqSynthUtil AgsSeqSynthUtil;

struct _AgsSeqSynthUtil
{
  gpointer source;
  guint source_stride;
  
  guint buffer_length;
  AgsSoundcardFormat format;
  guint samplerate;

  guint synth_oscillator_mode;
  
  gdouble frequency;
  gdouble phase;
  gdouble volume;

  gdouble seq_tuning_0;
  gdouble seq_tuning_1;
  gdouble seq_tuning_2;
  gdouble seq_tuning_3;
  gdouble seq_tuning_4;
  gdouble seq_tuning_5;
  gdouble seq_tuning_6;
  gdouble seq_tuning_7;

  gboolean seq_tuning_pingpong;
  gdouble seq_tuning_lfo_frequency;
  
  gdouble seq_volume_0;
  gdouble seq_volume_1;
  gdouble seq_volume_2;
  gdouble seq_volume_3;
  gdouble seq_volume_4;
  gdouble seq_volume_5;
  gdouble seq_volume_6;
  gdouble seq_volume_7;

  gboolean seq_volume_pingpong;
  gdouble seq_volume_lfo_frequency;
  
  guint lfo_oscillator_mode;

  gdouble lfo_frequency;
  gdouble lfo_depth;
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

GType ags_seq_synth_util_get_type(void);

AgsSeqSynthUtil* ags_seq_synth_util_alloc();

gpointer ags_seq_synth_util_copy(AgsSeqSynthUtil *ptr);
void ags_seq_synth_util_free(AgsSeqSynthUtil *ptr);

gpointer ags_seq_synth_util_get_source(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_source(AgsSeqSynthUtil *seq_synth_util,
				  gpointer source);

guint ags_seq_synth_util_get_source_stride(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_source_stride(AgsSeqSynthUtil *seq_synth_util,
					 guint source_stride);

guint ags_seq_synth_util_get_buffer_length(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_buffer_length(AgsSeqSynthUtil *seq_synth_util,
					 guint buffer_length);

AgsSoundcardFormat ags_seq_synth_util_get_format(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_format(AgsSeqSynthUtil *seq_synth_util,
				  AgsSoundcardFormat format);

guint ags_seq_synth_util_get_samplerate(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_samplerate(AgsSeqSynthUtil *seq_synth_util,
				      guint samplerate);

guint ags_seq_synth_util_get_synth_oscillator_mode(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_synth_oscillator_mode(AgsSeqSynthUtil *seq_synth_util,
						 guint synth_oscillator_mode);

gdouble ags_seq_synth_util_get_frequency(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_frequency(AgsSeqSynthUtil *seq_synth_util,
				     gdouble frequency);

gdouble ags_seq_synth_util_get_phase(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_phase(AgsSeqSynthUtil *seq_synth_util,
				 gdouble phase);

gdouble ags_seq_synth_util_get_volume(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_volume(AgsSeqSynthUtil *seq_synth_util,
				  gdouble volume);

gdouble ags_seq_synth_util_get_seq_tuning_0(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_seq_tuning_0(AgsSeqSynthUtil *seq_synth_util,
					 gdouble seq_tuning_0);

gdouble ags_seq_synth_util_get_seq_tuning_1(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_seq_tuning_1(AgsSeqSynthUtil *seq_synth_util,
					 gdouble seq_tuning_1);

gdouble ags_seq_synth_util_get_seq_tuning_2(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_seq_tuning_2(AgsSeqSynthUtil *seq_synth_util,
					 gdouble seq_tuning_2);

gdouble ags_seq_synth_util_get_seq_tuning_3(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_seq_tuning_3(AgsSeqSynthUtil *seq_synth_util,
					 gdouble seq_tuning_3);

gdouble ags_seq_synth_util_get_seq_tuning_4(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_seq_tuning_4(AgsSeqSynthUtil *seq_synth_util,
					 gdouble seq_tuning_4);

gdouble ags_seq_synth_util_get_seq_tuning_5(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_seq_tuning_5(AgsSeqSynthUtil *seq_synth_util,
					 gdouble seq_tuning_5);

gdouble ags_seq_synth_util_get_seq_tuning_6(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_seq_tuning_6(AgsSeqSynthUtil *seq_synth_util,
					 gdouble seq_tuning_6);

gdouble ags_seq_synth_util_get_seq_tuning_7(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_seq_tuning_7(AgsSeqSynthUtil *seq_synth_util,
					 gdouble seq_tuning_7);

gboolean ags_seq_synth_util_get_seq_tuning_pingpong(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_seq_tuning_pingpong(AgsSeqSynthUtil *seq_synth_util,
						gboolean seq_tuning_pingpong);

gdouble ags_seq_synth_util_get_seq_tuning_lfo_frequency(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_seq_tuning_lfo_frequency(AgsSeqSynthUtil *seq_synth_util,
						     gdouble seq_tuning_lfo_frequency);

gdouble ags_seq_synth_util_get_seq_volume_0(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_seq_volume_0(AgsSeqSynthUtil *seq_synth_util,
					 gdouble seq_volume_0);

gdouble ags_seq_synth_util_get_seq_volume_1(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_seq_volume_1(AgsSeqSynthUtil *seq_synth_util,
					 gdouble seq_volume_1);

gdouble ags_seq_synth_util_get_seq_volume_2(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_seq_volume_2(AgsSeqSynthUtil *seq_synth_util,
					 gdouble seq_volume_2);

gdouble ags_seq_synth_util_get_seq_volume_3(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_seq_volume_3(AgsSeqSynthUtil *seq_synth_util,
					 gdouble seq_volume_3);

gdouble ags_seq_synth_util_get_seq_volume_4(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_seq_volume_4(AgsSeqSynthUtil *seq_synth_util,
					 gdouble seq_volume_4);

gdouble ags_seq_synth_util_get_seq_volume_5(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_seq_volume_5(AgsSeqSynthUtil *seq_synth_util,
					 gdouble seq_volume_5);

gdouble ags_seq_synth_util_get_seq_volume_6(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_seq_volume_6(AgsSeqSynthUtil *seq_synth_util,
					 gdouble seq_volume_6);

gdouble ags_seq_synth_util_get_seq_volume_7(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_seq_volume_7(AgsSeqSynthUtil *seq_synth_util,
					 gdouble seq_volume_7);

gboolean ags_seq_synth_util_get_seq_volume_pingpong(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_seq_volume_pingpong(AgsSeqSynthUtil *seq_synth_util,
						gboolean seq_volume_pingpong);

gdouble ags_seq_synth_util_get_seq_volume_lfo_frequency(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_seq_volume_lfo_frequency(AgsSeqSynthUtil *seq_synth_util,
						     gdouble seq_volume_lfo_frequency);

guint ags_seq_synth_util_get_lfo_oscillator_mode(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_lfo_oscillator_mode(AgsSeqSynthUtil *seq_synth_util,
					       guint lfo_oscillator_mode);

gdouble ags_seq_synth_util_get_lfo_frequency(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_lfo_frequency(AgsSeqSynthUtil *seq_synth_util,
					 gdouble lfo_frequency);

gdouble ags_seq_synth_util_get_lfo_depth(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_lfo_depth(AgsSeqSynthUtil *seq_synth_util,
				     gdouble lfo_depth);

gdouble ags_seq_synth_util_get_tuning(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_tuning(AgsSeqSynthUtil *seq_synth_util,
				  gdouble tuning);

gboolean ags_seq_synth_util_get_vibrato_enabled(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_vibrato_enabled(AgsSeqSynthUtil *seq_synth_util,
					     gboolean vibrato_enabled);

gdouble ags_seq_synth_util_get_vibrato_gain(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_vibrato_gain(AgsSeqSynthUtil *seq_synth_util,
					  gdouble vibrato_gain);

gdouble ags_seq_synth_util_get_vibrato_lfo_depth(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_vibrato_lfo_depth(AgsSeqSynthUtil *seq_synth_util,
					       gdouble vibrato_lfo_depth);

gdouble ags_seq_synth_util_get_vibrato_lfo_freq(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_vibrato_lfo_freq(AgsSeqSynthUtil *seq_synth_util,
					      gdouble vibrato_lfo_freq);

gdouble ags_seq_synth_util_get_vibrato_tuning(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_vibrato_tuning(AgsSeqSynthUtil *seq_synth_util,
					    gdouble vibrato_tuning);

guint ags_seq_synth_util_get_frame_count(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_frame_count(AgsSeqSynthUtil *seq_synth_util,
				       guint frame_count);

guint ags_seq_synth_util_get_offset(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_offset(AgsSeqSynthUtil *seq_synth_util,
				  guint offset);

gboolean ags_seq_synth_util_get_note_256th_mode(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_note_256th_mode(AgsSeqSynthUtil *seq_synth_util,
					   gboolean note_256th_mode);

guint ags_seq_synth_util_get_offset_256th(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_set_offset_256th(AgsSeqSynthUtil *seq_synth_util,
					guint offset_256th);

/*  */
gdouble ags_seq_synth_util_get_tuning_by_offset(AgsSeqSynthUtil *seq_synth_util,
						guint offset);

gdouble ags_seq_synth_util_get_volume_by_offset(AgsSeqSynthUtil *seq_synth_util,
						guint offset);

/* seq sin oscillator */
void ags_seq_synth_util_compute_sin_s8(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_sin_s16(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_sin_s24(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_sin_s32(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_sin_s64(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_sin_float(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_sin_double(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_sin_complex(AgsSeqSynthUtil *seq_synth_util);

void ags_seq_synth_util_compute_sin(AgsSeqSynthUtil *seq_synth_util);

/* seq sawtooth oscillator */
void ags_seq_synth_util_compute_sawtooth_s8(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_sawtooth_s16(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_sawtooth_s24(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_sawtooth_s32(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_sawtooth_s64(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_sawtooth_float(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_sawtooth_double(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_sawtooth_complex(AgsSeqSynthUtil *seq_synth_util);

void ags_seq_synth_util_compute_sawtooth(AgsSeqSynthUtil *seq_synth_util);

/* seq triangle oscillator */
void ags_seq_synth_util_compute_triangle_s8(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_triangle_s16(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_triangle_s24(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_triangle_s32(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_triangle_s64(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_triangle_float(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_triangle_double(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_triangle_complex(AgsSeqSynthUtil *seq_synth_util);

void ags_seq_synth_util_compute_triangle(AgsSeqSynthUtil *seq_synth_util);

/* seq square oscillator */
void ags_seq_synth_util_compute_square_s8(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_square_s16(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_square_s24(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_square_s32(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_square_s64(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_square_float(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_square_double(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_square_complex(AgsSeqSynthUtil *seq_synth_util);

void ags_seq_synth_util_compute_square(AgsSeqSynthUtil *seq_synth_util);

/* seq impulse oscillator */
void ags_seq_synth_util_compute_impulse_s8(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_impulse_s16(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_impulse_s24(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_impulse_s32(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_impulse_s64(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_impulse_float(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_impulse_double(AgsSeqSynthUtil *seq_synth_util);
void ags_seq_synth_util_compute_impulse_complex(AgsSeqSynthUtil *seq_synth_util);

void ags_seq_synth_util_compute_impulse(AgsSeqSynthUtil *seq_synth_util);

G_END_DECLS

#endif /*__AGS_SEQ_SYNTH_UTIL_H__*/
