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

#ifndef __AGS_MODULAR_SYNTH_UTIL_H__
#define __AGS_MODULAR_SYNTH_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_synth_enums.h>

G_BEGIN_DECLS

#define AGS_TYPE_MODULAR_SYNTH_UTIL         (ags_modular_synth_util_get_type())

#define AGS_MODULAR_SYNTH_UTIL_DEFAULT_FREQUENCY (440.0)
#define AGS_MODULAR_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY (12.0)
#define AGS_MODULAR_SYNTH_UTIL_DEFAULT_LFO_DEPTH (0.0)
#define AGS_MODULAR_SYNTH_UTIL_DEFAULT_VOLUME (1.0)
#define AGS_MODULAR_SYNTH_UTIL_DEFAULT_TUNING (0.0)

#define AGS_MODULAR_SYNTH_UTIL_INITIALIZER ((AgsModularSynthUtil) {		\
      .source = NULL,							\
      .source_stride = 1,						\
      .buffer_length = 0,						\
      .format = AGS_SOUNDCARD_DEFAULT_FORMAT,				\
      .samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE,			\
      .osc_0_oscillator = AGS_SYNTH_OSCILLATOR_SIN,			\
      .osc_0_frequency = AGS_MODULAR_SYNTH_UTIL_DEFAULT_FREQUENCY,	\
      .osc_0_phase = 0.0,						\
      .osc_0_volume = 1.0,						\
      .osc_1_oscillator = AGS_SYNTH_OSCILLATOR_SIN,			\
      .osc_1_frequency = AGS_MODULAR_SYNTH_UTIL_DEFAULT_FREQUENCY,	\
      .osc_1_phase = 0.0,						\
      .osc_1_volume = 1.0,						\
      .pitch_util = NULL,						\
      .pitch_buffer = NULL,						\
      .pitch_type = AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL,		\
      .pitch_base_key = 440.0						\
      .pitch_tuning = 0.0						\
      .env_0_attack = 1.0,						\
      .env_0_decay = 1.0,						\
      .env_0_sustain = 1.0,						\
      .env_0_release = 1.0,						\
      .env_0_gain = 1.0,						\
      .env_0_sends = {0,},						\
      .env_1_attack = 1.0,						\
      .env_1_decay = 1.0,						\
      .env_1_sustain = 1.0,						\
      .env_1_release = 1.0,						\
      .env_1_gain = 1.0,						\
      .env_1_sends = {0,},						\
      .noise_util = NULL,						\
      .noise_frequency = 220.0,						\
      .noise_gain = 1.0,						\
      .noise_sends = {0,},						\
      .lfo_0_oscillator = AGS_SYNTH_OSCILLATOR_SIN,		\
      .lfo_0_frequency = AGS_MODULAR_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY,	\
      .lfo_0_depth = AGS_MODULAR_SYNTH_UTIL_DEFAULT_LFO_DEPTH,		\
      .lfo_0_tuning = AGS_MODULAR_SYNTH_UTIL_DEFAULT_TUNING,			\
      .lfo_0_sends = {0,},						\
      .lfo_1_oscillator = AGS_SYNTH_OSCILLATOR_SIN,		\
      .lfo_1_frequency = AGS_MODULAR_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY,	\
      .lfo_1_depth = AGS_MODULAR_SYNTH_UTIL_DEFAULT_LFO_DEPTH,		\
      .lfo_1_tuning = AGS_MODULAR_SYNTH_UTIL_DEFAULT_TUNING,			\
      .lfo_1_sends = {0,},						\
      .frame_count = (AGS_SOUNDCARD_DEFAULT_SAMPLERATE / 6.0),		\
      .offset = 0,							\
      .note_256th_mode = TRUE,						\
      .offset_256th = 0 })

typedef struct _AgsModularSynthUtil AgsModularSynthUtil;

typedef enum{
  AGS_MODULAR_SYNTH_SENDS_FREQUENCY   = 1,
  AGS_MODULAR_SYNTH_SENDS_PHASE       = 1 <<  1,
  AGS_MODULAR_SYNTH_SENDS_VOLUME      = 1 <<  2,
}AgsModularSynthSends;

struct _AgsModularSynthUtil
{
  gpointer source;
  guint source_stride;
  
