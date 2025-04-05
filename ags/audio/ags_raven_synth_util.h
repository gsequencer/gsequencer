/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#ifndef __AGS_RAVEN_SYNTH_UTIL_H__
#define __AGS_RAVEN_SYNTH_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_synth_enums.h>

G_BEGIN_DECLS

#define AGS_TYPE_RAVEN_SYNTH_UTIL         (ags_raven_synth_util_get_type())

#define AGS_RAVEN_SYNTH_UTIL_DEFAULT_FREQUENCY (440.0)
#define AGS_RAVEN_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY (12.0)
#define AGS_RAVEN_SYNTH_UTIL_DEFAULT_LFO_DEPTH (0.0)
#define AGS_RAVEN_SYNTH_UTIL_DEFAULT_TUNING (0.0)

#define AGS_RAVEN_SYNTH_UTIL_SEQ_COUNT (16)
#define AGS_RAVEN_SYNTH_UTIL_SYNC_COUNT (4)

#define AGS_RAVEN_SYNTH_UTIL_INITIALIZER ((AgsRavenSynthUtil) {		\
      .source = NULL,							\
      .source_stride = 1,						\
      .buffer_length = 0,						\
      .format = AGS_SOUNDCARD_DEFAULT_FORMAT,				\
      .samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE,			\
      .synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_SIN,		\
      .frequency = AGS_RAVEN_SYNTH_UTIL_DEFAULT_FREQUENCY,		\
      .phase = 0.0,							\
      .volume = 1.0,							\
      .seq_tuning = {0,},						\
      .seq_tuning_pingpong = FALSE,					\
      .seq_tuning_lfo_frequency = 6.0,					\
      .seq_volume = {0,},						\
      .seq_volume_pingpong = FALSE,					\
      .seq_volume_lfo_frequency = 6.0,					\
      .lfo_oscillator_mode = AGS_SYNTH_OSCILLATOR_SIN,			\
      .lfo_frequency = AGS_RAVEN_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY,	\
      .lfo_depth = AGS_RAVEN_SYNTH_UTIL_DEFAULT_LFO_DEPTH,		\
      .tuning = AGS_RAVEN_SYNTH_UTIL_DEFAULT_TUNING,			\
      .sync_enabled = FALSE,						\
      .sync_relative_attack_factor = {0,},				\
      .sync_phase = {0,},						\
      .sync_attack = {0,},						\
      .vibrato_enabled = FALSE,						\
      .vibrato_gain = 1.0,						\
      .vibrato_lfo_depth = 1.0,						\
      .vibrato_lfo_freq = 8.172,					\
      .vibrato_tuning = 0.0,						\
      .frame_count = (AGS_SOUNDCARD_DEFAULT_SAMPLERATE / 6.0),		\
      .offset = 0,							\
      .note_256th_mode = TRUE,						\
      .offset_256th = 0 })

typedef struct _AgsRavenSynthUtil AgsRavenSynthUtil;

struct _AgsRavenSynthUtil
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

  gdouble seq_tuning[AGS_RAVEN_SYNTH_UTIL_SEQ_COUNT];

  gboolean seq_tuning_pingpong;
  gdouble seq_tuning_lfo_frequency;
  
  gdouble seq_volume[AGS_RAVEN_SYNTH_UTIL_SEQ_COUNT];

  gboolean seq_volume_pingpong;
  gdouble seq_volume_lfo_frequency;
  
  guint lfo_oscillator_mode;

  gdouble lfo_frequency;
  gdouble lfo_depth;
  gdouble tuning;

  gboolean sync_enabled;
  
  gdouble sync_relative_attack_factor[AGS_RAVEN_SYNTH_UTIL_SYNC_COUNT];
  gdouble sync_attack[AGS_RAVEN_SYNTH_UTIL_SYNC_COUNT];
  gdouble sync_phase[AGS_RAVEN_SYNTH_UTIL_SYNC_COUNT];
  
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

GType ags_raven_synth_util_get_type(void);

AgsRavenSynthUtil* ags_raven_synth_util_alloc();

gpointer ags_raven_synth_util_copy(AgsRavenSynthUtil *ptr);
void ags_raven_synth_util_free(AgsRavenSynthUtil *ptr);