  guint buffer_length;
  AgsSoundcardFormat format;
  guint samplerate;

  guint osc_0_oscillator;
  
  gdouble osc_0_frequency;
  gdouble osc_0_phase;
  gdouble osc_0_volume;

  guint osc_1_oscillator;
  
  gdouble osc_1_frequency;
  gdouble osc_1_phase;
  gdouble osc_1_volume;

  gpointer pitch_util;
  
  gpointer pitch_buffer;

  GType pitch_type;
  
  gdouble pitch_base_key;
  gdouble pitch_tuning;
  
  gdouble env_0_attack;
  gdouble env_0_decay;
  gdouble env_0_sustain;
  gdouble env_0_release;
  gdouble env_0_gain;
  gdouble env_0_frequency;

  gint env_0_sends[3];
  
  gdouble env_1_attack;
  gdouble env_1_decay;
  gdouble env_1_sustain;
  gdouble env_1_release;
  gdouble env_1_gain;
  gdouble env_1_frequency;

  gint env_1_sends[3];
  
  gdouble lfo_0_oscillator;
  gdouble lfo_0_frequency;
  gdouble lfo_0_depth;
  gdouble lfo_0_tuning;

  gint lfo_0_sends[3];
  
  gdouble lfo_1_oscillator;
  gdouble lfo_1_frequency;
  gdouble lfo_1_depth;
  gdouble lfo_1_tuning;

  gint lfo_1_sends[3];
  
  gpointer noise_util;
  
  gdouble noise_frequency;
  gdouble noise_gain;

  gint noise_sends[3];

  guint frame_count;
  guint offset;

  gboolean note_256th_mode;

  guint offset_256th;
};

GType ags_modular_synth_util_get_type(void);

AgsModularSynthUtil* ags_modular_synth_util_alloc();

gpointer ags_modular_synth_util_copy(AgsModularSynthUtil *ptr);
void ags_modular_synth_util_free(AgsModularSynthUtil *ptr);

gpointer ags_modular_synth_util_get_source(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_source(AgsModularSynthUtil *modular_synth_util,
				       gpointer source);

guint ags_modular_synth_util_get_source_stride(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_source_stride(AgsModularSynthUtil *modular_synth_util,
					      guint source_stride);

guint ags_modular_synth_util_get_buffer_length(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_buffer_length(AgsModularSynthUtil *modular_synth_util,
					      guint buffer_length);

AgsSoundcardFormat ags_modular_synth_util_get_format(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_format(AgsModularSynthUtil *modular_synth_util,
				       AgsSoundcardFormat format);

guint ags_modular_synth_util_get_samplerate(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_samplerate(AgsModularSynthUtil *modular_synth_util,
					   guint samplerate);

AgsSynthOscillatorMode ags_modular_synth_util_get_osc_0_oscillator(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_osc_0_oscillator(AgsModularSynthUtil *modular_synth_util,
						 AgsSynthOscillatorMode osc_0_oscillator);

gdouble ags_modular_synth_util_get_osc_0_frequency(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_osc_0_frequency(AgsModularSynthUtil *modular_synth_util,
						gdouble osc_0_frequency);

gdouble ags_modular_synth_util_get_osc_0_phase(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_osc_0_phase(AgsModularSynthUtil *modular_synth_util,
					    gdouble osc_0_phase);

gdouble ags_modular_synth_util_get_osc_0_volume(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_osc_0_volume(AgsModularSynthUtil *modular_synth_util,
					     gdouble osc_0_volume);

AgsSynthOscillatorMode ags_modular_synth_util_get_osc_1_oscillator(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_osc_1_oscillator(AgsModularSynthUtil *modular_synth_util,
						 AgsSynthOscillatorMode osc_1_oscillator);

gdouble ags_modular_synth_util_get_osc_1_frequency(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_osc_1_frequency(AgsModularSynthUtil *modular_synth_util,
						gdouble osc_1_frequency);

gdouble ags_modular_synth_util_get_osc_1_phase(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_osc_1_phase(AgsModularSynthUtil *modular_synth_util,
					    gdouble osc_1_phase);

gdouble ags_modular_synth_util_get_osc_1_volume(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_osc_1_volume(AgsModularSynthUtil *modular_synth_util,
					     gdouble osc_1_volume);

GType ags_modular_synth_util_get_pitch_type(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_pitch_type(AgsModularSynthUtil *modular_synth_util,
					   GType pitch_type);

gdouble ags_modular_synth_util_get_pitch_base_key(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_pitch_base_key(AgsModularSynthUtil *modular_synth_util,
					       gdouble pitch_base_key);

gdouble ags_modular_synth_util_get_pitch_tuning(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_pitch_tuning(AgsModularSynthUtil *modular_synth_util,
					     gdouble pitch_tuning);

gdouble ags_modular_synth_util_get_env_0_attack(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_env_0_attack(AgsModularSynthUtil *modular_synth_util,
					     gdouble env_0_attack);

gdouble ags_modular_synth_util_get_env_0_sustain(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_env_0_sustain(AgsModularSynthUtil *modular_synth_util,
					      gdouble env_0_sustain);

gdouble ags_modular_synth_util_get_env_0_decay(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_env_0_decay(AgsModularSynthUtil *modular_synth_util,
					    gdouble env_0_decay);

gdouble ags_modular_synth_util_get_env_0_release(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_env_0_release(AgsModularSynthUtil *modular_synth_util,
					      gdouble env_0_release);

gdouble ags_modular_synth_util_get_env_0_gain(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_env_0_gain(AgsModularSynthUtil *modular_synth_util,
					   gdouble env_0_gain);

gint* ags_modular_synth_util_get_env_0_sends(AgsModularSynthUtil *modular_synth_util,
					     guint *env_0_sends_count);
void ags_modular_synth_util_set_env_0_sends(AgsModularSynthUtil *modular_synth_util,
					    gint env_0_sends,
					    guint env_0_sends_count);

gdouble ags_modular_synth_util_get_env_1_attack(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_env_1_attack(AgsModularSynthUtil *modular_synth_util,
					     gdouble env_1_attack);

gdouble ags_modular_synth_util_get_env_1_sustain(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_env_1_sustain(AgsModularSynthUtil *modular_synth_util,
					      gdouble env_1_sustain);

gdouble ags_modular_synth_util_get_env_1_decay(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_env_1_decay(AgsModularSynthUtil *modular_synth_util,
					    gdouble env_1_decay);

gdouble ags_modular_synth_util_get_env_1_release(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_env_1_release(AgsModularSynthUtil *modular_synth_util,
					      gdouble env_1_release);

gdouble ags_modular_synth_util_get_env_1_gain(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_env_1_gain(AgsModularSynthUtil *modular_synth_util,
					   gdouble env_1_gain);

gint* ags_modular_synth_util_get_env_1_sends(AgsModularSynthUtil *modular_synth_util,
					     guint *env_1_sends_count);
void ags_modular_synth_util_set_env_1_sends(AgsModularSynthUtil *modular_synth_util,
					    gint env_1_sends,
					    guint env_1_sends_count);

AgsSynthOscillatorMode ags_modular_synth_util_get_lfo_0_oscillator(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_lfo_0_oscillator(AgsModularSynthUtil *modular_synth_util,
						 AgsSynthOscillatorMode lfo_0_oscillator);

gdouble ags_modular_synth_util_get_lfo_0_frequency(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_lfo_0_frequency(AgsModularSynthUtil *modular_synth_util,
						gdouble lfo_0_frequency);

gdouble ags_modular_synth_util_get_lfo_0_depth(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_lfo_0_depth(AgsModularSynthUtil *modular_synth_util,
					    gdouble lfo_0_depth);

gdouble ags_modular_synth_util_get_lfo_0_tuning(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_lfo_0_tuning(AgsModularSynthUtil *modular_synth_util,
					     gdouble lfo_0_tuning);

gint* ags_modular_synth_util_get_lfo_0_sends(AgsModularSynthUtil *modular_synth_util,
					     guint *lfo_0_sends_count);
void ags_modular_synth_util_set_lfo_0_sends(AgsModularSynthUtil *modular_synth_util,
					    gint lfo_0_sends,
					    guint lfo_0_sends_count);

AgsSynthOscillatorMode ags_modular_synth_util_get_lfo_1_oscillator(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_lfo_1_oscillator(AgsModularSynthUtil *modular_synth_util,
						 AgsSynthOscillatorMode lfo_1_oscillator);

gdouble ags_modular_synth_util_get_lfo_1_frequency(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_lfo_1_frequency(AgsModularSynthUtil *modular_synth_util,
						gdouble lfo_1_frequency);

gdouble ags_modular_synth_util_get_lfo_1_depth(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_lfo_1_depth(AgsModularSynthUtil *modular_synth_util,
					    gdouble lfo_1_depth);

gdouble ags_modular_synth_util_get_lfo_1_tuning(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_lfo_1_tuning(AgsModularSynthUtil *modular_synth_util,
					     gdouble lfo_1_tuning);

gint* ags_modular_synth_util_get_lfo_1_sends(AgsModularSynthUtil *modular_synth_util,
					     guint *lfo_1_sends_count);
void ags_modular_synth_util_set_lfo_1_sends(AgsModularSynthUtil *modular_synth_util,
					    gint lfo_1_sends,
					    guint lfo_1_sends_count);

gdouble ags_modular_synth_util_get_noise_frequency(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_noise_frequency(AgsModularSynthUtil *modular_synth_util,
						gdouble noise_frequency);

gdouble ags_modular_synth_util_get_noise_gain(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_noise_gain(AgsModularSynthUtil *modular_synth_util,
					   gdouble noise_gain);

gint* ags_modular_synth_util_get_noise_sends(AgsModularSynthUtil *modular_synth_util,
					     guint *noise_sends_count);
void ags_modular_synth_util_set_noise_sends(AgsModularSynthUtil *modular_synth_util,
					    gint noise_sends,
					    guint noise_sends_count);

guint ags_modular_synth_util_get_frame_count(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_frame_count(AgsModularSynthUtil *modular_synth_util,
					    guint frame_count);

guint ags_modular_synth_util_get_offset(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_offset(AgsModularSynthUtil *modular_synth_util,
				       guint offset);

gboolean ags_modular_synth_util_get_note_256th_mode(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_note_256th_mode(AgsModularSynthUtil *modular_synth_util,
						gboolean note_256th_mode);

guint ags_modular_synth_util_get_offset_256th(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_set_offset_256th(AgsModularSynthUtil *modular_synth_util,
					     guint offset_256th);

/* seq sin oscillator */
void ags_modular_synth_util_compute_sin_s8(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_sin_s16(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_sin_s24(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_sin_s32(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_sin_s64(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_sin_float(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_sin_double(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_sin_complex(AgsModularSynthUtil *modular_synth_util);

void ags_modular_synth_util_compute_sin(AgsModularSynthUtil *modular_synth_util);

/* seq sawtooth oscillator */
void ags_modular_synth_util_compute_sawtooth_s8(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_sawtooth_s16(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_sawtooth_s24(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_sawtooth_s32(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_sawtooth_s64(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_sawtooth_float(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_sawtooth_double(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_sawtooth_complex(AgsModularSynthUtil *modular_synth_util);

void ags_modular_synth_util_compute_sawtooth(AgsModularSynthUtil *modular_synth_util);

/* seq triangle oscillator */
void ags_modular_synth_util_compute_triangle_s8(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_triangle_s16(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_triangle_s24(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_triangle_s32(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_triangle_s64(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_triangle_float(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_triangle_double(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_triangle_complex(AgsModularSynthUtil *modular_synth_util);

void ags_modular_synth_util_compute_triangle(AgsModularSynthUtil *modular_synth_util);

/* seq square oscillator */
void ags_modular_synth_util_compute_square_s8(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_square_s16(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_square_s24(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_square_s32(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_square_s64(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_square_float(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_square_double(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_square_complex(AgsModularSynthUtil *modular_synth_util);

void ags_modular_synth_util_compute_square(AgsModularSynthUtil *modular_synth_util);

/* seq impulse oscillator */
void ags_modular_synth_util_compute_impulse_s8(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_impulse_s16(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_impulse_s24(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_impulse_s32(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_impulse_s64(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_impulse_float(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_impulse_double(AgsModularSynthUtil *modular_synth_util);
void ags_modular_synth_util_compute_impulse_complex(AgsModularSynthUtil *modular_synth_util);

void ags_modular_synth_util_compute_impulse(AgsModularSynthUtil *modular_synth_util);

G_END_DECLS

#endif /*__AGS_MODULAR_SYNTH_UTIL_H__*/