gpointer ags_raven_synth_util_get_source(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_set_source(AgsRavenSynthUtil *raven_synth_util,
				     gpointer source);

guint ags_raven_synth_util_get_source_stride(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_set_source_stride(AgsRavenSynthUtil *raven_synth_util,
					    guint source_stride);

guint ags_raven_synth_util_get_buffer_length(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_set_buffer_length(AgsRavenSynthUtil *raven_synth_util,
					    guint buffer_length);

AgsSoundcardFormat ags_raven_synth_util_get_format(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_set_format(AgsRavenSynthUtil *raven_synth_util,
				     AgsSoundcardFormat format);

guint ags_raven_synth_util_get_samplerate(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_set_samplerate(AgsRavenSynthUtil *raven_synth_util,
					 guint samplerate);

guint ags_raven_synth_util_get_synth_oscillator_mode(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_set_synth_oscillator_mode(AgsRavenSynthUtil *raven_synth_util,
						    guint synth_oscillator_mode);

gdouble ags_raven_synth_util_get_frequency(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_set_frequency(AgsRavenSynthUtil *raven_synth_util,
					gdouble frequency);

gdouble ags_raven_synth_util_get_phase(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_set_phase(AgsRavenSynthUtil *raven_synth_util,
				    gdouble phase);

gdouble ags_raven_synth_util_get_volume(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_set_volume(AgsRavenSynthUtil *raven_synth_util,
				     gdouble volume);

gdouble ags_raven_synth_util_get_seq_tuning(AgsRavenSynthUtil *raven_synth_util,
					    gint position);
void ags_raven_synth_util_set_seq_tuning(AgsRavenSynthUtil *raven_synth_util,
					 gint position,
					 gdouble seq_tuning);

gboolean ags_raven_synth_util_get_seq_tuning_pingpong(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_set_seq_tuning_pingpong(AgsRavenSynthUtil *raven_synth_util,
						  gboolean seq_tuning_pingpong);

gdouble ags_raven_synth_util_get_seq_tuning_lfo_frequency(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_set_seq_tuning_lfo_frequency(AgsRavenSynthUtil *raven_synth_util,
						       gdouble seq_tuning_lfo_frequency);

gdouble ags_raven_synth_util_get_seq_volume(AgsRavenSynthUtil *raven_synth_util,
					    gint position);
void ags_raven_synth_util_set_seq_volume(AgsRavenSynthUtil *raven_synth_util,
					 gint position,
					 gdouble seq_volume);

gboolean ags_raven_synth_util_get_seq_volume_pingpong(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_set_seq_volume_pingpong(AgsRavenSynthUtil *raven_synth_util,
						  gboolean seq_volume_pingpong);

gdouble ags_raven_synth_util_get_seq_volume_lfo_frequency(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_set_seq_volume_lfo_frequency(AgsRavenSynthUtil *raven_synth_util,
						       gdouble seq_volume_lfo_frequency);

guint ags_raven_synth_util_get_lfo_oscillator_mode(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_set_lfo_oscillator_mode(AgsRavenSynthUtil *raven_synth_util,
						  guint lfo_oscillator_mode);

gdouble ags_raven_synth_util_get_lfo_frequency(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_set_lfo_frequency(AgsRavenSynthUtil *raven_synth_util,
					    gdouble lfo_frequency);

gdouble ags_raven_synth_util_get_lfo_depth(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_set_lfo_depth(AgsRavenSynthUtil *raven_synth_util,
					gdouble lfo_depth);

gdouble ags_raven_synth_util_get_tuning(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_set_tuning(AgsRavenSynthUtil *raven_synth_util,
				     gdouble tuning);

gboolean ags_raven_synth_util_get_sync_enabled(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_set_sync_enabled(AgsRavenSynthUtil *raven_synth_util,
					   gboolean sync_enabled);

gdouble ags_raven_synth_util_get_sync_relative_attack_factor(AgsRavenSynthUtil *raven_synth_util,
							     gint position);
void ags_raven_synth_util_set_sync_relative_attack_factor(AgsRavenSynthUtil *raven_synth_util,
							  gint position,
							  gdouble sync_relative_attack_factor);

gdouble ags_raven_synth_util_get_sync_attack(AgsRavenSynthUtil *raven_synth_util,
					     gint position);
void ags_raven_synth_util_set_sync_attack(AgsRavenSynthUtil *raven_synth_util,
					  gint position,
					  gdouble sync_attack);

gdouble ags_raven_synth_util_get_sync_phase(AgsRavenSynthUtil *raven_synth_util,
					    gint position);
void ags_raven_synth_util_set_sync_phase(AgsRavenSynthUtil *raven_synth_util,
					 gint position,
					 gdouble sync_phase);

gboolean ags_raven_synth_util_get_vibrato_enabled(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_set_vibrato_enabled(AgsRavenSynthUtil *raven_synth_util,
					      gboolean vibrato_enabled);

gdouble ags_raven_synth_util_get_vibrato_gain(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_set_vibrato_gain(AgsRavenSynthUtil *raven_synth_util,
					   gdouble vibrato_gain);

gdouble ags_raven_synth_util_get_vibrato_lfo_depth(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_set_vibrato_lfo_depth(AgsRavenSynthUtil *raven_synth_util,
						gdouble vibrato_lfo_depth);

gdouble ags_raven_synth_util_get_vibrato_lfo_freq(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_set_vibrato_lfo_freq(AgsRavenSynthUtil *raven_synth_util,
					       gdouble vibrato_lfo_freq);

gdouble ags_raven_synth_util_get_vibrato_tuning(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_set_vibrato_tuning(AgsRavenSynthUtil *raven_synth_util,
					     gdouble vibrato_tuning);

guint ags_raven_synth_util_get_frame_count(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_set_frame_count(AgsRavenSynthUtil *raven_synth_util,
					  guint frame_count);

guint ags_raven_synth_util_get_offset(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_set_offset(AgsRavenSynthUtil *raven_synth_util,
				     guint offset);

gboolean ags_raven_synth_util_get_note_256th_mode(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_set_note_256th_mode(AgsRavenSynthUtil *raven_synth_util,
					      gboolean note_256th_mode);

guint ags_raven_synth_util_get_offset_256th(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_set_offset_256th(AgsRavenSynthUtil *raven_synth_util,
					   guint offset_256th);

/*  */
gdouble ags_raven_synth_util_get_tuning_by_offset(AgsRavenSynthUtil *raven_synth_util,
						  guint offset);

gdouble ags_raven_synth_util_get_volume_by_offset(AgsRavenSynthUtil *raven_synth_util,
						  guint offset);

/* seq sin oscillator */
void ags_raven_synth_util_compute_sin_s8(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_sin_s16(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_sin_s24(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_sin_s32(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_sin_s64(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_sin_float(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_sin_double(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_sin_complex(AgsRavenSynthUtil *raven_synth_util);

void ags_raven_synth_util_compute_sin(AgsRavenSynthUtil *raven_synth_util);

/* seq sawtooth oscillator */
void ags_raven_synth_util_compute_sawtooth_s8(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_sawtooth_s16(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_sawtooth_s24(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_sawtooth_s32(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_sawtooth_s64(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_sawtooth_float(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_sawtooth_double(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_sawtooth_complex(AgsRavenSynthUtil *raven_synth_util);

void ags_raven_synth_util_compute_sawtooth(AgsRavenSynthUtil *raven_synth_util);

/* seq triangle oscillator */
void ags_raven_synth_util_compute_triangle_s8(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_triangle_s16(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_triangle_s24(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_triangle_s32(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_triangle_s64(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_triangle_float(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_triangle_double(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_triangle_complex(AgsRavenSynthUtil *raven_synth_util);

void ags_raven_synth_util_compute_triangle(AgsRavenSynthUtil *raven_synth_util);

/* seq square oscillator */
void ags_raven_synth_util_compute_square_s8(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_square_s16(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_square_s24(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_square_s32(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_square_s64(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_square_float(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_square_double(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_square_complex(AgsRavenSynthUtil *raven_synth_util);

void ags_raven_synth_util_compute_square(AgsRavenSynthUtil *raven_synth_util);

/* seq impulse oscillator */
void ags_raven_synth_util_compute_impulse_s8(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_impulse_s16(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_impulse_s24(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_impulse_s32(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_impulse_s64(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_impulse_float(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_impulse_double(AgsRavenSynthUtil *raven_synth_util);
void ags_raven_synth_util_compute_impulse_complex(AgsRavenSynthUtil *raven_synth_util);

void ags_raven_synth_util_compute_impulse(AgsRavenSynthUtil *raven_synth_util);

G_END_DECLS

#endif /*__AGS_RAVEN_SYNTH_UTIL_H__*/
